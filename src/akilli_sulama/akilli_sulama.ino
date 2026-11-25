/*
 * ESP32 Tabanlı Akıllı Bitki Takip ve Sulama Sistemi
 * Gömülü Sistemler Dersi Projesi
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// ---- OLED ekran ----
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET   -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---- Pinler ----
#define DHTPIN   14
#define DHTTYPE  DHT11
#define SOIL_PIN 34
#define ROLE_PIN 25

// ---- Toprak nem sensörü kalibrasyon değerleri ----
#define KURU_DEGER  4000
#define ISLAK_DEGER 1500

DHT dht(DHTPIN, DHTTYPE);

float sonSicaklik = 0.0;
float sonHavaNemi = 0.0;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);  // SDA, SCL

  pinMode(ROLE_PIN, OUTPUT);
  digitalWrite(ROLE_PIN, LOW);

  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  // Sıcaklık ve hava nemi (okuma hatalıysa son geçerli değer korunur)
  float anlikT = dht.readTemperature();
  float anlikH = dht.readHumidity();

  if (!isnan(anlikT)) {
    sonSicaklik = anlikT;
  }
  if (!isnan(anlikH)) {
    sonHavaNemi = anlikH;
  }

  // Toprak nemi (% olarak, 10'un katlarına yuvarlanır)
  int hamDeger = analogRead(SOIL_PIN);
  int gercekYuzde = map(hamDeger, KURU_DEGER, ISLAK_DEGER, 0, 100);
  int yuvarlanmisNem = ((gercekYuzde + 5) / 10) * 10;

  if (yuvarlanmisNem < 0)   yuvarlanmisNem = 0;
  if (yuvarlanmisNem > 100) yuvarlanmisNem = 100;

  // Ekran
  display.clearDisplay();
  display.setTextSize(1);

  display.setCursor(10, 5);
  display.println("- AKILLI SULAMA -");
  display.drawLine(10, 14, 118, 14, SSD1306_WHITE);

  display.setCursor(10, 22);
  display.print("Sicaklik     : ");
  display.print(sonSicaklik, 1);
  display.print(" C");

  display.setCursor(10, 32);
  display.print("Hava Nemi : %");
  display.println(sonHavaNemi, 0);

  display.setCursor(10, 42);
  display.print("Toprak Nemi : %");
  display.println(yuvarlanmisNem);

  // Sulama kararı
  display.setCursor(10, 55);
  if (yuvarlanmisNem <= 30) {
    display.print(">> SU VERILIYOR...");
    digitalWrite(ROLE_PIN, HIGH);
  } else {
    display.print(">> Toprak nemli.");
    digitalWrite(ROLE_PIN, LOW);
  }

  display.display();
  delay(2000);
}
