#include<Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include "SSD1306.h"
#include<Arduino.h>
#include <Adafruit_BMP085.h>

//OLED pins to ESP32 GPIOs via this connecthin:
//OLED_SDA -- GPIO4
//OLED_SCL -- GPIO15
//OLED_RST -- GPIO16
 
SSD1306  display(0x3c, 4, 15);
Adafruit_BMP085 bmp;


 
#define SS      18
#define RST     14
#define DI0     26
#define BAND    433E6  //915E6 
 
int counter = 0;
/***********************************************************************************************/
const int MPU=0x68;
float h;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ ;
/***********************************************************************************************/ 
void setup() {
  
  
  
  //Serial.print("Real altitude = ");
  //h = bmp.readAltitude(102000);
  //Serial.print(h);
  //Serial.println(" meters");
  pinMode(25,OUTPUT); //Send success, LED will bright 1 second
  pinMode(16,OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH);
/**********************************************************************************************/
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x68);
  Wire.write(0); /* MPU-6050 çalıştırıldı */
  Wire.endTransmission(true);
/**********************************************************************************************/   
  Serial.begin(115200);
  while (!Serial); //If just the the basic function, must connect to a computer
  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(5,5,"LoRa Sender");
  display.display();
   
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
  delay(350);
}
void loop() {
/************************************************************************************************/
  verileriOku();
  /* IMU'dan değerler okundu */
  
  /* Okunan değerler serial monitör'e yazdırılıyor */
  Serial.print(" | ivmeX = "); Serial.print(AcX);
  Serial.print(" | ivmeY = "); Serial.print(AcY);
  Serial.print(" | ivmeZ = "); Serial.print(AcZ);
  
  Serial.print(" | GyroX = "); Serial.print(GyX);
  Serial.print(" | GyroY = "); Serial.print(GyY);
  Serial.print(" | GyroZ = "); Serial.println(GyZ);
  
 /************************************************************************************************/ 
 //WİFİ LORA 32 Verici kart ekranına değerler yazdırılıyor
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(3, 5, "AcX = ");
  display.drawString(50, 5, String(AcX));
  display.drawString(3, 25,"AcY = ");
  display.drawString(50, 25, String(AcY));
  display.drawString(3, 45, "AcZ = ");
  display.drawString(50, 45, String(AcZ));
  display.display();
  // WİFİ LORA 32 Alıcı karta istenilen değerler gönderiliyor
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
  //LoRa.print("Yukseklik");
  //LoRa.println(b);
  
  LoRa.endPacket();
  
  digitalWrite(25, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);                       // wait for a second
  digitalWrite(25, LOW);    // turn the LED off by making the voltage LOW
  delay(500);                       // wait for a second
  //delay(500);
}
/***************************************************************************************************/
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
  delay(500);
  /* 
  * Sırası ile okunan her iki byte birleştirilerek sırası ile değişkenlere yazdırıldı
  * Böylece IMU'dan tüm değerler okunmuş oldu
  * 0X3B adresi imu değerlerinden ilk sensörün değerine denk gelmektedir.
  * IMU'dan tüm değerlerin okunabilmesi için bu adresten başlandı
  */
}
/******************************************************************************************************/
