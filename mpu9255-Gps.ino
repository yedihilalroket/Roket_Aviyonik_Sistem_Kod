#include <config.h>
#include "SSD1306.h"
#include<Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include<Arduino.h>
#include <TinyGPS++.h> // Library über http://arduiniana.org/libraries/tinygpsplus/ downloaden und installieren
#include <HardwareSerial.h> 

//TinyGPSPlus gps;
 

//HardwareSerial SerialGPS(2);


//OLED pins to ESP32 GPIOs via this connecthin:
//OLED_SDA -- GPIO4
//OLED_SCL -- GPIO15
//OLED_RST -- GPIO16

SSD1306  display(0x3c, 4, 15);

#define SS      18
#define RST     14
#define DI0     26
#define BAND    433E6  //915E6 

struct GpsDataState_t {
  double originLat = 0;
  double originLon = 0;
  double originAlt = 0;
  double distMax = 0;
  double dist = 0;
  double altMax = -999999;
  double altMin = 999999;
  double spdMax = 0;
  double prevDist = 0;
};
GpsDataState_t gpsState = {};
 

#define TASK_SERIAL_RATE 1000 // ms
uint32_t nextSerialTaskTs = 0;
uint32_t nextOledTaskTs = 0;

boolean tepe_noktasi = false;

int i; 
int counter = 0;
const int MPU=0X68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ ;

unsigned long startMillis_2;  //some global variables available anywhere in the program
unsigned long currentMillis_2;

const unsigned long period2 = 5000;
void setup() {
 
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x68);
  Wire.write(0); /* MPU-6050 çalıştırıldı */
  Wire.endTransmission(true);
  Serial.begin(9600);
  
  SerialGPS.begin(9600, SERIAL_8N1, 16, 17);
 
  // Initialising the UI will init  display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(5,5,"LoRa Sender");
  display.display();
  

  pinMode(12,OUTPUT); //Send success, LED will bright 1 second
  
  delay(50); 
 
  SPI.begin(5,19,27,18);
  LoRa.setPins(SS,RST,DI0);
  Serial.println("LoRa Sender");
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initial OK!");
  display.drawString(5,20,"LoRa Initializing OK!");
  display.display();
  delay(300);
  pinMode(25, OUTPUT);
  digitalWrite (25,LOW);
}
  
void loop() {

  static int p0 = 0;

  gpsState.originLat = gps.location.lat();
  gpsState.originLon = gps.location.lng();
  gpsState.originAlt = gps.altitude.meters();
 
  long writeValue;
 
  gpsState.distMax = 0;
  gpsState.altMax = -999999;
  gpsState.spdMax = 0;
  gpsState.altMin = 999999;


  while (SerialGPS.available() > 0) {
    gps.encode(SerialGPS.read());
  }
 


 

  if (nextSerialTaskTs < millis()) {
    Serial.print("LAT=");  Serial.println(gps.location.lat(), 6);
    Serial.print("LONG="); Serial.println(gps.location.lng(), 6);
    Serial.print("ALT=");  Serial.println(gps.altitude.meters());
    Serial.print("Sats=");  Serial.println(gps.satellites.value());
    Serial.print("DST: ");
    Serial.println(gpsState.dist, 1);
    nextSerialTaskTs = millis() + TASK_SERIAL_RATE;
  }
    
 
  verileriOku();
  Serial.print(" | ivmeX = "); Serial.print(AcX);
  Serial.print(" | ivmeY = "); Serial.print(AcY);
  Serial.print(" | ivmeZ = "); Serial.print(AcZ);
  
  Serial.print(" | GyroX = "); Serial.print(GyX);
  Serial.print(" | GyroY = "); Serial.print(GyY);
  Serial.print(" | GyroZ = "); Serial.println(GyZ);

  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(3, 5, "AcX = ");
  display.drawString(50, 5, String(AcX));
  display.drawString(3, 25,"AcY = ");
  display.drawString(50, 25, String(AcY));
  display.drawString(3, 45, "AcZ = ");
  display.drawString(50, 45, String(AcZ));
  display.display();
  
 
  
  Serial.println();
 
  LoRa.beginPacket();
  LoRa.print("AcX=");
  LoRa.println(AcX);
  LoRa.print("AcY=");
  LoRa.println(AcY);
  LoRa.print("AcZ=");
  LoRa.println(AcZ);
  LoRa.print("GyroX=");
  LoRa.println(GyX);
  LoRa.print("GyroY=");
  LoRa.println(GyY);
  LoRa.print("GyroZ=");
  LoRa.println(GyZ);

  
  LoRa.endPacket();
  
}
void verileriOku(){
  Wire.beginTransmission(MPU);
  /* I2C haberleşmesi yapılacak kart seçildi */
  Wire.write(0x3B); 
  /* 0x3B adresindeki register'a ulaşıldı */
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,12,true);
  /* 12 BYTE'lık veri istendi */
  
  AcX=Wire.read()<<8|Wire.read()/16384;   
  AcY=Wire.read()<<8|Wire.read()/16384; 
  AcZ=Wire.read()<<8|Wire.read()/16384; 
   
  GyX=Wire.read()<<8|Wire.read()/33; 
  GyY=Wire.read()<<8|Wire.read()/33;  
  GyZ=Wire.read()<<8|Wire.read()/33;
  
  Serial.println();
  delay(40);
  /* 
  * Sırası ile okunan her iki byte birleştirilerek sırası ile değişkenlere yazdırıldı
  * Böylece IMU'dan tüm değerler okunmuş oldu
  * 0X3B adresi imu değerlerinden ilk sensörün değerine denk gelmektedir.
  * IMU'dan tüm değerlerin okunabilmesi için bu adresten başlandı
  */
}
