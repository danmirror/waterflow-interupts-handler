/*
 * Danmirror protocol interupts
 * rev 2 wire PORT Register
 * master slave A4 A5
 * 
 * 2021 
 * 
 */
#include <Wire.h> /* master slave| i2c*/
#include <LiquidCrystal_I2C.h> // Library modul I2C LCD
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2); 

#define flowSensor_pin1 2
#define flowSensor_pin2 8
//#define flowSensor_pin3 A0
#define relay1  3
#define relay2  4
#define relay3  5
int pump1 = 0;
int pump2 = 0;

long int times;
int count_switch=0;
int clear_cursor = 0;
float staging1=0;
float staging2=0;

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
char *sends;
String pack;
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
      Serial.print("#pin 8 > ");
      Serial.println(count2);
    }
 }
 
ISR (PCINT1_vect) // handle pin change interrupt for A0 to A5 here
 {
//    bool read_A0= digitalRead(flowSensor_pin3);
//    if( read_A0 == false)
//    {
////      count3++;/
//      Serial.print("#pin A0 > ");
//      Serial.println(count3);
//    }
 }  
ISR (PCINT2_vect) // handle pin change interrupt for D0 to D7 here
 {
    bool read_2 = digitalRead(flowSensor_pin1);
   
    if( read_2 == false)
    {
      count1++;
      Serial.print("#pin 2 >");
      Serial.println(count1);
    }
 }  

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
//  pinMode(relay3, OUTPUT);
  
  // set pullups, if necessary
  digitalWrite(flowSensor_pin1, HIGH);
  digitalWrite(flowSensor_pin2, HIGH);
//  digitalWrite(flowSensor_pin3, HIGH);/
  
  // enable interrupt for pin...
  pciSetup(flowSensor_pin1);
  pciSetup(flowSensor_pin2);
//  pciSetup(flowSensor_pin3);
  
  /* ---------------- slave setup-----------------------*/
  Wire.begin(8);                /* join i2c bus with address 8 */
  Wire.onReceive(receiveEvent); /* register receive event */
  Wire.onRequest(requestEvent); /* register request event */
  
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 1); 
  lcd.print("setup...");
}

void loop() {
  display_lcd();
  if ((millis() - times) > 3000) {
    count_switch +=1;
    times = millis();
    lcd.clear();
  }
  
  if ((millis() - oldTime) > 1000) {

      staging1 += ((1000.0 / (millis() - oldTime)) * count1) / konst1;
      staging2 += ((1000.0 / (millis() - oldTime)) * count2) / konst2;

      debit_air1 = staging1;
      debit_air2 = staging2;
      
      oldTime = millis();
      flow_mlt1 = (staging1 / 60) * 1000;
      flow_mlt2 = (staging2 / 60) * 1000;

      total_volume1 += flow_mlt1;
      total_volume2 += flow_mlt2;
     
              
      Serial.print("Debit air1: ");  Serial.print(int(debit_air1));   Serial.print(" L/min");
      Serial.print("\t Volume air1: "); Serial.print(total_volume1);     Serial.println(" mL");

      Serial.print("Debit air2: ");  Serial.print(int(debit_air2));   Serial.print(" L/min");
      Serial.print("\t Volume air2: "); Serial.print(total_volume2);     Serial.println(" mL"); 
      count1 = 0;
      count2 = 0;
  }
  delay(500);
 
   /* adjusting*/
  Serial.println(container);
  if(container == "11"){
    digitalWrite(relay1, HIGH);
    pump1 = 1;
    
  }
  else if(container == "10"){
    digitalWrite(relay1, LOW);
    pump1 = 0;
  }
  else if(container == "21"){
    digitalWrite(relay1, HIGH);
    pump2 = 1;
  }
  else if(container == "20"){
    digitalWrite(relay1, LOW);
    pump2 = 0;
  }
 
  else if(container == "d1"){
    pack = String(debit_air1);
    sends = pack.c_str();
  }
  else if(container == "d2"){
    pack = String(debit_air2);
    sends = pack.c_str();
  }
 
  else if(container == "v1"){
    pack = String(total_volume1);
    sends = pack.c_str();
  }
  else if(container == "v2"){
    pack = String(total_volume2);
    sends = pack.c_str();
  }
}

// function that executes whenever data is received from master
void receiveEvent(int howMany) {
 String staging = "";
 while (0 <Wire.available()) {
    char c = Wire.read();     
//    Serial.print(c);    
    staging +=c;       
  }
// Serial.println();  
 container = staging;          
}

// function that executes whenever data is requested from master
void requestEvent() 
{
  Wire.write(sends);  
}


void display_lcd()
{
  if(count_switch %2==1)
  {
    lcd.setCursor(0, 0); 
    lcd.print("pump1:");
    lcd.setCursor(7, 0); 
    lcd.print(pump1);
    lcd.setCursor(0, 1); 
    lcd.print("pump2:");
    lcd.setCursor(7, 1); 
    lcd.print(pump2);
  }
  else
  {
       
    lcd.setCursor(0, 0); 
    lcd.print("D1:");
    lcd.setCursor(4, 0); 
    lcd.print(debit_air1);
    lcd.setCursor(9, 0); 
    lcd.print("D2:");
    lcd.setCursor(12, 0); 
    lcd.print(debit_air2);
  
    lcd.setCursor(0, 1); 
    lcd.print("V1:");
    lcd.setCursor(4, 1); 
    lcd.print(total_volume1);
    lcd.setCursor(9, 1); 
    lcd.print("V2:");
    lcd.setCursor(12, 1); 
    lcd.print(total_volume2);
  }
    
}
