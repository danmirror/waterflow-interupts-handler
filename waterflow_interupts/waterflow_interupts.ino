/*
 * Danmirror protocol interupts
 * rev 3 wire PORT Register
 * master slave A4 A5
 * 
 * 2021 
 * 
 */
#include <Wire.h> /*master slave*/

#define flowSensor_pin1 2
#define flowSensor_pin2 8
#define flowSensor_pin3 A0
#define relay1  3
#define relay2  4
#define relay3  5

byte sensorInt1 = 0;
byte sensorInt2 = 0;
byte sensorInt3 = 0;

float konst1 = 4.5;
float konst2 = 4.5;
float konst3 = 4.5;

float debit_air1 = 0;
float debit_air2 = 0;
float debit_air3 = 0;

//counter
volatile int count1 = 0;
volatile int count2 = 0;
volatile int count3 = 0;
//flow
unsigned int flow_mlt1 = 0;
unsigned int flow_mlt2 = 0;
unsigned int flow_mlt3 = 0;
//volume
long total_volume1,total_volume2,total_volume3;
//time
unsigned long oldTime = 0;
int data = 0;
// Install Pin change interrupt for a pin, can be called multiple times

/*inisial master slave data*/
String container="";
char send_to_master[3]="";


void pciSetup(byte pin)
{
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}
 
// Use one Routine to handle each group
  
ISR (PCINT0_vect) // handle pin change interrupt for D8 to D13 here
 {    
    bool read_8 = digitalRead(flowSensor_pin2);
    if( read_8 == false)
    {
      count2++;
      Serial.print("pin 8 > ");
      Serial.println(count2);
    }
 }
 
ISR (PCINT1_vect) // handle pin change interrupt for A0 to A5 here
 {
    bool read_A0= digitalRead(flowSensor_pin3);
    if( read_A0 == false)
    {
      count3++;
      Serial.print("pin A0 > ");
      Serial.println(count3);
    }
 }  
ISR (PCINT2_vect) // handle pin change interrupt for D0 to D7 here
 {
    bool read_2 = digitalRead(flowSensor_pin1);
   
    if( read_2 == false)
    {
      count1++;
      Serial.print("pin 2 >");
      Serial.println(count1);
    }
 }  
 
void setup() {  
  Serial.begin(9600);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  
  // set pullups, if necessary
  digitalWrite(flowSensor_pin1, HIGH);
  digitalWrite(flowSensor_pin2, HIGH);
  digitalWrite(flowSensor_pin3, HIGH);
  
  // enable interrupt for pin...
  pciSetup(flowSensor_pin1);
  pciSetup(flowSensor_pin2);
  pciSetup(flowSensor_pin3);
  
  /*---------------- slave setup-----------------------*/
  Wire.begin(8);                /* join i2c bus with address 8 */
  Wire.onReceive(receiveEvent); /* register receive event */
  Wire.onRequest(requestEvent); /* register request event */
 
}
 
void loop() {
  if ((millis() - oldTime) > 1000) {

      debit_air1 = ((1000.0 / (millis() - oldTime)) * count1) / konst1;
      debit_air2 = ((1000.0 / (millis() - oldTime)) * count2) / konst2;
      debit_air3 = ((1000.0 / (millis() - oldTime)) * count3) / konst3;

      oldTime = millis();
      flow_mlt1 = (debit_air1 / 60) * 1000;
      flow_mlt2 = (debit_air2 / 60) * 1000;
      flow_mlt3 = (debit_air3 / 60) * 1000;

      total_volume1 += flow_mlt1;
      total_volume2 += flow_mlt2;
      total_volume3 += flow_mlt3;
      
              
      Serial.print("Debit air1: ");  Serial.print(int(debit_air1));   Serial.print(" L/min");
      Serial.print("\t Volume air1: "); Serial.print(total_volume1);     Serial.println(" mL");

      Serial.print("Debit air2: ");  Serial.print(int(debit_air2));   Serial.print(" L/min");
      Serial.print("\t Volume air2: "); Serial.print(total_volume2);     Serial.println(" mL");

      Serial.print("Debit air3: ");  Serial.print(int(debit_air3));   Serial.print(" L/min");
      Serial.print("\t Volume air3: "); Serial.print(total_volume3);     Serial.println(" mL \n");
      
      count1 = 0;
      count2 = 0;
      count3 = 0;
  }
  delay(500);

  /*adjusting*/
  Serial.println(container);
  if(container == "11"){
    digitalWrite(relay1, HIGH);
  }
  else if(container == "10"){
    digitalWrite(relay1, LOW);
  }
  else if(container == "21"){
    digitalWrite(relay1, HIGH);
  }
  else if(container == "20"){
    digitalWrite(relay1, LOW);
  }
  else if(container == "31"){
    digitalWrite(relay1, HIGH);
  }
  else if(container == "30"){
    digitalWrite(relay1, LOW);
  }
  else if(container == "d1"){
    dtostrf(debit_air1,3, 3, send_to_master);
  }
  else if(container == "d2"){
    dtostrf(debit_air1,3, 3, send_to_master);
  }
  else if(container == "d3"){
    dtostrf(debit_air3,3, 3, send_to_master);
  }
  else if(container == "v1"){
    dtostrf(total_volume1,3, 3, send_to_master);
  }
  else if(container == "v"){
    dtostrf(total_volume2,3, 3, send_to_master);
  }
  else if(container == "v3"){
   dtostrf(total_volume3,3, 3, send_to_master);
  }
}

// function that executes whenever data is received from master
void receiveEvent(int howMany) {
 String staging = "";
 while (0 <Wire.available()) {
    char c = Wire.read();     
//    Serial.print(c);    /
    staging +=c;       
  }
// Serial.println();  /
 container = staging;          
}

// function that executes whenever data is requested from master
void requestEvent() {
//  strcat( msg, send_to_master.c_str() );/
  Wire.write(send_to_master);  
}
