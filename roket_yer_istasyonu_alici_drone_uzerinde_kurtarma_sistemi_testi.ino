#include <SPI.h>
#include <LoRa.h>
#include "SSD1306.h"
 
SSD1306  display(0x3c, 4, 15);
 
 
#define SS      18
#define RST     14
#define DI0     26
#define BAND    433E6
String bosluk;
int spacePos;
int b,c;

 
void setup() {
  pinMode(25, OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(16,OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
   
  Serial.begin(115200);
  while (!Serial); //if just the the basic function, must connect to a computer
  delay(350);
  
  display.drawString(5,5,"LoRa Receiver"); 
  display.display();
  SPI.begin(5,19,27,18);
  LoRa.setPins(SS,RST,DI0);
   
  if (!LoRa.begin(BAND))
  {
    display.drawString(5,25,"Starting LoRa failed!");
    while (1);
  }
  
  display.drawString(5,25,"LoRa Initializing OK!");
  display.display();
}
void loop()
{

  //paket alınıp alınmadığını kontrol ediyor
  int packetSize = LoRa.parsePacket();
  //paket alındı ise
  if (packetSize) 
  {
    // received a packets
    display.clear();
    display.setFont(ArialMT_Plain_16);
    
  
    // read packet
   
    while (LoRa.available()) 
   {
   
    {
    String a = LoRa.readString();
    Serial.println(a);
    spacePos = a.indexOf(' ');    
    b = a.substring(0,spacePos).toInt();  
    c = a.substring(spacePos+1).toInt();
    if(c >= 235){
      digitalWrite(13, HIGH);
      for(int i=0;i<=20;i++){
         digitalWrite (25,HIGH);
         delayMicroseconds(1500);
         digitalWrite (25,LOW);
         delay(20);
    
        }
      }
    else{
      digitalWrite(13, LOW);
       for(int i=0;i<=20;i++){
         digitalWrite (25,HIGH);
         delayMicroseconds(500);
         digitalWrite (25,LOW);
         delay(20);
    
        }
      }
    
     
     

    display.drawString(20,5, a);
    display.display();
    }
   }
   
  }
}
