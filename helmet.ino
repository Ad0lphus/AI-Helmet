

// include the library code:
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>

SoftwareSerial gsmSerial(9, 10);
SoftwareSerial gpsSerial(14,15);

#define TILT_PIN 42
#define LED_PIN 8
#define RST_CNT_PIN 21
#define TOUCH_SENSOR 36
#define FAN       32

TinyGPS gps;

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

volatile bool intOccured = false;

struct stGPSCoordinates
{
  float latitude;
  float longitude;
};

stGPSCoordinates gpsCoord;

void setup() {
  //initialisation
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  gsmSerial.begin(9600);   // Setting the Baud Rate for the Gsm Serial Port
  Serial.begin(9600);  //Setting the Serial Baud Rate for Serial Monitor
  gpsSerial.begin(9600); //Setting the baud rate for the GPS Serial Communication

  Serial.println("Successfully set up GSM and LCD");

  pinMode(TILT_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(RST_CNT_PIN, INPUT);
  pinMode(TOUCH_SENSOR,INPUT);
  pinMode(FAN,OUTPUT);

  attachInterrupt(digitalPinToInterrupt(RST_CNT_PIN), onSwitchPress, LOW);

  //print Welcome message
  lcd.clear();
  Serial.println("Entered LCD print");
  lcd.print("Welcome!!");
  delay(3000);
  lcd.setCursor(0, 1);
 
  lcd.print("Pls Wear Helmet");
  delay(5000);
  
  while(digitalRead(TOUCH_SENSOR) != LOW)
  {

  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Have Safe Jrney");
  digitalWrite(FAN,HIGH);
}

//void fillGPSCoordinates(stGPSCoordinates& gpsCoord)
//{
//  float lat = 0.0,lng = 0.0;
//  Serial.println("GPS Method Entered");
//   while(gpsSerial.available())
//   {
//    Serial.println("GPS Signal Available");
//      if(gps.encode(gpsSerial.read()))
//      {
//        gps.f_get_position(&lat,&lng);
//        gpsCoord.latitude = lat;
//        gpsCoord.longitude = lng;
//        String latitude = String(lat,6); 
//        String longitude = String(lng,6); 
//        Serial.println(latitude+" : "+longitude);
//      }
//   }
//}

void sendMessagetoPhone()
{
 // fillGPSCoordinates(gpsCoord);
  
  gsmSerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1 second
  gsmSerial.println("AT+CMGS=\"+917994558908\"\r"); // Replace x with mobile number
  delay(1000);
  gsmSerial.println("your vehicle has undergone accident @ https://goo.gl/maps/CBu6y5v8eGM2 .(the above info may not be intact due to errors. )");// The SMS text you want to send
  delay(100);
  gsmSerial.println((char)26);// ASCII code of CTRL+Z for saying the end of sms to  the module
  delay(1000);
}

void switchPressHandler()
{
   intOccured = true;
   digitalWrite(LED_PIN, LOW); // set the LED pin LOW and buzzer will stop buzz
   digitalWrite(FAN,HIGH);
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print(F("Ready to Go.."));
}

long debouncing_time = 15;
volatile unsigned long last_micros = 0;

void onSwitchPress()
{
  if((long)(micros() - last_micros) >=  (debouncing_time*1000))
  {
     switchPressHandler();
     last_micros = micros();
  }
}

bool isPersonOK()
{
  lcd.clear();
  lcd.setCursor(0, 0);

  lcd.print(F("R U OK?U Hve 15s"));

  delay(100);
  digitalWrite(LED_PIN, HIGH); // set the LED pin HIGH and buzzer will buzz

  int nCount = 15;

  char sBuf[16] = "Count :";

  while (nCount > 0)
  {
    if (intOccured)
    {
      return true;
    }
    nCount--;
    sprintf(sBuf, "Count : %d", nCount);
    if (nCount < 10)
    {
      sBuf[9] = ' ';
    }
    lcd.setCursor(0, 1);
    lcd.print(sBuf);
    delay(1000);
  }
  return false;
}

void loop()
{
  int nTiltStatus = digitalRead(TILT_PIN);
  intOccured = false;
  if (nTiltStatus == LOW)
  {
    digitalWrite(FAN,LOW);
    Serial.println(F("Tilt Detected and Entered"));
    if (!isPersonOK())
    {
      sendMessagetoPhone();
      delay(100);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("Message Sent"));
      delay(100);
    }
    
  }

  digitalWrite(LED_PIN, LOW); // Set the LED pin LOW to turn it OFF or buzzer OFF

  delay(500);
}
