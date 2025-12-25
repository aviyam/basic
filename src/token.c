#include "bas.h"

typedef struct {
    char *name;
    TokenType type;
} Keyword;

static Keyword keywords[] = {
    {"PRINT", TOK_PRINT},
    {"IF", TOK_IF},
    {"THEN", TOK_THEN},
    {"GOTO", TOK_GOTO},
    {"FOR", TOK_FOR},
    {"TO", TOK_TO},
    {"STEP", TOK_STEP},
    {"NEXT", TOK_NEXT},
    {"REM", TOK_REM},
    {"LET", TOK_LET},
    {"INPUT", TOK_INPUT},
    {"END", TOK_END},
    {"SLEEP", TOK_SLEEP},
    {"GOSUB", TOK_GOSUB},
    {"RETURN", TOK_RETURN},
    {"DIM", TOK_DIM},
    {"RUN", TOK_RUN},
    {"LIST", TOK_LIST},
    {"NEW", TOK_NEW},
    {"BYE", TOK_BYE},
    {"EXIT", TOK_BYE},
    {"QUIT", TOK_BYE},
    {"CLS", TOK_CLS},
    {"SIN", TOK_SIN},
    {"COS", TOK_COS},
    {"TAN", TOK_TAN},
    {"ATN", TOK_ATN},
    {"EXP", TOK_EXP},
    {"LOG", TOK_LOG},
    {"SQR", TOK_SQR},
    {"INT", TOK_INT},
    {"ABS", TOK_ABS},
    {"SGN", TOK_SGN},
    {"RND", TOK_RND},
    {"TAB", TOK_TAB},
    {"SPC", TOK_SPC},
    {"INKEY$", TOK_INKEY},
    {"LEN", TOK_LEN},
    {"ASC", TOK_ASC},
    {"CHR$", TOK_CHR},
    {"MID$", TOK_MID},
    {"LEFT$", TOK_LEFT},
    {"RIGHT$", TOK_RIGHT},
    {"VAL", TOK_VAL},
    {"STR$", TOK_STR},
    {"SAVE", TOK_SAVE},
    {"LOAD", TOK_LOAD},
    {"EDIT", TOK_EDIT},
    {"DATA", TOK_DATA},
    {"READ", TOK_READ},
    {"RESTORE", TOK_RESTORE},
    {NULL, TOK_NONE}
};

void init_tokenizer(char *line) {
    token_ptr = line;
    next_token();
}

void next_token(void) {
    char *start;
    int i;
    
    while (*token_ptr && isspace(*token_ptr)) token_ptr++;

    if (*token_ptr == '\0') {
        current_token = TOK_EOL;
        return;
    }

    if (isdigit(*token_ptr) || *token_ptr == '.') {
        current_token = TOK_NUMBER;
        token_number = strtod(token_ptr, &token_ptr);
        return;
    }

    if (*token_ptr == '"') {
        current_token = TOK_STRING;
        token_ptr++;
        start = token_ptr;
        while (*token_ptr && *token_ptr != '"') token_ptr++;
        if (*token_ptr == '"') {
            *token_ptr = '\0';
            strcpy(token_string, start);
            *token_ptr = '"'; /* Restore for safety? No need if we copy */
            token_ptr++;
        } else {
            error("Unterminated string");
        }
        return;
    }

    if (isalpha(*token_ptr)) {
        /* Identifier or Keyword */
        char buffer[MAX_VAR_NAME];
        int len = 0;
        
        /* Allow alphanumeric and '$' */
        while ((isalnum(*token_ptr) || *token_ptr == '$') && len < MAX_VAR_NAME - 1) {
            buffer[len++] = toupper(*token_ptr);
            token_ptr++;
        }
        buffer[len] = '\0';

        /* Check keywords */
        for (i = 0; keywords[i].name != NULL; i++) {
            if (strcmp(buffer, keywords[i].name) == 0) {
                current_token = keywords[i].type;
                return;
            }
        }

        /* Not a keyword, must be identifier */
        current_token = TOK_IDENTIFIER;
        strcpy(token_string, buffer);
        return;
    }

    /* Operators and Punctuation */
    switch (*token_ptr) {
        case '+': current_token = TOK_PLUS; token_ptr++; break;
        case '-': current_token = TOK_MINUS; token_ptr++; break;
        case '*': current_token = TOK_MUL; token_ptr++; break;
        case '/': current_token = TOK_DIV; token_ptr++; break;
        case '%': current_token = TOK_MOD; token_ptr++; break;
        case '(': current_token = TOK_LPAREN; token_ptr++; break;
        case ')': current_token = TOK_RPAREN; token_ptr++; break;
        case ',': current_token = TOK_COMMA; token_ptr++; break;
        case ';': current_token = TOK_SEMICOLON; token_ptr++; break;
        case ':': current_token = TOK_COLON; token_ptr++; break;
        case '=': current_token = TOK_EQ; token_ptr++; break;
        case '<':
            token_ptr++;
            if (*token_ptr == '>') { current_token = TOK_NE; token_ptr++; }
            else if (*token_ptr == '=') { current_token = TOK_LE; token_ptr++; }
            else current_token = TOK_LT;
            break;
        case '>':
            token_ptr++;
            if (*token_ptr == '=') { current_token = TOK_GE; token_ptr++; }
            else current_token = TOK_GT;
            break;
        default:
            error("Unknown character");
            break;
    }
}

int match(TokenType t) {
    if (current_token == t) {
        next_token();
        return 1;
    }
    return 0;
}
