//Hot Car Detection Program
//By Andrew Bloschichak, Steven Santangelo, Michael Sinyagin

//This so far is the rough framework. 
//As of now, parts for the pressure, motion, and temperature sensors, and the window opener have arrived. This is what we have as of now 
//New parts have been added to the code, for detecting pressure and CO2, as well as for opening the windows. 
//Also the code has been recently edited to add detections for the temperature thresholds.
//This time, I was able to allow it to connect to the SIM Card and allow it to send text messages
//Also we have now made it work in sending the emergency message, and also put the emergency message in a separate function as to allow 
//more object-oriented design and cleaner overall code. 

//As of now, this is the code we are using in the final project demonstration.

#include <GSM.h> //Needed for sending the text message and 911 call
#define PINNUMBER "" //PIN number for the SIM Card used in the device

//Variables for new GSM Shield input
const int led_pin = 8;
const int receive_pin = 4;
int pinSpeaker = 5; 

// initialize the library instance
GSM gsmAccess; //For the phone functions
GSM_SMS sms; //For the text message part
GSMVoiceCall vcs; //For the phone call part for phoning 911

const int TEMP0 = 0;   //Temperature Sensor is on pin A0
int CO2 = 1;    //CO2 Sensor is on pin 1
int motionDetect1 = 5;   //Motion Sensor 1 is on pin 2
int motionDetect2 = 4;   //Motion Sensor 2 is on pin 3
int pirState1 = LOW;      //We start, assuming no motion detected
int pirState2 = LOW;      //We start, assuming no motion detected
int pressureSensor = 4;  //Pressure Sensor is on pin 4

const int lower_bound = 80; //Lower bound for the temperature detection 
const int mid_bound1 = 85; //Middle bound 1 for the temperature detection 
const int mid_bound2 = 90; //Middle bound 2 for the temperature detection 
const int upper_bound = 95; //Upper bound for the temperature detection 

int val = 0;     //Variable to hold analog reading. 
int valmotion1 = 5;  //Variable to hold motion reading of first motion sensor
int valmotion2 = 4;  //Variable to hold motion reading of second motion sensor
int pdetect = 0;     //Variable to hold voltage of the pressure sensor
int co2detect = 0;   //Variable to hold voltage of the CO2 sensor
int batterydetect = 0;   //Variable to hold voltage of battery life

boolean motion1 = false; //Variable for the first Motion Detection
boolean motion2 = false; //Variable for the second Motion Detection
boolean pressure = false; //Variable for the pressure detection
boolean co2high = false;  //Variable for the CO2 detection

String call911 = "14847884770";  // variable for the 911 call, currently wired to my number as to keep from accidentally phoning the police
String smsnumber = "17179820203"; //Variable for the social message number
char smsbuffer[20]; //Converts the smsnumber or 911 number into Char Array for it to be used in vcs

//Variables to make the emergency actions only run once, and not repeatedly.
boolean sentmessage1 = false;
boolean sentmessage2 = false;
boolean soundedalarm = false;
boolean windowsdown = false;
boolean calledemergency = false; 

// char array of the message
char txtMsg[100]="Alert, your child or pet is in danger in your vehicle. Please attend to them as soon as possible.";
char txt911Msg[100]="Alert, there is child or pet in critical heat danger in this car. Please attend immediately.";

void setup() {
  Serial.begin(9600);    
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("SMS Messages Sender");

  // connection state
  boolean notConnected = true;
  //Connect to the GSM Shield
  while (notConnected)
  {
    if (gsmAccess.begin() == GSM_READY)
      notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }
  Serial.println("GSM initialized");
}

void loop() {
   //BatteryVoltage();
   val = analogRead(TEMP0); //Get readings from the temperature sensor 
   pressure = analogRead(pressureSensor); //Get readings from the pressure detection
   float voltage, degreesC, degreesF;   //Variables for the pressure

   voltage = getVoltage(TEMP0); //Voltage reading of the temperature sensor
   degreesC = (voltage - 0.5) * 100.0; //Convert Temperature Sensor voltage to degrees Celsius
   degreesF = degreesC * (9.0/5.0) + 32.0; //Convert Degrees Celsius to Degrees Fahrenheit

   pdetect = analogRead(A1); //Read Pressure Sensor Input Voltage
   co2detect = analogRead(A2); //Read CO2 Sensor Input Voltage
   batterydetect = analogRead(A0);
   //Test CO2 and Pressure Outputs
   Serial.print("Pressure: ");
   Serial.println(pdetect);
   Serial.print("CO2: ");
   Serial.println(co2detect);
   valmotion1 = digitalRead(motionDetect1);  // read input value
   valmotion2 = digitalRead(motionDetect2);  // read input value

   //Test outputs for Temperature Sensors
   Serial.print("voltage: ");
   Serial.print(voltage);
   Serial.print("  deg C: ");
   Serial.print(degreesC);
   Serial.print("  deg F: ");
   Serial.println(degreesF);

    //Test  sensors
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

    //Pressure Sensor
    if (pdetect > 700) {
      pressure = true;
      Serial.println("Pressure detected!");
    }
    else {
      pressure = false;
      Serial.println("No Pressure detected.");
    }
    
    //CO2 Sensor
    if (co2detect > 800) {
      co2high = true;
    }
    else {
      co2high = false;
    }
    
    //Start when both Motion Sensors or Pressure Sensor comes up true
    if (motion1 == true && motion2 == true || pressure == true) {
      if (co2high == true) { //Test for CO2
        Serial.println("CO2 concentration high!");
        //Send Emergency Message to phone of the user. 
        if (sentmessage1 == false) {  //Code to make the emergency action not repeatedly run when threshold is hit
          if (smsnumber.equals("")) { //In case the user has not set their phone number yet
            Serial.println("No phone number set. Message send failure.");
          }
          else { //If the phone number has been set
            sendEmergencyMessage(); //Invoke the emergency message
          }
          sentmessage1 = true; //Set variable after it has run to true.
        }
      }
      
      //Temperature Threshold code is now nested to allow each of them to run, instead of only one at a time
      if (lower_bound <= degreesF) { //Test for above 80 degrees
        Serial.println("Temperature threshold 1");
         //Send Emergency Message to phone of the user. 

         if (sentmessage2 == false) {  //Code to make the emergency action not repeatedly run when threshold is hit
          if (smsnumber.equals("")) { //In case the user has not set their phone number yet
            Serial.println("No phone number set. Message send failure.");
          }
          else { //If the phone number has been set
            sendEmergencyMessage();  //Invoke the emergency message
          }
          sentmessage2 = true; //Set variable after it has run to true.
         }

        if (mid_bound1 <= degreesF) { //Test for above 85 degrees
          Serial.println("Temperature threshold 2");
          //Open the windows of the car
          
          if (windowsdown == false){  //Code to make the emergency action not repeatedly run when threshold is hit
            digitalWrite(8, HIGH);   // sets the digital pin 8 on for the windows
            delay(2000);             // waits for two seconds for it to move
            digitalWrite(8, LOW);    // sets the digital pin 8 off for the windows
            delay(2000);
            windowsdown = true;  //Set variable after it has run to true.
          }
          
          if (mid_bound2 <= degreesF) { //Test for above 90 degrees
            Serial.println("Temperature threshold 3");
            //Could not make the Car Alarm sound due to required parts not coming in.

            if (upper_bound <= degreesF) { //Test for above 95 degrees
              Serial.println("Temperature threshold 4");
              //Alert Emergency Services to attend to the problem
              
              if (calledemergency == false) {  //Code to make the emergency action not repeatedly run when threshold is hit
                if (call911.equals("")) { //In case the user has not set their phone number yet
                  Serial.println("No phone number set. Message send failure.");
                }
                else { //If the phone number has been set
                  send911Message();  //Invoke the emergency message
                }
                  calledemergency = true; //Set variable after it has run to true.
                }

            }
          }
        }     
      }
     
      else {
        Serial.println("No Temperature threshold");
        //TODO code for no severe temperature threshold once parts arrive
      }
    }
    else {
      Serial.println("Standby");
    }
    
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
  
/*void BatteryVoltage() //Function to test the battery juice left
{
  int batteryValue = analogRead(A0); //read the battery pin value of remaining power
  float batteryvoltage = batteryValue * (5.00 / 1023.00) * 2; //convert the value to a true voltage.
  Serial.print("Energy Left: ");
  Serial.println(batteryvoltage);
   if (batteryvoltage < 10.00) //set the voltage considered low battery here
  {
    Serial.println("Battery Level low. Must recharge.");
  }
  else {
    Serial.println("Battery Level sufficient.");
  }
}*/

void sendEmergencyMessage() { //Function to send the emergency Message
Serial.print("Message to mobile number: ");
  smsnumber.toCharArray(smsbuffer, 20);
  Serial.println(smsbuffer);

  // sms text
  Serial.println("SENDING");
  Serial.println();
  Serial.println("Message:");
  Serial.println(txtMsg);

  // send the message
  sms.beginSMS(smsbuffer);
  sms.print(txtMsg);
  sms.endSMS(); 
  sms.flush();
  Serial.println("\nCOMPLETE!\n");  
  delay(1000);
}

void send911Message() { //Function to send the 911 Message
//NOTE: Originally was to be phone call but made text call instead
//Had to resort to text message due to Text To Speech Shield not coming in.
/*if (calledemergency == false) { //Code to make the emergency action not repeatedly run when threshold is hit
  //TODO code for the last threshold once parts arrive
  vcs.voiceCall(call911buffer); //Phone 911 here
  vcs.hangCall(); //Hang up 911
  calledemergency = true; //Set variable after it has run to true.
}*/

Serial.print("Message to 911 number: ");
  call911.toCharArray(smsbuffer, 20);
  Serial.println(smsbuffer);

  // sms text
  Serial.println("SENDING");
  Serial.println();
  Serial.println("Message:");
  Serial.println(txt911Msg);

  // send the message
  sms.beginSMS(smsbuffer);
  sms.print(txt911Msg);
  sms.endSMS(); 
  sms.flush();
  Serial.println("\nCOMPLETE!\n");  
  delay(1000);
}
