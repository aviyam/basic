#include "bas.h"

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
        /* Strip trailing newline */
        p = buffer + strlen(buffer) - 1;
        while (p >= buffer && (*p == '\r' || *p == '\n')) {
            *p = '\0';
            p--;
        }
        
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
    
    printf("BASIC v1.0 Ready\n");
    
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
