#include <reg51.h>
// =====================================================================
// DEFINIÇÕES DE HARDWARE
// =====================================================================
// Teclado
sbit KEY_ROW_2 = P0^1;
sbit KEY_COL_2 = P0^5;
// LCD
sbit RS = P1^3;
sbit EN = P1^2;
#define LCD_PORT P1
// =====================================================================
// VARIÁVEIS GLOBAIS
// =====================================================================
unsigned int score = 0;
unsigned int speed_delay = 200;
// Dino
unsigned char dino_y = 1;   // 1 = chão / 0 = pulando
unsigned char jumping = 0;
unsigned char jump_timer = 0;
//byte dino[] = { B00000, B00111, B00101, B10111, B10111, B11100, B11110, B10100 };
unsigned char code dino[] = {
    0x00, 
    0x07, 
    0x06, 
    0x17, 
    0x17,
		0x1C,
		0x1E,
		0x14
		
};
// Obstáculo
int obst_x = 15;
unsigned char obst_type = 0; // 0 = cacto / 1 = pássaro
int obstac[] = {1,0,1,0,0,1,1,1,0,0,1,0,0,0,1,1,0,1,0,0,1};
int nobs = 20;
// Ambiente
unsigned char day_night = 0;
// =====================================================================
// DEFININDO FUNÇÕES
// =====================================================================
void delay(unsigned int ms);
void lcd_pulse();
void lcd_send(unsigned char val, unsigned char type);
void lcd_init();
void draw_frame();
void scan_key();
void update_game();
void game_over();
void you_win();
// =====================================================================
// FUNÇÃO PRINCIPAL
// =====================================================================
void main() {
    lcd_init();
    while(1) {
        scan_key();
        update_game();
        draw_frame();
        delay(speed_delay);
    }
}
// =====================================================================
// LÓGICA DO JOGO
// =====================================================================
void update_game() {
    // =========================================================
    // CONTROLE DO PULO
    // =========================================================
    if (jumping) {
        if (jump_timer > 0) {
            jump_timer--;
        } else {
            jumping = 0;
            dino_y = 1; // volta ao chão
        }
    }
    // =========================================================
    // MOVIMENTO DO OBSTÁCULO
    // =========================================================
    obst_x--;
		nobs--;
		if (nobs < 0){
			nobs = 20;
		}
    // =========================================================
    // COLISÃO
    // =========================================================

    if (obst_x == 0) {
        // Cacto -> dino no chão
        if (obst_type == 0 && dino_y == 1) {
            game_over();
        }
        // Pássaro -> dino pulando
        if (obst_type == 1 && dino_y == 0) {
            game_over();
        }
    }
    // =========================================================
    // NOVO OBSTÁCULO
    // =========================================================
    if (obst_x < 0) {
        obst_x = 15;
        score = score + 10;
				if (score == 1000){
					you_win();
				}
        // Alterna tipo do obstáculo
        obst_type = obstac[nobs];
        // Aumenta dificuldade
        if (score > 0 && score % 50 == 0) {
            if (speed_delay > 20) {
                speed_delay -= 20;
            }
        }
        // Alterna dia/noite
        if (score > 0 && score % 100 == 0) {
            day_night = !day_night;
        }
    }
}
// =====================================================================
// LEITURA DO BOTÃO
// =====================================================================

void scan_key() {

    P0 = 0xFF;

    KEY_ROW_2 = 0;

    // Inicia pulo somente se não estiver pulando
    if (KEY_COL_2 == 0 && jumping == 0) {

        jumping = 1;

        jump_timer = 4;

        dino_y = 0;
    }
}

// =====================================================================
// GAME OVER
// =====================================================================

void game_over() {

    lcd_send(0x01, 0);
    delay(10);
    lcd_send(0x80, 0);

    lcd_send('G',1);
    lcd_send('A',1);
    lcd_send('M',1);
    lcd_send('E',1);
    lcd_send(' ',1);
    lcd_send('O',1);
    lcd_send('V',1);
    lcd_send('E',1);
    lcd_send('R',1);
    while(1);
}

void you_win() {

    lcd_send(0x01, 0);
    delay(10);
    lcd_send(0x80, 0);

    lcd_send('Y',1);
    lcd_send('O',1);
    lcd_send('U',1);
    lcd_send(' ',1);
    lcd_send('W',1);
    lcd_send('I',1);
    lcd_send('N',1);
    while(1);
}

// =====================================================================
// RENDERIZAÇÃO
// =====================================================================

void draw_frame() {
    unsigned char i;
    // =========================================================
    // LINHA SUPERIOR (Pulo / Céu)
    // =========================================================
    lcd_send(0x80, 0); // Posiciona cursor no início da linha 1
    for (i = 0; i < 16; i++) {
        // Dino pulando (Usando o caractere customizado 0)
        if (i == 0 && dino_y == 0) {
            lcd_send(0, 1); 
        }
        // Pássaro
        else if (i == obst_x &&   == 1) {
            lcd_send('>', 1);
        }
        // Sol/Lua
        else if (i == 15 && day_night == 0) {
            lcd_send('*', 1);
        }
        else if (i == 15 && day_night == 1) {
            lcd_send(')', 1);
        }
        else {
            lcd_send(' ', 1);
        }
    }
    // =========================================================
    // LINHA INFERIOR (Chão)
    // =========================================================
    lcd_send(0xC0, 0); // Posiciona cursor no início da linha 2
    for (i = 0; i < 16; i++) {
        // Dino no chão (Usando o caractere customizado 0)
        if (i == 0 && dino_y == 1) {
            lcd_send(0, 1);
        }
        // Cacto
        else if (i == obst_x && obst_type == 0) {
            lcd_send('#', 1);
        }
        else {
            lcd_send('_', 1);
        }
    }
}

// =====================================================================
// LCD - MODO 4 BITS
// =====================================================================

void lcd_pulse() {
    EN = 1;
    delay(1);
    EN = 0;
}

void lcd_send(unsigned char val, unsigned char type) {
    RS = type;
    // Nibble alto
    LCD_PORT = (LCD_PORT & 0x0F) | (val & 0xF0);
    lcd_pulse();
    // Nibble baixo
    LCD_PORT = (LCD_PORT & 0x0F) | ((val << 4) & 0xF0);
    lcd_pulse();
    delay(2);
}

void lcd_load_char() {
    unsigned char i;
    // 0x40 é o endereço inicial da CGRAM para o caractere 0
    lcd_send(0x40, 0); 
    for (i = 0; i < 8; i++) {
        lcd_send(dino[i], 1); // Envia cada bit do desenho
    }
}

void lcd_init() {
    delay(20);
    RS = 0;
    // Reset LCD
    LCD_PORT = (LCD_PORT & 0x0F) | 0x30;
    lcd_pulse();
    delay(5);
    lcd_pulse();
    delay(1);
    lcd_pulse();
    // Modo 4 bits
    LCD_PORT = (LCD_PORT & 0x0F) | 0x20;
    lcd_pulse();
    // Configurações
    lcd_send(0x28, 0); // 4 bits / 2 linhas
    lcd_send(0x0C, 0); // Display ON
    lcd_send(0x06, 0); // Incremento cursor
    lcd_send(0x01, 0); // Clear
    delay(5);
		lcd_load_char();
}

// =====================================================================
// DELAY
// =====================================================================
void delay(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 120; j++);
    }
}
