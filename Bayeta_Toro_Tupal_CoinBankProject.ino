// ================= Libraries ================= //
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

/*
  Submitted by:
  BAYETA IV, Reginald Geoffrey L.
  TORO, Matthew Edward S.
  TUPAL, Isaiah Jassen L.

  Version: 1/18/2021 | 0518H
*/

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ================= Variables ================= //

// Pin assignments
const int p1 = 2;
const int p5 = 3;
const int p10 = 4;
const int reset = 5;

// Target amount
const int target = 10000;

// Display strings
String display_top = "";
String display_bottom = "";

// Sensors and Reset button
int isP1, isP5, isP10, isReset;

// [TOTAL] Total count for P1, 5, 10
int p1Total, p5Total, p10Total, total = 0;

// [NEED]  Total need count for P1, 5, 10
int p1Need, p5Need, p10Need, need = 0;

// Bool if coin is detected by sensor
int isCoinInserted = 0;

void calculateLowestCoins(int need) {
  /*
     Function name: calculateLowestCoins
     Parameters: need
     Function:
     Returns the least amount of P1, P5, P10 coins
     needed to reach the target amount

     Example:
     Target = 100
     Need = 20 (Means that 80 is the current value)
     Returns: P1 = 0; P5 = 0; P10 = 2
  */
  int val = need;

  while (val != 0) {
    p10Need = val / 10;
    val %= 10;
    p5Need = val / 5;
    val %= 5;
    p1Need = val / 1;
    val %= 1;
  }
}

void setup() {
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();

  pinMode(p1, INPUT);
  pinMode(p5, INPUT);
  pinMode(p10, INPUT);
  pinMode(reset, INPUT);
}

void loop() {
  isCoinInserted = 0;

  // Read incase of system shutdown
  p1Total = EEPROM.read(5);
  p5Total = EEPROM.read(6);
  p10Total = EEPROM.read(7);

  // Button reading
  isP1 = digitalRead(p1);
  isP5 = digitalRead(p5);
  isP10 = digitalRead(p10);
  isReset = digitalRead(reset);

  // Handles reset button
  if (isReset == 1) {
    Serial.println("RESET");
    lcd.clear();
    p1Total = 0;
    p5Total = 0;
    p10Total = 0;
    EEPROM.update(5, p1Total);
    EEPROM.update(6, p5Total);
    EEPROM.update(7, p10Total);
  }

  // Handles coin insertion
  if (isP1 == 0) {
    p1Total += 1;
    EEPROM.update(5, p1Total);
    isCoinInserted = 1;
    delay(100);
  }
  if (isP5 == 0) {
    p5Total += 5;
    EEPROM.update(6, p5Total);
    isCoinInserted = 1;
    delay(100);
  }
  if (isP10 == 0) {
    p10Total += 10;
    EEPROM.update(7, p10Total);
    isCoinInserted = 1;
    delay(100);
  }

  // Handles updating values in LCD
  total = p1Total + p5Total + p10Total;
  lcd.setCursor(0, 0);
  display_top = "TOTAL: " + String(total);
  lcd.print(display_top);
  lcd.setCursor(0, 1);
  display_bottom = String(p1Total) + "  " + String(p5Total / 5) + "  " + String(p10Total / 10);
  lcd.print(display_bottom);

  // Handles updating need values in LCD
  need = target - total;
  if (need > 0 && isCoinInserted == 1) {

    lcd.clear();
    lcd.setCursor(0, 0);
    display_top = "NEED: " + String(need);
    lcd.print(display_top);
    lcd.setCursor(0, 1);
    calculateLowestCoins(need); // Gets the least number of coins needed to reach requirement
    display_bottom = String(p1Need) + "  " + String(p5Need) + "  " + String(p10Need);
    lcd.print(display_bottom);

    delay(1000);
    lcd.clear();
  }

  // Handles if target amount reached
  if (need <= 0) {
    p1Total = 0;
    p5Total = 0;
    p10Total = 0;
    EEPROM.update(5, p1Total);
    EEPROM.update(6, p5Total);
    EEPROM.update(7, p10Total);

    lcd.clear();
    lcd.setCursor(0, 0);
    display_top = "Congratulations!";
    lcd.print(display_top);
    lcd.setCursor(0, 1);
    display_bottom = "Amount achieved!";
    lcd.print(display_bottom);
    delay(3000);
    lcd.clear();
  }
}
