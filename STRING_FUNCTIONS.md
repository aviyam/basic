# String Manipulation Functions in BASIC

This document provides detailed information about the string manipulation functions available in this BASIC interpreter: `LEFT$`, `RIGHT$`, and `MID$`.

## Overview

String manipulation functions allow you to extract portions of strings, which is essential for text processing, data parsing, and many other programming tasks. All three functions return a new string and do not modify the original string.

## Function Reference

### LEFT$(string, length)

Extracts characters from the left (beginning) of a string.

**Syntax:**
```basic
result$ = LEFT$(source_string$, number_of_characters)
```

**Parameters:**
- `source_string$`: The string to extract from
- `number_of_characters`: How many characters to extract from the left

**Examples:**
```basic
10 LET S$ = "HELLO WORLD"
20 PRINT LEFT$(S$, 5)    ' Prints "HELLO"
30 PRINT LEFT$(S$, 3)    ' Prints "HEL"
40 PRINT LEFT$(S$, 0)    ' Prints ""
50 PRINT LEFT$(S$, 20)   ' Prints "HELLO WORLD" (entire string)
```

**Notes:**
- If length is 0, returns an empty string
- If length is greater than the string length, returns the entire string
- If length is negative, returns an empty string

### RIGHT$(string, length)

Extracts characters from the right (end) of a string.

**Syntax:**
```basic
result$ = RIGHT$(source_string$, number_of_characters)
```

**Parameters:**
- `source_string$`: The string to extract from
- `number_of_characters`: How many characters to extract from the right

**Examples:**
```basic
10 LET S$ = "HELLO WORLD"
20 PRINT RIGHT$(S$, 5)   ' Prints "WORLD"
30 PRINT RIGHT$(S$, 3)   ' Prints "RLD"
40 PRINT RIGHT$(S$, 0)   ' Prints ""
50 PRINT RIGHT$(S$, 20)  ' Prints "HELLO WORLD" (entire string)
```

**Notes:**
- If length is 0, returns an empty string
- If length is greater than the string length, returns the entire string
- If length is negative, returns an empty string

### MID$(string, start [, length])

Extracts characters from the middle of a string, starting at a specified position.

**Syntax:**
```basic
result$ = MID$(source_string$, start_position)
result$ = MID$(source_string$, start_position, length)
```

**Parameters:**
- `source_string$`: The string to extract from
- `start_position`: Starting position (1-based indexing)
- `length`: (Optional) Number of characters to extract

**Examples:**
```basic
10 LET S$ = "HELLO WORLD"
20 PRINT MID$(S$, 3, 5)   ' Prints "LLO W"
30 PRINT MID$(S$, 7)      ' Prints "WORLD" (from position 7 to end)
40 PRINT MID$(S$, 1, 3)   ' Prints "HEL"
50 PRINT MID$(S$, 5, 0)   ' Prints ""
60 PRINT MID$(S$, 15)     ' Prints "" (start beyond string)
```

**Notes:**
- Uses 1-based indexing (first character is at position 1)
- If start position is beyond the string length, returns an empty string
- If length is omitted, extracts from start position to the end of the string
- If length is 0, returns an empty string
- If start + length exceeds the string length, extracts only available characters

## Practical Examples

### Name Processing
```basic
10 LET FULLNAME$ = "John Michael Smith"
20 REM Find first space
30 LET SPACE1 = 0
40 FOR I = 1 TO LEN(FULLNAME$)
50     IF MID$(FULLNAME$, I, 1) = " " THEN LET SPACE1 = I: GOTO 70
60 NEXT I
70 LET FIRSTNAME$ = LEFT$(FULLNAME$, SPACE1 - 1)
80 PRINT "First name: "; FIRSTNAME$
```

### File Extension Extraction
```basic
10 LET FILENAME$ = "document.txt"
20 REM Find last dot
30 LET DOTPOS = 0
40 FOR I = LEN(FILENAME$) TO 1 STEP -1
50     IF MID$(FILENAME$, I, 1) = "." THEN LET DOTPOS = I: GOTO 70
60 NEXT I
70 LET BASENAME$ = LEFT$(FILENAME$, DOTPOS - 1)
80 LET EXTENSION$ = RIGHT$(FILENAME$, LEN(FILENAME$) - DOTPOS)
90 PRINT "Base: "; BASENAME$; " Extension: "; EXTENSION$
```

### Phone Number Formatting
```basic
10 LET PHONE$ = "5551234567"
20 LET AREA$ = LEFT$(PHONE$, 3)
30 LET EXCHANGE$ = MID$(PHONE$, 4, 3)
40 LET NUMBER$ = RIGHT$(PHONE$, 4)
50 LET FORMATTED$ = "(" + AREA$ + ") " + EXCHANGE$ + "-" + NUMBER$
60 PRINT FORMATTED$  ' Prints "(555) 123-4567"
```

### Text Padding and Alignment
```basic
10 LET TEXT$ = "BASIC"
20 LET PADDING$ = "          "  ' 10 spaces
30 LET PADDED$ = PADDING$ + TEXT$
40 LET RIGHTALIGNED$ = RIGHT$(PADDED$, 10)
50 PRINT "'"; RIGHTALIGNED$; "'"  ' Right-aligned in 10 characters
```

### String Truncation
```basic
10 LET LONGTEXT$ = "This is a very long string that needs truncation"
20 LET MAXLEN = 20
30 IF LEN(LONGTEXT$) > MAXLEN THEN GOSUB 100
40 PRINT TRUNCATED$
50 END
100 LET TRUNCATED$ = LEFT$(LONGTEXT$, MAXLEN - 3) + "..."
110 RETURN
```

## Common Patterns

### Finding Character Positions
```basic
10 LET TEXT$ = "HELLO WORLD"
20 LET CHAR$ = " "
30 FOR I = 1 TO LEN(TEXT$)
40     IF MID$(TEXT$, I, 1) = CHAR$ THEN PRINT "Found at position "; I
50 NEXT I
```

### String Reversal
```basic
10 LET ORIGINAL$ = "HELLO"
20 LET REVERSED$ = ""
30 FOR I = LEN(ORIGINAL$) TO 1 STEP -1
40     LET REVERSED$ = REVERSED$ + MID$(ORIGINAL$, I, 1)
50 NEXT I
60 PRINT REVERSED$  ' Prints "OLLEH"
```

### Case-by-Case Processing
```basic
10 LET TEXT$ = "Hello World"
20 FOR I = 1 TO LEN(TEXT$)
30     LET CHAR$ = MID$(TEXT$, I, 1)
40     IF CHAR$ >= "A" AND CHAR$ <= "Z" THEN PRINT "Uppercase: "; CHAR$
50     IF CHAR$ >= "a" AND CHAR$ <= "z" THEN PRINT "Lowercase: "; CHAR$
60 NEXT I
```

## Tips and Best Practices

1. **Always check string lengths** before using these functions to avoid unexpected results
2. **Remember 1-based indexing** - the first character is at position 1, not 0
3. **Use LEN() function** to get string length for bounds checking
4. **Combine with other string functions** like ASC(), CHR$(), and VAL() for more complex operations
5. **Handle edge cases** like empty strings and out-of-bounds positions
6. **Use meaningful variable names** to make code more readable

## Error Handling

The functions handle common error conditions gracefully:
- Negative lengths return empty strings
- Positions beyond string length return empty strings
- Zero lengths return empty strings
- Out-of-bounds conditions are handled safely

## Performance Notes

- String functions create new strings, so avoid excessive string manipulation in tight loops
- For complex string processing, consider breaking operations into smaller steps
- Use appropriate buffer sizes when building strings incrementally

## See Also

- `LEN(string$)` - Returns the length of a string
- `ASC(string$)` - Returns ASCII value of first character
- `CHR$(number)` - Returns character from ASCII value
- `VAL(string$)` - Converts string to number
- `STR$(number)` - Converts number to string