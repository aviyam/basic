#include "bas.h"
#include <time.h>

/* Global definitions */
Line program[MAX_LINES];
int program_line_count = 0;
Variable variables[100];
int var_count = 0;
Array arrays[MAX_ARRAYS];
int array_count = 0;
UserFunc user_functions[26];
ForLoop for_stack[STACK_SIZE];
int for_sp = 0;
GosubFrame gosub_stack[STACK_SIZE];
int gosub_sp = 0;
int current_line_idx = 0;
int execution_finished = 0;

/* Data pointer globals */
int data_line_idx = 0;
char *data_ptr = NULL;

char *jump_to_ptr = NULL;

jmp_buf error_jmp;
int interactive_mode_active = 0;

/* Lexer definitions */
char *token_ptr = NULL;
TokenType current_token = TOK_NONE;
double token_number = 0.0;
char token_string[MAX_LINE_LEN];

/* History for interactive mode */
static char *history[HISTORY_SIZE];
static int history_count = 0;
static int history_idx = 0; // Current position in history when navigating
void error(const char *msg) {
    if (current_line_idx >= 0 && current_line_idx < program_line_count) {
        fprintf(stderr, "Error in line %d: %s\n", program[current_line_idx].number, msg);
    } else {
        fprintf(stderr, "Error: %s\n", msg);
    }

    if (interactive_mode_active) {
        restore_terminal();
        longjmp(error_jmp, 1);
    }
    exit(1);
}

// Adds a command to history, managing the buffer as a circular queue
void add_to_history(const char *command) {
    if (strlen(command) == 0) return; // Don't add empty commands

    // Don't add duplicate consecutive commands
    if (history_count > 0 && strcmp(history[(history_count - 1 + HISTORY_SIZE) % HISTORY_SIZE], command) == 0) {
        return;
    }

    int idx_to_add = history_count % HISTORY_SIZE;
    if (history[idx_to_add] != NULL) {
        free(history[idx_to_add]);
    }
    history[idx_to_add] = strdup(command);
    if (history_count < HISTORY_SIZE) {
        history_count++;
    }
    history_idx = history_count; // Reset history index to end after adding
}

// Custom line reader with history and basic editing
char *read_line_with_history(const char *prompt) {
    static char line_buffer[MAX_LINE_LEN];
    int buffer_len = 0;
    int cursor_pos = 0; // New: Tracks cursor position within the buffer
    int key; // Declared here, used in loop.

    line_buffer[0] = '\0';
    history_idx = history_count; // Start at the end of history for new input

    setup_terminal(); // Ensure terminal is in raw mode

    printf("%s", prompt);
    fflush(stdout);

    while (1) {
        key = read_key();

        if (key == KEY_NORMAL) {
            // No key pressed, continue
            continue;
        } else if (key >= 32 && key <= 126) { // Printable ASCII character
            if (buffer_len < MAX_LINE_LEN - 1) { // Check for buffer overflow
                if (cursor_pos < buffer_len) {
                    // Insert character in the middle of the line
                    memmove(line_buffer + cursor_pos + 1, line_buffer + cursor_pos, buffer_len - cursor_pos);
                    line_buffer[cursor_pos] = (char)key;
                    buffer_len++;
                    line_buffer[buffer_len] = '\0';
                    print_line_buffer(prompt, line_buffer, cursor_pos + 1); // Redraw and position cursor
                    cursor_pos++;
                } else {
                    // Append character to the end of the line
                    line_buffer[buffer_len++] = (char)key;
                    line_buffer[buffer_len] = '\0';
                    printf("%c", (char)key); // Echo character
                    cursor_pos++;
                    fflush(stdout);
                }
            }
        } else if (key == KEY_BACKSPACE) {
            if (buffer_len > 0) {
                if (cursor_pos < buffer_len) {
                    // Delete character before cursor and shift
                    memmove(line_buffer + cursor_pos - 1, line_buffer + cursor_pos, buffer_len - cursor_pos);
                    buffer_len--;
                    line_buffer[buffer_len] = '\0';
                    cursor_pos--;
                    print_line_buffer(prompt, line_buffer, cursor_pos); // Redraw and position cursor
                } else {
                    // Delete last character
                    buffer_len--;
                    line_buffer[buffer_len] = '\0';
                    printf("\b \b"); // Erase character
                    cursor_pos--;
                    fflush(stdout);
                }
            }
        } else if (key == KEY_ENTER) {
            printf("\n"); // Newline for the entered command
            break;
        } else if (key == KEY_UP) {
            if (history_count > 0 && history_idx > 0) {
                history_idx--;
                strcpy(line_buffer, history[history_idx % HISTORY_SIZE]);
                buffer_len = strlen(line_buffer);
                cursor_pos = buffer_len; // Cursor at end of history command
                print_line_buffer(prompt, line_buffer, cursor_pos);
            }
        } else if (key == KEY_DOWN) {
            if (history_count > 0 && history_idx < history_count) {
                history_idx++;
                if (history_idx == history_count) { // Past the last history item, show empty line
                    line_buffer[0] = '\0';
                    buffer_len = 0;
                    cursor_pos = 0;
                } else {
                    strcpy(line_buffer, history[history_idx % HISTORY_SIZE]);
                    buffer_len = strlen(line_buffer);
                }
                cursor_pos = buffer_len; // Cursor at end of history command
                print_line_buffer(prompt, line_buffer, cursor_pos);
            }
        } else if (key == KEY_LEFT) { // New: Left arrow
            if (cursor_pos > 0) {
                cursor_pos--;
                printf("\033[D"); // Move terminal cursor left
                fflush(stdout);
            }
        } else if (key == KEY_RIGHT) { // New: Right arrow
            if (cursor_pos < buffer_len) {
                cursor_pos++;
                printf("\033[C"); // Move terminal cursor right
                fflush(stdout);
            }
        } else if (key == KEY_EOF) { // Ctrl+D or Del
            if (buffer_len == 0) {
                printf("\n");
                restore_terminal();
                return NULL; // Indicate EOF
            }
        }
        // Ignore other special keys for now (e.g., KEY_LEFT, KEY_RIGHT)
    }

    restore_terminal(); // Restore terminal to normal mode after line input
    return line_buffer;
}

void process_line(char *buffer) {
    int line_num;
    char *text_start;
    char *p = buffer;
    int i;
    
    while (*p && isspace(*p)) p++;
    if (!*p) return;

    if (isdigit(*p)) {
        line_num = atoi(p);
        while (isdigit(*p)) p++;
        while (*p && isspace(*p)) p++;
        
        text_start = p;
        /* Strip trailing newlines */
        p = text_start + strlen(text_start) - 1;
        while (p >= text_start && (*p == '\r' || *p == '\n')) {
            *p = '\0';
            p--;
        }

        /* Check if line exists */
        for (i = 0; i < program_line_count; i++) {
            if (program[i].number == line_num) {
                free(program[i].text);
                if (*text_start == '\0') {
                    /* Delete line */
                    int j;
                    for (j = i; j < program_line_count - 1; j++) {
                        program[j] = program[j+1];
                    }
                    program_line_count--;
                } else {
                    /* Replace line */
                    program[i].text = (char *)malloc(strlen(text_start) + 1);
                    strcpy(program[i].text, text_start);
                }
                return;
            }
        }

        /* Add new line */
        if (*text_start == '\0') return; /* Nothing to add */
        
        if (program_line_count < MAX_LINES) {
            program[program_line_count].number = line_num;
            program[program_line_count].text = (char *)malloc(strlen(text_start) + 1);
            strcpy(program[program_line_count].text, text_start);
            program_line_count++;
            sort_program();
        } else {
            fprintf(stderr, "Program too large\n");
        }
    } else {
        /* Immediate mode execution */
        // The read_line_with_history function already ensures no trailing newline
        
        current_line_idx = -1; /* Special value for immediate mode? */
        init_tokenizer(buffer);
        while (current_token != TOK_EOL && current_token != TOK_EOF) {
            exec_statement();
            if (current_token == TOK_COLON) next_token();
        }
    }
}

void load_program(const char *filename) {
    FILE *fp;
    char buffer[MAX_LINE_LEN];
    char path[MAX_LINE_LEN];
    int first_line = 1;

    /* Try to open the file as is */
    fp = fopen(filename, "r");
    if (!fp) {
        /* Try with .bas extension */
        strcpy(path, filename);
        ensure_extension(path);
        fp = fopen(path, "r");
    }
    
    if (!fp) {
        fprintf(stderr, "Could not open file %s\n", filename);
        exit(1);
    }

    while (fgets(buffer, MAX_LINE_LEN, fp)) {
        if (first_line) {
            first_line = 0;
            if (buffer[0] == '#') continue; 
        }
        process_line(buffer);
    }
    fclose(fp);
}

void sort_program(void) {
    int i, j;
    Line temp;
    
    for (i = 0; i < program_line_count - 1; i++) {
        for (j = 0; j < program_line_count - i - 1; j++) {
            if (program[j].number > program[j + 1].number) {
                temp = program[j];
                program[j] = program[j + 1];
                program[j + 1] = temp;
            }
        }
    }
}

void interactive_mode(void) {
    char buffer[MAX_LINE_LEN];
	#ifdef _WIN32
    system("cls");
	#else
    printf("\033[2J\033[H");
    fflush(stdout);
	#endif // _WIN32
    printf("BASIC %s Ready\n", BASIC_VERSION);
    
    interactive_mode_active = 1;
    
    setjmp(error_jmp);
    /* If we come here via longjmp, execution resumes here. 
       We should reset execution state if needed. */
    execution_finished = 0;
    /* Don't clear variables, users want to inspect them */
    
    while (1) {
        printf("] ");
        if (!fgets(buffer, MAX_LINE_LEN, stdin)) break;
        
        process_line(buffer);
    }
}

int main(int argc, char **argv) {
    srand(time(NULL));

    if (argc < 2) {
        interactive_mode();
        return 0;
    }

    load_program(argv[1]);
    run_program();

    return 0;
}

void run_program(void) {
    current_line_idx = 0;
    
    /* Reset Data Pointer */
    data_line_idx = 0;
    data_ptr = NULL;
    jump_to_ptr = NULL;
    
    while (current_line_idx < program_line_count && !execution_finished) {
        if (jump_to_ptr != NULL) {
            token_ptr = jump_to_ptr;
            next_token();
            jump_to_ptr = NULL;
        } else {
            init_tokenizer(program[current_line_idx].text);
        }
        
        while (current_token != TOK_EOL && current_token != TOK_EOF && !execution_finished) {
            int entry_line_idx = current_line_idx;
            
            exec_statement();
            
            if (current_line_idx != entry_line_idx) break;
            
            if (current_token == TOK_COLON) {
                next_token();
            } else if (current_token != TOK_EOL && current_token != TOK_EOF) {
                 while (current_token != TOK_EOL && current_token != TOK_EOF) next_token();
            }
        }
        if (jump_to_ptr == NULL) {
            current_line_idx++;
        }
    }
    restore_terminal();
}
