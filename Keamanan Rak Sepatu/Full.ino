#include <SPI.h>
#include <Wire.h>
#include <Servo.h>
#include <Keypad.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>
#include <LiquidCrystal_I2C.h>

Servo myservo;
MFRC522 mfrc522(53, 48); // MFRC522 mfrc522(SS_PIN, RST_PIN)
SoftwareSerial mySerial(10, 11);
LiquidCrystal_I2C lcd(0x27, 16, 2); 

const byte rows = 4;
const byte columns = 4;

char hexaKeys[rows][columns] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte row_pins[rows] = {9,8,7,6};
byte column_pins[columns]= {5,4,3,2};

Keypad keypad_key = Keypad( makeKeymap(hexaKeys), row_pins, column_pins, rows, columns);
  
uint8_t i = 0; 
char password[4];  
char key_pressed = 0;
String tagUID = "B9 BD FE E2";
char initial_password[4] = {'1', '2', '1', '2'};   

int ir1;
int ir = 26;
int LEDmerah = 24;
int LEDhijau = 25;

int status_tampilan=0;
boolean IrOn = false;
boolean RFIDMode = false; 
boolean KeyMode = false; 

void setup () 
{
  Serial.begin (9600);
  mySerial.begin (9600);

  while (!Serial);           
  SPI.begin();                
  mfrc522.PCD_Init();

  
  pinMode(ir, INPUT);
  
  mp3_set_serial (mySerial);   
  delay(1);  
  mp3_set_volume (25);

  pinMode(LEDhijau, OUTPUT);  
  pinMode(LEDmerah, OUTPUT);  
  myservo.attach(22); 
    
  digitalWrite(LEDhijau, LOW);
  digitalWrite(LEDmerah, LOW);

  lcd.init();
  lcd.backlight();

  myservo.write(0);
}

void loop ()
{
  if (status_tampilan==0){
    lcd.setCursor(0,0);
             //0123456789012345
    lcd.print("SISTEM  KEAMANAN");
    lcd.setCursor(0,1);
              //0123456789012345
    lcd.print("   RAK SEPATU   ");
    delay(2000);
    status_tampilan=1;
    IrOn = true;
  }
  
  if (IrOn == true)
  {
    lcd.setCursor(0,0);
             //0123456789012345
    lcd.print("Dekatkan  Tangan");
    lcd.setCursor(0,1);
             //0123456789012345
    lcd.print("  Pada  Sensor  ");

    ir1 = digitalRead(ir);
  
    if (ir1 == LOW){
      mp3_play(1);
      delay (6000);
      IrOn = false;
      RFIDMode = true;
    }
  }

  if (RFIDMode == true) {
    lcd.setCursor(0,0);
             //0123456789012345
    lcd.print(" Tempelkan  Tag ");
    lcd.setCursor(0,1);
             //0123456789012345
    lcd.print("   RFID Anda!   ");

    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      return;
    }

    if ( ! mfrc522.PICC_ReadCardSerial()) {
      return;
    }

    String tag = "";
    for (byte j = 0; j < mfrc522.uid.size; j++) {
      tag.concat(String(mfrc522.uid.uidByte[j] < 0x10 ? " 0" : " "));
      tag.concat(String(mfrc522.uid.uidByte[j], HEX));
    }
    tag.toUpperCase();

    if (tag.substring(1) == tagUID) {
      //mp3_play(2);
      lcd.clear();
      lcd.setCursor(0,0);
               //0123456789012345
      lcd.print("Akses di Terima ");
      mp3_play(2);
      delay(3000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Masukan Password");
      lcd.setCursor(0,1);
      lcd.print("Pass: ");
      RFIDMode = false; 
      KeyMode = true;
    }
    else {
      lcd.clear();
      lcd.setCursor(0,0);
               //0123456789012345
      lcd.print(" Akses di Tolak ");
      mp3_play(5);
      delay(3000);
      lcd.clear();
    }
  }

  if (KeyMode == true) {
      key_pressed = keypad_key.getKey(); 
      if (key_pressed) {
      password[i++] = key_pressed;
      lcd.print("*");
    }
    if (i == 4) {
      delay(200);
      if (!(strncmp(password, initial_password, 4))) {
        lcd.clear();
        lcd.setCursor(0,0);  
        lcd.print(" PASSWORD  BENAR ");
        lcd.setCursor(0,1);   
        lcd.print("                " );
        delay(20);
        digitalWrite(LEDhijau, HIGH);
        digitalWrite(LEDmerah, LOW);
        mp3_play(3);
        myservo.write(90);
        delay(8000);  
        myservo.write(0);
        digitalWrite(LEDhijau, LOW);
        i = 0;
        KeyMode = false;
        IrOn = true; 
      }
      else {
        Serial.print('A');
        myservo.write(0);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(" PASSWORD  SALAH ");
        lcd.setCursor(0,1);   
        lcd.print("  COBA KEMBALI  " );
        delay(20);
        digitalWrite(LEDmerah, HIGH);
        digitalWrite(LEDhijau, LOW); 
        mp3_play(4);
        lcd.clear();
        i = 0;
        lcd.print("Masukan Password");
        lcd.setCursor(0,1);
        lcd.print("Pass: "); 
      }
    }
  }
}
