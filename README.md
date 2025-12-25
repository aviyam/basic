# Tiny BASIC Interpreter

A lightweight, efficient BASIC interpreter written in C89. Optimized for small binary size (~36KB).
Written for fun and learning purposes, by Gemini AI with guidance (and a few tweaks) from me.

## Build

```bash
make
```

## Usage
You can either run it on its own, or pass a file to it.

## Features

- **Variables**: Numeric (`A=10`) and String (`A$="Hello"`)
- **Control Flow**: `IF/THEN/GOTO`, `FOR/NEXT`, `GOSUB/RETURN`
- **Math**: `SIN`, `COS`, `TAN`, `LOG`, `EXP`, `SQR`, `INT`, `ABS`, `RND`
- **I/O**: `PRINT`, `INPUT`, `INKEY$`, `CLS`, `SLEEP`
- **Management**: `LIST`, `RUN`, `NEW`, `SAVE`, `LOAD`, `EDIT`

## Example

```basic
10 PRINT "Square Number Table"
20 FOR I = 1 TO 5
30 PRINT I, I*I
40 NEXT I
```
