10 REM Comprehensive String Function Demonstration
20 REM Shows MID$, LEFT$, and RIGHT$ functions with examples
30 CLS
40 PRINT "======================================================"
50 PRINT "      COMPLETE STRING FUNCTION DEMONSTRATION"
60 PRINT "======================================================"
70 PRINT
80 PRINT "This program demonstrates all three string functions:"
90 PRINT "LEFT$, RIGHT$, and MID$ with practical examples."
100 PRINT
110 REM Basic function demonstration
120 PRINT "BASIC FUNCTION EXAMPLES:"
130 PRINT "========================"
140 LET DEMO$ = "PROGRAMMING"
150 PRINT "Working with string: "; CHR$(34); DEMO$; CHR$(34)
160 PRINT "String length: "; LEN(DEMO$)
170 PRINT
180 PRINT "LEFT$ function (get characters from left):"
190 FOR I = 1 TO 5
200     PRINT "LEFT$("; CHR$(34); DEMO$; CHR$(34); ", "; I; ") = "; CHR$(34); LEFT$(DEMO$, I); CHR$(34)
210 NEXT I
220 PRINT
230 PRINT "RIGHT$ function (get characters from right):"
240 FOR I = 1 TO 5
250     PRINT "RIGHT$("; CHR$(34); DEMO$; CHR$(34); ", "; I; ") = "; CHR$(34); RIGHT$(DEMO$, I); CHR$(34)
260 NEXT I
270 PRINT
280 PRINT "MID$ function (get characters from middle):"
290 PRINT "MID$("; CHR$(34); DEMO$; CHR$(34); ", 3, 4) = "; CHR$(34); MID$(DEMO$, 3, 4); CHR$(34)
300 PRINT "MID$("; CHR$(34); DEMO$; CHR$(34); ", 5) = "; CHR$(34); MID$(DEMO$, 5); CHR$(34)
310 PRINT "MID$("; CHR$(34); DEMO$; CHR$(34); ", 1, 1) = "; CHR$(34); MID$(DEMO$, 1, 1); CHR$(34)
320 PRINT
330 PRINT "Press any key to continue to practical examples..."
340 LET K$ = INKEY$: IF K$ = "" THEN GOTO 340
350 CLS
360 REM Practical examples
370 PRINT "PRACTICAL EXAMPLES:"
380 PRINT "=================="
390 PRINT
400 REM Example 1: Name parsing
410 PRINT "1. Name Parsing:"
420 LET NAME$ = "Alice Jane Doe"
430 PRINT "Full name: "; CHR$(34); NAME$; CHR$(34)
440 GOSUB 800
450 PRINT "First name: "; CHR$(34); FIRST$; CHR$(34)
460 PRINT "Middle name: "; CHR$(34); MIDDLE$; CHR$(34)
470 PRINT "Last name: "; CHR$(34); LAST$; CHR$(34)
480 PRINT
490 REM Example 2: Email processing
500 PRINT "2. Email Processing:"
510 LET EMAIL$ = "user" + CHR$(64) + "company.com"
520 PRINT "Email: "; CHR$(34); EMAIL$; CHR$(34)
530 GOSUB 900
540 PRINT "Username: "; CHR$(34); USERNAME$; CHR$(34)
550 PRINT "Domain: "; CHR$(34); DOMAIN$; CHR$(34)
560 PRINT
570 REM Example 3: File path processing
580 PRINT "3. File Path Processing:"
590 LET FILEPATH$ = "documents" + CHR$(47) + "report.pdf"
600 PRINT "File path: "; CHR$(34); FILEPATH$; CHR$(34)
610 GOSUB 1000
620 PRINT "Directory: "; CHR$(34); DIR$; CHR$(34)
630 PRINT "Filename: "; CHR$(34); FILENAME$; CHR$(34)
640 PRINT "Extension: "; CHR$(34); EXT$; CHR$(34)
650 PRINT
660 REM Example 4: Data formatting
670 PRINT "4. Data Formatting:"
680 LET PHONE$ = "5551234567"
690 PRINT "Raw phone: "; CHR$(34); PHONE$; CHR$(34)
700 GOSUB 1100
710 PRINT "Formatted: "; CHR$(34); FORMATTED$; CHR$(34)
720 PRINT
730 PRINT "5. Text Processing:"
740 LET TEXT$ = "The quick brown fox"
750 PRINT "Original: "; CHR$(34); TEXT$; CHR$(34)
760 GOSUB 1200
770 PRINT "Reversed: "; CHR$(34); REVERSED$; CHR$(34)
780 PRINT
790 GOTO 1300
800 REM Parse name subroutine
810 LET SPACE1 = 0: LET SPACE2 = 0
820 FOR I = 1 TO LEN(NAME$)
830     IF MID$(NAME$, I, 1) = " " AND SPACE1 = 0 THEN LET SPACE1 = I: GOTO 850
840     IF MID$(NAME$, I, 1) = " " AND SPACE1 > 0 THEN LET SPACE2 = I: GOTO 860
850 NEXT I
860 LET FIRST$ = LEFT$(NAME$, SPACE1 - 1)
870 IF SPACE2 > 0 THEN LET MIDDLE$ = MID$(NAME$, SPACE1 + 1, SPACE2 - SPACE1 - 1): LET LAST$ = RIGHT$(NAME$, LEN(NAME$) - SPACE2)
880 IF SPACE2 = 0 THEN LET MIDDLE$ = "": LET LAST$ = RIGHT$(NAME$, LEN(NAME$) - SPACE1)
890 RETURN
900 REM Parse email subroutine
910 LET ATPOS = 0
920 FOR I = 1 TO LEN(EMAIL$)
930     IF MID$(EMAIL$, I, 1) = CHR$(64) THEN LET ATPOS = I: GOTO 950
940 NEXT I
950 LET USERNAME$ = LEFT$(EMAIL$, ATPOS - 1)
960 LET DOMAIN$ = RIGHT$(EMAIL$, LEN(EMAIL$) - ATPOS)
970 RETURN
980 REM Parse file path subroutine
1000 LET SLASHPOS = 0: LET DOTPOS = 0
1010 FOR I = 1 TO LEN(FILEPATH$)
1020     IF MID$(FILEPATH$, I, 1) = CHR$(47) THEN LET SLASHPOS = I
1030 NEXT I
1040 FOR I = LEN(FILEPATH$) TO 1 STEP -1
1050     IF MID$(FILEPATH$, I, 1) = "." THEN LET DOTPOS = I: GOTO 1070
1060 NEXT I
1070 LET DIR$ = LEFT$(FILEPATH$, SLASHPOS - 1)
1080 LET FILENAME$ = MID$(FILEPATH$, SLASHPOS + 1, DOTPOS - SLASHPOS - 1)
1090 LET EXT$ = RIGHT$(FILEPATH$, LEN(FILEPATH$) - DOTPOS)
1100 RETURN
1110 REM Format phone number subroutine
1120 LET AREA$ = LEFT$(PHONE$, 3)
1130 LET EXCHANGE$ = MID$(PHONE$, 4, 3)
1140 LET NUMBER$ = RIGHT$(PHONE$, 4)
1150 LET FORMATTED$ = "(" + AREA$ + ") " + EXCHANGE$ + "-" + NUMBER$
1160 RETURN
1170 REM Reverse string subroutine
1180 REM String reversal using MID$
1200 LET REVERSED$ = ""
1210 FOR I = LEN(TEXT$) TO 1 STEP -1
1220     LET REVERSED$ = REVERSED$ + MID$(TEXT$, I, 1)
1230 NEXT I
1240 RETURN
1250 REM Final demonstration
1300 PRINT "INTERACTIVE TEST:"
1310 PRINT "================"
1320 INPUT "Enter a string to test: "; TEST$
1330 IF TEST$ = "" THEN GOTO 1450
1340 PRINT
1350 PRINT "Your string: "; CHR$(34); TEST$; CHR$(34)
1360 PRINT "Length: "; LEN(TEST$)
1370 PRINT
1380 INPUT "How many chars from LEFT? "; L
1390 PRINT "LEFT$("; CHR$(34); TEST$; CHR$(34); ", "; L; ") = "; CHR$(34); LEFT$(TEST$, L); CHR$(34)
1400 INPUT "How many chars from RIGHT? "; R
1410 PRINT "RIGHT$("; CHR$(34); TEST$; CHR$(34); ", "; R; ") = "; CHR$(34); RIGHT$(TEST$, R); CHR$(34)
1420 INPUT "MID$ start position? "; S
1430 INPUT "MID$ length (0 for rest)? "; ML
1440 IF ML = 0 THEN PRINT "MID$("; CHR$(34); TEST$; CHR$(34); ", "; S; ") = "; CHR$(34); MID$(TEST$, S); CHR$(34) ELSE PRINT "MID$("; CHR$(34); TEST$; CHR$(34); ", "; S; ", "; ML; ") = "; CHR$(34); MID$(TEST$, S, ML); CHR$(34)
1450 PRINT
1460 INPUT "Test another string? (Y/N) "; AGAIN$
1470 IF LEFT$(AGAIN$, 1) = "Y" OR LEFT$(AGAIN$, 1) = "y" THEN GOTO 1320
1480 PRINT
1490 PRINT "SUMMARY:"
1500 PRINT "======="
1510 PRINT "LEFT$(string, n)    - Gets first n characters"
1520 PRINT "RIGHT$(string, n)   - Gets last n characters"
1530 PRINT "MID$(string, pos)   - Gets from position to end"
1540 PRINT "MID$(string, pos, n)- Gets n chars from position"
1550 PRINT
1560 PRINT "These functions are essential for:"
1570 PRINT "- Text parsing and processing"
1580 PRINT "- Data validation and extraction"
1590 PRINT "- String formatting and manipulation"
1600 PRINT "- File and path processing"
1610 PRINT
1620 PRINT "Thank you for exploring string functions!"
1630 END
