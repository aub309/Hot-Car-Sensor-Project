//Hot Car Detection Program
//By Andrew Bloschichak, Steven Santangelo, Michael Sinyagin

//This so far is the rough framework. 
//The sensor and trigger parts have not arrived, but so far it compiles and works on the Microcontroller.

int TEMP0 = 0;   //Temperature Sensor is on pin A0
int CO2 = 1;    //CO2 Sensor is on pin 1
int motionDetect1 = 2;   //Motion Sensor 1 is on pin 2
int motionDetect2 = 3;   //Motion Sensor 2 is on pin 3
int pressureSensor = 4;  //Pressure Sensor is on pin 4

const int lower_bound = 80; //Lower bound for the temperature detection 
const int mid_bound1 = 85; //Middle bound 1 for the temperature detection 
const int mid_bound2 = 90; //Middle bound 2 for the temperature detection 
const int upper_bound = 95; //Upper bound for the temperature detection 

int val = 0;     //Variable to hold analog reading. 
boolean motion1 = false; //Variable for the first Motion Detection
boolean motion2 = false; //Variable for the second Motion Detection
boolean pressure = false; //Variable for the pressure detection
void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:

}

void loop() {
   val = analogRead(TEMP0); //Get readings from the temperature sensor 
   motion1 = analogRead(motionDetect1); //Get readings from the motion detection
   motion2 = analogRead(motionDetect2); //Get readings from the second motion detection
   pressure = analogRead(pressureSensor); //Get readings from the pressure detection
    
    if (motion1 == true || motion2 == true || pressure == true) {
      if (lower_bound <= val && val < mid_bound1) { //Test for above 80 degrees
        //TODO code for the first threshold once parts arrive
      }
      else if (mid_bound1 <= val && val < mid_bound2) { //Test for above 85 degrees
        //TODO code for the second threshold once parts arrive
      }
      else if (mid_bound2 <= val && val < upper_bound) { //Test for above 85 degrees
        //TODO code for the third threshold once parts arrive
      }
      else if (val >= upper_bound) { //Test for above 85 degrees
        //TODO code for the last threshold once parts arrive
      }
      else {
        //TODO code for no severe temperature threshold once parts arrive
      }
    }

    // put your main code here, to run repeatedly:
  }
  

