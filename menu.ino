/*
Brayan Estiven Benavides Murcia
Cesar Paz Payán
*/

#include <LiquidCrystal.h>
#include <LiquidMenu.h>
#include <Keypad.h>
//#include "AsyncTaskLib.h"
#include <EasyBuzzer.h>
#include <EEPROM.h>

#define BUZZER_PASIVO 6
#define LED_RED 40
#define LED_GREEN 41
#define LED_BLUE 42
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

  const byte ROWS = 4;
  const byte COLS = 4;
  char keys[ROWS][COLS] = {
      {'1', '2', '3', 'A'},
      {'4', '5', '6', 'B'},
      {'7', '8', '9', 'C'},
      {'*', '0', '#', 'D'}};
  byte rowPins[ROWS] = {22, 24, 26, 28};
  byte colPins[COLS] = {30, 32, 34, 36};
  Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//void tarea();
//AsyncTask asyncTask(300, true, tarea);

/**
 * Valores por defecto de los umbrales de temperatura y luz, y valores maximos de temperatura y luz
 * Valor de simulacion de sensor de temperatura DHT11
 */
#define DEFAULT_TEMPHIGH 29
#define DEFAULT_TEMPLOW 26
#define DEFAULT_LUZHIGH 800
#define DEFAULT_LUZLOW 300
#define TEST_TEMP 20
#define MAX_TEMP 125
#define MAX_LIGTH 1023

int umbrTempHigh = DEFAULT_TEMPHIGH, umbrTempLow = DEFAULT_TEMPLOW, umbrLuzHigh = DEFAULT_LUZHIGH, umbrLuzLow = DEFAULT_LUZLOW;

int leerKeypad();
void editar_valor(String titulo, byte *varimp);
void color(unsigned char red, unsigned char green, unsigned char blue);

  LiquidScreen *lastScreen = nullptr;

  LiquidLine screen_1_line_1(0, 0, "1.UmbTempHigh");
  LiquidLine screen_1_line_2(0, 1, "2.UmbTempLow");
  LiquidScreen screen_1(screen_1_line_1, screen_1_line_2);

  LiquidLine screen_2_line_1(0, 0, "2.UmbTempLow");
  LiquidLine screen_2_line_2(0, 1, "3.UmbLuzHigh");
  LiquidScreen screen_2(screen_2_line_1, screen_2_line_2);

  LiquidLine screen_3_line_1(0, 0, "3.UmbLuzHigh");
  LiquidLine screen_3_line_2(0, 1, "4.UmbLuzLow");
  LiquidScreen screen_3(screen_3_line_1, screen_3_line_2);

  LiquidLine screen_4_line_1(0, 0, "4.UmbLuzLow");
  LiquidLine screen_4_line_2(0, 1, "5.Reset");
  LiquidScreen screen_4(screen_4_line_1, screen_4_line_2);

  LiquidLine screen_5_line_1(0, 0, "");
  LiquidLine screen_5_line_2(0, 1, "");
  LiquidScreen screen_5(screen_5_line_1, screen_5_line_2);

  LiquidMenu menu(lcd, screen_1, screen_2, screen_3, screen_4);

int leerKeypad() {
  String strNumber = "";
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '*') {
        break;
      } else if (key > 'A' && key < 'D') {
        continue;
      } else if (key >= '0' && key <= '9') {
        strNumber += key;
        lcd.print(key);
      }
    }
  }
  return strNumber.toInt();
}

void color(unsigned char red, unsigned char green, unsigned char blue)  // the color generating function
{
  analogWrite(LED_RED, red);
  analogWrite(LED_BLUE, blue);
  analogWrite(LED_GREEN, green);
}
/*
void guardar() {
  EEPROM.put(0, umbrTempHigh);
  EEPROM.put(10, umbrTempLow);
}

void recuperar(int ) {
  EEPROM.get(0, umbrTempHigh);
  EEPROM.get(10,  umbrTempLow);
}
*/
void editar_valor(String titulo, int *varimp) {
  char pressedKey;
  menu.change_screen(&screen_5);
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print(titulo);
  lcd.setCursor(0, 1);
  lcd.print(*varimp);
  lcd.print(" edit=Press 0");  
  while ((pressedKey = keypad.getKey()) != '*' && pressedKey == NO_KEY && pressedKey != '#') {
  }
  if (pressedKey == '#') {
    menu.change_screen(lastScreen);
    return;
  }
  int number = leerKeypad();
  if (varimp == &umbrTempLow && number < umbrTempHigh || varimp == &umbrTempHigh && number > umbrTempLow && number <= MAX_TEMP) {
    *varimp = number;
    menu.change_screen(lastScreen);
    return;
  }

  if (varimp == &umbrLuzLow && number < umbrLuzHigh || varimp == &umbrLuzHigh && number > umbrLuzLow && number <= MAX_LIGTH) {
    *varimp = number;
    menu.change_screen(lastScreen);
    return;
  }
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("Error press \"*\"");
  while ((pressedKey = keypad.getKey()) != '*' && pressedKey == NO_KEY) {
  }

  menu.change_screen(lastScreen);
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16,2);
  menu.add_screen(screen_5);
  pinMode(BUZZER_PASIVO, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  EasyBuzzer.setPin(BUZZER_PASIVO);
//  asyncTask.Start();
  screen_1_line_1.attach_function(1, []() {
    editar_valor("UmbTempHigh", &umbrTempHigh);
  });
  screen_1_line_2.attach_function(1, []() {
    editar_valor("UmbTempLow", &umbrTempLow);
  });

  screen_2_line_1.attach_function(1, []() {
    editar_valor("UmbTempLow", &umbrTempLow);
  });
  screen_2_line_2.attach_function(1, []() {
    editar_valor("UmbLuzHigh", &umbrLuzHigh);
  });

  screen_3_line_1.attach_function(1, []() {
    editar_valor("UmbLuzHigh", &umbrLuzHigh);
  });
  screen_3_line_2.attach_function(1, []() {
    editar_valor("UmbLuzLow", &umbrLuzLow);
  });

  screen_4_line_1.attach_function(1, []() {
    editar_valor("UmbLuzLow", &umbrLuzLow);
  });
  screen_4_line_2.attach_function(1, []() {
    menu.change_screen(&screen_5);
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print("\"*\" to confirm ");
    lcd.setCursor(0, 1);
    lcd.print("\"#\" to cancel  ");
    char pressedKey;
    while ((pressedKey = keypad.getKey()) != '*' && pressedKey == NO_KEY && pressedKey != '#') {
    }
    if (pressedKey == '#') {
      menu.change_screen(lastScreen);
      return;
    }
    umbrTempHigh = DEFAULT_TEMPHIGH;
    umbrTempLow = DEFAULT_TEMPLOW;
    menu.change_screen(lastScreen);
  });
  menu.update();
}

void loop() {
//  asyncTask.Update();
EasyBuzzer.update();

  char key = keypad.getKey();
  if (key == 'A') {
    if (menu.get_currentScreen() != &screen_1) {
      menu.previous_screen();
    }
  } else if (key == 'D') {
    if (menu.get_currentScreen() != &screen_4) {
      menu.next_screen();
    }
  } else if (key == '#') {
    menu.switch_focus();
  } else if (key == '*') {
    lastScreen = menu.get_currentScreen();
    menu.call_function(1);
  }
  if (TEST_TEMP > umbrTempHigh) {
//    EEPROM.put(0, umbrTempHigh);
    color(255, 0, 0);
 //   delay(1000);
    EasyBuzzer.singleBeep(300,500);
  } else if (TEST_TEMP < umbrTempLow) {
//    EEPROM.put(10, umbrTempLow);
    color(0, 0, 255);
  //  delay(1000);
    EasyBuzzer.stopBeep();
  } else {
    color(0, 255, 0);
//    delay(1000);
    EasyBuzzer.stopBeep();
  }
}
