#include "DHT.h"
#include "Sodaq_DS3231.h"
#include <stdlib.h>
#include <SPI.h>
#include <SD.h>


#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

const int chipSelect = 10; // 4
const int OUTPUT_PIN = 4;
int soilPin = A0;
int soilHumidity = 0;
float analog_read =0;
DateTime now;

File myFile;
DHT dht(DHTPIN, DHTTYPE);

void adjustDate(int year, int month, int date, int hour, int min, int sec, int weekday){
  //year, month, date, hour, min, sec and week-day(starts from 0 and goes to 6)
  DateTime dt(year, month, date, hour, min, sec, weekday);
  rtc.setDateTime(dt); //Adjust date-time as defined 'dt' above 
  //Serial.println(rtc.now().getEpoch());//debug info
}

void setup() {
  
  digitalWrite(OUTPUT_PIN, HIGH);
  pinMode(OUTPUT_PIN, OUTPUT);
  
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  rtc.begin();
  dht.begin();
  
  //adjustDate(2021,5,4,23,3,0,1);
  
  Serial.print("Initializing SD card...");
  
  if (!SD.begin(chipSelect)) {
  Serial.println("initialization failed!");
  while (1);
  }
  
  Serial.println("initialization done.");
}

void loop() {
  now = rtc.now();

  float h = dht.readHumidity();  
  float t = dht.readTemperature(); 
  char dateBuffer[20];
  char fileNameBuffer[12];

  analog_read = analogRead(soilPin);
  soilHumidity = map(analog_read, 0, 1023, 100, 0);
  
  sprintf(fileNameBuffer, "%4d%02d%02d.csv", now.year(), now.month(), now.date());
  sprintf(dateBuffer, "%4d/%02d/%02d %02d:%02d:%02d", now.year(), now.month(), now.date(), now.hour(), now.minute(), now.second());

  String filename = String(fileNameBuffer);
  
  if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
  }

  Serial.print("FileName: ");
  Serial.println(fileNameBuffer);
  //Serial.println("#");
  
  Serial.print(dateBuffer);
  Serial.print(",");
  Serial.print(t);
  Serial.print("°C");
  Serial.print(",");
  Serial.print(h);
  Serial.print("%");
  Serial.print(",");
  Serial.print(soilHumidity);
  Serial.println("%");

  myFile = SD.open(filename, FILE_WRITE);
  if (myFile) {
    myFile.print(dateBuffer);
    myFile.print(",");
    myFile.print(t);
    myFile.print("°C");
    myFile.print(",");
    myFile.print(h);
    myFile.print("%");
    myFile.print(",");
    myFile.print(soilHumidity);
    myFile.println("%");
  
    myFile.close();
  } 
  else {
    Serial.print("error opening ");
    Serial.println(fileNameBuffer);
    //Serial.println("#");
  }

  if(now.hour() == 23 && now.minute() == 59){
    delay(60000);
    Serial.println("Resetting Arduino");
    digitalWrite(OUTPUT_PIN, LOW); 
  }

   delay(60000);
}
