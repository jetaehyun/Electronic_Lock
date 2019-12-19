#include <Keypad.h>
#include <LiquidCrystal.h>

const byte ROWS = 4; 
const byte COLS = 4; 
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

char hexaKeys[ROWS][COLS] = {
  {'A', '3', '2', '1'},
  {'B', '6', '5', '4'},
  {'C', '9', '8', '7'},
  {'D', '#', '0', '*'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup(){
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  lcd.begin(16, 2);
  lcd.setCursor(0,1);
  lcd.print("Lock Activated");
}
  
void loop(){
  char customKey = customKeypad.getKey();
  digitalWrite(13, LOW);
  if (customKey){
    Serial.println(customKey);
    digitalWrite(13, HIGH);
    delay(3000);
  }

  
}
