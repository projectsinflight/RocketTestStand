/*
 * RocketTestStand
 *
 * Device for gathering and logging thrust data for small rocket motors
 */

/* INCLUDES */
#include <SPI.h>
#include <SD.h>

/* DEFINES */
#define NEWTONS_PER_COUNT (0.01144 * 0.0098)
#define DEBUG (true)
#define RUNTIME (60000)

/* GLOBAL VARIABLES */
const int chipSelect = 10;
const int dataPin = 9;     
const int clkPin =  8;
const int ledPin = 2;     
File dataFile;
unsigned long int zero_point = 0;

/* FUNCTIONS */
void error() {
  while (true) {
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
  }
}

unsigned long int measure() {
  unsigned char i;
  unsigned long int count = 0;
  while (digitalRead(dataPin) == HIGH);
  // read the value:
  for (int i=0; i<24; i++){
    digitalWrite(clkPin, HIGH);  
    count = count << 1;
    digitalWrite(clkPin, LOW);
    if (digitalRead(dataPin)) count++;
  }
  digitalWrite(clkPin, HIGH);
  count = count^0x800000;
  digitalWrite(clkPin, LOW);
  if (count < zero_point) {
    count = zero_point;
  }
  return count;
}

void set_zero_point() {
  unsigned char i;
  unsigned long int val = 0;
  String dataString = "";
  for (i = 0; i<100; i++){
    val += measure();
  }
  zero_point = val / 100;
  dataString.concat("Zero Point found: ");
  dataString.concat(val);
  Serial.println(dataString);
}

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  pinMode(ledPin, OUTPUT);
  pinMode(clkPin, OUTPUT);
  pinMode(dataPin, INPUT);
  digitalWrite(clkPin, LOW);
  digitalWrite(ledPin, LOW);

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    error();
  }
  Serial.println("card initialized.");

  // Zero the scale
  set_zero_point();
}

void loop() {
  float newtons;
  dataFile = SD.open("log.txt", FILE_WRITE);
    if (!dataFile) {
    Serial.println("error opening datalog.txt");
    error();
  }
  // STORE DATA EVERY 1 MINUTES
  digitalWrite(ledPin, HIGH);
  while (millis() < RUNTIME) {
    newtons = NEWTONS_PER_COUNT * ((float)(measure() - zero_point));
    // make a string for assembling the data to log:
    String dataString = "";
    String t_str = String(millis()/1000.0, 3);
    dataString.concat(t_str);
    dataString.concat(",");
    dataString.concat(newtons);
    dataFile.println(dataString);
    if (DEBUG) {
      Serial.println(dataString);
    }
  }
  Serial.println("Data Saved");
  dataFile.close();

  digitalWrite(ledPin, LOW);
  while(1);
}
