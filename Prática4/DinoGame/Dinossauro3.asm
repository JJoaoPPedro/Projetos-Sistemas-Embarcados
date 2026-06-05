; =====================================================================
; T-REX GAME - EDSIM51
; LCD 16x2 - HD44780 - MODO 4 BITS
; =====================================================================

ORG 0000H
LJMP START

; =====================================================================
; LCD
; =====================================================================

RS EQU P1.3
EN EQU P1.2

; =====================================================================
; RAM
; =====================================================================

SCORE_L EQU 30H
SPEED   EQU 31H
DINO_Y  EQU 32H
OBST_X  EQU 33H
OBST_T  EQU 34H
RANDOM  EQU 35H

; =====================================================================
; START
; =====================================================================

ORG 0030H

START:

    MOV SP,#60H

    ; LCD

    CLR RS

    ACALL FuncS
    ACALL DispC
    ACALL EntryM

    ; VARIÁVEIS

    MOV SCORE_L,#00H

    MOV SPEED,#30

    ; 1 = chão
    ; 0 = pulando

    MOV DINO_Y,#01H

    MOV OBST_X,#15

    ; 0 = cacto
    ; 1 = pássaro

    MOV OBST_T,#00H

    MOV RANDOM,#07H

; =====================================================================
; LOOP PRINCIPAL
; =====================================================================

GAME_LOOP:

    ACALL SCAN_KEY

    ACALL UPDATE_GAME

    ACALL DRAW_FRAME

    ACALL DELAY_GAME

    SJMP GAME_LOOP

; =====================================================================
; UPDATE GAME
; =====================================================================

UPDATE_GAME:

    INC RANDOM

    DEC OBST_X

    MOV A,OBST_X

    CJNE A,#0FFH,CHECK_HIT

    ; RESET OBSTÁCULO

    MOV OBST_X,#15

    INC SCORE_L

    ; RANDOMIZA

    MOV A,RANDOM

    ANL A,#01H

    MOV OBST_T,A

    ; VELOCIDADE

    MOV A,SCORE_L

    MOV B,#50

    DIV AB

    MOV A,B

    JNZ CHECK_HIT

    MOV A,SPEED

    CJNE A,#05,SPD_OK

    SJMP CHECK_HIT

SPD_OK:

    DEC SPEED

; =====================================================================
; COLISÃO
; =====================================================================

CHECK_HIT:

    MOV A,OBST_X

    JNZ NO_HIT

    MOV A,OBST_T

    XRL A,DINO_Y

    JNZ NO_HIT

    ACALL GAME_OVER

NO_HIT:

    RET

; =====================================================================
; GAME OVER
; =====================================================================

GAME_OVER:

    ; limpa display

    CLR RS

    MOV A,#01H
    ACALL SendC

    ACALL DELAY_LONG

    ; linha 1

    MOV A,#80H
    ACALL LCD_CMD

    ; GAME OVER

    MOV A,#'G'
    ACALL LCD_DATA

    MOV A,#'A'
    ACALL LCD_DATA

    MOV A,#'M'
    ACALL LCD_DATA

    MOV A,#'E'
    ACALL LCD_DATA

    MOV A,#' '
    ACALL LCD_DATA

    MOV A,#'O'
    ACALL LCD_DATA

    MOV A,#'V'
    ACALL LCD_DATA

    MOV A,#'E'
    ACALL LCD_DATA

    MOV A,#'R'
    ACALL LCD_DATA

STOP:

    SJMP STOP

; =====================================================================
; TECLADO
; TECLA 8 = PULO
; =====================================================================

SCAN_KEY:

    ; CONFIGURA TECLADO

    MOV P0,#0FFH

    ; ATIVA LINHA 2
    ; linha da tecla 8

    CLR P0.1

    ; coluna da tecla 8 = P0.5

    JB P0.5,NO_JUMP

    ; PULO

    MOV DINO_Y,#00H

    RET

NO_JUMP:

    ; CHÃO

    MOV DINO_Y,#01H

    RET

; =====================================================================
; DESENHA FRAME
; =====================================================================

DRAW_FRAME:

; =====================================================================
; LINHA 1
; =====================================================================

    MOV A,#80H
    ACALL LCD_CMD

    MOV R0,#00

LINE1_LOOP:

    MOV A,R0

    CJNE A,#00,CHK_BIRD

    ; DINO NO AR

    MOV A,DINO_Y

    JZ DRAW_DINO_UP

    MOV A,#' '

    SJMP SEND_L1

DRAW_DINO_UP:

    MOV A,#'D'

    SJMP SEND_L1

CHK_BIRD:

    MOV A,R0

    CJNE A,OBST_X,EMPTY_SKY

    MOV A,OBST_T

    JNZ DRAW_BIRD

EMPTY_SKY:

    MOV A,#' '

    SJMP SEND_L1

DRAW_BIRD:

    MOV A,#'>'

SEND_L1:

    ACALL LCD_DATA

    INC R0

    CJNE R0,#16,LINE1_LOOP

; =====================================================================
; LINHA 2
; =====================================================================

    MOV A,#0C0H
    ACALL LCD_CMD

    MOV R0,#00

LINE2_LOOP:

    MOV A,R0

    CJNE A,#00,CHK_CACTUS

    ; DINO NO CHÃO

    MOV A,DINO_Y

    JNZ DRAW_DINO_DOWN

    MOV A,#'_'

    SJMP SEND_L2

DRAW_DINO_DOWN:

    MOV A,#'D'

    SJMP SEND_L2

CHK_CACTUS:

    MOV A,R0

    CJNE A,OBST_X,GROUND

    MOV A,OBST_T

    JZ DRAW_CACTUS

GROUND:

    MOV A,#'_'

    SJMP SEND_L2

DRAW_CACTUS:

    MOV A,#'#'

SEND_L2:

    ACALL LCD_DATA

    INC R0

    CJNE R0,#16,LINE2_LOOP

    RET

; =====================================================================
; LCD CMD
; =====================================================================

LCD_CMD:

    CLR RS

    ACALL SendC

    RET

; =====================================================================
; LCD DATA
; =====================================================================

LCD_DATA:

    SETB RS

    ACALL SendC

    RET

; =====================================================================
; CONFIGURA LCD
; =====================================================================

FuncS:

    ; DAT=02h -> 4 bits

    CLR P1.7
    CLR P1.6
    SETB P1.5
    CLR P1.4

    ACALL Clk

    ACALL Delay

    ACALL Clk

    ; DAT=08h -> 2 linhas

    SETB P1.7
    CLR P1.6
    CLR P1.5
    CLR P1.4

    ACALL Clk

    ACALL Delay

    RET

; =====================================================================
; DISPLAY ON
; =====================================================================

DispC:

    ; 0000

    CLR P1.7
    CLR P1.6
    CLR P1.5
    CLR P1.4

    ACALL Clk

    ; 1111

    SETB P1.7
    SETB P1.6
    SETB P1.5
    SETB P1.4

    ACALL Clk

    ACALL Delay

    RET

; =====================================================================
; ENTRY MODE
; =====================================================================

EntryM:

    ; 0000

    CLR P1.7
    CLR P1.6
    CLR P1.5
    CLR P1.4

    ACALL Clk

    ; 0110

    CLR P1.7
    SETB P1.6
    SETB P1.5
    CLR P1.4

    ACALL Clk

    ACALL Delay

    RET

; =====================================================================
; ENVIA BYTE LCD
; =====================================================================

SendC:

    ; HIGH NIBBLE

    MOV C,ACC.7
    MOV P1.7,C

    MOV C,ACC.6
    MOV P1.6,C

    MOV C,ACC.5
    MOV P1.5,C

    MOV C,ACC.4
    MOV P1.4,C

    ACALL Clk

    ; LOW NIBBLE

    MOV C,ACC.3
    MOV P1.7,C

    MOV C,ACC.2
    MOV P1.6,C

    MOV C,ACC.1
    MOV P1.5,C

    MOV C,ACC.0
    MOV P1.4,C

    ACALL Clk

    ACALL Delay

    RET

; =====================================================================
; CLOCK LCD
; =====================================================================

Clk:

    SETB EN

    ACALL Delay

    CLR EN

    RET

; =====================================================================
; DELAY GAME
; =====================================================================

DELAY_GAME:

    MOV R4,SPEED

DG1:

    MOV R5,#255

DG_LOOP:

    DJNZ R5,DG_LOOP

    DJNZ R4,DG1

    RET

; =====================================================================
; DELAY CURTO
; =====================================================================

Delay:

    MOV R7,#100

DS_LOOP:

    DJNZ R7,DS_LOOP

    RET

; =====================================================================
; DELAY LONGO
; =====================================================================

DELAY_LONG:

    MOV R6,#50

DL1:

    MOV R7,#255

DL2:

    DJNZ R7,DL2

    DJNZ R6,DL1

    RET

END