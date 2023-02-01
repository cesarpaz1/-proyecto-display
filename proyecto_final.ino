/*
SISTEMA DE SEGURIDAD 
    - BRAYAN BENAVIDES
    - JORGE AYERBE
	- CESAR PAZ
*/

/********************************************//**
 * Inclusion de librerias
 ***********************************************/
#include <LiquidCrystal.h>
#include <LiquidMenu.h>
#include <Keypad.h>
#include <AsyncTaskLib.h>
#include "DHTStable.h"
#include <EEPROM.h>

/********************************************//**
 * Declarion de pines de rgb, buzzer, sensor de
 * luz, sensor de temperatura analogico
 ***********************************************/
#define BUZZER_PIN 6
#define DHT11_PIN 13
#define photo 10
const int LED_RED = 40;
const int LED_GREEN = 41;
const int LED_BLUE = 42;

/********************************************//**
 * Posiciones en memoria para los datos
 ***********************************************/
#define DATOS_LUZHIGH 0
#define DATOS_LUZLOW 2
#define DATOS_TEMPHIGH 4
#define DATOS_TEMPLOW 6

/********************************************//**
 * Declaracion de variables auxiliares
 ***********************************************/
bool band = false;
int estado = 0;
DHTStable DHT;
unsigned long tiempo;
#define beta 4090 //the beta of the thermistor
float temperatura;
int luz;

#define DEFAULT_TEMPHIGH 29
#define DEFAULT_TEMPLOW 26
#define DEFAULT_LUZHIGH 190
#define DEFAULT_LUZLOW 150

int umbrTempHigh = DEFAULT_TEMPHIGH, umbrTempLow = DEFAULT_TEMPLOW;
int umbrLuzHigh = DEFAULT_LUZHIGH, umbrLuzLow = DEFAULT_LUZLOW;

int counter=1;

/********************************************//**
 * Declaracion de variables para el keypad
 ***********************************************/
const byte ROWS = 4; 
const byte COLS = 4; 

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {22, 24, 26, 28}; 
byte colPins[COLS] = {30, 32, 34, 36}; 
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

/********************************************//**
 * Declaracion de variable para la contrasenia
 ***********************************************/
String password = "";
String passwordMask = "";
const String clave = "8989";
int contKeyPress = 0;
int Intentos = 0;
int iniKeyPress = 0;

/********************************************//**
 * Pines para el lcd
 ***********************************************/
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/********************************************//**
 * Declaracion de variables para el control del
 * buzzer analogico
 ***********************************************/
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
int duraciones[] = { 8, 8, 4, 4 };    // array con la duracion de cada nota
int melodia[] = { NOTE_B6, NOTE_C7, NOTE_CS7, NOTE_D7 };//// array con las notasa a reproducir

/********************************************//**
 * Declaracion de procedimientos
 ***********************************************/
void color(unsigned char red, unsigned char green, unsigned char blue);
void imprimirLCD(String cadena);
void verificarContrasenia();
void contarCadena();
void sonar_buzz();

/********************************************//**
 * Declaracion de tareas asincronicas
 ***********************************************/
AsyncTask keyPress(4000, true, []() { contarCadena(); });

/********************************************//**
 * Creacion del LiquidMenu
 ***********************************************/
LiquidLine line1(1, 0, "1.UmbralHighTemp");
LiquidLine line2(1, 1, "2.UmbralLowTemp");
LiquidLine line3(1, 1, "3.UmbralLuzHigh");
LiquidLine line4(1, 1, "4.UmbralLuzLow");
LiquidLine line5(1, 1, "5.Reset");

LiquidLine linea1_pant1(1, 0, "HighUmbralTemp");
LiquidLine linea2_pant1(7, 1, umbrTempHigh);

LiquidLine linea1_pant2(1, 0, "LowUmbralTemp");
LiquidLine linea2_pant2(7, 1, umbrTempLow);

LiquidLine linea1_pant3(1, 0, "HighUmbralLuz");
LiquidLine linea2_pant3(7, 1, umbrLuzHigh);

LiquidLine linea1_pant4(2, 0, "LowUmbralLuz");
LiquidLine linea2_pant4(7, 1, umbrLuzLow);

LiquidLine linea1_pantT(1, 0, "  Ingrese el ");
LiquidLine linea2_pantT(1, 1, " nuevo valor. ");

LiquidLine linea1_pantE(1, 0, "Error");

LiquidScreen pantalla1m;

LiquidScreen pantalla1;
LiquidScreen pantalla2;
LiquidScreen pantalla3;
LiquidScreen pantalla4;
LiquidScreen pantallaT;
LiquidScreen pantallaE;

LiquidMenu menu(lcd);

/*F**************************************************************************
* NAME: obtenerDatoMemoria
*----------------------------------------------------------------------------
* PARAMS:	ninguno
* return:   nada
*----------------------------------------------------------------------------
* PURPOSE: Obtiene el dato de EEPROM y lo envia a la posicion de memoria
*----------------------------------------------------------------------------
* NOTE:
*****************************************************************************/
void obtenerDatoMemoria(){
  EEPROM.get(DATOS_TEMPHIGH,umbrTempHigh); 
  EEPROM.get(DATOS_TEMPLOW,umbrTempLow);
  EEPROM.get(DATOS_LUZHIGH,umbrLuzHigh);
  EEPROM.get(DATOS_LUZLOW,umbrLuzLow);
}

/*F**************************************************************************
* NAME: obtenerMemoria
*----------------------------------------------------------------------------
* PARAMS:	ninguno
* return:   nada
*----------------------------------------------------------------------------
* PURPOSE: Escribe los datos en EEPROM teniendo encuenta la posicion de memoria
*----------------------------------------------------------------------------
* NOTE: 
*****************************************************************************/
void escribirMemoria(){
  EEPROM.put(DATOS_TEMPHIGH,umbrTempHigh); 
  EEPROM.put(DATOS_TEMPLOW,umbrTempLow);
  EEPROM.put(DATOS_LUZHIGH,umbrLuzHigh);
  EEPROM.put(DATOS_LUZLOW,umbrLuzLow); 
}

/*F**************************************************************************
* NAME: umbralTempHigh
*----------------------------------------------------------------------------
* PARAMS:	ninguno
* return:   nada
*----------------------------------------------------------------------------
* PURPOSE: Modifica el valor de umbrTempHigh con el valor ingresado en el keypad
*----------------------------------------------------------------------------
* NOTE: El valor ingresado debe ser mayor a 29 y menor a 60
*****************************************************************************/
void umbralTempHigh(){
  menu.change_screen(6);
  delay(1000);
  menu.change_screen(2);
  String temp="";
  tiempo = millis() + 4000;
  while(temp.length() < 2 && millis() < tiempo){
    char key = keypad.getKey();
    if (key){
      if(key != '#' && key != '*' && key != 'A' &&key != 'B' && key != 'C' && key != 'D'){
        temp = temp + key;
      }
    }
  }
  if(temp.length() != 0){
    verificarTempHigh(temp.toInt());
    if(temp.toInt() > 35){
      Serial.println("Estado de alarma");
      color(255, 0, 0);
      sonar_buzz();
      delay(1000);
    }
  }
}

/*F**************************************************************************
* NAME: umbralTempLow
*----------------------------------------------------------------------------
* PARAMS:	ninguno
* return:   nada
*----------------------------------------------------------------------------
* PURPOSE: Modifica el valor de umbrTempLow con el valor ingresado en el keypad
*----------------------------------------------------------------------------
* NOTE: El valor ingresado debe ser menor a 26 y mayor a 0
*****************************************************************************/
void umbralTempLow(){
  menu.change_screen(6);
  delay(1000);
  menu.change_screen(3);
  String temp = "";
  tiempo = millis() + 4000;
  while(temp.length() < 2 && millis() < tiempo){
    char key = keypad.getKey();
    if (key){
      if(key != '#' && key != '*' && key != 'A' && key != 'B' && key != 'C' && key != 'D'){
        temp = temp + key;
      }
    }
  }
  if(temp.length() != 0){
    verificarTempLow(temp.toInt());
  }
}

/*F**************************************************************************
* NAME: umbralLuzHigh
*----------------------------------------------------------------------------
* PARAMS:	ninguno
* return:   nada
*----------------------------------------------------------------------------
* PURPOSE: Modifica el valor de umbrLuzHigh con el valor ingresado en el keypad
*----------------------------------------------------------------------------
* NOTE: El valor ingresado debe ser mayor a 190 y menor a 1023
*****************************************************************************/
void umbralLuzHigh(){
  menu.change_screen(6);
  delay(1000);
  menu.change_screen(4);
  String temp = "";
  tiempo = millis() + 4000;
  while(temp.length() < 3 && millis() < tiempo){
    char key = keypad.getKey();
    if (key){
      if(key != '#' && key != '*' && key != 'A' && key != 'B' && key != 'C' && key != 'D'){
        temp = temp + key;
      }
    }
  }
  if(temp.length() != 0){
    verificarLuzHigh(temp.toInt());
  }
}

/*F**************************************************************************
* NAME: umbralTempLow
*----------------------------------------------------------------------------
* PARAMS:	ninguno
* return:   nada
*----------------------------------------------------------------------------
* PURPOSE: Modifica el valor de umbrLuzLow con el valor ingresado en el keypad
*----------------------------------------------------------------------------
* NOTE: El valor ingresado debe ser menor a 150 y mayor a 0
*****************************************************************************/
void umbralLuzLow(){
  menu.change_screen(6);
  delay(1000);
  menu.change_screen(5);
  String temp = "";
  tiempo = millis() + 4000;
  while(temp.length() < 3 && millis() < tiempo){
    char key = keypad.getKey();
    if (key){
      if(key != '#' && key != '*' && key != 'A' && key != 'B' && key != 'C' && key != 'D'){
        temp = temp + key;
      }
    }
  }
  if(temp.length() != 0){
    verificarLuzLow(temp.toInt());
  }
}

/*F**************************************************************************
* NAME: reset
*----------------------------------------------------------------------------
* PARAMS:	ninguno
* return:   nada
*----------------------------------------------------------------------------
* PURPOSE: Modifica el valor de umbrTempHigh, umbrTempLow, umbrLuzLow, y
*          umbrLuzLow por sus valores por defecto
*----------------------------------------------------------------------------
* NOTE: 
*****************************************************************************/
void reset(){
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
    menu.change_screen(1);
    return;
  }
  umbrTempHigh = DEFAULT_TEMPHIGH;
  umbrTempLow = DEFAULT_TEMPLOW;
  umbrLuzHigh = DEFAULT_LUZHIGH;
  umbrLuzLow = DEFAULT_LUZLOW;
  escribirMemoria();
}

void setup() {
  int dato;
  EEPROM.get(0,dato);
  if(dato==-1){
    escribirMemoria();
  }
  obtenerDatoMemoria();

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT); 
  pinMode(LED_BLUE, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  keyPress.Start();

  Serial.begin(9600);

  lcd.begin(16, 2);

  pantalla1m.add_line(line1);
  pantalla1m.add_line(line2);
  pantalla1m.add_line(line3);
  pantalla1m.add_line(line4);
  pantalla1m.add_line(line5);

  line1.set_focusPosition(Position::LEFT);
  line2.set_focusPosition(Position::LEFT);
  line3.set_focusPosition(Position::LEFT);
  line4.set_focusPosition(Position::LEFT);
  line5.set_focusPosition(Position::LEFT);

  line1.attach_function(1, umbralTempHigh);
  line2.attach_function(2, umbralTempLow);
  line3.attach_function(3, umbralLuzHigh);
  line4.attach_function(4, umbralLuzLow);
  line5.attach_function(5, reset);

  pantalla1.add_line(linea1_pant1);
  pantalla1.add_line(linea2_pant1);

  pantalla2.add_line(linea1_pant2);
  pantalla2.add_line(linea2_pant2);

  pantalla3.add_line(linea1_pant3);
  pantalla3.add_line(linea2_pant3);

  pantalla4.add_line(linea1_pant4);
  pantalla4.add_line(linea2_pant4);

  pantallaT.add_line(linea1_pantT);
  pantallaT.add_line(linea2_pantT);

  pantallaE.add_line(linea1_pantE);

  menu.add_screen(pantalla1m);
  menu.add_screen(pantalla1);
  menu.add_screen(pantalla2);
  menu.add_screen(pantalla3);
  menu.add_screen(pantalla4);
  menu.add_screen(pantallaT);
  menu.add_screen(pantallaE);

  pantalla1m.set_displayLineCount(2);
}

void loop() {
  Estados();
}

/*F**************************************************************************
* NAME: color
*----------------------------------------------------------------------------
* PARAMS:	unsigned char red, unsigned char green, unsigned char blue
* return:   nada
*----------------------------------------------------------------------------
* PURPOSE: Genera los colores azul, rojo y verde
*----------------------------------------------------------------------------
* NOTE: 
*****************************************************************************/
void color (unsigned char red, unsigned char green, unsigned char blue) {
  analogWrite(LED_RED, red);
  analogWrite(LED_BLUE, blue);
  analogWrite(LED_GREEN, green);
}

/*F**************************************************************************
* NAME: Cestado
*----------------------------------------------------------------------------
* PARAMS:	int state
* return:   nada
*----------------------------------------------------------------------------
* PURPOSE: 
*----------------------------------------------------------------------------
* NOTE: 
*****************************************************************************/
void Cestado(int state){
  estado=state;
}

/*F**************************************************************************
* NAME: Estados
*----------------------------------------------------------------------------
* PARAMS:	ninguno
* return:   nada
*----------------------------------------------------------------------------
* PURPOSE: Cambiar su funcionamiento segun el estado en que se encuentre el 
*          sistema
*----------------------------------------------------------------------------
* NOTE: 
*****************************************************************************/
void Estados(){
  char key = keypad.getKey();
  switch(estado){
    case 0:
      keyPress.Update();
      if (Intentos == 3) {
        color(255, 0, 0);
        imprimirLCD("Clave bloqueada");
        sonar_buzz();
        delay(2000);
        Intentos = 0;
        contKeyPress = 0;
        iniKeyPress = 0;
        color(0, 0, 0);
      } else if (contKeyPress == 1) {
        verificarContrasenia();
      } else if (contKeyPress == 0){
        iniKeyPress = 0;
        lcd.setCursor(0, 0);
        lcd.print("Ingrese la clave");
      }
      if (key) {
        keyPress.Reset();
        if (key != '#') {
          iniKeyPress = 1;
          password = password + key;
          passwordMask = passwordMask + "*";
        }else if(key == '#'){
          contKeyPress = 1;
        }
        lcd.setCursor(0, 1);
        lcd.print(passwordMask);
      }
      break;
    case 1: 
      color(0, 0, 0);
      if (key){
        if (key == 'A') {
          menu.switch_focus(true);
          if(counter == 6){
            counter = 1;
          }else{
            counter += 1;
          }
        }else if(key == 'D'){
          menu.switch_focus(false);
          if(counter == 0){
            counter = 5;
          }else{
            counter -= 1;
          }
        }else if (key == '*') {
          switch (counter){
            case 1:
              menu.call_function(1);
              delay(1000);
              menu.change_screen(1);
              break;
            case 2:
              menu.call_function(2);
              delay(1000);
              menu.change_screen(1);
              break;
            case 3:
              menu.call_function(3);
              delay(1000);
              menu.change_screen(1);
              break;
            case 4:
              menu.call_function(4);
              delay(1000);
              menu.change_screen(1);
              break;
            case 5:
              menu.call_function(5);
              delay(1000);
              menu.change_screen(1);
              break;
          }
        }else if(key == '#'){
          estado = 2;
          lcd.clear();
          lcd.print(" Monitoreo ");
          Serial.println("Estado de monitoreo");
        }
      }
      break;
    case 3:
      Serial.println("Estado de alarma");
      color(255, 0, 0);
      sonar_buzz();
      delay(10000);
      break;
    case 2:
      lcd.setCursor(1, 1);
	    lcd.print("T: ");
      lcd.print(temperatura);
      lcd.print(" L: ");
      lcd.print(luz);       
      if(key){
        if(key == 'D'){
          estado = 1;
          menu.change_screen(1);
        }
      }else if(band){
        estado = 3;
      }
      delay(500);
      luz = analogRead(photo);
      long a =1023-analogRead(DHT11_PIN);
      temperatura = beta / (log((1025.0 * 10 / a - 10) / 10) + beta / 298.0) - 273.0;
      if(temperatura > umbrTempHigh && luz < umbrLuzLow){
        band = true;
      }else if (temperatura > umbrTempLow && temperatura < umbrTempHigh && luz < umbrLuzHigh && luz > umbrLuzLow){
        color(0, 255, 0);
      }else{
        color(0, 0, 255);
      }
      Serial.print("temp:");
      Serial.println(temperatura);
      Serial.print("luz:");
      Serial.println(luz);
      break;
    default:
      estado=0;
      break;
  }
}

/*F**************************************************************************
* NAME: verificarContrasenia
*----------------------------------------------------------------------------
* PARAMS:	ninguno
* return:   nada
*----------------------------------------------------------------------------
* PURPOSE: Verifica si la contrasenia ingresada es correcta
*----------------------------------------------------------------------------
* NOTE: 
*****************************************************************************/
void verificarContrasenia() {
  lcd.clear();
  if (password.equals(clave)) {
    color(0,255,0);
    delay(500);
    estado = 1;
    menu.init();
    menu.update();
    menu.switch_focus();
  } else {
    Intentos++;
    color(0, 0, 255);
    imprimirLCD("Clave erronea!");
  }
  password = "";
  passwordMask = "";
  contKeyPress = 0;
  delay(1500);
}

/*F**************************************************************************
* NAME: imprimirLCD
*----------------------------------------------------------------------------
* PARAMS:	String cadena
* return:   nada
*----------------------------------------------------------------------------
* PURPOSE: Imprimir en la pantalla lcd
*----------------------------------------------------------------------------
* NOTE: 
*****************************************************************************/
void imprimirLCD(String cadena) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(cadena);
}

/*F**************************************************************************
* NAME: contarCadena
*----------------------------------------------------------------------------
* PARAMS:	ninguno
* return:   nada
*----------------------------------------------------------------------------
* PURPOSE: Cuenta los caracteres ingresados en la constrasenia
*----------------------------------------------------------------------------
* NOTE: 
*****************************************************************************/
void contarCadena() {
  if (iniKeyPress == 1) {
    contKeyPress = 4;
  }
}

/*F**************************************************************************
* NAME: sonar_buzz
*----------------------------------------------------------------------------
* PARAMS:	ninguno
* return:   nada
*----------------------------------------------------------------------------
* PURPOSE: Se encarga de hacer sonar el buzzer
*----------------------------------------------------------------------------
* NOTE: 
*****************************************************************************/
void sonar_buzz() { 
  for (int i = 0; i < 3; i++) {      
    int duracion = 1000 / duraciones[i]; 
    tone(BUZZER_PIN, melodia[i], duracion); 
    int pausa = duracion * 2.30;   
    delay(pausa);      
    noTone(BUZZER_PIN);  
  }
}

/*F**************************************************************************
* NAME: verificarTempHigh
*----------------------------------------------------------------------------
* PARAMS:	int temp
* return:   nada
*----------------------------------------------------------------------------
* PURPOSE: Verifica si se modifico el valor en umbrTempHigh
*----------------------------------------------------------------------------
* NOTE: 
*****************************************************************************/
//comprobar los la modificacion de los valores
void verificarTempHigh(int temp){
  if(temp < 60 && temp > umbrTempLow && temp > 10){
    umbrTempHigh = temp;
    escribirMemoria();
  }else{
    menu.change_screen(7);
    delay(2000);
    menu.change_screen(1);
  }
}

/*F**************************************************************************
* NAME: verificarTempLow
*----------------------------------------------------------------------------
* PARAMS:	int temp
* return:   nada
*----------------------------------------------------------------------------
* PURPOSE: Verifica si se modifico el valor en umbrTempLow
*----------------------------------------------------------------------------
* NOTE: 
*****************************************************************************/
void verificarTempLow(int temp){
  if(temp < 35 && temp < umbrTempHigh && temp > 0){
    umbrTempLow = temp;
    escribirMemoria();
  }else{
    menu.change_screen(7);
    delay(2000);
    menu.change_screen(1);
  }
}

/*F**************************************************************************
* NAME: verificarLuzHigh
*----------------------------------------------------------------------------
* PARAMS:	int luz
* return:   nada
*----------------------------------------------------------------------------
* PURPOSE: Verifica si se modifico el valor en umbrLuzHigh
*----------------------------------------------------------------------------
* NOTE: 
*****************************************************************************/
void verificarLuzHigh(int luz){
  if(luz < 1023 && luz > umbrLuzLow && luz > 150){
    umbrLuzHigh = luz;
    escribirMemoria();
  }else{
    menu.change_screen(7);
    delay(2000);
    menu.change_screen(1);
  }
}

/*F**************************************************************************
* NAME: verificarLuzLow
*----------------------------------------------------------------------------
* PARAMS:	int luz
* return:   nada
*----------------------------------------------------------------------------
* PURPOSE: Verifica si se modifico el valor en umbrLuzLow
*----------------------------------------------------------------------------
* NOTE: 
*****************************************************************************/
void verificarLuzLow(int luz){
  if(luz < 200 && luz < umbrLuzHigh&&luz >= 89){
    umbrLuzLow = luz;
    escribirMemoria();
  }else{
    menu.change_screen(7);
    delay(2000);
    menu.change_screen(1);
  }
}
