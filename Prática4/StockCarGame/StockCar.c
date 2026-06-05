#include <reg51.h>

// ST7920 - GLCD 128x64
#define GlcdDataBus P3
sbit RS  = P2^0;
sbit RW  = P2^1;
sbit EN  = P2^2;
sbit PSB = P2^3;
sbit RST = P2^5;

// ------
// BOTÕES
// ------

// B4 e B5
sbit KEY_ROW   = P0^2; 
sbit BTN_LEFT  = P0^6; 
sbit BTN_RIGHT = P0^4;

// B1, Linha 1 e Coluna 1
sbit KEY_ROW_1 = P0^3; 
sbit BTN_COL_1 = P0^6;

// ---------
// VARIÁVEIS
// ---------
unsigned int score = 0;
unsigned int speed_delay = 40;
unsigned char player_x = 3;     
unsigned char enemy_x = 3;
signed int enemy_y = -16;       

// Física da Pista
unsigned char track_anim = 0;   
int track_curve[4] = {0, 0, 0, 0}; // Deslocamento físico dos 4 segmentos da tela
int target_curve = 0;              // Para onde a pista está tentando virar (-2 a +2)

// --------------
// SPRITES (16x16)
// --------------
unsigned char code car_bmp[32] = {
    0x00, 0x00, 0x07, 0xE0, 0x0F, 0xF0, 0x1F, 0xF8,
    0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC, 0x1F, 0xF8,
    0x0F, 0xF0, 0x0F, 0xF0, 0x1F, 0xF8, 0x3F, 0xFC,
    0x3F, 0xFC, 0x3F, 0xFC, 0x1F, 0xF8, 0x0F, 0xF0
};

unsigned char code enemy_bmp[32] = {
    0x00, 0x00, 0x07, 0xE0, 0x0F, 0xF0, 0x1F, 0xF8,
    0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC, 0x1F, 0xF8,
    0x0F, 0xF0, 0x0F, 0xF0, 0x1F, 0xF8, 0x3F, 0xFC,
    0x3F, 0xFC, 0x3F, 0xFC, 0x1F, 0xF8, 0x0F, 0xF0
};

// ----------
// PROTÓTIPOS
// ----------
void delay(unsigned int ms);
void GLCD_ShortDelay();
void GLCD_Enable();
void GLCD_SendCmd(unsigned char cmd);
void GLCD_SendData(unsigned char dat);
void GLCD_Init();
void GLCD_SetGraphicAddress(unsigned char x, unsigned char y);
void GLCD_ClearText();
void GLCD_DrawSprite16Clip(unsigned char x, signed int y, unsigned char code *bmp);
void GLCD_DrawTrack(unsigned char anim);
void GLCD_PrintScore();
void GLCD_PrintText(unsigned char line, unsigned char col, char *str);
void draw_frame();
void scan_key();
void update_game();
void game_over();
void you_win();
void home_screen();

// ----
// MAIN
// ----
void main() {
    TMOD |= 0x01; // Timer 0 livre para RNG 
    TR0 = 1;

    GLCD_Init();
    GLCD_ClearText();

    home_screen();
	
    while(1) {
			
        scan_key();
        update_game();
        draw_frame();
        GLCD_PrintScore();
        delay(speed_delay);
    }
}

// ----------------------
// HARDWARE GLCD E TIMERS
// ----------------------
void delay(unsigned int ms) {
    unsigned int i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 120; j++);
}

void GLCD_ShortDelay() { 
		unsigned char i; 

		for(i=0; i<10; i++); 

}
void GLCD_Enable() { 
	EN = 1; 
	GLCD_ShortDelay(); 
	EN = 0; 
}
void GLCD_SendCmd(unsigned char cmd) { 
	
	RS = 0; 
	RW = 0; 
	GlcdDataBus = cmd; 
	GLCD_Enable(); 
	GLCD_ShortDelay(); 

}
void GLCD_SendData(unsigned char dat) { 

	RS = 1; 
	RW = 0; 
	GlcdDataBus = dat; 
	GLCD_Enable(); 
	GLCD_ShortDelay(); 

}

void GLCD_Init() {
    PSB = 1; 
    RST = 1; 
    delay(50);
    
    GLCD_SendCmd(0x30); 
    delay(10); 
    GLCD_SendCmd(0x30); 
    delay(10);
    GLCD_SendCmd(0x0C); 
    delay(10); 
    GLCD_SendCmd(0x01); 
    delay(10); 
    GLCD_SendCmd(0x06); 
    delay(10); 
    GLCD_SendCmd(0x36); 
    delay(10); 
}

void GLCD_SetGraphicAddress(unsigned char x, unsigned char y) {
    if(y < 32) {
        GLCD_SendCmd(0x80 | y);     
        GLCD_SendCmd(0x80 | x);     
    } else {
        GLCD_SendCmd(0x80 | (y - 32));
        GLCD_SendCmd(0x88 | x);     
    }
}

void GLCD_ClearText() { 

		GLCD_SendCmd(0x30); 
		GLCD_SendCmd(0x01); 

		delay(5); 

}

// ======================
// LÓGICA DE RENDERIZAÇÃO
// ======================
void GLCD_DrawSprite16Clip(unsigned char x, signed int y, unsigned char code *bmp) {
    unsigned char i;
    for(i = 0; i < 16; i++) {
        signed int draw_y = y + i;
        if(draw_y >= 0 && draw_y < 64) {
            GLCD_SetGraphicAddress(x, (unsigned char)draw_y);
            GLCD_SendData(bmp[i*2]);     
            GLCD_SendData(bmp[i*2 + 1]); 
        }
    }
}

// Função de Pista
void GLCD_DrawTrack(unsigned char anim) {
    unsigned char y, x, seg;
    int offset;
    unsigned char dash_on;

    for(y = 0; y < 64; y++) {
        // Divide a tela em 4 segmentos para a física da curva
        seg = y / 16;
        offset = track_curve[seg];

        // Define se estamos na parte visível ou apagada do tracejado
        dash_on = ((y + anim) % 16 < 8);

        GLCD_SetGraphicAddress(0, y);
        for(x = 0; x < 8; x++) {
            if (dash_on && x == (1 + offset)) {
                // Borda Esquerda: Desenha 4 pixels colados à direita do bloco
                GLCD_SendData(0x00); 
                GLCD_SendData(0x0F); 
            } 
            else if (dash_on && x == (6 + offset)) {
                // Borda Direita: Desenha 4 pixels colados à esquerda do bloco
                GLCD_SendData(0xF0); 
                GLCD_SendData(0x00); 
            } 
            else {
                // Restante da tela totalmente limpa (vazio)
                GLCD_SendData(0x00);
                GLCD_SendData(0x00);
            }
        }
    }
}

// -------------
// TEXTO (DDRAM)
// -------------
void GLCD_PrintText(unsigned char line, unsigned char col, char *str) {
    unsigned char addr;
    if(line == 1) addr = 0x80;
    else if(line == 2) addr = 0x90;
    else if(line == 3) addr = 0x88;
    else addr = 0x98;
    
    GLCD_SendCmd(0x30); 
    GLCD_SendCmd(addr + col);
    while(*str) GLCD_SendData(*str++);
}

void GLCD_PrintScore() {
    char buf[6];
    buf[0] = (score / 10000) + '0';
    buf[1] = ((score / 1000) % 10) + '0';
    buf[2] = ((score / 100) % 10) + '0';
    buf[3] = ((score / 10) % 10) + '0';
    buf[4] = (score % 10) + '0';
    buf[5] = '\0';
    GLCD_PrintText(1, 1, " KM/H:");
    GLCD_PrintText(1, 4, buf);
}

// ------------------------------------
// CONTROLE DO JOGADOR E LÓGICA DO JOGO
// ------------------------------------
void scan_key() {
    P0 = 0xFF;
    KEY_ROW = 0;
    
    if(BTN_LEFT == 0) {
        delay(10); 
        if(BTN_LEFT == 0 && player_x > 0) player_x--;
        while(BTN_LEFT == 0); 
    }
    if(BTN_RIGHT == 0) {
        delay(10);
        if(BTN_RIGHT == 0 && player_x < 7) player_x++;
        while(BTN_RIGHT == 0);
    }
}

void update_game() {
    enemy_y += 4;
    track_anim = (track_anim + 4) % 16;

    if (track_anim == 0) {
        // Desloca as curvas (Cascata de física)
        track_curve[3] = track_curve[2];
        track_curve[2] = track_curve[1];
        track_curve[1] = track_curve[0];

        if (track_curve[0] < target_curve) track_curve[0]++;
        else if (track_curve[0] > target_curve) track_curve[0]--;
        else {
            target_curve = (TL0 % 5) - 2; 
        }
    }

    if(enemy_y > 63) {
        enemy_y = -16;
        enemy_x = (TL0 % 4) + 2 + track_curve[0]; 
        
        score += 10;
        if(score % 100 == 0 && speed_delay > 10) speed_delay -= 3; 
        if(score >= 1000) you_win();
    }

    // 1. Detecção de Colisão com Inimigo
    if(enemy_y + 16 > 46 && enemy_y < 46 + 16) {
        if(player_x == enemy_x) {
            game_over();
        }
    }
    
    // 2. Detecção de Colisão com a Parede Tracejada
    if (player_x < (2 + track_curve[3]) || player_x > (5 + track_curve[3])) {
        game_over(); 
    }
}

void draw_frame() {
    GLCD_SendCmd(0x36); 
    
    GLCD_DrawTrack(track_anim); 
    
    if (enemy_y > -16 && enemy_y < 64) {
        GLCD_DrawSprite16Clip(enemy_x, enemy_y, enemy_bmp);
    }
    GLCD_DrawSprite16Clip(player_x, 46, car_bmp);
    
    GLCD_SendCmd(0x30); 
}

// --------------
// ESTADOS FINAIS
// --------------
void game_over() { 
    GLCD_ClearText(); 
    
    // Alinhamento vertical na Linha 2
    // col = 1 (caractere 3) + 1 espaço na string = Começa no caractere 4
    GLCD_PrintText(2, 1, " GAME OVER"); 
    
    while(1); 
}

void home_screen() { 
    GLCD_ClearText(); 
    
    GLCD_PrintText(1, 0, " STOCK CAR GAME");
    GLCD_PrintText(3, 0, "PRESS B1 TO INIT");
        
    // Laço infinito para varrer o teclado matricial esperando B1
    while(1) {
        P0 = 0xFF;         // Garante que todas as colunas estão em nível lógico alto
        KEY_ROW_1 = 0;     // Habilita a leitura da Linha 1
        
        // Verifica se a Coluna 1 foi pressionada
        if(BTN_COL_1 == 0) {
            delay(15); // Debounce físico do botão
            if(BTN_COL_1 == 0) {
                while(BTN_COL_1 == 0); // Trava aqui até soltar o dedo do botão
								GLCD_ClearText();
                break; // Sai do laço while e o jogo inicia
            }
        }
    }
}

void you_win() { 

		GLCD_ClearText(); 
		GLCD_PrintText(2, 2, "   YOU WIN!   "); 

		while(1); 

}

