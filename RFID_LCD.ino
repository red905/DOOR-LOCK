#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

// Inisialisasi LCD I2C (alamat I2C bisa berbeda, misal 0x27 atau 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin untuk RFID RC522
#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  // Inisialisasi Serial Monitor
  Serial.begin(9600);
  
  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();
  
  // Inisialisasi SPI untuk komunikasi dengan RFID
  SPI.begin();
  rfid.PCD_Init();
  
  // Menampilkan pesan awal di LCD
  lcd.setCursor(0, 0);
  lcd.print("Scan your card");
}

void loop() {
  // Cek apakah ada kartu yang dekat dengan reader
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Tampilkan UID dari kartu RFID di Serial Monitor
  Serial.print("UID: ");
  String uidString = "";
  
  // Baca UID dan simpan dalam bentuk string
  for (byte i = 0; i < rfid.uid.size; i++) {
    uidString += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    uidString += String(rfid.uid.uidByte[i], HEX);
  }
  
  uidString.toUpperCase();  // Ubah UID menjadi huruf besar
  Serial.println(uidString);  // Tampilkan di Serial Monitor

  // Tampilkan UID pada LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("UID: ");
  lcd.setCursor(0, 1);
  lcd.print(uidString);

  delay(2000);  // Tampilkan selama 2 detik sebelum kembali ke loop
  
  // Hentikan komunikasi dengan kartu
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
