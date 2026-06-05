Org 0000h ;endereço de inicio do programa na memória

  ; Atribuir pinos usando EQU
  ; Dando nome para "variáveis"
  RS EQU P1.3
  EN EQU P1.2

  Main:

    ;Configura LCD
   
    ;RS = 0 indica que o LCD irá receber instruções (limpar tela, configurar modo, etc)
    CLR RS ;Pino RS = 0 do LCD
   
    ;Subrotinas de configuração
    CALL FuncS ;Configura Modo de 4 bits e 2 linhas - LCD
    CALL DispC ;Liga Display e Cursor
    CALL EntryM ;Configura Shift Right

  Next:

    ;Lê teclado e mostra no LCD
    CALL ScanKey ;Varrer teclado para detectar se tecla foi apertada
    SETB RS ;RS=1 altera o LCD para o modo de recebimento de dados
    CLR A ;A=0 limpa acumulador
    MOV A, R7 ;Acumulador=R7
    CALL SendC ;Mostra tecla pressionada no display

    CJNE R7, #'#', Next ;Se tecla pressionada != '#' => Next => Volta a varrer

    CALL Fim ;Salta Fim, caso R7 = '#'

  FuncS:
 
    ;Configura LCD modo 4 bits (4 pinos de dados) e 2 linhas
   
    ; DAT=02h; Modo de 4 bits - LCD
    CLR p1.7
    CLR p1.6
    SETB p1.5
    CLR p1.4

    CALL Clk
    CALL Delay
    CALL Clk

    ; DAT=08h; LCD de 2 linhas
    SETB p1.7
    CLR p1.6
    CLR p1.5
    CLR p1.4

    CALL Clk
    CALL Delay

    RET

  DispC:
 
    ;0000 1111 enviados, em duas partes de 4bits, para ligar o LCD
   
    ;DAT=0h
    CLR p1.7
    CLR p1.6
    CLR p1.5
    CLR p1.4
   
    CALL Clk
   
    ;DAT=0Fh Liga LCD e cursor
    SETB p1.7
    SETB p1.6
    SETB p1.5
    SETB p1.4
   
    CALL Clk
    CALL Delay
   
    RET

  EntryM:
 
    ;Configura cursor para: Deslocamento à Direita toda vez que caractere for enviado para o LCD
    ;0000 0110 enviados, em duas partes de 4bits, para ativar deslocamento do cursor
   
    ;DAT=0h
    CLR p1.7
    CLR p1.6
    CLR p1.5
    CLR p1.4
   
    CALL Clk
   
    ;DAT=06h
    CLR p1.7
    SETB p1.6
    SETB p1.5
    CLR p1.4
   
    CALL Clk
    CALL Delay
   
    RET

  ScanKey:

    ;Varredura de teclado 3x4
    ; Primeira Linha(0): 0 1 2
    ; Segunda Linha(1):  0 1 2
    ; Terceira Linha(2): 0 1 2
    ; Quarta Linha(3):   0 1 2
   
    CLR p0.0 ;P0.0=0 Ativa quarta linha
    CALL Icode3; Varre coluna
    SETB p0.0 ;P0.0=1 Desativa
    JB F0, Done ;Se F0 = 1 => Tecla pressionada
   
    CLR p0.1 ;P0.1=0 Ativa terceira linha
    CALL Icode2; Varre coluna
    SETB p0.1 ;P0.1=1 Desativa
    JB F0, Done ;Se F0 = 1 => Tecla pressionada
   
    CLR p0.2 ;P0.2=0 Ativa segunda linha
    CALL Icode1; Varre coluna
    SETB p0.2 ;P0.2=1 Desativa
    JB F0, Done ;Se F0 = 1 => Tecla pressionada
   
    CLR p0.3 ;P0.3=0 Ativa primeira linha
    CALL Icode0; Varre coluna
    SETB p0.3 ;P0.3=1 Desativa
    JB F0, Done ; Se F0 = 1 => Tecla pressionada
   
    JMP ScanKey ;Volta a varrer até encontrar

   Done:
   
    CLR F0 ;F0=0
   
    RET
   
   ;Lógica icodeX: Se pino da coluna for zero => tecla pressionada =>

   ;Quarta linha
   Icode3:

        JNB p0.6, KeyC30 ;Se ~P0.6, KeyC30
        JNB p0.5, KeyC31 ;Se ~P0.5, KeyC31
        JNB p0.4, KeyC32 ;Se ~P0.4, KeyC32
       
        RET

      ;Tecla '*'
      KeyC30:
        SETB F0
        CALL ESPSOL
        MOV R7, #'*'
        RET

      ;Tecla '0'
      KeyC31:
        SETB F0
        CALL ESPSOL
        MOV R7, #'0'
        RET
       
      ;Tecla '#'
      KeyC32:
        SETB F0
        CALL ESPSOL
        MOV R7, #'#'
        RET

    ;Terceira Linha
    Icode2:

        JNB p0.6, KeyC20 ;Se ~P0.6, KeyC20
        JNB p0.5, KeyC21 ;Se ~P0.5, KeyC21
        JNB p0.4, KeyC22 ;Se ~P0.4, KeyC22
       
        RET

      ;Tecla '7'
      KeyC20:
        SETB F0
        CALL ESPSOL
        MOV R7, #'7'
        RET

      ;Tecla '8'
      KeyC21:
        SETB F0
        CALL ESPSOL
        MOV R7, #'8'
        RET
       
      ;Tecla '9'
      KeyC22:
        SETB F0
        CALL ESPSOL
        MOV R7, #'9'
        RET

    ;Segunda Linha
    Icode1:

        JNB p0.6, KeyC10 ;Se ~P0.6, KeyC10
        JNB p0.5, KeyC11 ;Se ~P0.5, KeyC11
        JNB p0.4, KeyC12 ;Se ~P0.4, KeyC12
       
        RET

      ;Tecla '4'
      KeyC10:
        SETB F0
        CALL ESPSOL
        MOV R7, #'4'
        RET

      ;Tecla '5'
      KeyC11:
        SETB F0
        CALL ESPSOL
        MOV R7, #'5'
        RET
       
      ;Tecla '6'
      KeyC12:
        SETB F0
        CALL ESPSOL
        MOV R7, #'6'
        RET

    ;Primeira Linha
    Icode0:

        JNB p0.6, KeyC00 ;Se ~P0.6, KeyC00
        JNB p0.5, KeyC01 ;Se ~P0.5, KeyC01
        JNB p0.4, KeyC02 ;Se ~P0.4, KeyC02
       
        RET

      ;Tecla '1'
      KeyC00:
        SETB F0
        CALL ESPSOL
        MOV R7, #'1'
        RET

      ;Tecla '2'
      KeyC01:
        SETB F0
        CALL ESPSOL
        MOV R7, #'2'
        RET
       
      ;Tecla '3'
      KeyC02:
        SETB F0
        CALL ESPSOL
        MOV R7, #'3'
        RET

  SendC:

    ;Envia o caractere de 8 bits armazenado no acumulador em duas levas de 4bits
    ;Acumulador é um registrador endereçavel bit a bit
    ;C = Carry Flag = registrador de 1 bit

    MOV C, ACC.7 ;C=Acc.7
    MOV p1.7, C ;P1.7=C

    MOV C, ACC.6 ;C=Acc.6
    MOV p1.6, C ;P1.6=C

    MOV C, ACC.5 ;C=Acc.5
    MOV p1.5, C ;P1.5=C

    MOV C, ACC.4 ;C=Acc.4
    MOV p1.4, C ;P1.4=C

    CALL Clk

    MOV C, ACC.3 ;C=Acc.3
    MOV p1.7, C ;P1.7=C

    MOV C, ACC.2 ;C=Acc.2
    MOV p1.6, C ;P1.6=C

    MOV C, ACC.1 ;C=Acc.1
    MOV p1.5, C ;P1.5=C

    MOV C, ACC.0 ;C=Acc.0
    MOV p1.4, C ;P1.4=C

    CALL Clk
    CALL Delay

    Ret

  Clk:

    SETB EN
    CALL Delay ;Precisa disso?
    CLR EN

    RET

  Delay:

    mov R0, #50 ;R0=50
   
   D:
    DJNZ R0, D ;Decrementa e pula para D até zerar

    RET
        
  ESPSOL:

    MOV A, P0
    ANL A, #070h
    CJNE A, #070h, ESPSOL

    MOV TMOD, #01h
    MOV TH0, #21h
    MOV TL0, #840h

    SETB tr0
    JNB tf0, $
    CLR tr0
    CLR tf0

    RET

  Fim:

    JMP $
