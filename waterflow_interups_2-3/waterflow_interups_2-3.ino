/*
 * Danmirror protocol interupts
 * rev 3 wire PORT Register
 * master slave A4 A5
 * 
 * 2021 
 * 
 */
#include <Wire.h> /* master slave*/

#define flowSensor_pin1 2
#define flowSensor_pin2 3
#define relay1  4
#define relay2  5

float konst1 = 4.5;
float konst2 = 4.5;

float debit_air1 = 0;
float debit_air2 = 0;

//counter
volatile int count1 = 0;
volatile int count2 = 0;
//flow
unsigned int flow_mlt1 = 0;
unsigned int flow_mlt2 = 0;
//volume
long total_volume1,total_volume2;
//time
unsigned long oldTime = 0;
int data = 0;
// Install Pin change interrupt for a pin, can be called multiple times

/* inisial master slave data*/
String container="";
char send_to_master[3]="";


void flow1(){
    count1++;
    Serial.print("pin 2 > ");
    Serial.println(count1);
}
void flow2(){
    count2++;
    Serial.print("pin 3 > ");
    Serial.println(count2);
}

 
void setup() {  
  Serial.begin(9600);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  
  // set pullups, if necessary
  pinMode(flowSensor_pin1, INPUT);
  pinMode(flowSensor_pin2, INPUT);
  
  digitalWrite(flowSensor_pin1, HIGH);
  digitalWrite(flowSensor_pin2, HIGH);
  
  
  attachInterrupt(0,flow1,FALLING);
  attachInterrupt(1,flow2,FALLING);
  
  /* ---------------- slave setup-----------------------*/
  Wire.begin(8);                /* join i2c bus with address 8 */
  Wire.onReceive(receiveEvent); /* register receive event */
  Wire.onRequest(requestEvent); /* register request event */
 
}
 
void loop() {
  if ((millis() - oldTime) > 1000) {
      detachInterrupt(0);
      detachInterrupt(1);

      debit_air1 = ((1000.0 / (millis() - oldTime)) * count1) / konst1;
      debit_air2 = ((1000.0 / (millis() - oldTime)) * count2) / konst2;
      
      oldTime = millis();
      flow_mlt1 = (debit_air1 / 60) * 1000;
      flow_mlt2 = (debit_air2 / 60) * 1000;
      
      total_volume1 += flow_mlt1;
      total_volume2 += flow_mlt2;
      
              
      Serial.print("Debit air1: ");  Serial.print(debit_air1);   Serial.print(" L/min");
      Serial.print("\t Volume air1: "); Serial.print(total_volume1);     Serial.println(" mL");

      Serial.print("Debit air2: ");  Serial.print(debit_air2);   Serial.print(" L/min");
      Serial.print("\t Volume air2: "); Serial.print(total_volume2);     Serial.println(" mL");
      
      count1 = 0;
      count2 = 0;
      
      attachInterrupt(0,flow1,FALLING);
      attachInterrupt(1,flow2,FALLING);
      
  }
  delay(500);

  /* adjusting*/
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
  
  else if(container == "d1"){
    dtostrf(debit_air1,3, 3, send_to_master);
  }
  else if(container == "d2"){
    dtostrf(debit_air1,3, 3, send_to_master);
  }
  
  else if(container == "v1"){
    dtostrf(total_volume1,3, 3, send_to_master);
  }
  else if(container == "v2"){
    dtostrf(total_volume2,3, 3, send_to_master);
  }
}

// function that executes whenever data is received from master
void receiveEvent(int howMany) {
  //reset interupt
// detachInterrupt(0);
// detachInterrupt(1);
 
 String staging = "";
 while (0 <Wire.available()) {
    char c = Wire.read();     
//    Serial.print(c);    /
    staging +=c;       
  }
// Serial.println();  /
 container = staging;  

  //reactivated
//  attachInterrupt(0,flow1,FALLING);
//  attachInterrupt(1,flow2,FALLING);
}

// function that executes whenever data is requested from master
void requestEvent() {
//  strcat( msg, send_to_master.c_str() );/
  Wire.write(send_to_master);  
}
