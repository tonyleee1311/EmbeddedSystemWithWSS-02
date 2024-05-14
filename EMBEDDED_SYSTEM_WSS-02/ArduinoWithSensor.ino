#include <SoftwareSerial.h>

// Define the pins for RS485 communication
#define RX              2  // RO
#define TX              3  // DI
#define DE_RE           8

// Define default parameters for the sensor
#define RESPONSE_SIZE   13
#define WSPEED_HIGH     3
#define WSPEED_LOW      4
#define WDIRECTION_HIGH 9
#define WDIRECTION_LOW  10
#define WANGLE_HIGH     7
#define WANGLE_LOW      8

// Software Serial pins for ESP8266 NodeMCU communication
#define Rx_ESP 11
#define Tx_ESP 12

// DELAY PERIOD
#define MESSAGE_SENT_DELAY 7000
#define SYSTEM_DELAY 500

// Request frame for the wind sensor
const byte sensorRequest[] = {0x07, 0x03, 0x00, 0x00, 0x00, 0x04, 0x44, 0x6F};
byte sensorResponse[RESPONSE_SIZE];
float windSpeed=0;
float windAngle=0;
int windDirectionValue = 0;
char windDirection[5];
uint16_t counter = 0;


// Software serial for RS485 communication
SoftwareSerial mod(RX, TX);

// Software serial for UART communication with ESP8266 NodeMCU
SoftwareSerial mySerial(Rx_ESP, Tx_ESP);
 
void setup()
{
  pinMode(DE_RE, OUTPUT);
  Serial.begin(9600); // Initialize serial communication for debugging
  delay(100);
  
  while (!Serial) {

    ; // wait for serial port to connect. Needed for native USB port only

  }

  // Start each software serial port
  mod.begin(9600);    // Initialize software serial communication at 9600 baudrate
  mySerial.begin(9600);
}
 
void loop()
{
  resetDirection(); // Reset wind direction
  mod.listen();
  readData();
  parseData();
  printData();

  // Normalize sensor's information
  // Speed
  String mySpeed = String(windSpeed);
  String formattingSpeed = "0";
  if (mySpeed.length() == 4)
  {
    formattingSpeed.concat(mySpeed);
    mySpeed = formattingSpeed;
  }
  // Angle
  String myAngle = String(windAngle);
  String formattingAngle = "0";
  if (myAngle.length() == 4)
  {
    formattingAngle = "00";
    formattingAngle.concat(myAngle);
    myAngle = formattingAngle;
  }
  else if (myAngle.length() == 5)
  {
    formattingAngle.concat(myAngle);
    myAngle = formattingAngle;
  }

  // Send message to ESP8266 NodeMCU
  String message = mySpeed + myAngle + windDirection;

  // counter for message sent
  if(counter < (MESSAGE_SENT_DELAY/SYSTEM_DELAY))
  {
    counter++;
    if (counter == MESSAGE_SENT_DELAY/SYSTEM_DELAY)
    {
      counter = 0;
      mySerial.println(message);
    }
  }
  delay(SYSTEM_DELAY); // Data is update every 0.5 second
}

void readData(){
  digitalWrite(DE_RE, HIGH);
  delay(10);
  
  // Send the request frame to the soil sensor
  mod.write(sensorRequest, sizeof(sensorRequest));
 
  digitalWrite(DE_RE, LOW);
  delay(10);

 //Serial.print(mod.available());
  if(mod.available()){
       byte index = 0;
    while (mod.available() && index < RESPONSE_SIZE)
    {
      sensorResponse[index] = mod.read();
      index++;
    }
  }
  else{
  }
}

void printData(){
  //print speed
  Serial.print("Wind speed: ");
  Serial.print(windSpeed);
  Serial.println(" m/s");
  //print direction
  Serial.print("Wind direction: ");
  decideDirection(windDirection);
  Serial.println(windDirection);
  // print angle
  Serial.print("Wind angle: ");
  Serial.print(windAngle);
  Serial.println("°");
}

void parseData(){
    //calculate wind speed
    windSpeed = (sensorResponse[WSPEED_HIGH] << 8) | sensorResponse[WSPEED_LOW];
    windSpeed = windSpeed/10;
    // calculate wind direction
    windDirectionValue = (sensorResponse[WDIRECTION_HIGH] << 8) | sensorResponse[WDIRECTION_LOW];
    // calculate wind angle:
    windAngle = (sensorResponse[WANGLE_HIGH]<< 8) | sensorResponse[WANGLE_LOW];
    windAngle = windAngle/10;
}

void decideDirection(char windDirection[])
{
  // "N", "N-NE", "NE", "E-NE", "E", "E-SE", "SE", "S-SE", "S", "S-SW", "SW", "W-SW", "W", "W-NW", "NW", "N-NW"
  switch(windDirectionValue)
  {
    case 0:
      windDirection[0] = 'N';
      break;
    case 1:
      windDirection[0] = 'N';
      windDirection[1] = '-';
      windDirection[2] = 'N';
      windDirection[3] = 'E';
      break;
    case 2:
      windDirection[0] = 'N';
      windDirection[1] = 'E';
      break;
    case 3:
      windDirection[0] = 'E';
      windDirection[1] = '-';
      windDirection[2] = 'N';
      windDirection[3] = 'E';
      break;
    case 4:
      windDirection[0] = 'E';
      break;
    case 5:
      windDirection[0] = 'E';
      windDirection[1] = '-';
      windDirection[2] = 'S';
      windDirection[3] = 'E';
      break;
    case 6:
      windDirection[0] = 'S';
      windDirection[1] = 'E';
      break;
    case 7:
      windDirection[0] = 'S';
      windDirection[1] = '-';
      windDirection[2] = 'S';
      windDirection[3] = 'E';
      break;
    case 8:
      windDirection[0] = 'S';
      break;
    case 9:
      windDirection[0] = 'S';
      windDirection[1] = '-';
      windDirection[2] = 'S';
      windDirection[3] = 'W';
      break;
    case 10:
      windDirection[0] = 'S';
      windDirection[1] = 'W';
      break;
    case 11:
      windDirection[0] = 'W';
      windDirection[1] = '-';
      windDirection[2] = 'S';
      windDirection[3] = 'W';
      break;
    case 12:
      windDirection[0] = 'W';
      break;
    case 13:
      windDirection[0] = 'W';
      windDirection[1] = '-';
      windDirection[2] = 'N';
      windDirection[3] = 'W';
      break;
    case 14:
      windDirection[0] = 'N';
      windDirection[1] = 'W';
      break;
    case 15:
      windDirection[0] = 'N';
      windDirection[1] = '-';
      windDirection[2] = 'N';
      windDirection[3] = 'W';
      break;
    default: break;
  }
  return windDirection;
}

void resetDirection()
{
  for (int i = 0; i < 5; i++)
  {
    windDirection[i] = NULL;
  }
}