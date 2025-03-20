#include <LiquidCrystal.h>
LiquidCrystal lcd(13, 12, 11, 10, 9, 8, 7);


int pos = 0;

void setup() {
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("teste");
  pinMode(6, INPUT);
  pinMode(8, OUTPUT);
  pinMode(5, INPUT);
  pinMode(4, INPUT);
  pinMode(3, INPUT);
  pinMode(2, INPUT);
}

void loop() {
}