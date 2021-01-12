/**************************************************************************
Low Cost Mass Flow Controller - November 2020
 **************************************************************************/
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Encoder.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Rotary Encoder
Encoder encoder1(5, 6);
Encoder encoder2(13, 12);

double factorgas1; 
double factorgas2;


double flow1;
double flow2;

bool page1 = true;

void setup() {
  // put your setup code here, to run once:
  pinMode(11, INPUT);
  pinMode(8, INPUT);
  analogWrite(10, 0);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  //Define the gas factor and MFC flow capabilities
  factorgas1 = 1.4573 * 1000.0000;
  factorgas2 = 1.4573 * 200.0000;
}

bool buttonPressed = false;

int potval1;
long oldPosition = -999;
long lastUpdateMillis = 0;

long oldPosition2 = -999;
long lastUpdateMillis2 = 0;

double sp1 = 0;
double sp2 = 0;

void loop() {
  // main code here, to run repeatedly:
   flow1 = analogRead(A2) * factorgas1 / 1023.0;
   potval1 = sp1 / factorgas1;
   
  // flow2 = analogRead(A3) * factorgas2 / 1023.0;
   maintainDisplay(); 
   maintainRotaryEncoder();
   maintainRotaryEncoder2();

  // Checks if the button is pressed down to change setpoint 2.
  if (digitalRead(11) == HIGH ){
    buttonPressed = true;
  }
  else{
    buttonPressed = false;
  }

}

//Maintains the display - by not calling delay() allows multiple tasks to take place at once
void maintainDisplay()
{
  static  const unsigned long REFRESH_INTERVAL = 500; // ms
  static unsigned long lastRefreshTime = 0;

  static const unsigned long REFRESH_INTERVAL_PAGE = 4000; // ms
  static unsigned long lastRefreshTimePage = 0;
  
  if(millis() - lastRefreshTimePage >= REFRESH_INTERVAL_PAGE)
  {
    lastRefreshTimePage += REFRESH_INTERVAL_PAGE;
   // page1 = !page1;

  }
  
  if(millis() - lastRefreshTime >= REFRESH_INTERVAL)
  {
    lastRefreshTime += REFRESH_INTERVAL;

    
   // if (STATE != 0  || initialRefresh == false) {
                refreshDisplay();
              // initialRefresh = true;
  //  }
  }

 
}



void maintainRotaryEncoder(){
  long newPosition = encoder1.read();

  if (newPosition != oldPosition){
    oldPosition = newPosition;

    sp1 += newPosition * 0.1;

    if (sp1 < 0) {
      sp1 = 0;
    }

    if (sp1 > 5) {
      sp1 = 5;
    }

     refreshDisplay();
  }
  // software debounce
  if (millis() - lastUpdateMillis > 50){
    lastUpdateMillis = millis();
    encoder1.write(0);
  }
}

void maintainRotaryEncoder2(){
  long newPosition2 = encoder2.read();

  if (newPosition2 != oldPosition2){
 
    oldPosition2 = newPosition2;

    sp2 += newPosition2 * 0.1;

    if (sp2 < 0) {
      sp2 = 0;
    }

    
    if (sp2 > 5) {
      sp2 = 5;
    }

    refreshDisplay();
  }
  // software debounce
  if (millis() - lastUpdateMillis > 50){
    lastUpdateMillis = millis();
    encoder1.write(0);
  }
}

//Refreshes the display with the current rate.
void refreshDisplay(void)
{
  
  display.clearDisplay();
  display.setCursor(0,5);  
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE);

  if (buttonPressed)
  {
    display.setTextSize(1);
    display.print("setting to ");
    display.print(sp1);
    analogWrite(10, 255*(sp1/5)); //Analog Write is HIGH (5V) at 255, and LOW (0V) 100% of the time at 0
  }
  else {
    display.setCursor(0, 25);
    display.setTextSize(1);
    display.print("---------------------");
    display.setTextSize(1);
    
    display.setCursor(0,5);
    display.print("REAL FLUX 1:   ");
    display.print(flow1, 1);
    
    display.setCursor(0,40);
    display.println("SETPOINT");
    display.print("FLUX 1:");
    display.setTextSize(2);
    
    display.setCursor(80,40);
    display.print(sp1, 1);
    display.print("V");
    display.setTextSize(1);
  }
  display.display();
}
