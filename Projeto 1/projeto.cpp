// melhorei um pouco as musicas (o 0 nas notas eh uma pausa, tem que ver como fica na hora de jogar o jogo, nao sei se vai dar b.o), coloquei escolha de dificuldade, mas ainda nao implementado como modo, so a escolha dela 
// pensei em facil(5 notas), medio (10 notas), dificil (15 notas)
// as musica ja estao tocando, o jogo do genius ainda nao (os botoes na hora que iniciam o jogo ainda estao sem funcionalidades)
// o progmem eh pra salvar as musicas na memoria flash, pra nao usar toda a memoria do arduino
// tem que fazer a func pra pegar o aperto dos botoes e relacionar com as notas, e comparar com as musicas

#include <LiquidCrystal.h>
#include <avr/pgmspace.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define NOTA_C 262  // Dó (C)
#define NOTA_E 330  // Mi (E)
#define NOTA_G 392  // Sol (G)
#define NOTA_C_HIGH 523

const int buzzer = 13;
const int botoes[4] = {A0, A1, A2, A3};
const int ledAcerto = 10;
const int ledErro = 9;

const int musica_tetris[] PROGMEM = {
    NOTA_E, NOTA_E, NOTA_E, NOTA_C,
    NOTA_G, NOTA_G, NOTA_E, NOTA_E, 
    NOTA_C, NOTA_C, NOTA_G, NOTA_G,
    NOTA_E, NOTA_C_HIGH, NOTA_G, 0, 0
};

const int musica_mario[] PROGMEM = {
    NOTA_E, NOTA_E, 0, NOTA_E, 
    NOTA_C, NOTA_E, NOTA_G, 
    NOTA_C_HIGH, NOTA_G, 0, 
    NOTA_E, NOTA_C, NOTA_G, 
    NOTA_E, 0
};

const int musica_pirates[] PROGMEM = {
    NOTA_G, NOTA_E, NOTA_E, 
    NOTA_C, NOTA_G, NOTA_C_HIGH, 
    0, NOTA_G, NOTA_E, 
    NOTA_C, 0, NOTA_G, 
    NOTA_C_HIGH, NOTA_E, 0
};

const int* const musicas_notas[] PROGMEM = {musica_mario, musica_tetris, musica_pirates};

// variáveis de ambiente
int nivel = 1;
int vidas = 3;

// lista das músicas
String musicas[] = {"Mario", "Tetris", "Pirates"};
String dificuldades[] = {"Facil", "Medio", "Dificil"};

int totalMusicas = 3;
int indiceMusica = 0; 
int indiceDif = 0;

void setup() {
    pinMode(buzzer, OUTPUT);
    pinMode(ledAcerto, OUTPUT);
    pinMode(ledErro, OUTPUT);
    for (int i = 0; i < 4; i++) {
        pinMode(botoes[i], INPUT);
    }

    Serial.begin(9600);
    lcd.begin(16, 2);
    
    lcd.print("Genius Musical");
    delay(2000);
    
    tocar_musica_menu();
    
    lcd.clear();
}

void loop() {
    mostrarMenu();
}

int aguardarBotao() {
    while (true) {
      for (int i = 0; i < 4; i++) {
        if(digitalRead(botoes[i]) != LOW) {  
          delay(200); 
          Serial.println(i);
          return i;  
        }
      }
    }
}

void mostrarMenu() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Escolher musica:");
    lcd.setCursor(0, 1);
    String exemplo = "-> " + musicas[indiceMusica];
    lcd.print(exemplo);
    
    while (true) {
        int botao = aguardarBotao();
        
        if (botao == 0) {  // subir música
            indiceMusica = (indiceMusica - 1 + totalMusicas) % totalMusicas;
            lcd.setCursor(0, 1);
            lcd.print("                ");
            lcd.setCursor(0, 1);
            exemplo = "-> " + musicas[indiceMusica];
            lcd.print(exemplo);
        } 
        else if (botao == 1) {  // descer música
            indiceMusica = (indiceMusica + 1) % totalMusicas;
            lcd.setCursor(0, 1);
            lcd.print("                "); 
            lcd.setCursor(0, 1);
            exemplo = "-> " + musicas[indiceMusica];
            lcd.print(exemplo);
        } 
        else if (botao == 2) {  // selecionar música
            int dificuldade = escolher_dificuldade();
            comeco_jogo(indiceMusica, dificuldade);
            break;
        }
    }
}

void tocar_musica_menu() {
    int melodia[] = {262, 294, 330, 349, 392, 440, 494, 523};
  
    int duracao = 200;

    for (int i = 0; i < 8; i++) {
        tone(buzzer, melodia[i], duracao);
        delay(duracao + 50);
    }
    noTone(buzzer);
}

int escolher_dificuldade() {
    indiceDif = 0; 
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dificuldade:");
    lcd.setCursor(0, 1);
    String exemplo = "-> " + dificuldades[indiceDif];
    lcd.print(exemplo);
    
    while (true) {
        int botao = aguardarBotao();
        
        if (botao == 0) {  // subir opção
            indiceDif = (indiceDif - 1 + 3) % 3;
            lcd.setCursor(0, 1);
            lcd.print("                "); 
            lcd.setCursor(0, 1);
            exemplo = "-> " + dificuldades[indiceDif];
            lcd.print(exemplo);
        } 
        else if (botao == 1) {  // descer opção
            indiceDif = (indiceDif + 1) % 3;
            lcd.setCursor(0, 1);
            lcd.print("                ");
            lcd.setCursor(0, 1);
            exemplo = "-> " + dificuldades[indiceDif];
            lcd.print(exemplo);
        } 
        else if (botao == 2) {  // selecionar dificuldade
            return indiceDif;
        }
    }
}

bool sequencia(int idx) {
  // falta a logica pra ver se o user acertou a sequencia
  // return true se acertar
  // return false se errar
    return true; 
}

void comeco_jogo(int idx_musica, int dificuldade){
    int cond = 1;
    const int* melodia = (const int*)pgm_read_word(&musicas_notas[idx_musica]);
    int tamanho = 15;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tocando:");
    lcd.setCursor(0, 1);
    lcd.print(musicas[idx_musica]);

    for (int j = 0; j < tamanho; j++) {
        int nota = pgm_read_word(&melodia[j]);
        tone(buzzer, nota, 300);
        delay(350);
        noTone(buzzer);
        delay(100);
    }
  
    while(cond) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Sua vez:");
        if(sequencia(idx_musica)) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Voce acertou!");
        }   else {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Voce errou!");
        }
        cond = 0;
    }
}