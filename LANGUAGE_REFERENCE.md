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

### Basic String Functions
| Function | Description | Example |
| -------- | ----------- | ------- |
| `LEN(S$)` | Length of string | `PRINT LEN("ABC")` returns 3 |
| `ASC(S$)` | ASCII code of first char | `PRINT ASC("A")` returns 65 |
| `CHR$(N)` | Character from ASCII code | `PRINT CHR$(65)` returns "A" |
| `VAL(S$)` | Converts string to number | `N = VAL("123")` returns 123 |
| `STR$(N)` | Converts number to string | `S$ = STR$(123)` returns "123" |
| `INKEY$` | Reads a single keypress | `K$ = INKEY$` |

### String Manipulation Functions

#### LEFT$(string, length)
Extracts characters from the left (beginning) of a string.
- **Syntax**: `LEFT$(source_string$, number_of_characters)`
- **Returns**: A new string containing the first N characters
- **Parameters**:
  - `source_string$`: The string to extract from
  - `number_of_characters`: How many characters to extract (must be >= 0)
- **Examples**:
```basic
10 LET S$ = "PROGRAMMING"
20 PRINT LEFT$(S$, 4)     ' Prints "PROG"
30 PRINT LEFT$(S$, 0)     ' Prints ""
40 PRINT LEFT$(S$, 20)    ' Prints "PROGRAMMING" (full string)
```
- **Notes**: 
  - If length is 0, returns empty string
  - If length exceeds string length, returns entire string
  - If length is negative, returns empty string

#### RIGHT$(string, length)
Extracts characters from the right (end) of a string.
- **Syntax**: `RIGHT$(source_string$, number_of_characters)`
- **Returns**: A new string containing the last N characters
- **Parameters**:
  - `source_string$`: The string to extract from
  - `number_of_characters`: How many characters to extract (must be >= 0)
- **Examples**:
```basic
10 LET S$ = "PROGRAMMING"
20 PRINT RIGHT$(S$, 4)    ' Prints "MING"
30 PRINT RIGHT$(S$, 0)    ' Prints ""
40 PRINT RIGHT$(S$, 20)   ' Prints "PROGRAMMING" (full string)
```
- **Notes**: 
  - If length is 0, returns empty string
  - If length exceeds string length, returns entire string
  - If length is negative, returns empty string

#### MID$(string, start [, length])
Extracts characters from any position within a string.
- **Syntax**: 
  - `MID$(source_string$, start_position)`
  - `MID$(source_string$, start_position, length)`
- **Returns**: A new string containing the specified substring
- **Parameters**:
  - `source_string$`: The string to extract from
  - `start_position`: Starting position (1-based indexing, first char is position 1)
  - `length`: (Optional) Number of characters to extract
- **Examples**:
```basic
10 LET S$ = "PROGRAMMING"
20 PRINT MID$(S$, 3, 4)   ' Prints "OGRA"
30 PRINT MID$(S$, 5)      ' Prints "RAMMING" (from pos 5 to end)
40 PRINT MID$(S$, 1, 1)   ' Prints "P"
50 PRINT MID$(S$, 15)     ' Prints "" (start beyond string)
```
- **Notes**: 
  - Uses 1-based indexing (first character is at position 1)
  - If start position is beyond string length, returns empty string
  - If length is omitted, extracts from start position to end of string
  - If length is 0, returns empty string
  - If start + length exceeds string length, extracts only available characters

### String Manipulation Examples

#### Name Parsing
```basic
10 LET FULLNAME$ = "John Michael Smith"
20 REM Find first space
30 FOR I = 1 TO LEN(FULLNAME$)
40     IF MID$(FULLNAME$, I, 1) = " " THEN LET SPACE = I: GOTO 60
50 NEXT I
60 LET FIRST$ = LEFT$(FULLNAME$, SPACE - 1)
70 LET LAST$ = RIGHT$(FULLNAME$, LEN(FULLNAME$) - SPACE)
80 PRINT "First: "; FIRST$; " Last: "; LAST$
```

#### File Extension Processing
```basic
10 LET FILENAME$ = "document.txt"
20 REM Find last dot
30 FOR I = LEN(FILENAME$) TO 1 STEP -1
40     IF MID$(FILENAME$, I, 1) = "." THEN LET DOT = I: GOTO 60
50 NEXT I
60 LET BASENAME$ = LEFT$(FILENAME$, DOT - 1)
70 LET EXTENSION$ = RIGHT$(FILENAME$, LEN(FILENAME$) - DOT)
80 PRINT "Base: "; BASENAME$; " Ext: "; EXTENSION$
```

#### Phone Number Formatting
```basic
10 LET PHONE$ = "5551234567"
20 LET AREA$ = LEFT$(PHONE$, 3)
30 LET EXCHANGE$ = MID$(PHONE$, 4, 3)
40 LET NUMBER$ = RIGHT$(PHONE$, 4)
50 LET FORMATTED$ = "(" + AREA$ + ") " + EXCHANGE$ + "-" + NUMBER$
60 PRINT FORMATTED$  ' Prints "(555) 123-4567"
```

#### String Reversal
```basic
10 LET ORIGINAL$ = "HELLO"
20 LET REVERSED$ = ""
30 FOR I = LEN(ORIGINAL$) TO 1 STEP -1
40     LET REVERSED$ = REVERSED$ + MID$(ORIGINAL$, I, 1)
50 NEXT I
60 PRINT REVERSED$  ' Prints "OLLEH"
```

### String Manipulation Best Practices

#### Common Patterns

**Finding Character Positions**
```basic
10 LET TEXT$ = "HELLO WORLD"
20 LET CHAR$ = " "
30 FOR I = 1 TO LEN(TEXT$)
40     IF MID$(TEXT$, I, 1) = CHAR$ THEN PRINT "Found at position "; I
50 NEXT I
```

**Validating String Format**
```basic
10 LET EMAIL$ = "user@domain.com"
20 LET VALID = 0
30 FOR I = 2 TO LEN(EMAIL$) - 1
40     IF MID$(EMAIL$, I, 1) = "@" THEN LET VALID = 1: GOTO 60
50 NEXT I
60 IF VALID THEN PRINT "Valid email format" ELSE PRINT "Invalid format"
```

**Text Padding and Alignment**
```basic
10 LET TEXT$ = "BASIC"
20 LET WIDTH = 10
30 LET SPACES$ = "          "  ' 10 spaces
40 LET PADDED$ = SPACES$ + TEXT$
50 LET RIGHT_ALIGNED$ = RIGHT$(PADDED$, WIDTH)
60 PRINT "'"; RIGHT_ALIGNED$; "'"
```

**String Truncation with Ellipsis**
```basic
10 LET LONGTEXT$ = "This is a very long string"
20 LET MAXLEN = 15
30 IF LEN(LONGTEXT$) > MAXLEN THEN GOSUB 100
40 PRINT TRUNCATED$
50 END
100 LET TRUNCATED$ = LEFT$(LONGTEXT$, MAXLEN - 3) + "..."
110 RETURN
```

#### Tips and Guidelines

1. **Always validate string lengths** before using manipulation functions
2. **Remember 1-based indexing** - first character is at position 1
3. **Use `LEN()` function** to get string length for bounds checking
4. **Handle edge cases** like empty strings and out-of-bounds positions
5. **Combine functions** for complex operations (e.g., `LEFT$` + `MID$` + `RIGHT$`)
6. **Use meaningful variable names** for clarity (e.g., `FIRSTNAME$` vs `F$`)

#### Error Prevention

- Check for empty strings before processing
- Validate position parameters are within valid range (1 to LEN(string))
- Test with edge cases like single-character strings
- Use conditional logic to handle optional string parts

#### Performance Considerations

- Minimize string concatenation in loops
- Store frequently used string lengths in variables
- Break complex operations into smaller, readable steps
- Use appropriate string sizes to avoid unnecessary memory usage

## Operators
- **Math**: `+`, `-`, `*`, `/`, `%` (Modulus)
- **Comparison**: `=`, `<>`, `<`, `>`, `<=`, `>=`
- **Logic**: `AND`, `OR`, `NOT`
