#include "bas.h"

/* Forward declarations */
Value additive(void);
Value term(void);
Value factor(void);

Value expression(void) {
    Value left = additive();
    while (current_token == TOK_EQ || current_token == TOK_NE ||
           current_token == TOK_LT || current_token == TOK_GT ||
           current_token == TOK_LE || current_token == TOK_GE) {
        TokenType op = current_token;
        Value right;
        next_token();
        right = additive();
        
        if (left.type == VAL_NUM && right.type == VAL_NUM) {
            double res = 0.0;
            switch (op) {
                case TOK_EQ: res = (left.num == right.num) ? -1.0 : 0.0; break;
                case TOK_NE: res = (left.num != right.num) ? -1.0 : 0.0; break;
                case TOK_LT: res = (left.num < right.num) ? -1.0 : 0.0; break;
                case TOK_GT: res = (left.num > right.num) ? -1.0 : 0.0; break;
                case TOK_LE: res = (left.num <= right.num) ? -1.0 : 0.0; break;
                case TOK_GE: res = (left.num >= right.num) ? -1.0 : 0.0; break;
                default: break;
            }
            left.num = res;
        } else if (left.type == VAL_STR && right.type == VAL_STR) {
            int cmp = strcmp(left.str, right.str);
            double res = 0.0;
            switch (op) {
                case TOK_EQ: res = (cmp == 0) ? -1.0 : 0.0; break;
                case TOK_NE: res = (cmp != 0) ? -1.0 : 0.0; break;
                case TOK_LT: res = (cmp < 0) ? -1.0 : 0.0; break;
                case TOK_GT: res = (cmp > 0) ? -1.0 : 0.0; break;
                case TOK_LE: res = (cmp <= 0) ? -1.0 : 0.0; break;
                case TOK_GE: res = (cmp >= 0) ? -1.0 : 0.0; break;
                default: break;
            }
            /* Result of comparison is always a number */
            if (left.str) free(left.str);
            if (right.str) free(right.str);
            left.type = VAL_NUM;
            left.num = res;
        } else {
            error("Type mismatch in comparison");
        }
    }
    return left;
}

Value additive(void) {
    Value left = term();
    while (current_token == TOK_PLUS || current_token == TOK_MINUS) {
        TokenType op = current_token;
        Value right;
        next_token();
        right = term();
        
        if (op == TOK_PLUS) {
            if (left.type == VAL_NUM && right.type == VAL_NUM) {
                left.num += right.num;
            } else if (left.type == VAL_STR && right.type == VAL_STR) {
                /* String concatenation */
                char *new_str = malloc(strlen(left.str) + strlen(right.str) + 1);
                strcpy(new_str, left.str);
                strcat(new_str, right.str);
                free(left.str);
                free(right.str);
                left.str = new_str;
            } else {
                error("Type mismatch in addition");
            }
        } else {
            if (left.type == VAL_NUM && right.type == VAL_NUM) {
                left.num -= right.num;
            } else {
                error("Type mismatch in subtraction");
            }
        }
    }
    return left;
}

Value term(void) {
    Value left = factor();
    while (current_token == TOK_MUL || current_token == TOK_DIV || current_token == TOK_MOD) {
        TokenType op = current_token;
        Value right;
        next_token();
        right = factor();
        
        if (left.type == VAL_NUM && right.type == VAL_NUM) {
            if (op == TOK_MUL) left.num *= right.num;
            else if (op == TOK_DIV) {
                if (right.num == 0.0) error("Division by zero");
                left.num /= right.num;
            } else {
                if (right.num == 0.0) error("Division by zero");
                left.num = (int)left.num % (int)right.num;
            }
        } else {
            error("Type mismatch in multiplication/division");
        }
    }
    return left;
}

Value factor(void) {
    Value val;
    val.type = VAL_NUM;
    val.num = 0.0;
    
    if (current_token == TOK_NUMBER) {
        val.type = VAL_NUM;
        val.num = token_number;
        next_token();
    } else if (current_token == TOK_STRING) {
        val.type = VAL_STR;
        val.str = malloc(strlen(token_string) + 1);
        strcpy(val.str, token_string);
        next_token();
    } else if (current_token == TOK_IDENTIFIER) {
        val = get_var(token_string);
        next_token();
    } else if (current_token == TOK_LPAREN) {
        next_token();
        val = expression();
        if (!match(TOK_RPAREN)) error("Missing ')'");
    } else if (current_token == TOK_MINUS) {
        next_token();
        val = factor();
        if (val.type == VAL_NUM) val.num = -val.num;
        else error("Type mismatch for unary minus");
    } else if (current_token == TOK_PLUS) {
        next_token();
        val = factor(); /* Unary plus, do nothing */
    } else if (current_token == TOK_INKEY) {
        next_token(); /* Consume INKEY$ */
        val.type = VAL_STR;
        val.str = get_inkey();
    } else if (current_token == TOK_LEN) {
        next_token();
        if (!match(TOK_LPAREN)) error("Expected '(' for LEN");
        Value v = expression();
        if (v.type != VAL_STR) error("LEN expects string");
        val.type = VAL_NUM;
        val.num = (double)strlen(v.str);
        free(v.str);
        if (!match(TOK_RPAREN)) error("Missing ')' for LEN");
    } else if (current_token >= TOK_SIN && current_token <= TOK_RND) {
        TokenType func = current_token;
        next_token();
        if (!match(TOK_LPAREN)) error("Expected '(' for function");
        val = expression();
        if (!match(TOK_RPAREN)) error("Missing ')' for function");
        
        if (val.type != VAL_NUM) error("Function expects number");
        
        switch (func) {
            case TOK_SIN: val.num = sin(val.num); break;
            case TOK_COS: val.num = cos(val.num); break;
            case TOK_TAN: val.num = tan(val.num); break;
            case TOK_ATN: val.num = atan(val.num); break;
            case TOK_EXP: val.num = exp(val.num); break;
            case TOK_LOG: 
                if (val.num <= 0) error("Log of non-positive number");
                val.num = log(val.num); 
                break;
            case TOK_SQR:
                if (val.num < 0) error("Sqrt of negative number");
                val.num = sqrt(val.num);
                break;
            case TOK_INT: val.num = floor(val.num); break;
            case TOK_ABS: val.num = fabs(val.num); break;
            case TOK_SGN: val.num = (val.num > 0) ? 1 : ((val.num < 0) ? -1 : 0); break;
            case TOK_RND: val.num = ((double)rand() / (double)RAND_MAX); break;
            default: break;
        }
    } else {
        error("Expected number, variable, or function");
    }
    return val;
}
