Org 0000h ;endereço de inicio do programa na memória

  ; Dando nome para "variáveis"
  LEDS EQU P1
  A1 EQU P3.4
  A0 EQU P3.3
  CS EQU P0.7

  ;Display é anodo comum. Nível zero para ligar LEDs

  ;--------------------------------------------------------------
  ;Função principal de entrada e que chama o Decoder
  Main:

    SETB CS ;Habilita Decoder

    Loop:

      CALL Decoder ;Chama subrotina
      JMP Loop ;Loop infinito
  ;--------------------------------------------------------------

  ;--------------------------------------------------------------
  Decoder: ;81180

    ;11 -> escreve o Número 8 no Display 3
    SETB A0
    SETB A1
    CALL N8
    CALL Delay
    CALL ApagaD

       ;10 -> escreve o Número 1 no Display 2
    CLR A0
    SETB A1
    CALL N1
    CALL Delay
    CALL ApagaD

    ;01 -> escreve o Número 1 no Display 1
    SETB A0
    CLR A1
    CALL N1
    CALL Delay
    CALL ApagaD

    ;00 -> escreve o Número 8 no Display 0
    CLR A0
    CLR A1
    CALL N8
    CALL Delay
    CALL ApagaD

    RET
  ;--------------------------------------------------------------

  ;--------------------------------------------------------------
  Delay: ;200microsegundos

    mov R0, #100 ;R0=100
   
   D:
    DJNZ R0, D ;Decrementa e pula para D até zerar

    RET
  ;--------------------------------------------------------------

  ;--------------------------------------------------------------
  ;Subrotinas de Números para o Display
  N0:
    MOV LEDS, #11000000b  ; Liga a, b, c, d, e, f
    RET

  N1:
    MOV LEDS, #11111001b  ; Liga b, c
    RET

  N2:
    MOV LEDS, #10100100b  ; Liga a, b, d, e, g
    RET

  N3:
    MOV LEDS, #10110000b  ; Liga a, b, c, d, g
    RET

  N4:
    MOV LEDS, #10011001b  ; Liga b, c, f, g
    RET
    
  N8:
    MOV LEDS, #10000000b  ; Liga a, b, c, d, e, f, g (só o dp fica desligado)
    RET

  N7:
    MOV LEDS, #11111000b  ; Liga a, b, c
    RET
    
  N6:
    MOV LEDS, #10000010b  ; Liga a, d, c, e, f, g
    RET

  N9:
    MOV LEDS, #10011000b  ; Liga a, b, c, f, g
    RET

  ApagaD:
    MOV LEDS, #11111111b  ; Desliga todos os segmentos
    RET
  ;--------------------------------------------------------------
