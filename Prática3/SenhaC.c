// biblioteca padrão do 8051
#include <reg51.h>

// ================= LCD =================
sfr LCD_Port = 0x90;   //Define endereço de P1

sbit rs = P1^3; //Define o pino (RS) do LCD
sbit rw = P1^1; //Define o pino de leitura/escrita (RW)

//en é ativado e desativado para avisar LCD que há dados a receber
sbit en = P1^2; // Define pino do EN

// ================= TECLADO =================
#define teclado P0

// ================= PROTOTIPOS =================
void delay(unsigned int);
void LCD_Command(unsigned char);
void LCD_Char(unsigned char);
void LCD_String(char *);
void LCD_String_xy(unsigned char, unsigned char, char *);
void LCD_Init(void);

char scanKey(void);
void esperaSoltar(void);

// ================= DELAY =================
void delay(unsigned int count) {

    unsigned int i, j;

    for(i = 0; i < count; i++){
        for(j = 0; j < 112; j++);
    }
}

// ================= LCD =================
// Para enviar comandos (limpar tela, posicionar cursor).
void LCD_Command(unsigned char cmd) {

    LCD_Port = (LCD_Port & 0x0F) | (cmd & 0xF0);
    rs = 0; 
	rw = 0; 
	en = 1;
    delay(1);
    en = 0;
	delay(2);

    LCD_Port = (LCD_Port & 0x0F) | (cmd << 4);
    en = 1;
    delay(1);
    en = 0;
    delay(5);
}

//Para enviar um caractere para aparecer na tela do LCD
void LCD_Char(unsigned char char_data) {

    LCD_Port = (LCD_Port & 0x0F) | (char_data & 0xF0);
    rs = 1; 
	rw = 0; 
	en = 1;
    delay(1);
    en = 0;
	delay(2);

    LCD_Port = (LCD_Port & 0x0F) | (char_data << 4);
    en = 1;
    delay(1);
    en = 0;
    delay(5);
}

// Para enviar uma string para aparecer na tela do LCD
void LCD_String(char *str) {

    while(*str)
        LCD_Char(*str++);

}

// Envia string para uma posição (x,y) específica da tela do LCD
void LCD_String_xy(unsigned char row, unsigned char pos, char *str) {

    if(row == 0)
        LCD_Command(0x80 + pos);
    else
        LCD_Command(0xC0 + pos);

    LCD_String(str);
}

// Configura o LCD
void LCD_Init(void) {

    delay(20);

    LCD_Command(0x02);
    LCD_Command(0x28);
    LCD_Command(0x0C);
    LCD_Command(0x06);
    LCD_Command(0x01);
}

// ================= TECLADO =================
// Varre o teclado para ver qual tecla foi pressionada.
char scanKey() {

    while(1) {

        // Linha 4
		teclado = 0xFE;
        if(!(teclado & 0x40)) { esperaSoltar(); return '*'; }
        if(!(teclado & 0x20)) { esperaSoltar(); return '0'; }
        if(!(teclado & 0x10)) { esperaSoltar(); return '#'; }

        // Linha 3
        teclado = 0xFD;
        if(!(teclado & 0x40)) { esperaSoltar(); return '7'; }
        if(!(teclado & 0x20)) { esperaSoltar(); return '8'; }
        if(!(teclado & 0x10)) { esperaSoltar(); return '9'; }

        // Linha 2
        teclado = 0xFB;
        if(!(teclado & 0x40)) { esperaSoltar(); return '4'; }
        if(!(teclado & 0x20)) { esperaSoltar(); return '5'; }
        if(!(teclado & 0x10)) { esperaSoltar(); return '6'; }

        // Linha 1
        teclado = 0xF7;
        if(!(teclado & 0x40)) { esperaSoltar(); return '1'; }
        if(!(teclado & 0x20)) { esperaSoltar(); return '2'; }
        if(!(teclado & 0x10)) { esperaSoltar(); return '3'; }
    }
}

// Trava o código até que o usuário tire o dedo da tecla.
void esperaSoltar() {
    while((teclado & 0x70) != 0x70);
}

// ================= MAIN =================
void main(void) {

    unsigned char senha[4];
    unsigned char i;

    // Configura LCD
    LCD_Init();

    while(1) {

        LCD_Command(0x01); // limpa tela

        LCD_String_xy(0,0,"Digite senha:"); 
        LCD_Command(0xC0);

        // leitura da senha via teclado
        for(i = 0; i < 4; i++) {

            senha[i] = scanKey();

            LCD_Char('*'); // "mascara"
        }

        // verificacao da senha digitada
        LCD_Command(0xC0); // volta para o inicio da segunda linha

        if(senha[0]=='1' && senha[1]=='2' &&
           senha[2]=='3' && senha[3]=='4') {
						
            LCD_String("COFRE ABERTO");
        }
        else {

            LCD_String("SENHA INCORRETA");
        }

        delay(5000);
    }
}