# Tiny BASIC Interpreter

A lightweight, efficient BASIC interpreter written in C99. Optimized for small binary size (~36KB) and compatibility with MS-DOS BASIC.
Written for fun and learning purposes, by Gemini AI with guidance (and a few tweaks) from me.

Tested with programs from the [BASIC Computer Games](http://www.vintage-basic.net/games.html) book. All BCG files are in the examples/bcg directory.

## Build

```bash
make
```

## Usage
You can either run it on its own (to enter interactive mode), or pass a file to it to start running.


## Features

- **Variables**: Numeric (`A=10`) and String (`A$="Hello"`)
- **Arrays**: 1D Numeric and String arrays implicitly or via `DIM` (e.g. `DIM A(10)`)
- **Control Flow**: `IF/THEN/GOTO`, `FOR/NEXT`, `GOSUB/RETURN`
- **Math**: `SIN`, `COS`, `TAN`, `LOG`, `EXP`, `SQR`, `INT`, `ABS`, `RND`
- **String**: `LEN`, `ASC`, `CHR$`, `MID$`, `LEFT$`, `RIGHT$`, `VAL`, `STR$`, `INKEY$`
- **I/O**: `PRINT`, `INPUT`, `CLS`, `SLEEP`
- **Data**: `DATA`, `READ`, `RESTORE`
- **Management**: `LIST`, `RUN`, `NEW`, `SAVE`, `LOAD`, `EDIT`

Note: File operations (`SAVE`, `LOAD`, `EDIT`, CLI argument) automatically append `.bas` if no extension is provided.
Arrays must be declared with `DIM`. Indices are 0-based.

## Example

```basic
10 PRINT "Square Number Table"
20 FOR I = 1 TO 5
30 PRINT I, I*I
40 NEXT I
```
