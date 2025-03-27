#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int buzzer = 13;
const int botoes[4] = {A0, A1, A2, A3};
const int ledAcerto = 10;
const int ledErro = 9;

// variáveis de ambiente
int nivel = 1;
int vidas = 3;

// lista das músicas
String musicas[] = {"Mario", "Star Wars", "Pirates"};


int totalMusicas = 3;
int indiceMusica = 0; 

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
    //tocar_musica_menu();
    mostrarMenu();
}

void loop() {
    mostrarMenu();
	int botao = aguardarBotao();
    while (true) {
      Serial.println(botao);
        if (botao == 0) {  // subir música
            indiceMusica = (indiceMusica - 1 + totalMusicas) % totalMusicas;
            return;
        } 
        else if (botao == 1) {  // descer música
             indiceMusica = (indiceMusica + 1) % totalMusicas;
            return;
        } 
        else if (botao == 2) {  // selecionar música
            tocarMusica(indiceMusica);
            return;
        }
    }
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

void tocarMusica(int i) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tocando:");
    lcd.setCursor(0, 1);
    lcd.print(musicas[i]);


    tone(buzzer, 500, 500);
    delay(500);
    noTone(buzzer);

    delay(1000);
}