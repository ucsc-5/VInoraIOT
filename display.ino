#include <LiquidCrystal_I2C.h>

#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
char * message = "-- Vinora IOT --";
void setup_display() {



//Use predefined PINS consts

Wire.begin(D4, D3);

lcd.begin();

lcd.home();

lcd.setCursor(0,0);
lcd.print(message);

}

void loop_display() { 
 
}
