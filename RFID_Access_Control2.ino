#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

MFRC522 rfid(SS_PIN, RST_PIN);

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


const int RelaySelenoid = 2;
const int Buzzer = 3;
const int ledM = 5;
const int ledH = 4;

int status = 1;
int rset;
String uidstring = "";
int vl_rset = 0;
int  countKey = 1;
int erorTapCard = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //pinMode(pbReset, INPUT_PULLUP);
  pinMode(RelaySelenoid, OUTPUT);
  pinMode(ledM, OUTPUT);
  pinMode(ledH, OUTPUT);
  pinMode(Buzzer, OUTPUT);

  SPI.begin();
  rfid.PCD_Init();

  lcd.init();
  lcd.backlight();
  lcd.createChar(0, lock);

  lcd.setCursor(5,0);
  lcd.print("RFID ");
  lcd.write(0);

  lcd.setCursor(0, 1);
  lcd.print(" Access Control");

  delay(5000);
  lcd.clear();

}

void loop() {
  // put your main code here, to run repeatedly:
  

  //Serial.print("Kunci: ");
  Serial.println(uidstring);
  //rset = digitalRead(pbReset);

 
  lcd.setCursor(0,0);
  lcd.print("TAP YOUR CARD!");
  
  lcd.setCursor(0,1);
  lcd.print("ID :");
   lcd.setCursor(13,1);
   lcd.print("S:");
   lcd.setCursor(15,1);
   lcd.print(status);
  //cek apakah ada kartu yang dekat dengan reader
  if(!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()){
    return;
  }

  //Baca UID dan simpan dalam bentuk string
  uidstring = "";
  for(byte i = 0; i<rfid.uid.size; i++){
    uidstring += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    uidstring += String(rfid.uid.uidByte[i], HEX);
  }
  uidstring.toUpperCase();

  tone(Buzzer, 1000); // Bunyikan buzzer dengan frekuensi 1000 Hz
  delay(200);             // Bunyikan selama 500ms
  noTone(Buzzer);
  lcd.setCursor(5,1);

 
  if(uidstring == "8482BC71"){
    countKey++;
    if(countKey > 2){
      countKey = 1;
    }
    selenoidKondisi();
  }else{
     digitalWrite(ledM, HIGH);
    delay(1000);
    digitalWrite(ledM, LOW);
    lcd.print("INVALID    ");
    delay(400);
    lcd.clear();

     erorTapCard++;
    if(erorTapCard >= 3){
      for(int i = 3; i > 0; i--){
         digitalWrite(ledM, HIGH);
        lcd.clear();
        lcd.print("Tunggu 3 detik");
        lcd.setCursor(7,1);
        lcd.print(i);
        delay(3000);
        erorTapCard = 0;
         digitalWrite(ledM, LOW);
        lcd.clear();
      }
    }
  }
  
   
  delay(2000);
  //Hentikan komunikasi dengan kartu 
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

}



void selenoidKondisi(){
  switch(countKey){
    case 1:
    digitalWrite(RelaySelenoid, LOW);
    digitalWrite(ledH, LOW);
    //digitalWrite(RelaySelenoid, LOW);
    status = 1; // karena ngunci
    lcd.setCursor(4,1);
    lcd.print(uidstring );
    delay(400);
    lcd.clear();
    lcd.print("   VALID ID !");
    lcd.setCursor(0, 1);
    lcd.print("     CLOSE   ");
    delay(400); 
    lcd.clear(); 
    break;

    case 2:
     digitalWrite(RelaySelenoid, HIGH);
    digitalWrite(ledH, HIGH);
    //digitalWrite(RelaySelenoid, HIGH);
    status = 0; // karena open
    lcd.setCursor(4,1);
    lcd.print(uidstring );
    delay(400);
    lcd.clear();
    lcd.print("    VALID ID !");
    lcd.setCursor(0, 1);
    lcd.print("      OPEN   ");
    delay(400); 
    lcd.clear(); 
    break;

    default:
    countKey = 1;
     status = 1; 
    break;
  }
}


