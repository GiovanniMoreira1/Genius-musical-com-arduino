#include <LiquidCrystal.h>
#include <avr/pgmspace.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define NOTA_C 262
#define NOTA_E 330
#define NOTA_G 392
#define NOTA_C_HIGH 523

const int buzzer = 13;
const int botoes[4] = {A0, A1, A2, A3};
const int ledAcerto = 10; // Atualizado conforme sua montagem
const int ledErro = 9;

int vidas = 5;
int pontuacao = 0;

byte nota1[8] = { B00100, B00100, B01110, B11111, B01110, B00100, B00100, B00000 };
byte nota2[8] = { B00000, B00100, B00100, B01110, B11111, B01110, B00100, B00100 };
byte seta[8] = {
  B00100,
  B00110,
  B11111,
  B11111,
  B00110,
  B00100,
  B00000,
  B00000
};


byte nota[8] = {
  B00100,
  B00110,
  B00101,
  B00101,
  B00101,
  B11110,
  B01100,
  B00000
};

byte lampada[8] = {
  B00100,
  B01110,
  B01110,
  B01110,
  B00100,
  B00000,
  B00100,
  B00000
};

byte icone_loop[8] = {
  B00011,
  B00100,
  B01000,
  B10011,
  B10100,
  B01000,
  B00100,
  B00011
};


byte tecla[8] = {
  B11111,
  B10001,
  B10101,
  B10001,
  B11111,
  B00000,
  B00000,
  B00000
};

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

// Menu principal
enum ModoJogo {
  MODO_MUSICA = 0,
  MODO_GENIUS = 1,
  MODO_ADAPT = 2,
  MODO_TREINO = 3
};

String opcoesMenu[] = {
  "\x02 Musica",      // 0 → nota musical
  "\x03 Genius",      // 1 → lâmpada
  "\x04 Adaptativo",  // 2 → loop
  "\x05 Treino"       // 3 → piano
};


int totalOpcoes = 4;
int indiceModo = 0;

void setup() {
  pinMode(buzzer, OUTPUT);
  pinMode(ledAcerto, OUTPUT);
  pinMode(ledErro, OUTPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(botoes[i], INPUT);
  }
  
  
  lcd.createChar(0, coracao);   // ❤️
  lcd.createChar(1, seta);      // seta de seleção
  lcd.createChar(2, nota);      // 🎵
  lcd.createChar(3, lampada);   // 💡
  lcd.createChar(4, icone_loop);      // 🔁
  lcd.createChar(5, tecla);     // 🎹
  lcd.createChar(6, nota1);
  lcd.createChar(7, nota2);



  Serial.begin(9600);
  lcd.begin(16, 2);

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
  mostrarMenuPrincipal();
}

void mostrarMenuPrincipal() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Escolha o modo:");
  lcd.setCursor(0, 1);
  lcd.write(byte(1)); // seta
  lcd.print(" " + opcoesMenu[indiceModo]);

  while (true) {
    int botao = aguardarBotao();

    if (botao == 0) {
      indiceModo = (indiceModo - 1 + totalOpcoes) % totalOpcoes;
    } else if (botao == 1) {
      indiceModo = (indiceModo + 1) % totalOpcoes;
    } else if (botao == 2) {
      executarModoSelecionado(indiceModo);
      return;
    }
   lcd.setCursor(0, 1);
   lcd.print("                ");
   lcd.setCursor(0, 1);
   lcd.write(byte(1)); // imprime o caractere da seta
   lcd.print(" ");
   lcd.print(opcoesMenu[indiceModo]);

  }
}

void executarModoSelecionado(int modo) {
  switch (modo) {
    case MODO_MUSICA:
      mostrarMenu();
      break;
    case MODO_GENIUS:
      iniciarGeniusClassico();
      break;
    case MODO_ADAPT:
      iniciarModoAdaptativo();
      break;
    case MODO_TREINO:
      modo_treino();
      break;
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
      if (dificuldade != -1) {
        comeco_jogo(indiceMusica, dificuldade);
      }
      break;
    } else if (botao == 3) {
      // Retornar ao menu principal
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

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Iniciando...");

  // Loop de música + animação
  for (int i = 0; i < 8; i++) {
    tone(buzzer, melodia[i], duracao);

    // Mostra animação de notas na linha de baixo
    lcd.setCursor(0, 1);
    for (int j = 0; j < 8; j++) {
      lcd.write((i + j) % 2 == 0 ? 6 : 7);  // alterna nota1 e nota2
    }

    delay(duracao + 50);
    noTone(buzzer);
  }

  delay(500);
  lcd.clear();
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
    } else if (botao == 3) {
      // Cancelar escolha e voltar
      return -1;
    }

    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("-> " + dificuldades[indiceDif]);
  }
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

void mostrarVidasComCoracoes(int qtd) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Vidas:");
  lcd.setCursor(0, 1);
  for (int i = 0; i < 5; i++) {
    if (i < qtd) lcd.write(byte(0));
    else lcd.print(" ");
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
  digitalWrite(ledAcerto, LOW);
  digitalWrite(ledErro, LOW);

  if (acertou) {
    lcd.print("Voce acertou!");
    digitalWrite(ledAcerto, HIGH);
    delay(300);
    pontuacao += 10;
    mostrarPontuacao();
    if (vidas < 5) vidas++;
  } else {
    lcd.print("Voce errou!");
    digitalWrite(ledErro, HIGH);
    vidas--;
  }

  delay(1000);
  digitalWrite(ledAcerto, LOW);
  digitalWrite(ledErro, LOW);

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
    if (notaJogador != sequenciaOriginal[i]) return false;
    tone(buzzer, notaJogador, 300);
    delay(350);
    noTone(buzzer);
  }
  return true;
}

void modo_treino() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(byte(1)); // seta
  lcd.print(" " + opcoesMenu[indiceModo]);
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

void iniciarGeniusClassico() {
  const int MAX_SEQUENCIA = 20;
  int sequencia[MAX_SEQUENCIA];
  int tamanho = 1;
  int notaJogador;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Modo Genius");
  delay(1500);

  randomSeed(analogRead(A5)); // gerar sequências aleatórias

  while (true) {
    // Gera nova sequência
    for (int i = 0; i < tamanho; i++) {
      sequencia[i] = random(0, 4); // valores de 0 a 3
    }

    // Toca sequência
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Memorize...");
    delay(1000);
    for (int i = 0; i < tamanho; i++) {
      int botao = sequencia[i];
      int nota = 0;
      switch (botao) {
        case 0: nota = NOTA_C; break;
        case 1: nota = NOTA_E; break;
        case 2: nota = NOTA_G; break;
        case 3: nota = NOTA_C_HIGH; break;
      }

      tone(buzzer, nota, 300);
      digitalWrite(ledAcerto, HIGH);
      delay(300);
      noTone(buzzer);
      digitalWrite(ledAcerto, LOW);
      delay(250);
    }

    // Aguarda o jogador repetir
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sua vez!");
    delay(500);

    for (int i = 0; i < tamanho; i++) {
      int botaoPressionado = aguardarBotao();

      switch (botaoPressionado) {
        case 0: notaJogador = NOTA_C; break;
        case 1: notaJogador = NOTA_E; break;
        case 2: notaJogador = NOTA_G; break;
        case 3: notaJogador = NOTA_C_HIGH; break;
      }

      // Confere se o botão está correto
      if (botaoPressionado != sequencia[i]) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Errou! Fim de jogo");
        digitalWrite(ledErro, HIGH);
        tone(buzzer, 100, 1000);
        delay(2000);
        digitalWrite(ledErro, LOW);
        noTone(buzzer);
        return; // volta ao menu
      }

      // Toca nota de confirmação
      tone(buzzer, notaJogador, 300);
      digitalWrite(ledAcerto, HIGH);
      delay(300);
      noTone(buzzer);
      digitalWrite(ledAcerto, LOW);
      delay(100);
    }

    // Acertou toda sequência
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Boa!");
    delay(250);
    pontuacao += 1;
    mostrarPontuacao();
    delay(1000);

    tamanho++;
    if (tamanho > MAX_SEQUENCIA) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Vitoria total!");
      pontuacao += 50;
      mostrarPontuacao();
      delay(3000);
      return;
    }
  }
}

// ================================
// Músicas para Modo Adaptativo (Pop/Famosas) - Versões Estendidas
// ================================
// Cada valor representa o botão:
// 0: A0 (NOTA_C)
// 1: A1 (NOTA_E)
// 2: A2 (NOTA_G)
// 3: A3 (NOTA_C_HIGH)

// Runaway (Kanye West) – 8 notas (mantido)
const int adapt_runaway[] PROGMEM = {
  3, 0, 1, 0, 2, 0, 1, 3
};

// Someone Like You (Adele) – 12 notas (repetido + variação)
const int adapt_adele[] PROGMEM = {
  3, 3, 1, 0, 1, 3,
  1, 0, 1, 3, 3, 1
};

// Billie Jean (Michael Jackson) – 14 notas
const int adapt_billie_jean[] PROGMEM = {
  1, 2, 1, 0, 1, 2, 0,
  1, 0, 1, 2, 1, 0, 1
};

// Shape of You (Ed Sheeran) – 13 notas
const int adapt_shape_of_you[] PROGMEM = {
  1, 1, 2, 1, 3, 1, 0,
  1, 2, 1, 3, 1, 0
};

// Clocks (Coldplay) – 14 notas
const int adapt_clocks[] PROGMEM = {
  0, 1, 3, 1, 0, 1, 3,
  0, 1, 3, 1, 0, 1, 3
};

// Lista de músicas adaptativas
const int* const musicas_adaptativas[] PROGMEM = {
  adapt_runaway,
  adapt_adele,
  adapt_billie_jean,
  adapt_shape_of_you,
  adapt_clocks
};

String nomes_adaptativas[] = {
  "Runaway",
  "Someone Like You",
  "Billie Jean",
  "Shape of You",
  "Clocks"
};

const int totalMusicasAdaptativas = 5;

// ======= MENU DE SELEÇÃO PARA MÚSICAS ADAPTATIVAS =======

// ===== DEBUGADO: MENU DE SELEÇÃO PARA MÚSICAS ADAPTATIVAS =====
int mostrarMenuAdaptativo() {
  int indiceAdapt = 0;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Musica Adaptativa");
  lcd.setCursor(0, 1);
  lcd.write(byte(1)); // seta personalizada
  lcd.print(" ");
  lcd.print(nomes_adaptativas[indiceAdapt]);

  while (true) {
    int botao = aguardarBotao();

    if (botao == 0) {
      indiceAdapt = (indiceAdapt - 1 + totalMusicasAdaptativas) % totalMusicasAdaptativas;
    } else if (botao == 1) {
      indiceAdapt = (indiceAdapt + 1) % totalMusicasAdaptativas;
    } else if (botao == 2) {
      Serial.print("Selecionado: ");
      Serial.println(indiceAdapt);
      return indiceAdapt; // Selecionar
    } else if (botao == 3) {
      Serial.println("Cancelado");
      return -1; // Cancelar
    }

    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.write(byte(1));
    lcd.print(" ");
    lcd.print(nomes_adaptativas[indiceAdapt]);
  }
}

// ===== DEBUGADO: NOVO MODO ADAPTATIVO COM MÚSICA REAL =====
void iniciarModoAdaptativo() {
  const int NIVEL_MINIMO = 3;
  int progresso = NIVEL_MINIMO;
  int sequencia[20];
  int tamanhoTotal = 0;

  int indice = mostrarMenuAdaptativo();
  if (indice < 0) return;

  Serial.print("Carregando musica indice: ");
  Serial.println(indice);

  const int* melodia = (const int*)pgm_read_word(&musicas_adaptativas[indice]);
  Serial.println("Ponteiro da melodia carregado.");

  for (int i = 0; i < 20; i++) {
    int valor = pgm_read_word(&melodia[i]);
    if (valor < 0 || valor > 3) {
      Serial.print("Fim da musica em i=");
      Serial.println(i);
      break;
    }
    sequencia[i] = valor;
    tamanhoTotal++;
  }

  Serial.print("Tamanho da musica carregada: ");
  Serial.println(tamanhoTotal);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Adaptativo:");
  lcd.setCursor(0, 1);
  lcd.print(nomes_adaptativas[indice]);
  delay(2000);

  while (progresso <= tamanhoTotal) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Nivel: ");
    lcd.print(progresso);
    delay(800);

    for (int i = 0; i < progresso; i++) {
      int nota = 0;
      switch (sequencia[i]) {
        case 0: nota = NOTA_C; break;
        case 1: nota = NOTA_E; break;
        case 2: nota = NOTA_G; break;
        case 3: nota = NOTA_C_HIGH; break;
        default: nota = 0; break;
      }

      tone(buzzer, nota, 300);
      digitalWrite(ledAcerto, HIGH);
      delay(300);
      noTone(buzzer);
      digitalWrite(ledAcerto, LOW);
      delay(200);
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sua vez!");
    delay(300);

    bool errou = false;

    for (int i = 0; i < progresso; i++) {
      int botaoPressionado = aguardarBotao();

      if (botaoPressionado != sequencia[i]) {
        errou = true;
        break;
      }

      int notaJogador;
      switch (botaoPressionado) {
        case 0: notaJogador = NOTA_C; break;
        case 1: notaJogador = NOTA_E; break;
        case 2: notaJogador = NOTA_G; break;
        case 3: notaJogador = NOTA_C_HIGH; break;
      }

      tone(buzzer, notaJogador, 300);
      digitalWrite(ledAcerto, HIGH);
      delay(300);
      noTone(buzzer);
      digitalWrite(ledAcerto, LOW);
      delay(100);
    }

    if (errou) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Errou!");
      lcd.setCursor(0, 1);
      lcd.print("Nivel: ");
      lcd.print(progresso);
      digitalWrite(ledErro, HIGH);
      tone(buzzer, 100, 500);
      delay(1000);
      digitalWrite(ledErro, LOW);
      noTone(buzzer);
      progresso--;
      if (progresso < NIVEL_MINIMO) progresso = NIVEL_MINIMO;
      continue;
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Acertou!");
    delay(800);
    progresso++;
  }

  // 🎉 Música final completa
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Completou!");
  lcd.setCursor(0, 1);
  lcd.print(nomes_adaptativas[indice]);
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Recompensa:");
  lcd.setCursor(0, 1);
  pontuacao += 30;
  mostrarPontuacao();
  lcd.print("Tocando...");

  for (int i = 0; i < tamanhoTotal; i++) {
    int nota = 0;
    switch (sequencia[i]) {
      case 0: nota = NOTA_C; break;
      case 1: nota = NOTA_E; break;
      case 2: nota = NOTA_G; break;
      case 3: nota = NOTA_C_HIGH; break;
      default: nota = 0; break;
    }

    tone(buzzer, nota, 300);
    digitalWrite(ledAcerto, HIGH);
    delay(300);
    noTone(buzzer);
    digitalWrite(ledAcerto, LOW);
    delay(100);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Fim da musica!");
  delay(2000);
}

void mostrarPontuacao() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pontuacao:");
  lcd.setCursor(0, 1);
  lcd.print(String(pontuacao) + " pontos");
  delay(2500);
}



