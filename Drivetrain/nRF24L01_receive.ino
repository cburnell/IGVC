#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

#include <SPI.h>  
 

RF24 myRadio (7, 8); 
struct package
{
//  int id=0;
  int command = 0;
//  char  text[100] ="empty";
};

byte addresses[][6] = {"0"}; 
int16_t newData = 0;
int16_t oldData = 1;


typedef struct package Package;
Package data;

int IN1 = 4;
int IN2 = 4;

#define ON 0
#define OFF 1

void setup() 
{
  Serial.begin(115200);

  relay_init(); //INITIALIZING THE RELAY

  myRadio.begin(); 
  myRadio.setChannel(115); 
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.setDataRate( RF24_250KBPS ) ; 
  myRadio.openReadingPipe(1, addresses[0]);
  myRadio.startListening();
}

void loop()  
{  
  if ( myRadio.available()) 
  {

      newData = data.command;
      myRadio.read( &data, sizeof(data) );

      if(newData != oldData){
        Serial.write(newData);
        oldData = newData;
        relay_SetStatus(OFF);//if transmit changes power relay off
      }
    
  }
Serial.write(oldData);
Serial.println(oldData);
}

void relay_SetStatus(unsigned char status_1){
  digitalWrite(IN1, status_1);
}

void relay_init(void) {
  pinMode(IN1, OUTPUT);
  relay_SetStatus(ON);
  //delay(2000);
}

