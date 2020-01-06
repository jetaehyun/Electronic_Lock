 #include <Keypad.h>
#include <LiquidCrystal.h>

#define releaseLock 13

enum state {
  wait,
  check,
  newPassword
};

const byte ROWS = 4;
const byte COLS = 4;
LiquidCrystal lcd(A4, A5, A3, A2, A1, A0);
int attemptTries = 3;
bool lockMessage = true;
int isReset = 0;                                          // use to change password
state s = wait;                                           // use to change state of system

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {10, 9, 8, 7};
byte colPins[COLS] = {6, 5, 4, 3};


Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
int masterPassword[4] = {1, 2, 3, 4};;                    // master user defined password change

void setup() {
  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Lock Activated");
  lcd.setCursor(0, 1);
  lcd.print("Pass: 1, 2, 3, 4");
  lcd.setCursor(0, 0);
  systemDelay(4000);
  lcd.clear();

  pinMode(releaseLock, OUTPUT);

}

void loop() {

  int userPassword[4];
  bool isUserCorrect = false;
  char start = customKeypad.getKey();

  switch (s) {
    case wait:
      if (lockMessage) {
        lockMessage = false;
        lcd.print("System Locked..");
      }
      if (start) {
        s = check;
        lockMessage = true;
        lcd.clear();
        break;
      }
      break;
    case check:
      inputPassword(userPassword);
      if (isReset == 1) {                              //exit case and enter new password case
        s = newPassword;
        break;
      } else if (isReset == -1) {
        s = wait;
        isReset = 0;
        break;
      }
      isUserCorrect = checkPassword(userPassword);
      lcd.clear();
      if (isUserCorrect) {
        attemptTries = 3;
        lcd.print("Disengaging lock..");
        digitalWrite(releaseLock, HIGH);
        systemDelay(5000);                                //5 open for 5 seconds
        digitalWrite(releaseLock, LOW);
        lcd.clear();
      } else {
        attemptTries -= 1;
        lcd.print("Incorrect...");
        lcd.setCursor(0, 1);
        lcd.print("Tries left:");
        lcd.print(attemptTries);
        systemDelay(3000);
        lcd.clear();
        if (attemptTries == 0) {
          attemptTries = 3;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("LOCKED!!!");
          systemDelay(15000);
        }
      }
      s = wait;
      break;
    case newPassword:
      lcd.clear();
      lcd.print("Enter Old Pass..");
      systemDelay(3000);
      inputPassword(userPassword);
      if (checkPassword(userPassword) == false) {
        lcd.clear();
        lcd.print("Incorrect...");
        systemDelay(3000);
        lcd.clear();
        s = wait;
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
      s = wait;
      break;

  }
}

/*
    function gets user's input. Checks for appropriate button press and for reset password state
    @param int* password, pointer to first element of array (user's input)
    @return void, nothing
*/
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
          isReset = 1;
          break;
        } else {
          isReset = -1;
          lcd.clear();
          break;
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

/*
    function is to check the user's input to master password
    @param int* userPassword, pointer to first element of array (user's input array)
    @return isCorrect, false or true depending on case
*/
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

/*
    function to print out array
    @pararm int* pArray, pointer to first element of array
    @return void, nothing
*/
void printIt(int* pArray) { // For debugging purposes....
  lcd.clear();
  lcd.setCursor(0, 0);
  for (int i = 0; i < 4; i++) {
    lcd.write(*(pArray + i));
  }
  systemDelay(3000);
}

/*
    function is used to reset array to default value of 1, 2, 3, and 4 of corresponding elements
    @param int* password, pointer to first element of array
    @return void, nothing
*/
void reset(int* password) { // reset to 1, 2, 3, 4
  for (int i = 1; i < 5; i++) {
    *(password + i) = i;
  }
}

/*
    function to get a user's button response
    @return char value of button press
*/
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

/*
    function uses switch case to return int value of char
    @return int
*/
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

/*
    function is used to pause system. Purpose is to avoid using Delay()
    @param int wait, 1000 = 1 sec
    @return nothing
*/
void systemDelay(int wait) {
  long unsigned int timeStamp = millis();
  while ((millis() - timeStamp) < wait) { // wait designated time
    // do nothing
  }
}
