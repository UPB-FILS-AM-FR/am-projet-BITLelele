#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#define TFT_CS     10
#define TFT_RST    8
#define TFT_DC     9

#define BUTTON1_PIN 5  
#define BUTTON2_PIN 6  

#define GPS_RX 3
#define GPS_TX 4

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
SoftwareSerial ss(GPS_RX, GPS_TX);
TinyGPSPlus gps;

enum GameState {
  EXPLORING,
  COMBAT,
  RESULT
};

GameState gameState = EXPLORING;
bool pokemonCaught = false;

int characterX, characterY;
bool pokemonIsCircle;
int pokemonX, pokemonY;

double initialLat, initialLon;
bool initialPositionSet = false;

void setup() {
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  
  Serial.begin(9600);
  ss.begin(9600);
  tft.initR(INITR_144GREENTAB); 
  tft.fillScreen(ST7735_BLACK);
  
  characterX = tft.width() / 2;
  characterY = tft.height() / 2;
  spawnPokemon();
}

void loop() {
  while (ss.available() > 0) {
    gps.encode(ss.read());
  }

  if (digitalRead(BUTTON1_PIN) == LOW) {
    Serial.println("Button 1 pressed");
    if (gameState == EXPLORING) {
      gameState = COMBAT;
      tft.fillScreen(ST7735_BLACK);
    } else if (gameState == RESULT) {
      gameState = EXPLORING;
      tft.fillScreen(ST7735_BLACK);
      spawnPokemon();
    }
    delay(200); 
  }
  
  if (digitalRead(BUTTON2_PIN) == LOW) {
    Serial.println("Button 2 pressed");
    if (gameState == COMBAT) {
      gameState = RESULT;
      pokemonCaught = random(0, 2); 
      tft.fillScreen(ST7735_BLACK);
      if (pokemonCaught) {
        tft.setTextColor(ST7735_GREEN);
        tft.setCursor(10, 64);
        tft.print("You caught the Pokemon!");
      } else {
        tft.setTextColor(ST7735_RED);
        tft.setCursor(10, 64);
        tft.print("You failed to catch the Pokemon!");
      }
      delay(200); 
    }
  }
  
  if (gameState == EXPLORING) {
    drawExploring();
  } else if (gameState == COMBAT) {
    drawCombat();
  }
}

void spawnPokemon() {
  pokemonX = random(0, tft.width());
  pokemonY = random(0, tft.height());
  pokemonIsCircle = random(0, 2);
}

void drawExploring() {
  if (gps.location.isUpdated()) {
    if (!initialPositionSet) {
      initialLat = gps.location.lat();
      initialLon = gps.location.lng();
      initialPositionSet = true;
    }

    double deltaX = gps.location.lat() - initialLat;
    double deltaY = gps.location.lng() - initialLon;

    pokemonX = characterX + map(deltaX * 100000, -9000, 9000, -tft.width()/2, tft.width()/2);
    pokemonY = characterY + map(deltaY * 100000, -18000, 18000, -tft.height()/2, tft.height()/2);
  }

  tft.fillScreen(ST7735_BLACK);
  
  if (pokemonIsCircle) {
    tft.fillCircle(pokemonX, pokemonY, 5, ST7735_RED);  
    tft.fillRect(characterX - 5, characterY - 5, 10, 10, ST7735_BLUE);  
  } else {
    tft.fillRect(pokemonX - 5, pokemonY - 5, 10, 10, ST7735_RED);  
    tft.fillCircle(characterX, characterY, 5, ST7735_BLUE);  
  }
}

void drawCombat() {
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setCursor(10, 64);
  tft.print("Press button to throw Pokeball");
}
