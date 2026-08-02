#include <SPI.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <LiquidCrystal.h>
#include <WiFi.h>        
#include <HTTPClient.h> 


const char* ssid = "wifi-ssid";          
const char* password = "wifi-pass";  

const char* serverUrl = "http://server-ip:5000/api/gate-log"; 
const char* telemetryUrl = "http://server-ip:5000/api/telemetry";

unsigned long lastTelemetryTime = 0;
const unsigned long telemetryInterval = 600000; 

const char* mockPlates[10] = {
  "22B456-77", "33C789-11", "12D345-99", "65J987-55", "88D111-22",
  "45G888-21", "78B999-33", "19M432-88", "50S112-44", "91A777-66"
};

const int rs = 1, en = 2, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int servoPin = 14;
const int ledcChannel = 0;
const int freq = 50;
const int resolution = 14;

const int sdaPin = 10;
const int rstPin = 9;
const int avoidPin = 8; 

const int redPin = 15;
const int greenPin = 16;
const int bluePin = 17;

MFRC522DriverPinSimple ss_pin(sdaPin);
MFRC522DriverSPI driver{ss_pin};
MFRC522 mfrc522{driver};

int angleToDuty(int angle) {
  long minDuty = (500 * 16383) / 20000;
  long maxDuty = (2400 * 16383) / 20000;
  return minDuty + ((maxDuty - minDuty) * angle / 180);
}

String getSystemTime() {
  unsigned long ms = millis();
  unsigned long secs = ms / 1000;
  int h = secs / 3600;
  int m = (secs % 3600) / 60;
  int s = secs % 60;
  char buf[10];
  sprintf(buf, "%02d:%02d:%02d", h, m, s);
  return String(buf);
}

void setRGB(int r, int g, int b) {
  digitalWrite(redPin, r);
  digitalWrite(greenPin, g);
  digitalWrite(bluePin, b);
}

bool isObstacleDetected() {
  int temp = 0; 
  for (int i = 0; i < 10; i++) {
    int sensorState = digitalRead(avoidPin);
    if (sensorState == LOW) {
      temp += 1;
    }
    delay(5);
  }
  if (temp >= 3) {
    return true;
  } else {
    return false;
  }
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

void sendTelemetryData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(telemetryUrl);
    http.addHeader("Content-Type", "application/json");
    
    float mockTemp = random(220, 285) / 10.0; 
    float mockHum = random(400, 550) / 10.0;  
    String currentTime = getSystemTime();
    
    String payload = "{\"temperature\":" + String(mockTemp) + 
                     ", \"humidity\":" + String(mockHum) + 
                     ", \"time\":\"" + currentTime + "\"}";
                     
    http.POST(payload);
    http.end();
  }
}

void setup() {
  Serial.begin(115200); 

  randomSeed(analogRead(0));

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  setRGB(HIGH, LOW, LOW);

  pinMode(avoidPin, INPUT_PULLUP);

  lcd.begin(16, 2);
  lcd.print("System Booting...");
  delay(1000);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  pinMode(rstPin, OUTPUT);
  digitalWrite(rstPin, HIGH);

  SPI.begin();
  mfrc522.PCD_Init();

  ledcAttachChannel(servoPin, freq, resolution, ledcChannel);
  ledcWriteChannel(ledcChannel, angleToDuty(0));

  lcd.clear();
  lcd.print("Scan Your Card");
}

void loop() {
  if (millis() - lastTelemetryTime >= telemetryInterval) {
    lastTelemetryTime = millis();
    sendTelemetryData();
  }

  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  String uidString = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uidString += "0";
    uidString += String(mfrc522.uid.uidByte[i], HEX);
  }
  uidString.toUpperCase();

  String currentTime = getSystemTime();
  int randomIndex = random(0, 10);
  String selectedPlate = String(mockPlates[randomIndex]);

  String jsonPayload = "{\"uid\":\"" + uidString + 
                       "\", \"time\":\"" + currentTime + 
                       "\", \"license_plate\":\"" + selectedPlate + "\"}";
  
  String serverResponse = "";
  
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    
    int httpResponseCode = http.POST(jsonPayload);
    if (httpResponseCode == 200) {
      serverResponse = http.getString();
    }
    http.end();
  }

  if (serverResponse != "") {
    Serial.println(serverResponse); 
    
    String action = getJsonKeyValue(serverResponse, "action");
    String allocatedSlot = getJsonKeyValue(serverResponse, "slot");
    
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
    else {
      lcd.print("Act:" + action.substring(0, 12));
      lcd.setCursor(0, 1);
      lcd.print("Slot:" + allocatedSlot.substring(0, 11));
    }

    setRGB(LOW, HIGH, LOW); 
    ledcWriteChannel(ledcChannel, angleToDuty(90));
    
    delay(5000);

    int test = isObstacleDetected();
    if (test) {
      Serial.println("{\"ir\":\"1\"}"); 
      
      lcd.clear();
      lcd.print("Waiting...");
      lcd.setCursor(0, 1);
      lcd.print("Object in Doorway");
      
      setRGB(LOW, LOW, HIGH); 
      while (test) {
        delay(3000);
        test = isObstacleDetected();
      }
    }
    
    Serial.println("{\"ir\":\"0\"}"); 
    
  } else {
    lcd.clear();
    lcd.print("Conn Error or");
    lcd.setCursor(0, 1);
    lcd.print("Parking Full!");
    delay(3000);
  }

  setRGB(HIGH, LOW, LOW);
  ledcWriteChannel(ledcChannel, angleToDuty(0));

  lcd.clear();
  lcd.print("Scan Your Card");

  mfrc522.PICC_HaltA();
}