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

void create_array(const char *name, int dims, int *sizes) {
    int i;
    for (i = 0; i < array_count; i++) {
        if (strcmp(arrays[i].name, name) == 0) {
            error("Array already defined");
        }
    }
    
    if (array_count < MAX_ARRAYS) {
        int total_size = 1;
        int d;
        
        arrays[array_count].dims = dims;
        for (d = 0; d < dims; d++) {
            arrays[array_count].dim_sizes[d] = sizes[d];
            total_size *= sizes[d];
        }
        arrays[array_count].total_size = total_size;
        
        arrays[array_count].data = (Value *)calloc(total_size, sizeof(Value));
        if (!arrays[array_count].data) error("Out of memory for array");
        
        strcpy(arrays[array_count].name, name);
        
        if (strchr(name, '$')) {
             arrays[array_count].type = VAL_STR;
             /* Initialized to 0/NULL by calloc */
        } else {
             arrays[array_count].type = VAL_NUM;
        }
        
        array_count++;
    } else {
        error("Too many arrays");
    }
}

Value *get_array_ptr(const char *name, int dims, int *indices) {
    int i;
    for (i = 0; i < array_count; i++) {
        if (strcmp(arrays[i].name, name) == 0) {
            if (arrays[i].dims != dims) {
                error("Incorrect number of subscripts");
            }
            
            int offset = 0;
            int d;
            for (d = 0; d < dims; d++) {
                if (indices[d] < 0 || indices[d] >= arrays[i].dim_sizes[d]) {
                    error("Array index out of bounds");
                }
                /* Stride calculation:
                   For 2D: [x][y] -> x * size_y + y
                   For 3D: [x][y][z] -> (x * size_y + y) * size_z + z
                   General: offset = offset * next_dim_size + next_index
                   Wait, "next_dim_size" isn't correct in loop order.
                   Common C way: offset = offset * dim_size[d] + indices[d] ?
                   No. 
                   dim_sizes[0] = X_SIZE, dim_sizes[1] = Y_SIZE.
                   A[i][j].
                   Offset = i * Y_SIZE + j.
                   
                   Generalized:
                   offset = 0
                   for d = 0 to dims-1:
                       offset = offset * (dim_sizes[d]???) + indices[d]
                       
                   No, stride for dimension d is product of sizes[d+1...end].
                   
                   Let's use the iterative approach:
                   offset = indices[0]
                   for d = 1 to dims-1:
                       offset = offset * arrays[i].dim_sizes[d] + indices[d]
                */
                if (d == 0) offset = indices[d];
                else offset = offset * arrays[i].dim_sizes[d] + indices[d];
            }
            
            return &arrays[i].data[offset];
        }
    }
    error("Array not defined");
    return NULL; 
}
