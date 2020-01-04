#include <Keypad.h>
#include <LiquidCrystal.h>
#include <LowPower.h>

#define releaseLock 13

enum state {
  savePower,
  check,
  newPassword
};

const byte ROWS = 4;
const byte COLS = 4;
LiquidCrystal lcd(A4, A5, A3, A2, A1, A0);
int attemptTries = 0;
bool isReset = false; // use to change password
volatile state s = check;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};


Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
int masterPassword[4] = {1, 2, 3, 4};; // master user defined password change


/*
   main things to implement:
     1. Master password to create new password (DONE)
     2. Number of attempts
*/
void setup() {
  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Lock Activated");
  systemDelay(1000);
  lcd.clear();

  pinMode(releaseLock, OUTPUT);
}

void loop() {

  int userPassword[4];
  bool isUserCorrect = false;
  
  switch (s) {
    case savePower:
      // LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); //will implement once interrupt is configured
      //      lcd.setCursor(0, 0);
      lcd.print("System is awake!");
      systemDelay(1000);
      s = check;
      break;
    case check:
      inputPassword(userPassword);
      if (isReset == true) { //exit case and enter new password case
        s = newPassword;
        break;
      }
      isUserCorrect = checkPassword(userPassword);
      lcd.clear();
      //      Serial.print(isUserCorrect);
      if (isUserCorrect) {
        lcd.print("Disengaging lock..");
        digitalWrite(releaseLock, HIGH);
        systemDelay(5000); //5 open for 5 seconds
        digitalWrite(releaseLock, LOW);
        lcd.clear();
      } else {
        lcd.print("Incorrect...");
        //implement number of attempts
        systemDelay(3000);
        lcd.clear();
        attemptTries += 1;
        if (attemptTries == 3) {
          attemptTries = 0;
          //do something...alarm
        }
      }
      //      s = savePower;
      break;
    case newPassword:
      lcd.clear();
      lcd.print("Enter Old Pass..");
      systemDelay(3000);
      inputPassword(userPassword);
      if(checkPassword(userPassword) == false) {
        lcd.clear();
        lcd.print("Incorrect...");
        systemDelay(3000);
        lcd.clear();
        s = savePower;
        break;
      }
      lcd.clear();
      isReset = false;
      lcd.print("New Password:");
      lcd.setCursor(0, 1);
      for (int i = 0; i < 4; i++) {
        *(masterPassword + i) = charToInt(getUserPress());
        if (*(masterPassword + i) == 'C') {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Reset to 1234");
          systemDelay(2000);
          reset(masterPassword);
          break;
        }
        lcd.print(*(masterPassword + i));
        if (i == 3) {
          systemDelay(500);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Password Set!");
          systemDelay(2000);
          lcd.clear();
        }
      }
      s = savePower;
      break;

  }
}

void inputPassword(int* password) {
  int inputAmt = 0;
  lcd.clear();
  lcd.print("Password:");
  while (inputAmt < 4) { // Could use Keypad's waitForKey() instead
    char key = customKeypad.getKey();
    if (key) {
      if (key == '*' && isReset == false && inputAmt == 0) {
        lcd.clear();
        lcd.print("Reset Password?");
        lcd.setCursor(0, 1);
        lcd.print("A=yes, B=no");
        char userResponse = getUserPress();
        if (userResponse == 'A') {
          isReset = true;
          break;
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Password:");
          continue;
        }
      }
      int buttonPressed = charToInt(key);
      if (buttonPressed == -1) {
        lcd.setCursor(0, 1);
        lcd.print("Incorrect Input!");
        systemDelay(2000);
        lcd.setCursor(0, 1);
        lcd.print("                ");
        continue;
      }
      lcd.setCursor(9 + inputAmt, 0);
      lcd.print(key);
      *(password + inputAmt) = buttonPressed;
      inputAmt++;
    }
  }
  systemDelay(1000);
}

bool checkPassword(int* userPassword) {
  bool isCorrect = true;
  for (int i = 0; i < 4; i++) {
    if (*(userPassword + i) != *(masterPassword + i)) {
      isCorrect = false;
      break;
    }
  }
  reset(userPassword);
  return isCorrect;

}

void printIt(int* pArray) { // For debugging purposes....
  lcd.clear();
  lcd.setCursor(0, 0);
  for (int i = 0; i < 4; i++) {
    lcd.write(*(pArray + i));
  }
  systemDelay(3000);
}

void reset(int* password) { // reset to 1, 2, 3, 4
  for (int i = 1; i < 5; i++) {
    *(password + i) = i;
  }
}


char getUserPress() {
  long unsigned int timeStamp = millis();
  while (millis() - timeStamp < 5000) { //5 sec timer
    char key = customKeypad.getKey();
    if (key) {
      return key;
    }
  }
  return 'C';
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
  while ((millis() - timeStamp) < wait) { // wait designated time
    // do nothing
  }
}
