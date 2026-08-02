#include <LiquidCrystal.h>
#include <Servo.h>

const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

Servo gateServo;
const int servoPin = 9; 
const int redPin = A1;   
const int greenPin = A2; 
const int bluePin = A3;  

String inputData = "";
bool stringComplete = false;

void setRGB(int r, int g, int b) {
  digitalWrite(redPin, r);
  digitalWrite(greenPin, g);
  digitalWrite(bluePin, b);
}

String getJsonKeyValue(String json, String key) {
  int pos = json.indexOf("\"" + key + "\"");
  if (pos == -1) return "";
  int colonPos = json.indexOf(":", pos);
  if (colonPos == -1) return "";
  int startQuote = json.indexOf("\"", colonPos);
  if (startQuote == -1) return "";
  int endQuote = json.indexOf("\"", startQuote + 1);
  if (endQuote == -1) return "";
  return json.substring(startQuote + 1, endQuote);
}

void setup() {
  Serial.begin(115200); 
  inputData.reserve(200);
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  setRGB(HIGH, LOW, LOW); 

  lcd.begin(16, 2);
  lcd.print("Scan Your Card");
  
  gateServo.attach(servoPin);
  gateServo.write(0); 
}

void loop() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    
    if (inChar == '\n' || inChar == '\r') {
      if (inputData.length() > 0) { 
        stringComplete = true;
      }
    } else {
      inputData += inChar; 
    }
  }

  if (stringComplete) {
    inputData.trim();

    if (inputData.indexOf("\"ir\"") != -1) {
      String irStatus = getJsonKeyValue(inputData, "ir");
      if (irStatus == "1") {
        lcd.clear();
        lcd.print("Waiting...");
        lcd.setCursor(0, 1);
        lcd.print("Object in Doorway");
        setRGB(LOW, LOW, HIGH); 
        gateServo.write(90);    
      } 
      else if (irStatus == "0") {
        setRGB(HIGH, LOW, LOW);
        gateServo.write(0);    
        lcd.clear();
        lcd.print("Scan Your Card");
      }
    }
    else if (inputData.indexOf("\"action\"") != -1) {
      String action = getJsonKeyValue(inputData, "action");
      String allocatedSlot = getJsonKeyValue(inputData, "slot");
      action.trim();
      allocatedSlot.trim();

      lcd.clear();
      if (action.indexOf("entry") != -1) {
        lcd.print("Welcome! Entry");
        lcd.setCursor(0, 1);
        lcd.print("Slot: " + allocatedSlot);
      } 
      else if (action.indexOf("exit") != -1) {
        lcd.print("Goodbye! Exit");
        lcd.setCursor(0, 1);
        lcd.print(allocatedSlot + " Vacated");
      }
      
      setRGB(LOW, HIGH, LOW); 
      gateServo.write(90);    
    }

    // ریست کردن رشته برای داده‌های بعدی
    inputData = "";
    stringComplete = false;
  }
}