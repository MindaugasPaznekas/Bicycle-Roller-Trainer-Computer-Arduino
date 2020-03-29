#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);                     // set the LCD address to 0x27 for a 16 chars and 2 line display

#define SENSOR 2                                      // define the Hall magnetic sensor interface PIN
#define circumference 267.035375555                   //85 mm circle circumference  //calculate the circumference to avoid costly float operations 2*Pi*r or Pi*d
#define lenghtForSpeedCalculation circumference * 36  //multiply by number of samples taken - 10, 
                                                      //multiply by 3.6 to convert to km/h
                                                      //(we implicitly divide by 1000 to convert mm to m and multiply by 1000 to convert ms to s)
#define lenghtForDistanceCalculation circumference / 100000 //10 circles and convert mm to km                                             
volatile float totalDistance = lenghtForDistanceCalculation / 2; // To correct the initial 5 spins missing to full 

volatile int counter = 0 ;                            // counter which ticks every spin
volatile unsigned long deltatime = 0;                 // time delta 
volatile float currentSpeed = 0;                      //holds the last calculated speed value

// LCD geometry
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

const int decimalSymbolsForDistance = 3;              //How many numbers for distance after decimal to show

void setup ()
{
    pinMode (SENSOR, INPUT) ;  // define the Hall magnetic sensor line as input
    attachInterrupt(digitalPinToInterrupt(SENSOR), interrupt, FALLING);
    lcd.init();// initialize the lcd 
    lcd.backlight();//turn on backlight
    lcd.print("Hello!");
    lcd.setCursor(0,1);
    lcd.print("Booting up");
    delay(1000);
}
 
void loop ()
{
  delay(500);

  lcd.clear();//Clear previous data from screen
  
  lcd.print("DIST-");
  lcd.print(totalDistance, decimalSymbolsForDistance);
  lcd.print(" KM");
  
  
  lcd.setCursor(0,1);
  lcd.print("SPD-");
  lcd.print(currentSpeed, currentSpeed < 10 ? 2 : 1);
  lcd.print(" KM/H");
  delay(500);
}
// when the Hall sensor detects a magnetic field this interrupt is called
void interrupt() 
{
    counter++;
    
    //reads time stamp when counter is 1
    if (counter == 1)
    {
      deltatime = millis();
      return;
    }
    else if (counter == 5)//measures distance when counter is 5
    {
      totalDistance += lenghtForDistanceCalculation;
      return;
    }
    else if (counter == 10)//measures speed when counter is 10
    {
      currentSpeed = lenghtForSpeedCalculation / (millis() - deltatime);
      counter = 0;
      return;
    }
}
