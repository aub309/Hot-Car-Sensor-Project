//Hot Car Detection Program
//By Andrew Bloschichak, Steven Santangelo, Michael Sinyagin

//This so far is the rough framework. 
//As of now, parts for the pressure, motion, and temperature sensors have arrived. This is what we have as of now 
//New parts have been added to the code, and tests shows it is now responding more, meaning it is compiling and responding. 
//Also the code has been recently edited to add detections for the temperature thresholds.

#include <GSM.h> //Needed for sending the text message and 911 call

#define PINNUMBER "" //PIN number for the SIM Card used in the device

// initialize the library instance
GSM gsmAccess; //For the phone functions
GSM_SMS sms; //For the text message part
GSMVoiceCall vcs; //For the phone call part for phoning 911

const int TEMP0 = 0;   //Temperature Sensor is on pin A0
int CO2 = 1;    //CO2 Sensor is on pin 1
int motionDetect1 = 2;   //Motion Sensor 1 is on pin 2
int motionDetect2 = 3;   //Motion Sensor 2 is on pin 3
int pirState1 = LOW;      //We start, assuming no motion detected
int pirState2 = LOW;      //We start, assuming no motion detected
int pressureSensor = 4;  //Pressure Sensor is on pin 4

const int lower_bound = 80; //Lower bound for the temperature detection 
const int mid_bound1 = 85; //Middle bound 1 for the temperature detection 
const int mid_bound2 = 90; //Middle bound 2 for the temperature detection 
const int upper_bound = 95; //Upper bound for the temperature detection 

int val = 0;     //Variable to hold analog reading. 
int valmotion1 = 0;  //Variable to hold motion reading of first motion sensor
int valmotion2 = 0;  //Variable to hold motion reading of second motion sensor

boolean motion1 = false; //Variable for the first Motion Detection
boolean motion2 = false; //Variable for the second Motion Detection
boolean pressure = false; //Variable for the pressure detection

String call911 = "7179820203";  // variable for the 911 call, currently wired to my number as to keep from accidentally phoning the police
char call911buffer[11]; //Converts the call911 into Char Array for it to be used in vcs

void setup() {
  Serial.begin(9600);
  pinMode(motionDetect1, INPUT);  // declare motionDetect1 as input
  pinMode(motionDetect2, INPUT);  // declare motionDetect2 as input
   while (!Serial) {
     ; // wait for serial port to connect. Needed for native USB port only
   }

   Serial.println("SMS Messages Sender");

   // connection state
   boolean notConnected = true;
  // put your setup code here, to run once:

}

void loop() {
   call911.toCharArray(call911buffer, 3); //Convert the 911 phone number to a char array to be used in call911
   val = analogRead(TEMP0); //Get readings from the temperature sensor 
   pressure = analogRead(pressureSensor); //Get readings from the pressure detection
   float voltage, degreesC, degreesF;   //Variables for the pressure

   voltage = getVoltage(TEMP0); //Voltage reading of the temperature sensor
   degreesC = (voltage - 0.5) * 100.0; //Convert Temperature Sensor voltage to degrees Celsius
   degreesF = degreesC * (9.0/5.0) + 32.0; //Convert Degrees Celsius to Degrees Fahrenheit

   valmotion1 = digitalRead(motionDetect1);  // read input value
   valmotion2 = digitalRead(motionDetect2);  // read input value

   //Test outputs for Temperature Sensors
   Serial.print("voltage: ");
   Serial.print(voltage);
   Serial.print("  deg C: ");
   Serial.print(degreesC);
   Serial.print("  deg F: ");
   Serial.println(degreesF);

    //Test both motion sensors
    //Motion Sensor 1
    if (valmotion1 == HIGH) {
      motion1 = true;
      Serial.println("Motion1 detected!");
      if (pirState1 == LOW) {
        // we have just turned on
        // We only want to print on the output change, not state
        pirState1 = HIGH;
      }
    }
    else {
      motion1 = false;
      Serial.println("No Motion1 detected.");
      if (pirState1 == HIGH){
        // we have just turned of
        // We only want to work on the output change, not state
        pirState1 = LOW;
      }
    } 
    //Motion Sensor 2
    if (valmotion2 == HIGH) {
      motion2 = true;
      Serial.println("Motion2 detected!");
      if (pirState2 == LOW) {
        // we have just turned on
        // We only want to print on the output change, not state
        pirState2 = HIGH;
      }
    }
    else {
      motion2 = false;
      Serial.println("No Motion2 detected.");
      if (pirState2 == HIGH){
        // we have just turned of
        // We only want to work on the output change, not state
        pirState2 = LOW;
      }
    }
    //Start when both Motion Sensors come up true
    if (motion1 == true && motion2 == true) {
      if (CO2 <= 2) { //Test for CO2
        Serial.println("CO2 concentration high!");
        //TODO code for the CO2 Sensor once parts arrive. 
      }
      
      if (lower_bound <= degreesF && degreesF < mid_bound1) { //Test for above 80 degrees
        Serial.println("Temperature threshold 1");
        //TODO code for the first threshold once parts arrive
      }
      else if (mid_bound1 <= degreesF && degreesF < mid_bound2) { //Test for above 85 degrees
        Serial.println("Temperature threshold 2");
        //TODO code for the second threshold once parts arrive
      }
      else if (mid_bound2 <= degreesF && degreesF < upper_bound) { //Test for above 85 degrees
        Serial.println("Temperature threshold 3");
        //TODO code for the third threshold once parts arrive
      }
      else if (degreesF >= upper_bound) { //Test for above 85 degrees
        Serial.println("Temperature threshold 4");
        //TODO code for the last threshold once parts arrive
        vcs.voiceCall(call911buffer); //Phone 911 here

        
        vcs.hangCall(); //Hang up 911
      }
      else {
        Serial.println("No Temperature threshold");
        //TODO code for no severe temperature threshold once parts arrive
      }
    }
    else {

    }

    // put your main code here, to run repeatedly:

    delay(1000); // used to make it repeat once per second
  }

float getVoltage(int pin) //Function required for the voltage of the Temperature Sensor
{
  return (analogRead(pin) * 0.004882814);
}

int readSerial(char result[]) {
   int i = 0;
   while (1) {
     while (Serial.available() > 0) {
       char inChar = Serial.read();
       if (inChar == '\n') {
         result[i] = '\0';
         Serial.flush();
         return 0;
       }
       if (inChar != '\r') {
         result[i] = inChar;
         i++;
       }
     }
   }
}
  

