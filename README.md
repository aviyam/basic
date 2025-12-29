# BASIC Interpreter

A lightweight, efficient BASIC interpreter written in C99. Optimized for compatibility with MS-DOS BASIC.
Written for fun and learning purposes, by Gemini AI with guidance (and a few tweaks) from me.

Tested with programs from the [BASIC Computer Games](http://www.vintage-basic.net/games.html) book. All BCG files are in the examples/bcg directory.

## Build
1. Download source 
2. make sure you have ```gcc``` and ```make``` installed
3. run ```make```
binary will be built in the bin directory

## Usage
You can either run it on its own (to enter interactive mode), or pass a file to it to start running.

## Language Reference
For a complete list of commands, functions, and features, please see the [Language Reference](LANGUAGE_REFERENCE.md).

## Notes
- File operations (`SAVE`, `LOAD`, `EDIT`, CLI argument) automatically append `.bas` if no extension is provided.
- Arrays must be declared with `DIM`. Indices are 0-based.
- Shebang support allows running scripts directly (e.g. `#!/usr/local/bin/basic`).
