#include "bas.h"

/* Forward declarations */
Value expression(void);
Value logical_or(void);
Value logical_and(void);
Value logical_not(void);
Value relation(void);
Value additive(void);
Value term(void);
Value factor(void);

Value expression(void) {
    return logical_or();
}

Value logical_or(void) {
    Value left = logical_and();
    while (current_token == TOK_OR) {
        next_token();
        Value right = logical_and();
        if (left.type == VAL_NUM && right.type == VAL_NUM) {
            /* BASIC uses truthy/falsy logic. 0 is false, !=0 is true (usually -1 for built-ins).
               A OR B is true if A!=0 or B!=0. Result -1.
               Or bitwise? Standard BASIC behavior varies.
               MS BASIC: bitwise integer operations.
               For this simple interpreter, let's stick to logical boolean results (-1/0).
               Or bitwise if integer? Let's assume boolean for flow control.
            */
             int l = (left.num != 0);
             int r = (right.num != 0);
             left.num = (l || r) ? -1.0 : 0.0;
        } else {
             error("Type mismatch in OR");
        }
    }
    return left;
}

Value logical_and(void) {
    Value left = logical_not();
    while (current_token == TOK_AND) {
        next_token();
        Value right = logical_not();
        if (left.type == VAL_NUM && right.type == VAL_NUM) {
             int l = (left.num != 0);
             int r = (right.num != 0);
             left.num = (l && r) ? -1.0 : 0.0;
        } else {
             error("Type mismatch in AND");
        }
    }
    return left;
}

Value logical_not(void) {
    if (current_token == TOK_NOT) {
        next_token();
        Value val = logical_not(); /* Recursive for NOT NOT A */
        if (val.type == VAL_NUM) {
             val.num = (val.num == 0.0) ? -1.0 : 0.0;
        } else {
             error("Type mismatch in NOT");
        }
        return val;
    }
    return relation();
}

Value relation(void) {
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
        char var_name[MAX_VAR_NAME];
        strcpy(var_name, token_string);
        
        /* Check if User Function is defined */
        int is_fn = 0;
        int fn_idx = -1;
        if (strncmp(var_name, "FN", 2) == 0 && strlen(var_name) == 3) {
             fn_idx = toupper(var_name[2]) - 'A';
             if (fn_idx >= 0 && fn_idx <= 25 && user_functions[fn_idx].defined) {
                 is_fn = 1;
             }
        }
        
        next_token();
        
        if (current_token == TOK_LPAREN) {
            if (is_fn) {
                 /* Handle User Function Call */
                 next_token();
                 Value varg = expression();
                 if (!match(TOK_RPAREN)) error("Expected ')' for FN");
                 
                 /* Save Parser State */
                 char *save_token_ptr = token_ptr;
                 TokenType save_token = current_token;
                 double save_num = token_number;
                 char save_str[MAX_LINE_LEN];
                 strcpy(save_str, token_string);
                 
                 /* Save Arg Var */
                 Value old_val = get_var(user_functions[fn_idx].arg_name);
                 
                 /* Set Arg Var */
                 set_var(user_functions[fn_idx].arg_name, varg);
                 
                 /* Execute Expression */
                 init_tokenizer(user_functions[fn_idx].expr_text);
                 val = expression();
                 
                 /* Restore Arg Var */
                 set_var(user_functions[fn_idx].arg_name, old_val);
                 
                 /* Restore Parser State */
                 token_ptr = save_token_ptr;
                 current_token = save_token;
                 token_number = save_num;
                 strcpy(token_string, save_str);
            } else {
                /* Array access */
                int indices[MAX_DIMS];
                int dims = 0;
                
                next_token();
                do {
                    if (dims >= MAX_DIMS) error("Too many subscripts");
                    Value v = expression();
                    if (v.type != VAL_NUM) error("Array index must be number");
                    indices[dims++] = (int)v.num;
                } while (match(TOK_COMMA));

                if (!match(TOK_RPAREN)) error("Expected ')'");
                
                Value *ptr = get_array_ptr(var_name, dims, indices);
                if (ptr) {
                     val = *ptr;
                     if (val.type == VAL_STR && val.str) {
                         char *copy = malloc(strlen(val.str) + 1);
                         strcpy(copy, val.str);
                         val.str = copy;
                     }
                }
            }
        } else {
            /* Normal variable */
            val = get_var(var_name);
        }
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
    } else if (current_token == TOK_ASC) {
        next_token();
        if (!match(TOK_LPAREN)) error("Expected '(' for ASC");
        Value v = expression();
        if (v.type != VAL_STR) error("ASC expects string");
        val.type = VAL_NUM;
        val.num = (double)(unsigned char)v.str[0];
        free(v.str);
        if (!match(TOK_RPAREN)) error("Missing ')' for ASC");
    } else if (current_token == TOK_CHR) {
        next_token();
        if (!match(TOK_LPAREN)) error("Expected '(' for CHR$");
        Value v = expression();
        if (v.type != VAL_NUM) error("CHR$ expects number");
        val.type = VAL_STR;
        val.str = malloc(2);
        val.str[0] = (char)v.num;
        val.str[1] = '\0';
        if (!match(TOK_RPAREN)) error("Missing ')' for CHR$");
    } else if (current_token == TOK_VAL) {
        next_token();
        if (!match(TOK_LPAREN)) error("Expected '(' for VAL");
        Value v = expression();
        if (v.type != VAL_STR) error("VAL expects string");
        val.type = VAL_NUM;
        val.num = atof(v.str);
        free(v.str);
        if (!match(TOK_RPAREN)) error("Missing ')' for VAL");
    } else if (current_token == TOK_STR) {
        next_token();
        if (!match(TOK_LPAREN)) error("Expected '(' for STR$");
        Value v = expression();
        if (v.type != VAL_NUM) error("STR$ expects number");
        val.type = VAL_STR;
        val.str = malloc(64);
        if (v.num == (int)v.num) sprintf(val.str, "%d", (int)v.num);
        else sprintf(val.str, "%g", v.num);
        if (!match(TOK_RPAREN)) error("Missing ')' for STR$");
    } else if (current_token == TOK_MID) {
        next_token();
        if (!match(TOK_LPAREN)) error("Expected '(' for MID$");
        Value v = expression();
        if (v.type != VAL_STR) error("MID$ expects string");
        if (!match(TOK_COMMA)) error("Expected ',' for MID$");
        Value vstart = expression();
        if (vstart.type != VAL_NUM) error("MID$ start expects number");
        int start = (int)vstart.num;
        int len = -1;
        if (current_token == TOK_COMMA) {
             next_token();
             Value vlen = expression();
             if (vlen.type != VAL_NUM) error("MID$ length expects number");
             len = (int)vlen.num;
        }
        if (!match(TOK_RPAREN)) error("Missing ')' for MID$");
        
        val.type = VAL_STR;
        int slen = strlen(v.str);
        if (start < 1) start = 1; /* BASIC is 1-based usually */
        start--; /* 0-based for C */
        if (start >= slen) {
             val.str = malloc(1);
             val.str[0] = '\0';
        } else {
             if (len == -1 || start + len > slen) len = slen - start;
             if (len < 0) len = 0;
             val.str = malloc(len + 1);
             strncpy(val.str, v.str + start, len);
             val.str[len] = '\0';
        }
        free(v.str);
    } else if (current_token == TOK_LEFT) {
        next_token();
        if (!match(TOK_LPAREN)) error("Expected '(' for LEFT$");
        Value v = expression();
        if (v.type != VAL_STR) error("LEFT$ expects string");
        if (!match(TOK_COMMA)) error("Expected ',' for LEFT$");
        Value vlen = expression();
        if (vlen.type != VAL_NUM) error("LEFT$ length expects number");
        int len = (int)vlen.num;
        if (!match(TOK_RPAREN)) error("Missing ')' for LEFT$");
        
        val.type = VAL_STR;
        int slen = strlen(v.str);
        if (len > slen) len = slen;
        if (len < 0) len = 0;
        val.str = malloc(len+1);
        strncpy(val.str, v.str, len);
        val.str[len] = '\0';
        free(v.str);
    } else if (current_token == TOK_RIGHT) {
        next_token();
        if (!match(TOK_LPAREN)) error("Expected '(' for RIGHT$");
        Value v = expression();
        if (v.type != VAL_STR) error("RIGHT$ expects string");
        if (!match(TOK_COMMA)) error("Expected ',' for RIGHT$");
        Value vlen = expression();
        if (vlen.type != VAL_NUM) error("RIGHT$ length expects number");
        int len = (int)vlen.num;
        if (!match(TOK_RPAREN)) error("Missing ')' for RIGHT$");
        
        val.type = VAL_STR;
        int slen = strlen(v.str);
        if (len > slen) len = slen;
        if (len < 0) len = 0;
        val.str = malloc(len+1);
        strncpy(val.str, v.str + slen - len, len);
        val.str[len] = '\0';
        free(v.str);
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
            case TOK_RND: val.num = ((double)rand() / ((double)RAND_MAX + 1.0)); break;
            default: break;
        }
    } else {
        error("Expected number, variable, or function");
    }
    return val;
}
