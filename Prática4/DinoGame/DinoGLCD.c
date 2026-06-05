#include <reg51.h>

#define GlcdDataBus P3
sbit RS  = P2^0;
sbit RW  = P2^1;
sbit EN  = P2^2;
sbit PSB = P2^3;
sbit RST = P2^5;

// ============================================================
// BOTÃO (Tecla '5' para pular)
// ============================================================
sbit KEY_ROW_2 = P0^2; 
sbit KEY_COL_2 = P0^5; 

// ============================================================
// VARIÁVEIS DO JOGO
// ============================================================
unsigned int score = 0;
unsigned int speed_delay = 100;
unsigned char dino_y = 38;     // Coordenada vertical do Dino (Chão = 38, Ar = 18)
signed int obst_x = 7;         
unsigned char obst_type = 0;   
unsigned char day_night = 0;   // 0 = Dia (Sol), 1 = Noite (Lua)

// ============================================================
// MATRIZES DE SPRITES (16x16)
// ============================================================
unsigned char code dino_bmp[32] = {
    0x07, 0xFE, 0x07, 0xFF, 0x07, 0xFC, 0x03, 0xF0,
    0x0F, 0xF0, 0x3F, 0xF0, 0x7F, 0xE0, 0xFF, 0xE0,
    0x7F, 0xF0, 0x3F, 0xE0, 0x1F, 0xC0, 0x0F, 0x80,
    0x0D, 0x80, 0x09, 0x00, 0x0B, 0x00, 0x03, 0x00
};
unsigned char code cactus_bmp[32] = {
    0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x31, 0x8C,
    0x79, 0x9E, 0x7D, 0xBE, 0x3F, 0xFC, 0x0F, 0xF0,
    0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0,
    0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0
};
unsigned char code bird_bmp[32] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80,
    0x1F, 0xE0, 0x7F, 0xF8, 0xFF, 0xFC, 0x3F, 0xE0,
    0x0F, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
unsigned char code sun_bmp[32] = {
    0x00, 0x00, 0x01, 0x80, 0x11, 0x88, 0x09, 0x90, 
    0x07, 0xE0, 0x3F, 0xFC, 0x3F, 0xFC, 0x3F, 0xFC, 
    0x3F, 0xFC, 0x07, 0xE0, 0x09, 0x90, 0x11, 0x88, 
    0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
unsigned char code moon_bmp[32] = {
    0x00, 0x00, 0x01, 0xE0, 0x07, 0xF0, 0x0F, 0x80, 
    0x1F, 0x00, 0x1E, 0x00, 0x1E, 0x00, 0x1E, 0x00, 
    0x1F, 0x00, 0x0F, 0x80, 0x07, 0xF0, 0x01, 0xE0, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void delay(unsigned int ms);
void GLCD_ShortDelay();
void GLCD_Enable();
void GLCD_SendCmd(unsigned char cmd);
void GLCD_SendData(unsigned char dat);
void GLCD_Init();
void GLCD_SetGraphicAddress(unsigned char x, unsigned char y);
void GLCD_ClearGraphic();
void GLCD_ClearText();
void GLCD_DrawSprite16(unsigned char x, unsigned char y, unsigned char code *bmp);
void GLCD_ClearBlock(unsigned char x, unsigned char y_start, unsigned char height);
void GLCD_DrawGround();
void GLCD_PrintScore();
void GLCD_PrintText(unsigned char line, unsigned char col, char *str);
void draw_frame();
void scan_key();
void update_game();
void game_over();
void you_win();

void main() {
    TMOD |= 0x01; TR0 = 1;

    GLCD_Init();
    GLCD_ClearGraphic();
    GLCD_ClearText();
    
    GLCD_SendCmd(0x36); 
    GLCD_DrawGround();
    GLCD_SendCmd(0x30); 

    while(1) {
        scan_key();
        update_game();
        draw_frame();
        GLCD_PrintScore();
        delay(speed_delay);
    }
}

void delay(unsigned int ms) {
    unsigned int i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 120; j++);
}

void GLCD_ShortDelay() { 
  unsigned char i; 
  for(i = 0; i < 10; i++); 

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

void GLCD_ClearGraphic() {
    unsigned char x, y;
    GLCD_SendCmd(0x36);
    for(y = 0; y < 64; y++) {
        GLCD_SetGraphicAddress(0, y);
        for(x = 0; x < 16; x++) 
          GLCD_SendData(0x00);
    }
    GLCD_SendCmd(0x30);
}

void GLCD_ClearText() { 

GLCD_SendCmd(0x30); 
GLCD_SendCmd(0x01); 
delay(5); }

void GLCD_DrawSprite16(unsigned char x, unsigned char y, unsigned char code *bmp) {
    unsigned char i;
    for(i = 0; i < 16; i++) {
        GLCD_SetGraphicAddress(x, y + i);
        GLCD_SendData(bmp[i*2]);
        GLCD_SendData(bmp[i*2 + 1]);
    }
}

void GLCD_ClearBlock(unsigned char x, unsigned char y_start, unsigned char height) {
    unsigned char i;
    for(i = 0; i < height; i++) {
        GLCD_SetGraphicAddress(x, y_start + i);
        GLCD_SendData(0x00);
        GLCD_SendData(0x00);
    }
}

void GLCD_DrawGround() {
    unsigned char x;
    for(x = 0; x < 8; x++) {
        GLCD_SetGraphicAddress(x, 54);
        GLCD_SendData(0xFF); 
        GLCD_SendData(0xFF);
    }
}

void GLCD_PrintText(unsigned char line, unsigned char col, char *str) {
    unsigned char addr;
    if(line == 1) addr = 0x80;
    else if(line == 2) addr = 0x90;
    else if(line == 3) addr = 0x88;
    else addr = 0x98;
    
    GLCD_SendCmd(0x30); 
    GLCD_SendCmd(addr + col);
    while(*str) 
      GLCD_SendData(*str++);
}

void GLCD_PrintScore() {
    char buf[6];
    buf[0] = (score / 10000) + '0';
    buf[1] = ((score / 1000) % 10) + '0';
    buf[2] = ((score / 100) % 10) + '0';
    buf[3] = ((score / 10) % 10) + '0';
    buf[4] = (score % 10) + '0';
    buf[5] = '\0';
    
    GLCD_PrintText(1, 3, "SCR:");
    GLCD_PrintText(1, 5, buf);
}

void scan_key() {
    P0 = 0xFF;
    KEY_ROW_2 = 0;
    
    // Se a tecla 5 estiver sendo pressionada, o dino fica no ar
    if(KEY_COL_2 == 0) {
        dino_y = 18;
    } 
    // Se a tecla for solta, ele volta instantaneamente para o chão
    else {
        dino_y = 38;
    }
}

void update_game() {
    obst_x--;
    if(obst_x < 0) {
        obst_x = 7;
        obst_type = TL0 % 2; 
        score += 10;
        
        if(score % 100 == 0 && speed_delay > 10) speed_delay -= 5;
        if(score % 30 == 0) day_night = !day_night;
        
        if(score >= 1000) you_win();
    }

    // Detecção de colisão baseada na coordenada instantânea
    if(obst_x == 1) {
        if(obst_type == 0) {
            if(dino_y >= 28) game_over();
        } else {
            if(dino_y < 28) game_over();
        }
    }
}

void draw_frame() {
    GLCD_SendCmd(0x36); // Ativa modo gráfico
    
    // Limpa rastros de movimento (Cobre do ar até o chão)
    GLCD_ClearBlock(1, 18, 36);        
    GLCD_ClearBlock(obst_x + 1, 20, 34); 
    if (obst_x == 7) GLCD_ClearBlock(0, 20, 34); 

    // Desenha o Dino
    GLCD_DrawSprite16(1, dino_y, dino_bmp);

    // Desenha o obstáculo atual
    if(obst_type == 0) {
        GLCD_DrawSprite16(obst_x, 38, cactus_bmp);
    } else {
        GLCD_DrawSprite16(obst_x, 20, bird_bmp);   
    }
    
    // Desenha o Sol ou a Lua no Canto Superior Esquerdo
    if(day_night == 0) {
        GLCD_DrawSprite16(0, 4, sun_bmp);
    } else {
        GLCD_DrawSprite16(0, 4, moon_bmp);
    }
    
    GLCD_SendCmd(0x30); // Retorna ao modo texto
}

void game_over() { 
  GLCD_ClearText(); 
  GLCD_PrintText(2, 2, "   GAME OVER  "); 
  while(1); 

}
void you_win() { 
  GLCD_ClearText(); 
  GLCD_PrintText(2, 2, "   YOU WIN!   "); 
  while(1); 

}
