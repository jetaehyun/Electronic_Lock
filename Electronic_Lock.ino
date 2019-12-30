#include <Keypad.h>
#include <LiquidCrystal.h>

enum state {
  savePower,
  check,
  newPassword
};

const byte ROWS = 4;
const byte COLS = 4;
LiquidCrystal lcd(A4, A5, A0, A1, A2, A3);
volatile state s = savePower;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};


Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
int ogPassword[4] = {1, 2, 3, 4};

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 0);
  lcd.setCursor(0, 1);
  lcd.print("Lock Activated...");
  systemDelay(1000);
  lcd.clear();
}

void loop() {
  char customKey = customKeypad.getKey();
  if (customKey) {
    Serial.println(customKey);
    lcd.print(customKey);
    systemDelay(1000);
    lcd.clear();
  }
}

bool checkPassword(int* userPassword) {
  bool isCorrect = false;
  lcd.clear();
  if (isCorrect) {
    lcd.print("The password was correct...");
    systemDelay(1000);
    lcd.clear();
    return true;
  }
  lcd.print("Incorrect password...");
  systemDelay(1000);
  lcd.clear();
  return false;
}
void printIt() { // For debugging purposes....
  for (int i = 0; i < 4; i++) {
    lcd.print(*(ogPassword + i));
  }
}

void reset(int* password) {
  for (int i = 0; i < 4; i++) {
    *(password + i) = 0;
  }
}

void systemDelay(int wait) {
  long unsigned int timeStamp = millis();
  while ((millis() - timeStamp) < wait) {
    // do nothing
  }
}
