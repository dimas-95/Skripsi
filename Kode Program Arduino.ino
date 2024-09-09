#include <SoftwareSerial.h>
#include "DFRobotDFPlayerMini.h"

SoftwareSerial mySoftwareSerial(16, 17); // RX, TX
DFRobotDFPlayerMini dfPlayer;

// Deklarasi pin LED
const int redPin1 = 12;
const int redPin2 = 15;
const int yellowPin = 13;
const int greenPin1 = 14;
const int greenPin2 = 4;

// Fungsi untuk mematikan semua LED
void turnOffAllLEDs() {
  digitalWrite(redPin1, LOW);
  digitalWrite(redPin2, LOW);
  digitalWrite(yellowPin, LOW);
  digitalWrite(greenPin1, LOW);
  digitalWrite(greenPin2, LOW);
}

void setup() {
  Serial.begin(9600);
  // Inisialisasi pin LED
  pinMode(redPin1, OUTPUT);
  pinMode(redPin2, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin1, OUTPUT);
  pinMode(greenPin2, OUTPUT);

  mySoftwareSerial.begin(9600); // Inisialisasi komunikasi serial untuk DFPlayer Mini

  if (!dfPlayer.begin(mySoftwareSerial)) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1. Please recheck the connection!"));
    Serial.println(F("2. Please insert the SD card!"));
    while (true); // Berhenti jika DFPlayer tidak berhasil diinisialisasi
  }
  
  dfPlayer.volume(30); // Setel volume audio (0 hingga 30)
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "ON") {
      activateSystem();
    } else if (command == "OFF") {
      deactivateSystem();
    }
  }
}

// Fungsi untuk mengaktifkan sistem
void activateSystem() {
  turnOffAllLEDs(); // Pastikan semua LED dimatikan terlebih dahulu
  digitalWrite(redPin1, HIGH);
  digitalWrite(redPin2, HIGH);
  digitalWrite(yellowPin, LOW);
  digitalWrite(greenPin1, LOW);
  digitalWrite(greenPin2, LOW);
  Serial.println("SYSTEM ON");
  dfPlayer.play(1); // Memutar audio ketika sistem dinyalakan
}

// Fungsi untuk menonaktifkan sistem dengan pengecekan perintah baru
void deactivateSystem() {
  Serial.println("SYSTEM OFF");
  dfPlayer.stop(); // Menghentikan pemutaran audio

  int cycleCount = 5; // Jumlah siklus loop OFF
  for (int i = 0; i < cycleCount; i++) {
    // Merah menyala, sementara lainnya mati
    digitalWrite(redPin1, HIGH);
    digitalWrite(redPin2, LOW);
    digitalWrite(yellowPin, LOW);
    digitalWrite(greenPin1, LOW);
    digitalWrite(greenPin2, HIGH);
    if (waitForInterrupt(5000)) return; // Cek perintah baru selama 5 detik

    // Merah mati, kuning menyala, hijau mati
    digitalWrite(redPin1, LOW);
    digitalWrite(redPin2, LOW);
    digitalWrite(yellowPin, HIGH);
    digitalWrite(greenPin1, LOW);
    digitalWrite(greenPin2, LOW);
    if (waitForInterrupt(1500)) return; // Cek perintah baru selama 1,5 detik

    // Merah mati, kuning mati, hijau menyala
    digitalWrite(redPin1, LOW);
    digitalWrite(redPin2, HIGH);
    digitalWrite(yellowPin, LOW);
    digitalWrite(greenPin1, HIGH);
    digitalWrite(greenPin2, LOW);
    if (waitForInterrupt(5000)) return; // Cek perintah baru selama 5 detik

    // Merah mati, kuning menyala, hijau mati
    digitalWrite(redPin1, LOW);
    digitalWrite(redPin2, LOW);
    digitalWrite(yellowPin, HIGH);
    digitalWrite(greenPin1, LOW);
    digitalWrite(greenPin2, LOW);
    if (waitForInterrupt(1500)) return; // Cek perintah baru selama 1,5 detik
  }

  // Mematikan semua LED setelah loop
  turnOffAllLEDs();
}

// Fungsi untuk mengecek perintah baru selama proses menunggu
bool waitForInterrupt(int delayTime) {
  int elapsed = 0;
  while (elapsed < delayTime) {
    if (Serial.available() > 0) {
      String command = Serial.readStringUntil('\n');
      command.trim();
      if (command == "ON") {
        activateSystem(); // Pindah ke kondisi ON jika ada perintah baru
        return true; // Menandakan adanya interrupt
      }
    }
    delay(100); // Cek setiap 100 ms
    elapsed += 100;
  }
  return false; // Tidak ada interrupt
}
