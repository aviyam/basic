#include "bas.h"

#ifndef _WIN32
#include <termios.h>
#include <fcntl.h>
struct termios orig_termios;
int term_setup = 0;
#else
#include <conio.h>
#endif

int current_column = 0;

/* Terminal handling for INKEY$ on POSIX */
void setup_terminal(void) {
#ifndef _WIN32
    struct termios new_termios;
    if (term_setup) return;
    tcgetattr(STDIN_FILENO, &orig_termios);
    new_termios = orig_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    new_termios.c_cc[VMIN] = 0;
    new_termios.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
    term_setup = 1;
#endif
}

void restore_terminal(void) {
#ifndef _WIN32
    if (term_setup) {
        tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
        term_setup = 0;
    }
#endif
}

char *get_inkey(void) {
    char *s = malloc(2);
    s[0] = '\0';
    s[1] = '\0';

#ifdef _WIN32
    if (_kbhit()) {
        s[0] = _getch();
    }
#else
    {
        char c;
        setup_terminal(); /* Ensure terminal is in raw mode */
        if (read(STDIN_FILENO, &c, 1) == 1) {
            s[0] = c;
        }
        restore_terminal();
    }
#endif
    return s;
}

int find_line_index(int line_num) {
    int i;
    for (i = 0; i < program_line_count; i++) {
        if (program[i].number == line_num) return i;
    }
    return -1;
}

void cmd_run(void) {
    int i;
    next_token();
    
    /* Reset state */
    current_line_idx = 0;
    execution_finished = 0;
    for_sp = 0;
    gosub_sp = 0;
    /* Clear variables? Standard BASIC does. */
    for (i = 0; i < var_count; i++) {
        if (variables[i].val.type == VAL_STR && variables[i].val.str) {
            free(variables[i].val.str);
        }
    }
    var_count = 0;
    
    run_program();
    execution_finished = 0; /* Reset for interactive */
}

void cmd_list(void) {
    int i;
    next_token();
    for (i = 0; i < program_line_count; i++) {
        printf("%d %s\n", program[i].number, program[i].text);
    }
}

void cmd_new(void) {
    int i;
    next_token();
    for (i = 0; i < program_line_count; i++) {
        free(program[i].text);
    }
    program_line_count = 0;
    /* Clear variables */
    for (i = 0; i < var_count; i++) {
        if (variables[i].val.type == VAL_STR && variables[i].val.str) {
            free(variables[i].val.str);
        }
    }
    var_count = 0;
}

void cmd_bye(void) {
    exit(0);
}

void cmd_cls(void) {
    next_token();
#ifdef _WIN32
    system("cls");
#else
    printf("\033[2J\033[H");
    fflush(stdout);
#endif
    current_column = 0;
}

void cmd_print(void) {
    int newline = 1;
    next_token();
    
    if (current_token == TOK_EOL || current_token == TOK_EOF || current_token == TOK_COLON) {
        printf("\n");
        current_column = 0;
        return;
    }

    while (current_token != TOK_EOL && current_token != TOK_EOF && current_token != TOK_COLON) {
        if (current_token == TOK_TAB) {
            double pos;
            int target;
            next_token();
            if (!match(TOK_LPAREN)) error("Expected '(' for TAB");
            {
                Value v = expression();
                if (v.type != VAL_NUM) error("TAB expects number");
                pos = v.num;
            }
            if (!match(TOK_RPAREN)) error("Expected ')' for TAB");
            
            target = (int)pos;
            if (target > current_column) {
                while (current_column < target) {
                    printf(" ");
                    current_column++;
                }
            }
        } else if (current_token == TOK_SPC) {
            double count;
            int i;
            next_token();
            if (!match(TOK_LPAREN)) error("Expected '(' for SPC");
            {
                Value v = expression();
                if (v.type != VAL_NUM) error("SPC expects number");
                count = v.num;
            }
            if (!match(TOK_RPAREN)) error("Expected ')' for SPC");
            
            for (i = 0; i < (int)count; i++) {
                printf(" ");
                current_column++;
            }
        } else {
            Value val = expression();
            if (val.type == VAL_STR) {
                printf("%s", val.str);
                current_column += strlen(val.str);
                free(val.str);
            } else {
                if (val.num == (int)val.num) current_column += printf("%d", (int)val.num);
                else current_column += printf("%g", val.num);
            }
        }

        newline = 1;
        if (current_token == TOK_SEMICOLON) {
            newline = 0;
            next_token();
        } else if (current_token == TOK_COMMA) {
            newline = 0;
            printf("\t");
            current_column = (current_column / 8 + 1) * 8;
            next_token();
        } else if (current_token != TOK_EOL && current_token != TOK_EOF && current_token != TOK_COLON) {
             /* implicit separator */
        }
    }
    if (newline) {
        printf("\n");
        current_column = 0;
    }
}

void cmd_goto(void) {
    Value v;
    int idx;
    next_token();
    v = expression();
    if (v.type != VAL_NUM) error("Line number must be numeric");
    idx = find_line_index((int)v.num);
    if (idx == -1) error("Line not found");
    current_line_idx = idx - 1;
}

void cmd_if(void) {
    Value cond;
    next_token();
    cond = expression();
    
    if (cond.type != VAL_NUM) error("IF condition must be numeric");

    if (match(TOK_THEN)) {
        if (cond.num != 0.0) {
            if (current_token == TOK_NUMBER) {
                cmd_goto();
            } else {
                exec_statement();
            }
        } else {
            while (current_token != TOK_EOL && current_token != TOK_EOF) next_token();
        }
    } else if (match(TOK_GOTO)) {
         if (cond.num != 0.0) {
            Value v = expression();
            int idx;
            if (v.type != VAL_NUM) error("Line number must be numeric");
            idx = find_line_index((int)v.num);
            if (idx == -1) error("Line not found");
            current_line_idx = idx - 1;
         } else {
             while (current_token != TOK_EOL && current_token != TOK_EOF) next_token();
         }
    } else {
        error("Expected THEN or GOTO");
    }
}

void cmd_let(void) {
    char var_name[MAX_VAR_NAME];
    Value val;
    next_token();
    if (current_token != TOK_IDENTIFIER) error("Expected variable");
    strcpy(var_name, token_string);
    next_token();
    if (!match(TOK_EQ)) error("Expected =");
    val = expression();
    set_var(var_name, val);
}

void cmd_input(void) {
    char var_name[MAX_VAR_NAME];
    char buffer[128];
    Value val;
    next_token();
    if (current_token == TOK_STRING) {
        printf("%s", token_string);
        current_column += strlen(token_string);
        next_token();
        if (match(TOK_SEMICOLON) || match(TOK_COMMA)) {
            /* ok */
        }
    } else {
        printf("? ");
        current_column += 2;
    }
    
    if (current_token != TOK_IDENTIFIER) error("Expected variable");
    strcpy(var_name, token_string);
    next_token();

    restore_terminal();
    if (fgets(buffer, sizeof(buffer), stdin)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') buffer[len-1] = '\0';
        
        if (strchr(var_name, '$')) {
            val.type = VAL_STR;
            val.str = malloc(strlen(buffer) + 1);
            strcpy(val.str, buffer);
        } else {
            val.type = VAL_NUM;
            val.num = atof(buffer);
        }
        set_var(var_name, val);
        current_column = 0;
    }
}

void cmd_sleep(void) {
    double ticks;
    long msec; /* Milliseconds */

    next_token();
    if (!match(TOK_LPAREN)) error("Expected (");
    {
        Value v = expression();
        if (v.type != VAL_NUM) error("Sleep expects number");
        ticks = v.num;
    }
    if (!match(TOK_RPAREN)) error("Expected ) ");
    
    /* 60 ticks = 1 second. Convert to milliseconds. */
    msec = (long)(ticks * 1000 / 60);

#ifdef _WIN32
    Sleep(msec);
#else
    {
        long sec = msec / 1000;
        long usec = (msec % 1000) * 1000;
        struct timeval tv;
        tv.tv_sec = sec;
        tv.tv_usec = usec;
        
        select(0, NULL, NULL, NULL, &tv);
    }
#endif
}

void cmd_for(void) {
    char var_name[MAX_VAR_NAME];
    double start_val, end_val, step_val = 1.0;
    Value v;
    
    next_token();
    if (current_token != TOK_IDENTIFIER) error("Expected variable");
    strcpy(var_name, token_string);
    next_token();
    
    if (!match(TOK_EQ)) error("Expected =");
    v = expression();
    if (v.type != VAL_NUM) error("For start must be number");
    start_val = v.num;
    
    if (!match(TOK_TO)) error("Expected TO");
    v = expression();
    if (v.type != VAL_NUM) error("For end must be number");
    end_val = v.num;
    
    if (match(TOK_STEP)) {
        v = expression();
        if (v.type != VAL_NUM) error("For step must be number");
        step_val = v.num;
    }
    
    v.type = VAL_NUM;
    v.num = start_val;
    set_var(var_name, v);
    
    /* Push to stack */
    if (for_sp < STACK_SIZE) {
        strcpy(for_stack[for_sp].var, var_name);
        for_stack[for_sp].target = end_val;
        for_stack[for_sp].step = step_val;
        for_stack[for_sp].line_idx = current_line_idx;
        for_sp++;
    } else {
        error("FOR stack overflow");
    }
}

void cmd_next(void) {
    next_token();
    if (current_token != TOK_IDENTIFIER) {
        /* Optional variable name */
    } else {
        if (for_sp > 0 && strcmp(token_string, for_stack[for_sp-1].var) != 0) {
            error("NEXT without matching FOR variable");
        }
        next_token();
    }
    
    if (for_sp > 0) {
        ForLoop *loop = &for_stack[for_sp-1];
        Value v = get_var(loop->var);
        int loop_continues = 0;
        
        v.num += loop->step;
        set_var(loop->var, v);
        
        if (loop->step > 0) loop_continues = (v.num <= loop->target);
        else loop_continues = (v.num >= loop->target);
        
        if (loop_continues) {
            current_line_idx = loop->line_idx; 
        } else {
            for_sp--;
        }
    } else {
        error("NEXT without FOR");
    }
}

void cmd_gosub(void) {
    Value v;
    int idx;
    next_token();
    v = expression();
    if (v.type != VAL_NUM) error("Line number must be numeric");
    idx = find_line_index((int)v.num);
    if (idx == -1) error("Line not found");
    
    if (gosub_sp < STACK_SIZE) {
        gosub_stack[gosub_sp].line_idx = current_line_idx;
        gosub_sp++;
        current_line_idx = idx - 1;
    } else {
        error("GOSUB stack overflow");
    }
}

void cmd_return(void) {
    next_token();
    if (gosub_sp > 0) {
        gosub_sp--;
        current_line_idx = gosub_stack[gosub_sp].line_idx;
    } else {
        error("RETURN without GOSUB");
    }
}

void cmd_end(void) {
    next_token();
    execution_finished = 1;
}

void cmd_rem(void) {
    /* Skip everything */
    while (current_token != TOK_EOL && current_token != TOK_EOF) next_token();
}

void exec_statement(void) {
    if (current_token == TOK_PRINT) cmd_print();
    else if (current_token == TOK_IF) cmd_if();
    else if (current_token == TOK_GOTO) cmd_goto();
    else if (current_token == TOK_FOR) cmd_for();
    else if (current_token == TOK_NEXT) cmd_next();
    else if (current_token == TOK_LET) cmd_let();
    else if (current_token == TOK_INPUT) cmd_input();
    else if (current_token == TOK_SLEEP) cmd_sleep();
    else if (current_token == TOK_GOSUB) cmd_gosub();
    else if (current_token == TOK_RETURN) cmd_return();
    else if (current_token == TOK_END) cmd_end();
    else if (current_token == TOK_REM) cmd_rem();
    else if (current_token == TOK_IDENTIFIER) {
        char var_name[MAX_VAR_NAME];
        strcpy(var_name, token_string);
        next_token();
        if (current_token == TOK_EQ) {
            Value val;
            next_token();
            val = expression();
            set_var(var_name, val);
        } else {
            error("Syntax error or unknown command");
        }
    }
    else {
        if (current_token != TOK_EOL && current_token != TOK_EOF && current_token != TOK_COLON) {
             next_token(); 
        }
    }
}

void run_program(void) {
    current_line_idx = 0;
    while (current_line_idx < program_line_count && !execution_finished) {
        init_tokenizer(program[current_line_idx].text);
        
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
        current_line_idx++;
    }
    restore_terminal();
}
