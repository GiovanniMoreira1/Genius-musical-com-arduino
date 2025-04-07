// Jogo funcionando completamente, com as musicas e o modo treino.


#include <LiquidCrystal.h>
#include <avr/pgmspace.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define NOTA_C 262
#define NOTA_E 330
#define NOTA_G 392
#define NOTA_C_HIGH 523

const int buzzer = 13;
const int botoes[4] = {A0, A1, A2, A3};
const int ledAcerto = 10;
const int ledErro = 9;

int vidas = 5;

byte coracao[8] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000
};

const int musica_tetris[] PROGMEM = {
  NOTA_E, NOTA_E, NOTA_E, NOTA_C,
  NOTA_G, NOTA_G, NOTA_E, NOTA_E, 
  NOTA_C, NOTA_C, NOTA_G, NOTA_G,
  NOTA_E, NOTA_C_HIGH, NOTA_G
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
  lcd.createChar(0, coracao); // cria caractere coração

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Genius Musical");
  lcd.setCursor(0, 1);
  lcd.print("Bem-vindo!");
  delay(3000);

  lcd.clear();
  tocar_musica_menu();
  delay(1000);
  lcd.clear();
}

void loop() {
  mostrarMenu();
}

int aguardarBotao() {
  while (true) {
    for (int i = 0; i < 4; i++) {
      if (digitalRead(botoes[i]) != LOW) {
        delay(200);
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
  lcd.print("-> " + musicas[indiceMusica]);

  while (true) {
    int botao = aguardarBotao();
    if (botao == 0) {
      indiceMusica = (indiceMusica - 1 + totalMusicas) % totalMusicas;
    } else if (botao == 1) {
      indiceMusica = (indiceMusica + 1) % totalMusicas;
    } else if (botao == 2) {
      int dificuldade = escolher_dificuldade();
      comeco_jogo(indiceMusica, dificuldade);
      break;
    } else if (botao == 3) {
      modo_treino();
      return;
    }

    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("-> " + musicas[indiceMusica]);
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
  lcd.print("-> " + dificuldades[indiceDif]);

  while (true) {
    int botao = aguardarBotao();
    if (botao == 0) {
      indiceDif = (indiceDif - 1 + 3) % 3;
    } else if (botao == 1) {
      indiceDif = (indiceDif + 1) % 3;
    } else if (botao == 2) {
      return indiceDif;
    }
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("-> " + dificuldades[indiceDif]);
  }
}

void mostrarVidasComCoracoes(int qtd) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Vidas:");
  lcd.setCursor(0, 1);
  for (int i = 0; i < 5; i++) {
    if (i < qtd) {
      lcd.write(byte(0));
    } else {
      lcd.print(" ");
    }
  }
}

void comeco_jogo(int idx_musica, int dificuldade) {
  vidas = 5;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tocando:");
  lcd.setCursor(0, 1);
  lcd.print(musicas[idx_musica]);

  int maxDesejado = dificuldade == 0 ? 5 : (dificuldade == 1 ? 10 : 15);
  const int* melodia = (const int*)pgm_read_word(&musicas_notas[idx_musica]);

  int sequenciaOriginal[15];
  int notasValidas = 0;
  int j = 0;

  // Printar no Serial (cheat)
  Serial.println();
  Serial.print("[MUSICA: ");
  Serial.print(musicas[idx_musica]);
  Serial.print("] [Dificuldade: ");
  Serial.print(dificuldades[dificuldade]);
  Serial.println("]");
  Serial.println("Sequencia esperada:");

  while (notasValidas < maxDesejado) {
    int nota = pgm_read_word(&melodia[j]);
    if (nota == 0 && melodia[j] == 0xFFFF) break;

    if (nota != 0) {
      sequenciaOriginal[notasValidas] = nota;

      String botaoStr = "??";
      if (nota == NOTA_C) botaoStr = "A0";
      else if (nota == NOTA_E) botaoStr = "A1";
      else if (nota == NOTA_G) botaoStr = "A2";
      else if (nota == NOTA_C_HIGH) botaoStr = "A3";

      Serial.print(botaoStr);
      Serial.print(" ");

      tone(buzzer, nota, 300);
      notasValidas++;
    }

    delay(350);
    noTone(buzzer);
    delay(100);
    j++;
  }

  Serial.println();

  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sua vez!");

  bool acertou = sequencia(sequenciaOriginal, notasValidas);

  lcd.clear();
  lcd.setCursor(0, 0);
  if (acertou) {
    lcd.print("Voce acertou!");
    digitalWrite(ledAcerto, HIGH);
    delay(500);
    digitalWrite(ledAcerto, LOW);
    if (vidas < 5) vidas++;
  } else {
    lcd.print("Voce errou!");
    digitalWrite(ledErro, HIGH);
    delay(500);
    digitalWrite(ledErro, LOW);
    vidas--;
  }

  delay(1500);
  mostrarVidasComCoracoes(vidas);
  delay(2000);

  if (vidas == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Game Over");
    delay(2500);
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Fim da rodada");
    delay(1500);
  }
}

bool sequencia(int sequenciaOriginal[], int tamanho) {
  for (int i = 0; i < tamanho; i++) {
    int botao = aguardarBotao();
    int notaJogador;

    switch (botao) {
      case 0: notaJogador = NOTA_C; break;
      case 1: notaJogador = NOTA_E; break;
      case 2: notaJogador = NOTA_G; break;
      case 3: notaJogador = NOTA_C_HIGH; break;
      default: return false;
    }

    if (notaJogador != sequenciaOriginal[i]) {
      return false;
    }

    tone(buzzer, notaJogador, 300);
    delay(350);
    noTone(buzzer);
  }
  return true;
}

void modo_treino() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Modo Treino");
  lcd.setCursor(0, 1);
  lcd.print("Toque notas");

  int ultimoBotao = -1;

  while (true) {
    int botao = aguardarBotao();

    if (botao == ultimoBotao) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Saindo...");
      delay(1000);
      break;
    }

    ultimoBotao = botao;

    int nota;
    String nomeNota;

    switch (botao) {
      case 0: nota = NOTA_C; nomeNota = "Do"; break;
      case 1: nota = NOTA_E; nomeNota = "Mi"; break;
      case 2: nota = NOTA_G; nomeNota = "Sol"; break;
      case 3: nota = NOTA_C_HIGH; nomeNota = "Do+"; break;
      default: nota = 0; nomeNota = "???"; break;
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Botao A" + String(botao));
    lcd.setCursor(0, 1);
    lcd.print("Nota: " + nomeNota);

    tone(buzzer, nota, 300);
    delay(350);
    noTone(buzzer);
  }
}
