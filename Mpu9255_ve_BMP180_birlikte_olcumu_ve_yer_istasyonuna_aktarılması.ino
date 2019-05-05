#include<Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include<Arduino.h>


#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;

//OLED pins to ESP32 GPIOs via this connecthin:
//OLED_SDA -- GPIO4
//OLED_SCL -- GPIO15
//OLED_RST -- GPIO16

#define SS      18
#define RST     14
#define DI0     26
#define BAND    433E6  //915E6 

int16_t h;
int i; 
int counter = 0;
const int MPU=0X68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ ;

void setup() {

  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x68);
  Wire.write(0); /* MPU-6050 çalıştırıldı */
  Wire.endTransmission(true);
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
    
 
  verileriOku();
  Serial.print(" | ivmeX = "); Serial.print(AcX);
  Serial.print(" | ivmeY = "); Serial.print(AcY);
  Serial.print(" | ivmeZ = "); Serial.print(AcZ);
  
  Serial.print(" | GyroX = "); Serial.print(GyX);
  Serial.print(" | GyroY = "); Serial.print(GyY);
  Serial.print(" | GyroZ = "); Serial.println(GyZ);
  
  Serial.print("Altitude = ");
  Serial.print(bmp.readAltitude());
  Serial.println(" meters");

 
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
    
    for(i = 0; i<=20; i++){
        
      digitalWrite (25,HIGH);
      delayMicroseconds(500);
      digitalWrite (25,LOW);
      delay(20);
        }
    }

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
  
 
  LoRa.print("Yukseklik ");
  LoRa.println(h);
  
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
  delay(500);
  /* 
  * Sırası ile okunan her iki byte birleştirilerek sırası ile değişkenlere yazdırıldı
  * Böylece IMU'dan tüm değerler okunmuş oldu
  * 0X3B adresi imu değerlerinden ilk sensörün değerine denk gelmektedir.
  * IMU'dan tüm değerlerin okunabilmesi için bu adresten başlandı
  */
}
