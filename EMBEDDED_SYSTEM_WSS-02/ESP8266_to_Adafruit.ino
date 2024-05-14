// Essential libraries
#include "config.h"
#include <Adafruit_SH110X.h>
#include <splash.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <Wire.h>

// I2C default address
#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's

// OLED display width and height
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO

// Create an object called "display"
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Initialize feeds on Adafruit
AdafruitIO_Feed *speed     = io.feed("wind_speed");
AdafruitIO_Feed *angle     = io.feed("wind_angle");
AdafruitIO_Feed *direction = io.feed("wind_direction");

String mySpeed, myAngle, myDirection;

void setup() {

  // start the serial connection
  Serial.begin(9600);
  
  // wait for serial monitor to open
  while(! Serial);

  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());


  // Initialize OLED display
  display.begin(i2c_Address, true); // Address 0x3C default
  display.display();
  delay(2000);

  display.clearDisplay(); // Clear the display buffer
  display.setTextColor(SH110X_WHITE); // Set text color to white
  display.setTextSize(1); // Set text size (1 is default)
}

void loop() {
    if(Serial.available())  
    { 
      String RxBuffer="";
      while(Serial.available())
      {
        RxBuffer = Serial.readString(); //Đọc tất cả bộ đệm
      }
      Serial.println("Nhận data: ");

      mySpeed = RxBuffer.substring(0, 5);
      Serial.println(mySpeed);
      speed->save(mySpeed);
      delay(100);

      myAngle = RxBuffer.substring(5, 11);
      Serial.println(myAngle);
      angle->save(myAngle);
      delay(100);

      myDirection = RxBuffer.substring(11);
      Serial.println(myDirection);
      direction->save(myDirection);
      delay(100);

      displayOLED();
    }
  io.run();
}

void displayOLED()
{
  display.clearDisplay(); // Clear previous content

    // Set cursor position and display text
  display.setCursor(20, 0);
  display.println("Wind Monitoring");

  display.setCursor(10, 20);
  display.print("Speed: ");     // Display text
  display.setCursor(50, 20);
  display.print(mySpeed);
  display.setCursor(84, 20);
  display.print("m/s");

  display.setCursor(10, 35);
  display.print("Direction: "); // Display text
  display.setCursor(75, 35);
  display.print(myDirection);

  display.setCursor(10, 50);
  display.print("Angle: ");     // Display text
  display.setCursor(50, 50);
  display.print(myAngle);
  display.setCursor(87, 50);
  display.print((char)247);

  display.display();
}
