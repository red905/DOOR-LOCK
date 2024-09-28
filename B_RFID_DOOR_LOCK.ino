//Library modul RFID
#include <SPI.h>
#include <MFRC522.h>
#define RST_PIN 9
#define SS_PIN 10
MFRC522 rfid(SS_PIN, RST_PIN);

//library LCD 16 x 2
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

//logo gembok
byte lock[8] = {
  0b01110,
  0b10001,
  0b10001,
  0b10001,
  0b11111,
  0b11011,
  0b11111,
  0b00000
};

//deklarasi pin 
const int Relay = 2;
const int Buzzer = 3;
const int ledH = 4;
const int ledM = 5;

//deklarasi nilai
int status = 1;
int countKey = 1;
int erorTap = 0;
String uidstring = "";


void setup() {
  // put your setup code here, to run once:
  pinMode(Relay, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(ledH, OUTPUT);
  pinMode(ledM, OUTPUT);

  SPI.begin();
  rfid.PCD_Init();

  lcd.init();
  lcd.backlight();
  lcd.createChar(0, lock);

  lcd.setCursor(5,0);
  lcd.print("RFID ");
  lcd.write(0);

  lcd.setCursor(0,1);
  lcd.print(" Access Control ");

  delay(5000);
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.setCursor(0,0);
  lcd.print("TAP YOUR CARD!");

  lcd.setCursor(0,1);
  lcd.print("ID : ");
  lcd.setCursor(13,1);
  lcd.print("S:");
  lcd.setCursor(15,1);
  lcd.print(status);

  //cek apakah ada kartu yg dekat dengan reader
  if(!rfid.PICC_IsNewCardPresent()||rfid.PICC_ReadCardSerial()){
    return;
  }

  //Baca UID dan simpan dalam bentuk string
  uidstring = "";
  for(byte i = 0; i < rfid.uid.size; i++){
    uidstring += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    uidstring += String(rfid.uid.uidByte[i], HEX);
  }
  uidstring.toUpperCase();

  //jika kartu ditap ke reader buzzer bunyi
  tone(Buzzer, 1000);
  delay(200);
  noTone(Buzzer);

  //jika id bener atau salah
  if(uidstring == "8482BC71"){
    countKey++;
    if(countKey > 2){
      countKey = 1;
    }
    selenoid();
  }else{
    digitalWrite(ledM, HIGH);
    delay(1000);
    digitalWrite(ledM, LOW);
    lcd.setCursor(4,1);
    lcd.print("INVALID ");
    delay(400);
    lcd.clear();

    erorTap++;
    if(erorTap >= 3){
      for(int i = 3; i > 0; i++){
        digitalWrite(ledM, HIGH);
        lcd.clear();
        lcd.print("Tunggu 3 Detik");
        lcd.setCursor(7,1);
        lcd.print(i);
        delay(3000);
        erorTap = 0;
        digitalWrite(ledM, LOW);
        lcd.clear();
      }
    }

  }

  delay(2000);
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void selenoid(){
  switch(countKey){
    case 1:
    digitalWrite(Relay, LOW);
    digitalWrite(ledH, LOW);
    status = 1; // pengunci aktif
    lcd.setCursor(4,1);
    lcd.print(uidstring);
    delay(400);
    lcd.clear();
    lcd.print("   VALID ID !");
    lcd.setCursor(0,1);
    lcd.print("     CLOSE    ");
    delay(600);
    lcd.clear();
    break;

    case 2:
     digitalWrite(Relay, HIGH);
    digitalWrite(ledH, HIGH);
    status = 0; //pengunci terbuka
    lcd.setCursor(4,1);
    lcd.print(uidstring);
    delay(400);
    lcd.clear();
    lcd.print("   VALID ID !");
    lcd.setCursor(0,1);
    lcd.print("     OPEN    ");
    delay(600);
    lcd.clear();
    break;

    default:
    countKey = 1;
    status = 1;
    break;
  }
}
