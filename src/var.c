#include "bas.h"

Value get_var(const char *name) {
    int i;
    for (i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            /* Duplicate string if it is one, to avoid double free issues if caller frees it? 
               Usually caller reads. But we are returning by value. 
               The 'str' pointer in Value points to heap. 
               We should probably return a COPY of the string if we want to be safe, 
               or assume the variable owns the string and we return a reference (dangerous if var changes).
               For simplicity in this toy interpreter: return COPY. 
            */
            Value v = variables[i].val;
            if (v.type == VAL_STR && v.str) {
                /* We must return a new copy because expressions might free it?
                   Actually, if we return the same pointer, and the user does 'A$ = A$ + "x"', 
                   the expression evaluator might free the old A$ string?
                   Let's stick to rule: Values in variables own their strings.
                   Values returned by expressions own their strings (temporaries).
                   So get_var should return a copy.
                */
                char *copy = malloc(strlen(v.str) + 1);
                strcpy(copy, v.str);
                v.str = copy;
            }
            return v;
        }
    }
    
    /* Default value */
    {
        Value v;
        if (strchr(name, '$')) {
            v.type = VAL_STR;
            v.str = malloc(1);
            *v.str = '\0';
        } else {
            v.type = VAL_NUM;
            v.num = 0.0;
        }
        return v;
    }
}

void set_var(const char *name, Value val) {
    int i;
    
    /* Check type match */
    int is_str_var = (strchr(name, '$') != NULL);
    if (is_str_var && val.type != VAL_STR) {
        error("Type mismatch: Expected string");
    }
    if (!is_str_var && val.type != VAL_NUM) {
        error("Type mismatch: Expected number");
    }

    for (i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            /* Free old string if exists */
            if (variables[i].val.type == VAL_STR && variables[i].val.str) {
                free(variables[i].val.str);
            }
            variables[i].val = val;
            return;
        }
    }
    if (var_count < 100) {
        strcpy(variables[var_count].name, name);
        variables[var_count].val = val;
        var_count++;
    } else {
        error("Too many variables");
    }
}
