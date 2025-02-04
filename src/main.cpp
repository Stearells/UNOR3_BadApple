#include <Wire.h>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define IMG_WIDTH 15
#define IMG_HEIGHT 16
#define BLOCK_SIZE 5

uint8_t imageBuffer[IMG_HEIGHT][IMG_WIDTH];
volatile bool dataReceived = false;
volatile int index = 0;

const char hi[] = "Bad Apple | Stearells";
int hi_offset = 0;

void sayHi() {
    lcd.clear();
    lcd.setCursor(0, 0);

    for(int i = 0; i < strlen(hi); i++) {
        if(hi_offset >= 16) {
            lcd.scrollDisplayLeft();
        }

        for(int k = 0; k < 5; k++) {
            lcd.write(random(0x30, 0x5A));
            lcd.setCursor(i, 0);
            delay(50);
        }

        lcd.write(hi[i]);
        hi_offset++;
    }

    for(int i = strlen(hi) - 1; i >= 0; i--) {
        if(hi_offset > 0) {
            lcd.scrollDisplayRight();
        }

        lcd.setCursor(i, 0);
        lcd.write(' ');
        hi_offset--;
        delay(100);
    }
}

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  sayHi();
  Serial.write('K');
}

void receiveData() {
  while (Serial.available() > 0) {
    uint8_t pixel = Serial.read();
    int row = index / IMG_WIDTH;
    int col = index % IMG_WIDTH;
    imageBuffer[row][col] = pixel;
    index++;
    
    if (index == IMG_WIDTH * IMG_HEIGHT) {
      dataReceived = true;
      index = 0;
    }
  }
}

void createCustomChars() {
  for (int blockY = 0; blockY < 2; blockY++) {
    for (int blockX = 0; blockX < 3; blockX++) {
      byte charData[8] = {0};
      for (int y = 0; y < 8; y++) {
        int imgRow = blockY * 8 + y;
        for (int x = 0; x < BLOCK_SIZE; x++) {
          int imgCol = blockX * BLOCK_SIZE + x;
          charData[y] |= (imageBuffer[imgRow][imgCol] << (4 - x));
        }
      }
      lcd.createChar(blockX + blockY * 3, charData);
    }
  }
}

void displayBlocks() {
  lcd.setCursor(0, 0);
  for (int i = 0; i < 3; i++) lcd.write(i);
  lcd.setCursor(0, 1);
  for (int i = 3; i < 6; i++) lcd.write(i);
}

void loop() {
  receiveData();
  
  if (dataReceived) {
    lcd.clear();
    createCustomChars();
    displayBlocks();
    dataReceived = false;
    Serial.write('K');
  }
}