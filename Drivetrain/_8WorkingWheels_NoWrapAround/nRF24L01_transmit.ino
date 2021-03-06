#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>


#include <SPI.h>  


RF24 myRadio (7, 8);
byte addresses[][6] = {"0"};

bool buttonPressed;
int buttonPin;

struct package
{
  int id=1;
  int command = 0;
};

typedef struct package Package;
Package data;


void setup()
{
  buttonPin = 3;

  pinMode(buttonPin, INPUT_PULLUP);
  
  Serial.begin(115200);
  myRadio.begin();  
  myRadio.setChannel(115); 
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.setDataRate(RF24_250KBPS) ; 
  myRadio.openWritingPipe(addresses[0]);
}

void loop()
{
  if(digitalRead(buttonPin) == LOW){
    if(myRadio.available()){
    while(myRadio.available()){
      myRadio.read(&data, sizeof(data));
    }
    Serial.print("\nPackage");
    Serial.print(data.id);
    Serial.print("\n");
  }
  
  myRadio.write(&data, sizeof(data)); 

  Serial.print("\nPackage:");
  Serial.print(data.id);
  Serial.print("\n");
  Serial.println(data.command);
  }
}
