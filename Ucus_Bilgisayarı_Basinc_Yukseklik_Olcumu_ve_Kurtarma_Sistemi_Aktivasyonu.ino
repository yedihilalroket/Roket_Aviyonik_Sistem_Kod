#include<Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include<Arduino.h>


#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;


#define SS      18
#define RST     14
#define DI0     26
#define BAND    433E6  //915E6 

int16_t h;
int i;  
void setup() {
  
  Serial.begin(9600);
  
  
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
  while (1) {}
  }
  
  pinMode(12,OUTPUT); //Send success, LED will bright 1 second
  pinMode(16,OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH);
  LoRa.setPins(SS,RST,DI0);
  Serial.println("LoRa Sender");
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initial OK!");
  delay(350);
  pinMode(25, OUTPUT);
  digitalWrite (25,LOW);
}
  
void loop() {
    
  //Serial.print("Pressure = ");
  //Serial.print(bmp.readPressure());
  //Serial.println(" Pa");
    
  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  
  Serial.print("Altitude = ");
  Serial.print(bmp.readAltitude());
  Serial.println(" meters");

  //Serial.print("Pressure at sealevel (calculated) = ");
  //Serial.print(bmp.readSealevelPressure());
  //Serial.println(" Pa");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
  Serial.print("Real altitude = ");
  h = bmp.readAltitude(102000);
  Serial.print(h);
  Serial.println(" meters");

  if(h >= 110){
    
    for(i = 0; i<=20; i++){
      
      digitalWrite (25,HIGH);
      delayMicroseconds(1500);
      digitalWrite (25,LOW);
      delay(20);
  }
  }
  
  else{
    
    digitalWrite (12,LOW);
    for(i = 0; i<=20; i++){
        
      digitalWrite (25,HIGH);
      delayMicroseconds(500);
      digitalWrite (25,LOW);
      delay(20);
        }
    }

  Serial.println();
 
  LoRa.beginPacket();
  
  LoRa.print("Yukseklik ");
  LoRa.println(h);
  
  LoRa.endPacket();
  
}
