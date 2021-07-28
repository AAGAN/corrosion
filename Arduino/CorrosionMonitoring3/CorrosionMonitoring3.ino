#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include "staple.h"

// A simple data logger for the Arduino analog pins

#define LOG_INTERVAL  30000 // mills between entries (reduce to take more/faster data)
double tempLimit = 5.0; //below this temperature the indicator light comes on

#define SYNC_INTERVAL LOG_INTERVAL // mills between calls to flush() - to write data to the card
uint32_t syncTime = 0; // time of last sync()

#define ECHO_TO_SERIAL   1 // echo data to serial port
#define WAIT_TO_START    0 // Wait for serial input in setup()

RTC_DS1307 RTC; // define the Real Time Clock object

// for the data logging shield, we use digital pin 10 for the SD cs line
const int chipSelect = 10;

// the logging file
File logfile;

void error(char *str)
{
#if ECHO_TO_SERIAL  
  Serial.print("error: ");
  Serial.println(str);
#endif
  
  while(1);
}

  staple staple1(A3,A3,A0,A1,5,1022);
  staple staple2(A2,A2,A0,A1,6,1022);
  staple staple3(2,2,A0,A1,7,1022);
  staple staple4(3,3,A0,A1,8,1022); 
  staple staple5(4,4,A0); 

int blinkNum = 20;
void blinki()
{
delay(100);
}

void blinki(bool switchi)
{
    delay(200);
}

//temperature measurement
double measureTemp(double limit)
{
  blinki(true);
  double temp = ((analogRead(A0)*4.9)-500)/10;
  blinki(true);
  if (temp < limit) 
    digitalWrite(9, HIGH);
  else
    digitalWrite(9, LOW);
  
  return temp;
}

int loopnum = 10;

void initialize(int loopnumm)
{
  delay(100);
}

void setup(void)
{
#if ECHO_TO_SERIAL
  Serial.begin(9600);
  Serial.println();
#endif

#if ECHO_TO_SERIAL  
#if WAIT_TO_START
  Serial.println("Type any character to start");
  while (!Serial.available());
#endif //WAIT_TO_START
#endif
  // initialize the SD card
#if ECHO_TO_SERIAL
  Serial.print("Initializing SD card...");
#endif
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  pinMode(A1,INPUT); //temperature input pin (analog 1)
pinMode(9,OUTPUT); //temperature indicator pin (digital 9)
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    error("Card failed, or not present");
  }
#if ECHO_TO_SERIAL
  Serial.println("card initialized.");
#endif
  
  // create a new file
  char filename[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE); 
      break;  // leave the loop!
    }
  }
  
  if (! logfile) {
    error("couldnt create file");
  }
#if ECHO_TO_SERIAL  
  Serial.print("Logging to: ");
  Serial.println(filename);
#endif
  // connect to RTC
  Wire.begin();  
  if (!RTC.begin()) {
    logfile.println("RTC failed");
#if ECHO_TO_SERIAL
    Serial.println("RTC failed");
#endif  //ECHO_TO_SERIAL
  }
  

  logfile.println(" year , month , day , hour , minute, second , staple1 , staple2 , staple3 , staple4 , staple5 , temperature (C)");// , 1-2 , 3-2 , 1-4 , 3-4");    
#if ECHO_TO_SERIAL
  Serial.println(" datetime , staple1 , staple2 , staple3 , staple4 , staple5 , temperature (C)"); // , 1-2 , 3-2, 1-4, 3-4");
#endif //ECHO_TO_SERIAL
 
  initialize(loopnum);
}

void loop(void)
{
  DateTime now;

    // delay for the amount of time we want between readings
    delay((LOG_INTERVAL -1) - (millis() % LOG_INTERVAL));
  
  uint32_t m = millis();  
#if ECHO_TO_SERIAL
  //Serial.print(m);         // milliseconds since start
  //Serial.print(", ");  
#endif

  // fetch the time
  now = RTC.now();
  logfile.print(' ');
  logfile.print(now.year(), DEC);
  logfile.print(",");
  logfile.print(now.month(), DEC);
  logfile.print(",");
  logfile.print(now.day(), DEC);
  logfile.print(" , ");
  logfile.print(now.hour(), DEC);
  logfile.print(",");
  logfile.print(now.minute(), DEC);
  logfile.print(",");
  logfile.print(now.second(), DEC);
  logfile.print(' ');
#if ECHO_TO_SERIAL
  Serial.print('"');
  Serial.print(now.year(), DEC);
  Serial.print("/");
  Serial.print(now.month(), DEC);
  Serial.print("/");
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.print(now.second(), DEC);
  Serial.print('"');
#endif //ECHO_TO_SERIAL

  staple1.measureOn();
  staple2.measureOn();
  staple3.measureOn();
  staple4.measureOn();
  staple5.measureOn();
  
  logfile.print(", ");    
  logfile.print(staple1.get_raw_value());
  logfile.print(", ");    
  logfile.print(staple2.get_raw_value());
  logfile.print(", ");    
  logfile.print(staple3.get_raw_value());
  logfile.print(", ");    
  logfile.print(staple4.get_raw_value());
  logfile.print(", ");
  logfile.print(staple5.get_raw_value());
  logfile.print(", ");    
  logfile.print(measureTemp(tempLimit));
#if ECHO_TO_SERIAL
  Serial.print(", ");    
  Serial.print(staple1.get_raw_value());
  Serial.print(", ");    
  Serial.print(staple2.get_raw_value());
  Serial.print(", ");    
  Serial.print(staple3.get_raw_value());
  Serial.print(", ");    
  Serial.print(staple4.get_raw_value());
  Serial.print(", ");    
  Serial.print(staple5.get_raw_value());
  Serial.print(", ");  
  Serial.print(measureTemp(tempLimit));
#endif //ECHO_TO_SERIAL

  logfile.println();
#if ECHO_TO_SERIAL
  Serial.println();
#endif // ECHO_TO_SERIAL

  // Now we write data to disk! Don't sync too often - requires 2048 bytes of I/O to SD card
  // which uses a bunch of power and takes time
  if ((millis() - syncTime) < SYNC_INTERVAL) return;
  syncTime = millis();
  
  logfile.flush();  
}
