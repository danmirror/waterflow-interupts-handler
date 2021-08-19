
/*
 * Danmirror 
 * telegram
 * master slave D1 D2
 * 
 * 2021 
 * 
 */

#include "CTBot.h"
#include <Wire.h>
 
CTBot myBot;
 
String ssid  = "sasino"  ; // ssid wifi anda
String pass  = "1234lima"; // password wifi anda
String token = "1652016015:AAFvl924BYzMzQGol7CcFlCZIhlJnyHKYEw";
String container = "";
char* data_to_slave = "";
         
void setup() {
  // initialize the Serial
  Serial.begin(115200);
  Serial.println("IoT Telegram kendali LED");
  // connect the ESP8266 to the desired access point
  myBot.wifiConnect(ssid, pass);
  
  // set the telegram bot token
  myBot.setTelegramToken(token);
  
  // check if all things are ok
  if (myBot.testConnection())
    Serial.println("\ntestConnection OK");
  else
    Serial.println("\ntestConnection bad");
    
  /*slave */
  Wire.begin(D1, D2);             // join i2c bus with address #4


}
 
void loop() {

  /* 
   *  slave
   */
   
  slave();
  
  // a variable to store telegram message data
  TBMessage msg;
  
  // if there is an incoming message...
  if (myBot.getNewMessage(msg)) {
      Serial.println(msg.text);
      if (msg.text.equalsIgnoreCase("P1 on")) {      
          data_to_slave = "11";       
          myBot.sendMessage(msg.sender.id, "pump 1 on");  
      }
      else if (msg.text.equalsIgnoreCase("P1 off")) {   
          data_to_slave = "10";     
          myBot.sendMessage(msg.sender.id, "pump 1 off"); 
      }
      else if (msg.text.equalsIgnoreCase("P2 on")) {
          data_to_slave = "21";        
          myBot.sendMessage(msg.sender.id, "pump 2 on"); 
      }
      else if (msg.text.equalsIgnoreCase("P2 off")) {
          data_to_slave = "20";        
          myBot.sendMessage(msg.sender.id, "pump 2 off"); 
      }
      else if (msg.text.equalsIgnoreCase("P3 on")) {    
          data_to_slave = "31";    
          myBot.sendMessage(msg.sender.id, "pump 3 on"); 
      }
      else if (msg.text.equalsIgnoreCase("P3 off")) { 
          data_to_slave = "30";       
         myBot.sendMessage(msg.sender.id, "pump 3 off"); 
      }
      else if (msg.text.equalsIgnoreCase("status")) {        
         myBot.sendMessage(msg.sender.id, container); 
      }
      else {                                                   
          // generate the message for the sender
          String reply;
          reply = (String)"Welcome " + msg.sender.username + (String)". ketik p1 on / p1 off / status.";
          myBot.sendMessage(msg.sender.id, reply);             // and send it
      }
  }
  // wait 500 milliseconds
  delay(500);
}
void slave()
{
   String staging = "";
   Wire.beginTransmission(8);   /* begin with device address 8 */
   Wire.write(data_to_slave);   /* sends to slave */
   Wire.endTransmission();      /* stop transmitting */
  
   Wire.requestFrom(8, 13);     /* request & read data of size 13 from slave */
   while(Wire.available()){
      char c = Wire.read();
    Serial.print(c);
    staging +=c;
   }
   Serial.println();
   container = staging;
   delay(1000);
  }
