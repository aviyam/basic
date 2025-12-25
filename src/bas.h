#ifndef BAS_H
#define BAS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <setjmp.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#endif

/* Constants */
#define MAX_LINE_LEN 256
#define MAX_VAR_NAME 32
#define MAX_LINES 2000
#define STACK_SIZE 100
#define MAX_ARRAYS 50

/* Token Types */
typedef enum {
    TOK_NONE, TOK_EOF, TOK_EOL,
    TOK_NUMBER, TOK_STRING, TOK_IDENTIFIER,
    TOK_PRINT, TOK_IF, TOK_THEN, TOK_GOTO, TOK_FOR, TOK_TO, TOK_STEP, TOK_NEXT,
    TOK_REM, TOK_LET, TOK_INPUT, TOK_END, TOK_SLEEP, TOK_GOSUB, TOK_RETURN,
    TOK_DIM, TOK_RUN, TOK_LIST, TOK_NEW, TOK_BYE, TOK_CLS,
    TOK_SIN, TOK_COS, TOK_TAN, TOK_ATN, TOK_EXP, TOK_LOG, TOK_SQR, TOK_INT, TOK_ABS, TOK_SGN, TOK_RND,
    TOK_TAB, TOK_SPC, TOK_INKEY, TOK_LEN,
    TOK_ASC, TOK_CHR, TOK_MID, TOK_LEFT, TOK_RIGHT, TOK_VAL, TOK_STR,
    TOK_PLUS, TOK_MINUS, TOK_MUL, TOK_DIV, TOK_MOD,
    TOK_EQ, TOK_NE, TOK_LT, TOK_GT, TOK_LE, TOK_GE,
    TOK_LPAREN, TOK_RPAREN, TOK_COMMA, TOK_SEMICOLON, TOK_COLON,
    TOK_SAVE, TOK_LOAD, TOK_EDIT,
    TOK_DATA, TOK_READ, TOK_RESTORE,
    TOK_STOP
} TokenType;

/* Value Type */
typedef enum {
    VAL_NUM,
    VAL_STR
} ValType;

typedef struct {
    ValType type;
    double num;
    char *str; /* Dynamically allocated */
} Value;

/* Structure for a line of BASIC code */
typedef struct {
    int number;
    char *text;
} Line;

typedef struct {
    char name[MAX_VAR_NAME];
    Value val;
} Variable;

/* Structure for Array */
typedef struct {
    char name[MAX_VAR_NAME];
    ValType type;
    int size;
    Value *data; /* Dynamically allocated array of values */
} Array;

/* Structure for loop stack */
typedef struct {
    char var[MAX_VAR_NAME];
    double target;
    double step;
    int line_idx; /* Index in the 'lines' array */
    char *resume_ptr; /* Pointer into the line text to resume execution */
} ForLoop;

/* Structure for GOSUB stack */
typedef struct {
    int line_idx;
} GosubFrame;

/* Global State */
extern Line program[MAX_LINES];
extern int program_line_count;
extern Variable variables[100]; /* Simple symbol table */
extern int var_count;
extern Array arrays[MAX_ARRAYS];
extern int array_count;
extern ForLoop for_stack[STACK_SIZE];
extern int for_sp;
extern GosubFrame gosub_stack[STACK_SIZE];
extern int gosub_sp;
extern int current_line_idx; /* Program Counter (index in program array) */
extern int execution_finished;

extern char *jump_to_ptr; /* If non-NULL, resume execution from here instead of start of line */

extern jmp_buf error_jmp;
extern int interactive_mode_active;

/* Data Pointer State */
extern int data_line_idx;
extern char *data_ptr;

/* Lexer State */
extern char *token_ptr;
extern TokenType current_token;
extern double token_number;
extern char token_string[MAX_LINE_LEN];

/* Prototypes */
void load_program(const char *filename);
void sort_program(void);
void run_program(void);

/* Tokenizer */
void next_token(void);
void init_tokenizer(char *line);
int match(TokenType t);

/* Expression Evaluator */
Value expression(void);

/* Execution */
void exec_statement(void);
void cmd_print(void);
void cmd_if(void);
void cmd_goto(void);
void cmd_for(void);
void cmd_next(void);
void cmd_let(void);
void cmd_input(void);
void cmd_sleep(void);
void cmd_gosub(void);
void cmd_return(void);
void cmd_end(void);

/* Variables */
Value get_var(const char *name);
void set_var(const char *name, Value val);

/* Arrays */
Value *get_array_ptr(const char *name, int index);
void create_array(const char *name, int size);

/* Utils */
void error(const char *msg);
int find_line_index(int line_num);

/* Platform IO */
char *get_inkey(void);
void setup_terminal(void);
void restore_terminal(void);

/* Utils */
void ensure_extension(char *filename);

#endif
