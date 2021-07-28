#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include "staple.h"

// A simple data logger for the Arduino analog pins

// how many milliseconds between grabbing data and logging it. 1000 ms is once a second
#define LOG_INTERVAL  10000 // mills between entries (reduce to take more/faster data)
double tempLimit = 5.0; //below this temperature the indicator light comes on
//analogReference(EXTERNAL);

#define SYNC_INTERVAL LOG_INTERVAL // mills between calls to flush() - to write data to the card
uint32_t syncTime = 0; // time of last sync()

#define ECHO_TO_SERIAL   0 // echo data to serial port
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

//staple(positive_relay_pin, sensor_relay_pin, sensor_pin, indicator_pin, break_limit)
  staple staple1(A3,A3,A0,5,1000);//Each relay has two switches and that is why we can use the same pin for both positive pin and relay pin of each staple.
  staple staple2(A2,A2,A0,6,1000);
  staple staple3(2,2,A0,7,1000);
  staple staple4(3,3,A0,8,1000); //pins 10,11,12,13 are used for SPI, pins A4,A5 are used for I2C
  //staple staple5(4,4,A0); //this is not a staple, it is a resistance measurement sensor thing

int blinkNum = 20;
void blinki()
{
for (int i = 1 ; i < blinkNum ; i++)
  {
    digitalWrite(9, HIGH);
    delay(20/blinkNum);
    digitalWrite(9, LOW);
    delay(80/blinkNum);
  }
}
//temperature measurement
double measureTemp(double limit)
{
  digitalWrite(9, HIGH);
  blinki();
  //delay(100);
  double temp = ((analogRead(A1)*4.9)-500)/10;
  //delay(100);
  blinki();
  digitalWrite(9,LOW);
  if (temp < limit) digitalWrite(9, HIGH);
  return temp;
}
int loopnum = 10;
void initialize(int loopnumm)
{
 for (int j = 1; j < loopnumm; j++)
 { 
  for (int i = 5; i<10 ; i++)
  {
    digitalWrite(i, HIGH);
    delay(20);
    digitalWrite(i, LOW);
    delay(20);
  }
  for (int i = 9 ; i > 4; i--)
  {
    digitalWrite(i, HIGH);
    delay(20);
    digitalWrite(i, LOW);
    delay(20);
  }
 }
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
  

  logfile.println(" datetime , staple1 , staple2 , staple3 , staple4 , temperature (C) ");    
#if ECHO_TO_SERIAL
  Serial.println(" datetime , staple1 , staple2 , staple3 , staple4 , temperature (C) ");
#endif //ECHO_TO_SERIAL
 
  // If you want to set the aref to something other than 5v
  //analogReference(EXTERNAL);
  initialize(loopnum);
}

void loop(void)
{
  DateTime now;

    // delay for the amount of time we want between readings
    delay((LOG_INTERVAL -1) - (millis() % LOG_INTERVAL));
  
  //digitalWrite(greenLEDpin, HIGH);
  
  // log milliseconds since starting
  uint32_t m = millis();
  //logfile.print(m);           // milliseconds since start
  //logfile.print(", ");    
#if ECHO_TO_SERIAL
  Serial.print(m);         // milliseconds since start
  Serial.print(", ");  
#endif

  // fetch the time
  now = RTC.now();
  // log time
  //logfile.print(now.unixtime()); // seconds since 1/1/1970
  //logfile.print(", ");
  logfile.print(' ');
  logfile.print(now.year(), DEC);
  logfile.print("/");
  logfile.print(now.month(), DEC);
  logfile.print("/");
  logfile.print(now.day(), DEC);
  logfile.print(" , ");
  logfile.print(now.hour(), DEC);
  logfile.print(":");
  logfile.print(now.minute(), DEC);
  logfile.print(":");
  logfile.print(now.second(), DEC);
  logfile.print(' ');
#if ECHO_TO_SERIAL
  Serial.print(now.unixtime()); // seconds since 1/1/1970
  Serial.print(", ");
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

  staple1.measure();
  staple2.measure();
  staple3.measure();
  staple4.measure();
  //staple5.measure();
  
  logfile.print(", ");    
  logfile.print(staple1.get_raw_value());
  logfile.print(", ");    
  logfile.print(staple2.get_raw_value());
  logfile.print(", ");    
  logfile.print(staple3.get_raw_value());
  logfile.print(", ");    
  logfile.print(staple4.get_raw_value());
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
  Serial.print(measureTemp(tempLimit));
#endif //ECHO_TO_SERIAL

  logfile.println();
#if ECHO_TO_SERIAL
  Serial.println();
#endif // ECHO_TO_SERIAL

  //digitalWrite(greenLEDpin, LOW);

  // Now we write data to disk! Don't sync too often - requires 2048 bytes of I/O to SD card
  // which uses a bunch of power and takes time
  if ((millis() - syncTime) < SYNC_INTERVAL) return;
  syncTime = millis();
  
  // blink LED to show we are syncing data to the card & updating FAT!
  //digitalWrite(redLEDpin, HIGH);
  logfile.flush();
  //digitalWrite(redLEDpin, LOW);
  
}
