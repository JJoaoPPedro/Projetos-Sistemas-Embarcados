Org 0000h ;endereço de inicio do programa na memória

  ; Dando nome para "variáveis"
  LEDS EQU P1
  A1 EQU P3.4
  A0 EQU P3.3
  CS EQU P0.7

  ;Display é anodo comum então é necessário nível zero para ligar LEDs

  ;--------------------------------------------------------------
  ;Função principal que "envia sinais" e chama o Decoder
  Main:

    SETB CS ;Habilita Decoder

    ;11 - Display 3
    SETB A1
    SETB A0
    CALL Decoder ;Chama subrotina do decodificador

    ;10 - Display 2
    SETB A1
    CLR A0
    CALL Decoder ;Chama subrotina do decodificador

    ;01 - Display 1
    CLR A1
    SETB A0
    CALL Decoder ;Chama subrotina do decodificador

    ;00 - Display 0
    CLR A1
    CLR A0
    CALL Decoder ;Chama subrotina do decodificador

    CLR CS;Desabilita Decoder

    JMP Main ;Loop infinito
  ;--------------------------------------------------------------

  ;--------------------------------------------------------------
  ;Está setado para mostrar os 4 primeiros números do RA: 811804
  Decoder:

    ;Se A1 for 0, pula para investigar a metade de baixo (00 ou 01)
    JNB A1, Testa_A1_Zero

      ; --- METADE ALTA (A1 é 1) ---
      ; Se A0 for 0, é o caso 10. Se não pular, é o caso 11.
      JNB A0, Caso_10
        
        ; --- Caso 11 (DISP3) ---
        CALL N8
        CALL Delay
        CALL ApagaD
        JMP Fim

      Caso_10: 

        ; --- Caso 10 (DISP2) ---
        CALL N1
        CALL Delay
        CALL ApagaD
        JMP Fim

    Testa_A1_Zero:

      ; --- METADE BAIXA (A1 é 0) ---
      ; Se A0 for 0, é o caso 00. Se não pular, é o caso 01.
      JNB A0, Caso_00
        
        ; --- Caso 01 (DISP1) ---
        CALL N1
        CALL Delay
        CALL ApagaD
        JMP Fim
        
      Caso_00: 

        ; --- Caso 00 (DISP0) ---
        CALL N8
        CALL Delay
        CALL ApagaD
        JMP Fim

    Fim:

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

  ;--------------------------------------------------------------
  ;Subrotina de Delay de ~200 microsegundos
  Delay: 

    mov R0,#100 ;R0=100

    D:
    DJNZ R0, D ;Decrementa e pula para D até zerar

    RET
  ;--------------------------------------------------------------