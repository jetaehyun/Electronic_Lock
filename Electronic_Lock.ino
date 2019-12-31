#include <Keypad.h>
#include <LiquidCrystal.h>
#include <LowPower.h>

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
  switch (s) {
    case savePower:
      // LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); //will implement once interrupt is configured
      lcd.setCursor(0, 0);
      lcd.print("System is awake!");
      systemDelay(1000);
      lcd.clear();
      s = check;
      break;
    case check:
      lcd.print("Password: ");
      lcd.setCursor(0, 11);
      bool isUserCorrect = checkPassword(inputPassword());
      if (isUserCorrect) {
        / something
      } else {
        //something
      }
      break;
    case newPassword:
      break;

  }
}

int* inputPassword() {
  int inputAmt = 0;
  int* checkPassword = NULL;
  while (inputAmt < 3) { // Could use Keypad's waitForKey() instead
    char key = customKeypad.getKey();
    if (key) {
      int buttonPressed = charToInt(key);
      if (buttonPressed == -1) {
        lcd.setCursor(0, 1);
        lcd.write("Incorrect Input!");
        systemDelay(2000);
        lcd.write("                ");
        lcd.setCursor(inputAmt + 11, 0);
        continue;
      }
      lcd.print(key);
      *(checkPassword + inputAmt) = buttonPressed;
      inputAmt++;
    }
  }
  return checkPassword;
}

bool checkPassword(int* userPassword) {
  bool isCorrect = true;
  for (int i = 0; i < 4; i++) {
    if (*(userPassword + i) != *(ogPassword + i)) {
      isCorrect = false;
      break;
    }
  }
  reset(userPassword);
  return isCorrect;

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

int charToInt(char letter) {
  int number = 0;
  switch (letter) {
    case '1':
      number = 1;
      break;
    case '2':
      number = 2;
      break;
    case '3':
      number = 3;
      break;
    case '4':
      number = 4;
      break;
    case '5':
      number = 5;
      break;
    case '6':
      number = 6;
      break;
    case '7':
      number = 7;
      break;
    case '8':
      number = 8;
      break;
    case '9':
      number = 9;
      break;
    case '0':
      number = 0;
      break;
    default:
      number = -1;
      break;
  }
  return number;
}

void systemDelay(int wait) {
  long unsigned int timeStamp = millis();
  while ((millis() - timeStamp) < wait) {
    // do nothing
  }
}
