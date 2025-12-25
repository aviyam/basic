# BASIC Interpreter Language Reference

This guide provides an overview of the commands and functions supported by the BASIC interpreter.

## System Commands

| Command | Description |
| ------- | ----------- |
| `RUN` | Executes the program currently in memory. |
| `LIST` | Lists the program lines currently in memory. |
| `NEW` | Clears the current program and variables from memory. |
| `BYE`, `EXIT`, `QUIT` | Exits the BASIC interpreter. |
| `SAVE "filename"` | Saves the current program to disk. |
| `LOAD "filename"` | Loads a program from disk. |
| `EDIT` | Opens the current program in the system's external editor (e.g., nano, notepad). |
| `CLS` | Clears the terminal screen. |

## Input / Output

### PRINT
Outputs text or numbers to the console.
- **Syntax**: `PRINT [expression] [, or ;]`
- **Example**: 
```basic
10 PRINT "Hello"; " "; "World"
20 PRINT 10 * 5
```

### INPUT
Prompts the user to enter values for variables.
- **Syntax**: `INPUT ["Prompt";] var1, var2, ...`
- **Example**:
```basic
30 INPUT "Enter coordinates (x,y)"; X, Y
40 INPUT "What is your name"; N$
```

### SLEEP
Pauses execution for a specified duration (in 60Hz ticks).
- **Syntax**: `SLEEP ticks`
- **Example**:
```basic
50 SLEEP 60  ' Wait for 1 second
```

## Variables and Arrays

### LET
Assigns a value to a variable. The `LET` keyword is optional.
- **Syntax**: `[LET] variable = expression`
- **Example**:
```basic
60 LET A = 100
70 B$ = "BASIC"
```

### DIM
Declares arrays with specified dimensions. Arrays are 0-indexed.
- **Syntax**: `DIM variable(size1, size2, ...)`
- **Example**:
```basic
80 DIM A(10)      ' 1D Array size 11 (0-10)
90 DIM M(5, 5)    ' 2D Array 6x6
```

## Control Flow

### IF ... THEN / GOTO
Conditional execution.
- **Syntax**: `IF condition THEN statement` or `IF condition GOTO linenumber`
- **Example**:
```basic
100 IF A > 10 THEN PRINT "Greater"
110 IF A < 0 GOTO 200
```
Supports `AND`, `OR`, `NOT` operators.

### GOTO
Unconditional jump to a line number.
- **Syntax**: `GOTO linenumber`
- **Example**: `120 GOTO 10`

### ON ... GOTO
Computed jump based on the value of an expression.
- **Syntax**: `ON expression GOTO line1, line2, ...`
- **Example**:
```basic
130 ON X GOTO 100, 200, 300
' If X=1 goto 100, if X=2 goto 200...
```

### FOR ... TO ... STEP ... NEXT
Looping construct.
- **Syntax**: `FOR var = start TO end [STEP step]` ... `NEXT [var]`
- **Example**:
```basic
140 FOR I = 1 TO 10 STEP 2
150 PRINT I
160 NEXT I
```

### GOSUB ... RETURN
Jump to a subroutine and return.
- **Syntax**: `GOSUB linenumber` ... `RETURN`
- **Example**:
```basic
170 GOSUB 500
180 END
500 PRINT "In Subroutine"
510 RETURN
```

### DEF FN
Defines a single-line user function.
- **Syntax**: `DEF FN<Letter>(arg) = expression`
- **Example**:
```basic
190 DEF FNA(R) = 3.14159 * R * R
200 PRINT FNA(5)
```

### STOP / END
Stops program execution.
- **Example**: `210 END`

## Data Storage

### DATA, READ, RESTORE
Used to store and retrieve static data within the program.
- **Syntax**: 
  - `DATA val1, val2, ...`
  - `READ var1, var2, ...`
  - `RESTORE [linenumber]`
- **Example**:
```basic
220 DATA 10, 20, 30
230 READ A, B, C
240 RESTORE
250 READ D
```

## Math Functions

| Function | Description | Example |
| -------- | ----------- | ------- |
| `SIN(X)` | Sine of X (radians) | `y = SIN(3.14)` |
| `COS(X)` | Cosine of X (radians) | `y = COS(0)` |
| `TAN(X)` | Tangent of X (radians) | `y = TAN(1)` |
| `ATN(X)` | Arctangent of X | `y = ATN(1)` |
| `EXP(X)` | e raised to power X | `y = EXP(1)` |
| `LOG(X)` | Natural logarithm of X | `y = LOG(10)` |
| `SQR(X)` | Square root of X | `y = SQR(16)` |
| `INT(X)` | Integer part of X | `y = INT(3.9)` |
| `ABS(X)` | Absolute value of X | `y = ABS(-5)` |
| `SGN(X)` | Sign of X (-1, 0, 1) | `y = SGN(-5)` |
| `RND(X)` | Random number in [0, 1) | `y = RND(1)` |

## String Functions

| Function | Description | Example |
| -------- | ----------- | ------- |
| `LEN(S$)` | Length of string | `PRINT LEN("ABC")` |
| `ASC(S$)` | ASCII code of first char | `PRINT ASC("A")` |
| `CHR$(N)` | Character from ASCII code | `PRINT CHR$(65)` |
| `MID$(S$, start [,len])` | Substring | `PRINT MID$("HELLO", 2, 2)` |
| `LEFT$(S$, N)` | First N characters | `PRINT LEFT$("HELLO", 2)` |
| `RIGHT$(S$, N)` | Last N characters | `PRINT RIGHT$("HELLO", 2)` |
| `VAL(S$)` | Converts string to number | `N = VAL("123")` |
| `STR$(N)` | Converts number to string | `S$ = STR$(123)` |
| `INKEY$` | Reads a single keypress | `K$ = INKEY$` |

## Operators
- **Math**: `+`, `-`, `*`, `/`, `%` (Modulus)
- **Comparison**: `=`, `<>`, `<`, `>`, `<=`, `>=`
- **Logic**: `AND`, `OR`, `NOT`
