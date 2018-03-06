
/* This software will control the four hub motors of the wheels and the four turn motors for the wheels. 
 *  Also it will feed back current speed and direction for each wheel. For now the getDesiredInfo()and writeCurrentInfo()mehtodws will be 
 *  a shell until that software has been completed. We could also easilty make this into a loop and do it for wheels 0 through 3.
 */

 //2-14-18 all eight motors correctly track position and rpm
//#include "returnedVariables.h"
#include "parsedCmd.h"
#include "wheelControl.h"
#include "pinDef.h"


//Inits for all methods
int Hz = 10; //cycle time for main processing

//Creating what is needed to parse commands

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

boolean resetWheel0;

struct parsedCmd parsedCmdWh0;
struct parsedCmd parsedCmdWh1;
struct parsedCmd parsedCmdWh2;
struct parsedCmd parsedCmdWh3;
struct parsedCmd parsedCmdWheelArray[4];

void printWheelDesired(struct parsedCmd dbWheel)
{
  Serial.print("status -> ");
  Serial.print("wheel: ");
  Serial.print(dbWheel.wheelNum);
  Serial.print(" bearing: ");
  Serial.print(dbWheel.wheelBearing);
  Serial.print(" speed: ");
  Serial.println(dbWheel.wheelSpeed);
}

struct parsedCmd zeroParsedWheel(int wheelnum)
{
  struct parsedCmd newWheel;
  newWheel.wheelNum = wheelnum;
  newWheel.wheelBearing = 0;
  newWheel.wheelSpeed = 0;
  return newWheel;
}

struct parsedCmd parseRxCmd(String inputString)
{
  parsedCmd toReturn;
  int i = 0;
  int wheelNum = -1;
  
  int speedSize = 8;
  String speedString = "";
  float wheelSpeed;

  int bearingSize = 8;
  String bearingString = "";
  int wheelBearing;
  
  while(inputString[i] != 0)
  {
    if (inputString[i] == 'W')
    {
      wheelNum = inputString[i + 1] - 48;
      //wheelNum = 1;
      Serial.println(wheelNum);
      
    } 
    if (inputString[i] == 'B')
    {
      bearingString = inputString.substring(i+1, i+1+bearingSize);
      wheelBearing = bearingString.toFloat();
      //wheelBearing = 45.0;
      Serial.print("bearing: ");
      Serial.println(wheelBearing);
    }
    if (inputString[i] == 'S')
    {
      speedString = inputString.substring(i+1, i+1+speedSize);
      wheelSpeed  = speedString.toFloat(); 
      //wheelSpeed = 80.0;
      Serial.print("speed: ");
      Serial.println(wheelSpeed);
    }
    /*
     * for debugging
    if (inputString[i] == '\n')
    {
      mySerial.print("final -> ");
      mySerial.print("wheel: ");
      mySerial.print(wheelNum);
      mySerial.print(" bearing: ");
      mySerial.print(wheelBearing);
      mySerial.print(" speed: ");
      mySerial.println(wheelSpeed);
    }
    */
    i++;
  }
  toReturn.wheelNum = wheelNum;
  toReturn.wheelBearing = wheelBearing;
  toReturn.wheelSpeed = wheelSpeed;
  return toReturn;
}


// Inits for wheel 0 HUB motor interrupt routine

unsigned long beginingTicWheel0 = millis();
unsigned long beginingTic = millis();
int countWheel0 = 0;
float SumWheel0 = 0;
unsigned long CurrentTimeWheel0 = 0;
unsigned long LastIntTimeWheel0 = millis();
boolean FirstPassWheel0 = true;
boolean backToZeroWheel0 = false;
float AvgDeltaTimeWheel0 = 10000001;
int iWheel0;
float DeltaTimeWheel0[40];

/*
 * This method is called when the interrupt for wheel 0's hub (hall sensor) goes high. It is calculating the average time 
 * between interrupts to get an RPM. However, it needs to handle the case when the wheel is stopped and starts up. Hence 
 * the first interrupt after it has been stopped can't be used to calculate a time bewtween interrups (we need two), so  
 * teh first time we will calculate a time between intrrupts and get an RPM is during the second interrupt.
 */

void counterWheel0() {
  // Need first pass logic to set the last interrupt time to get an accurate time between interrupts on the second pass
  if ((FirstPassWheel0 or backToZeroWheel0)) {
    LastIntTimeWheel0 = millis();
    AvgDeltaTimeWheel0 = 10000001; // this is just a really large number to signify the wheel is not moving.
    FirstPassWheel0 = false;
    backToZeroWheel0 = false;
  }
  else {

    /* Here we are calculating the time bewteen this interrupt and the last and storing that value in an array, with
        index of the variable count (which starts at 0). Then each time we need to get an average time between interrupts
        which is what the main loop needs, and since we don't know when that we be run conpared to the interrups. We also need to
        increment the counter for the next interrupt.
    */
    CurrentTimeWheel0 = millis();
    DeltaTimeWheel0[countWheel0] = (float)(CurrentTimeWheel0 - LastIntTimeWheel0);
    
    LastIntTimeWheel0 = CurrentTimeWheel0;
    for (iWheel0 = 0; iWheel0 <= countWheel0; iWheel0++) {
      SumWheel0 = SumWheel0 + DeltaTimeWheel0[iWheel0];  
    }
    AvgDeltaTimeWheel0 = SumWheel0 /(float)(countWheel0 + 1);
    SumWheel0 = 0;
    countWheel0++;
  }

}

// Inits for planetary motor interrupt routine wheel 0

int encposWheel0 = 0;

/*
 *  This function gets called when the interrupt encoder A for wheel 0 changes. It looks at the position of both
 *  encoders to determine the direction of the motor.
 */
void encoderWheel0(){

  if (digitalRead(encAWheel0) == digitalRead(encBWheel0)){
    encposWheel0++;
  }
  else {
    encposWheel0--;
  }
}

/*
 * This method is called when the interrupt for wheel 1's hub (hall sensor) goes high. It is calculating the average time 
 * between interrupts to get an RPM. However, it needs to handle the case when the wheel is stopped and starts up. Hence 
 * the first interrupt after it has been stopped can't be used to calculate a time bewtween interrups (we need two), so  
 * teh first time we will calculate a time between intrrupts and get an RPM is during the second interrupt.
 */

// Inits for wheel 1 interrupt routine
int countWheel1 = 0;
float SumWheel1 = 0;
unsigned long CurrentTimeWheel1 = 0;
unsigned long LastIntTimeWheel1 = millis();
boolean FirstPassWheel1 = true;
boolean backToZeroWheel1 = false;
float AvgDeltaTimeWheel1 = 10000001;
int iWheel1;
float DeltaTimeWheel1[40];

void counterWheel1() {

  // Need first pass logic to set the last interrupt time to get an accurate time between interrupts on the second pass
  if ((FirstPassWheel1 or backToZeroWheel1)) {
    LastIntTimeWheel1 = millis();
    AvgDeltaTimeWheel1 = 10000001; // this is just a really large number to signify the wheel is not moving.
    FirstPassWheel1 = false;
    backToZeroWheel1 = false;
  }
  else {

    /* Here we are calculating the time bewteen this interrupt and the last and storing that value in an array, with
        index of the variable count (which starts at 0). Then each time we need to get an average time between interrupts
        which is what the main loop needs, and since we don't know when that we be run conpared to the interrups. We also need to
        increment the counter for the next interrupt.
    */
    CurrentTimeWheel1 = millis();
    DeltaTimeWheel1[countWheel1] = (float)(CurrentTimeWheel1 - LastIntTimeWheel1);
    
    LastIntTimeWheel1 = CurrentTimeWheel1;
    for (iWheel1 = 0; iWheel1 <= countWheel1; iWheel1++) {
      SumWheel1 = SumWheel1 + DeltaTimeWheel1[iWheel1];  
    }
    AvgDeltaTimeWheel1 = SumWheel1 /(float)(countWheel1 + 1);
    SumWheel1 = 0;
    countWheel1++;
  }

}

// Inits for planetary motor interrupt routine wheel 0

int encposWheel1 = 0;

/*
 *  This function gets called when the interrupt encoder A for wheel 0 changes. It looks at the position of both
 *  encoders to determine the direction of the motor.
 */
void encoderWheel1(){

  if (digitalRead(encAWheel1) == digitalRead(encBWheel1)){
    encposWheel1++;
  }
  else {
    encposWheel1--;
  }
}

/*
 * This method is called when the interrupt for wheel 2's hub (hall sensor) goes high. It is calculating the average time 
 * between interrupts to get an RPM. However, it needs to handle the case when the wheel is stopped and starts up. Hence 
 * the first interrupt after it has been stopped can't be used to calculate a time bewtween interrups (we need two), so  
 * teh first time we will calculate a time between intrrupts and get an RPM is during the second interrupt.
 */

// Inits for wheel 2 interrupt routine
int countWheel2 = 0;
float SumWheel2 = 0;
unsigned long CurrentTimeWheel2 = 0;
unsigned long LastIntTimeWheel2 = millis();
boolean FirstPassWheel2 = true;
boolean backToZeroWheel2 = false;
float AvgDeltaTimeWheel2 = 10000001;
int iWheel2;
float DeltaTimeWheel2[40];

void counterWheel2() {
  // Need first pass logic to set the last interrupt time to get an accurate time between interrupts on the second pass
  if ((FirstPassWheel2 or backToZeroWheel2)) {
    LastIntTimeWheel2 = millis();
    AvgDeltaTimeWheel2 = 10000001; // this is just a really large number to signify the wheel is not moving.
    FirstPassWheel2 = false;
    backToZeroWheel2 = false;
  }
  else {

    /* Here we are calculating the time bewteen this interrupt and the last and storing that value in an array, with
        index of the variable count (which starts at 0). Then each time we need to get an average time between interrupts
        which is what the main loop needs, and since we don't know when that we be run conpared to the interrups. We also need to
        increment the counter for the next interrupt.
    */
    CurrentTimeWheel2 = millis();
    DeltaTimeWheel2[countWheel2] = (float)(CurrentTimeWheel2 - LastIntTimeWheel2);
    
    LastIntTimeWheel2 = CurrentTimeWheel2;
    for (iWheel2 = 0; iWheel2 <= countWheel2; iWheel2++) {
      SumWheel2 = SumWheel2 + DeltaTimeWheel2[iWheel2];  
    }
    AvgDeltaTimeWheel2 = SumWheel2 /(float)(countWheel2 + 1);
    SumWheel2 = 0;
    countWheel2++;
  }

}

// Inits for planetary motor interrupt routine wheel 0

int encposWheel2 = 0;

/*
 *  This function gets called when the interrupt encoder A for wheel 0 changes. It looks at the position of both
 *  encoders to determine the direction of the motor.
 */
void encoderWheel2(){

  if (digitalRead(encAWheel2) == digitalRead(encBWheel2)){
    encposWheel2++;
  }
  else {
    encposWheel2--;
  }
}

/*
 * This method is called when the interrupt for wheel 3's hub (hall sensor) goes high. It is calculating the average time 
 * between interrupts to get an RPM. However, it needs to handle the case when the wheel is stopped and starts up. Hence 
 * the first interrupt after it has been stopped can't be used to calculate a time bewtween interrups (we need two), so  
 * teh first time we will calculate a time between intrrupts and get an RPM is during the second interrupt.
 */

// Inits for wheel 3 interrupt routine
int countWheel3 = 0;
float SumWheel3 = 0;
unsigned long CurrentTimeWheel3 = 0;
unsigned long LastIntTimeWheel3 = millis();
boolean FirstPassWheel3 = true;
boolean backToZeroWheel3 = false;
float AvgDeltaTimeWheel3 = 10000001;
int iWheel3;
float DeltaTimeWheel3[40];

void counterWheel3() {
  // Need first pass logic to set the last interrupt time to get an accurate time between interrupts on the second pass
  if ((FirstPassWheel3 or backToZeroWheel3)) {
    LastIntTimeWheel3 = millis();
    AvgDeltaTimeWheel3 = 10000001; // this is just a really large number to signify the wheel is not moving.
    FirstPassWheel3 = false;
    backToZeroWheel3 = false;
  }
  else {

    /* Here we are calculating the time bewteen this interrupt and the last and storing that value in an array, with
        index of the variable count (which starts at 0). Then each time we need to get an average time between interrupts
        which is what the main loop needs, and since we don't know when that we be run conpared to the interrups. We also need to
        increment the counter for the next interrupt.
    */
    CurrentTimeWheel3 = millis();
    DeltaTimeWheel3[countWheel3] = (float)(CurrentTimeWheel3 - LastIntTimeWheel3);
    
    LastIntTimeWheel3 = CurrentTimeWheel3;
    for (iWheel3 = 0; iWheel3 <= countWheel3; iWheel3++) {
      SumWheel3 = SumWheel3 + DeltaTimeWheel3[iWheel3];  
    }
    AvgDeltaTimeWheel3 = SumWheel3 /(float)(countWheel3 + 1);
    SumWheel3 = 0;
    countWheel3++;
  }

}

// Inits for planetary motor interrupt routine wheel 3

int encposWheel3 = 0;

/*
 *  This function gets called when the interrupt encoder A for wheel 0 changes. It looks at the position of both
 *  encoders to determine the direction of the motor.
 */
void encoderWheel3(){

  if (digitalRead(encAWheel3) == digitalRead(encBWheel3)){
    encposWheel3++;
  }
  else {
    encposWheel3--;
  }
}

wheelControl* wheel0Control;
wheelControl* wheel1Control;
wheelControl* wheel2Control;
wheelControl* wheel3Control;

void setup() {
  // put your setup code here, to run once:
  // this sets the pins we are going to use and sets up the serial output for debugging.
  // setup for wheel 0

  // for hub motor
  pinMode(vrPinWheel0, OUTPUT); // this tells the teensy OS that these pins will be sued for output.
  pinMode(zfPinWheel0, OUTPUT);
  attachInterrupt(hubIntWheel0, counterWheel0, RISING); // this attaches interrupt 1 to the counterWO method (wheel 0 hub motor)

   //for planetary motor 

  pinMode(pwmPinWheel0, OUTPUT);
  pinMode(dirPinWheel0, OUTPUT);
  pinMode(encAWheel0, INPUT);
  pinMode(encBWheel0, INPUT);
  attachInterrupt(encAWheel0, encoderWheel0, CHANGE);
  
 // setup for wheel 1, hub motor
  
  pinMode(vrPinWheel1, OUTPUT); // this tells the teensy OS that these pins will be sued for output.
  pinMode(zfPinWheel1, OUTPUT);
  attachInterrupt(hubIntWheel1, counterWheel1, RISING); // this attaches interrupt 1 to the counterWO method (wheel 0 hub motor)

  //for planetary motor 

  pinMode(pwmPinWheel1, OUTPUT);
  pinMode(dirPinWheel1, OUTPUT);
  pinMode(encAWheel1, INPUT);
  pinMode(encBWheel1, INPUT);
  attachInterrupt(encAWheel1, encoderWheel1, CHANGE);
  

  // setup for wheel 2, hub motor
  
  pinMode(vrPinWheel2, OUTPUT); // this tells the teensy OS that these pins will be sued for output.
  pinMode(zfPinWheel2, OUTPUT);
  attachInterrupt(hubIntWheel2, counterWheel2, RISING); // this attaches interrupt 1 to the counterWO method (wheel 0 hub motor)

  //for planetary motor 

  pinMode(pwmPinWheel2, OUTPUT);
  pinMode(dirPinWheel2, OUTPUT);
  pinMode(encAWheel2, INPUT);
  pinMode(encBWheel2, INPUT);
  attachInterrupt(encAWheel2, encoderWheel2, CHANGE);
  
 // setup for wheel 3, hub motor
  
  pinMode(vrPinWheel3, OUTPUT); // this tells the teensy OS that these pins will be sued for output.
  pinMode(zfPinWheel3, OUTPUT);
  attachInterrupt(hubIntWheel3, counterWheel3, RISING); // this attaches interrupt 1 to the counterWO method (wheel 0 hub motor)

  //for planetary motor 

  pinMode(pwmPinWheel3, OUTPUT);
  pinMode(dirPinWheel3, OUTPUT);
  pinMode(encAWheel3, INPUT);
  pinMode(encBWheel3, INPUT);
  attachInterrupt(encAWheel3, encoderWheel3, CHANGE);
  

  
  Serial.begin(115200); // sets up a serial interface to print to the monitor for debugging
  
  Serial.println("123");
  interrupts();  //Allows for intrrupts to call their associated methods

// need to intstaniate the four wheels
  wheel0Control = new wheelControl();
  wheel1Control = new wheelControl();
  wheel2Control = new wheelControl();
  wheel3Control = new wheelControl();

  parsedCmdWh0 = zeroParsedWheel(0);
  parsedCmdWh1 = zeroParsedWheel(1);
  parsedCmdWh2 = zeroParsedWheel(2);
  parsedCmdWh3 = zeroParsedWheel(3);
  
  parsedCmdWheelArray[0] = parsedCmdWh0;
  parsedCmdWheelArray[1] = parsedCmdWh1;
  parsedCmdWheelArray[2] = parsedCmdWh2;
  parsedCmdWheelArray[3] = parsedCmdWh3;

}

//Inits for main loop
   // we need a struct for the return variables when we call the wheel control methods

  
//  wheel0Control.returnVariables = returnVariablesWheel0;
//  wheel1Control.returnVariables = returnVariablesWheel1;
//  wheel2Control.returnVariables = returnVariablesWheel2;
//  wheel3Control.returnVariables = returnVariablesWheel3;

  float AvgerageDeltaTimeWheel0 = 10000001;
  float AvgerageDeltaTimeWheel1 = 10000001;
  float AvgerageDeltaTimeWheel2 = 10000001;
  float AvgerageDeltaTimeWheel3 = 10000001;

  unsigned long lastInterruptTimeWheel0;
  unsigned long lastInterruptTimeWheel1;
  unsigned long lastInterruptTimeWheel2;
  unsigned long lastInterruptTimeWheel3;

  int encoderPosWheel0 = 0; 
  int encoderPosWheel1 = 0;
  int encoderPosWheel2 = 0; 
  int encoderPosWheel3 = 0;


  long tempSpeedCheck0 = 0, tempSpeedCheck1 = 0, tempSpeedCheck2 = 0, tempSpeedCheck3 = 0;
  int writeCount = 0;

    


void loop() {
    /*
     We want the main loop to run at a specific frequency (normally you would set this in a realtime OS, but Arduino
     doesn't have that). So we use the system clock and keep checking to see if enough time has passed, and if so then
     execute the main loop.
  */
  if (millis() - beginingTic  > 1000 / Hz)  
  {

    beginingTic = millis();// reset the the last time this was executed.



/*
 * We need to diable interrupts during the time this loop reads the values that are manuipulated by the interrupt
 * software. This will keep us from getting corrupted data on the off chance that both threads are trying to
 * read/write the same variable.
 */
    noInterrupts();
    float AverageDeltaTimeWheel0 = AvgDeltaTimeWheel0;
    lastInterruptTimeWheel0 = LastIntTimeWheel0;
    countWheel0 = 0;
    encoderPosWheel0 = encposWheel0;

    float AverageDeltaTimeWheel1 = AvgDeltaTimeWheel1;
    lastInterruptTimeWheel1 = LastIntTimeWheel1;
    countWheel1 = 0;
    encoderPosWheel1 = encposWheel1;

    float AverageDeltaTimeWheel2 = AvgDeltaTimeWheel2;
    lastInterruptTimeWheel2 = LastIntTimeWheel2;
    countWheel2 = 0;
    encoderPosWheel2 = encposWheel2;

    float AverageDeltaTimeWheel3 = AvgDeltaTimeWheel3;
    lastInterruptTimeWheel3 = LastIntTimeWheel3;
    countWheel3 = 0;
    encoderPosWheel3 = encposWheel3;
    
    interrupts();
    
    float newdesiredRPMWheel0, newdesiredRPMWheel1, newdesiredRPMWheel2, newdesiredRPMWheel3;
    newdesiredRPMWheel0 = parsedCmdWheelArray[0].wheelSpeed;
    //newdesiredRPMWheel0 = 100.0;
    newdesiredRPMWheel1 = parsedCmdWheelArray[1].wheelSpeed;
    //newdesiredRPMWheel1 = 100.0;
    newdesiredRPMWheel2 = parsedCmdWheelArray[2].wheelSpeed;
    newdesiredRPMWheel3 = parsedCmdWheelArray[3].wheelSpeed;
    
    //newdesiredRPMWheel0 = newdesiredRPMWheel1 = newdesiredRPMWheel2 = newdesiredRPMWheel3 = 200;
    float desiredAngleWheel0, desiredAngleWheel1, desiredAngleWheel2, desiredAngleWheel3;
    desiredAngleWheel0 = parsedCmdWheelArray[0].wheelBearing;
    desiredAngleWheel1 = parsedCmdWheelArray[1].wheelBearing;
    desiredAngleWheel2 = parsedCmdWheelArray[2].wheelBearing;
    desiredAngleWheel3 = parsedCmdWheelArray[3].wheelBearing;


    //desiredAngleWheel0 = desiredAngleWheel1 = desiredAngleWheel2 = desiredAngleWheel3 = 0;
    
  // call the method for the wheel 0 calcuations
    struct returnVariables returnVariablesWheel0 = wheel0Control->calculate(newdesiredRPMWheel0, desiredAngleWheel0, AverageDeltaTimeWheel0,
                                  lastInterruptTimeWheel0, encoderPosWheel0);
    
  // call the method for the wheel 1 calcuations

    struct returnVariables returnVariablesWheel1 = wheel1Control->calculate(newdesiredRPMWheel1, desiredAngleWheel1, AverageDeltaTimeWheel1, 
                                  lastInterruptTimeWheel1, encoderPosWheel1);
    
      // call the method for the wheel 2 calcuations
    struct returnVariables returnVariablesWheel2 = wheel2Control->calculate(newdesiredRPMWheel2, desiredAngleWheel2, AverageDeltaTimeWheel2, 
                                  lastInterruptTimeWheel2, encoderPosWheel2);
    
      // call the method for the wheel 3 calcuations
    struct returnVariables returnVariablesWheel3 = wheel3Control->calculate(newdesiredRPMWheel3, desiredAngleWheel3, AverageDeltaTimeWheel3, 
                                  lastInterruptTimeWheel3, encoderPosWheel3);


    //create temp floats to store last value so that new data is only sent to the wheels once (no repeat)

    // if it changes to something above 50
     if((returnVariablesWheel0.speedCheck != tempSpeedCheck0) && (returnVariablesWheel0.speedCheck > 50.0)){
       analogWrite(power0, LOW);
       analogWrite(vrPinWheel0, returnVariablesWheel0.speedCheck);
       digitalWrite(zfPinWheel0, returnVariablesWheel0.forwardBackward);
       writeCount++;
       tempSpeedCheck0 = returnVariablesWheel0.speedCheck;
     }
     if((returnVariablesWheel1.speedCheck != tempSpeedCheck1) && (returnVariablesWheel1.speedCheck > 50.0)){
       analogWrite(power1, LOW);
       analogWrite(vrPinWheel1, returnVariablesWheel1.speedCheck);
       digitalWrite(zfPinWheel1, returnVariablesWheel1.forwardBackward);
       writeCount++;
       tempSpeedCheck1 = returnVariablesWheel1.speedCheck;
     }
     if((returnVariablesWheel2.speedCheck != tempSpeedCheck2) && (returnVariablesWheel2.speedCheck > 50.0)){
      analogWrite(power2, LOW);
       analogWrite(vrPinWheel2, returnVariablesWheel2.speedCheck);
       digitalWrite(zfPinWheel2, returnVariablesWheel2.forwardBackward);
       writeCount++;
       tempSpeedCheck2 = returnVariablesWheel2.speedCheck;
     }
     if((returnVariablesWheel3.speedCheck != tempSpeedCheck3) && (returnVariablesWheel3.speedCheck > 50.0)){
      analogWrite(power3, LOW);
       analogWrite(vrPinWheel3, returnVariablesWheel3.speedCheck);
       digitalWrite(zfPinWheel3, returnVariablesWheel3.forwardBackward);
       writeCount++;
       tempSpeedCheck3 = returnVariablesWheel3.speedCheck;
     }

     //if it changes to 0
     if((returnVariablesWheel0.speedCheck != tempSpeedCheck0) && (returnVariablesWheel0.speedCheck < 50.0)){
       analogWrite(power0, HIGH);
       analogWrite(vrPinWheel0, returnVariablesWheel0.speedCheck);
       digitalWrite(zfPinWheel0, returnVariablesWheel0.forwardBackward);
       writeCount++;
       tempSpeedCheck0 = returnVariablesWheel0.speedCheck;
     }
    if((returnVariablesWheel1.speedCheck != tempSpeedCheck1) && (returnVariablesWheel1.speedCheck < 50.0)){
       analogWrite(power1, HIGH);
       analogWrite(vrPinWheel1, returnVariablesWheel1.speedCheck);
       digitalWrite(zfPinWheel1, returnVariablesWheel1.forwardBackward);
       writeCount++;
       tempSpeedCheck1 = returnVariablesWheel1.speedCheck;
     }
     if((returnVariablesWheel2.speedCheck != tempSpeedCheck2) && (returnVariablesWheel2.speedCheck < 50.0)){
      analogWrite(power2, HIGH);
       analogWrite(vrPinWheel2, returnVariablesWheel2.speedCheck);
       digitalWrite(zfPinWheel2, returnVariablesWheel2.forwardBackward);
       writeCount++;
       tempSpeedCheck2 = returnVariablesWheel2.speedCheck;
     }
     if((returnVariablesWheel3.speedCheck != tempSpeedCheck3) && (returnVariablesWheel3.speedCheck < 50.0)){
      analogWrite(power3, HIGH);
       analogWrite(vrPinWheel3, returnVariablesWheel3.speedCheck);
       digitalWrite(zfPinWheel3, returnVariablesWheel3.forwardBackward);
       writeCount++;
       tempSpeedCheck3 = returnVariablesWheel3.speedCheck;
     }
     Serial.print("SpeedCheck0: ");
     Serial.println(returnVariablesWheel0.speedCheck);
     Serial.print("TempSpeedCheck0: ");
     Serial.println(tempSpeedCheck0);
     Serial.print("SpeedCheck1: ");
     Serial.println(returnVariablesWheel1.speedCheck);
     Serial.print("TempSpeedCheck1: ");
     Serial.println(tempSpeedCheck1);
//     Serial.print("SpeedCheck2: ");
//     Serial.println(returnVariablesWheel2.speedCheck);
//     Serial.print("SpeedCheck3: ");
//     Serial.println(returnVariablesWheel3.speedCheck);
//     Serial.print("WriteCount:  ");
//     Serial.println(writeCount);
     
//     Serial.println(millis()- lastInterruptTimeWheel0);
//

//     analogWrite(vrPinWheel0, returnVariablesWheel0.speedCheck);
//     digitalWrite(zfPinWheel0, returnVariablesWheel0.forwardBackward);

//PLANETARY CONTROL
     Serial.print("PLAN0 motorSpeed: ");
     Serial.println(returnVariablesWheel0.motorSpeed);
     Serial.print("PLAN0 direction: ");
     Serial.println(returnVariablesWheel0.planMotorDirection);
     analogWrite(pwmPinWheel0, returnVariablesWheel0.motorSpeed);
     digitalWrite(dirPinWheel0, returnVariablesWheel0.planMotorDirection);
     
     Serial.print("PLAN1 motorSpeed: ");
     Serial.println(returnVariablesWheel1.motorSpeed);
     Serial.print("PLAN1 direction: ");
     Serial.println(returnVariablesWheel1.planMotorDirection);
     analogWrite(pwmPinWheel1, returnVariablesWheel1.motorSpeed);
     digitalWrite(dirPinWheel1, returnVariablesWheel1.planMotorDirection);
     
     Serial.print("PLAN2 motorSpeed: ");
     Serial.println(returnVariablesWheel2.motorSpeed);
     Serial.print("PLAN2 direction: ");
     Serial.println(returnVariablesWheel2.planMotorDirection);
     analogWrite(pwmPinWheel2, returnVariablesWheel2.motorSpeed);
     digitalWrite(dirPinWheel2, returnVariablesWheel2.planMotorDirection);
     
     Serial.print("PLAN3 motorSpeed: ");
     Serial.println(returnVariablesWheel3.motorSpeed);
     Serial.print("PLAN3 direction: ");
     Serial.println(returnVariablesWheel3.planMotorDirection);
     analogWrite(pwmPinWheel3, returnVariablesWheel3.motorSpeed);
     digitalWrite(dirPinWheel3, returnVariablesWheel3.planMotorDirection);

    printWheelDesired(parsedCmdWheelArray[0]);
    printWheelDesired(parsedCmdWheelArray[1]);
    printWheelDesired(parsedCmdWheelArray[2]);
    printWheelDesired(parsedCmdWheelArray[3]);
    
// output data back to other processor through serial transfer software (still to be written)
/*
  serialtransfer.sendBackInfo(returnVariablesWheel0.currentWheelRPM, returnVariablesWheel0.currentWheelAngle,
                                  returnVariablesWheel1.currentWheelRPM, returnVariablesWheel1.currentWheelAngle,
                                  returnVariablesWheel2.currentWheelRPM, returnVariablesWheel2.currentWheelAngle,
                                  returnVariablesWheel3.currentWheelRPM, returnVariablesWheel3.currentWheelAngle);  
*/
  }

  
  /*
   * This is outside the main command loop so there can be more time for updates
   * and if the updates take to long it will just execute what updates were available before 
   */
  while (Serial.available() and !(millis() - beginingTic  > 1000 / Hz) ) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      break;
      }
    }
  if (stringComplete) {
    struct parsedCmd thisCmd = parseRxCmd(inputString);
      Serial.print("final -> ");
      Serial.print("wheel: ");
      Serial.print(thisCmd.wheelNum);
      Serial.print(" bearing: ");
      Serial.print(thisCmd.wheelBearing);
      Serial.print(" speed: ");
      Serial.println(thisCmd.wheelSpeed);
    parsedCmdWheelArray[thisCmd.wheelNum].wheelBearing = thisCmd.wheelBearing;
    parsedCmdWheelArray[thisCmd.wheelNum].wheelSpeed = thisCmd.wheelSpeed;
    //Serial.println(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  
}

