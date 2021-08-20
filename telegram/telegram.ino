
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
 
String ssid  = "dann"  ; // ssid wifi anda
String pass  = "danu12345"; // password wifi anda
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
    
  /* master initial */
  Wire.begin(D1, D2);             // join i2c bus with address #4


}
 
void loop() {

  /* 
   *  Master
   */
   
  master();
  
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
      else if (msg.text.equalsIgnoreCase("Debit 1")) {   
          data_to_slave = "d1";      
         myBot.sendMessage(msg.sender.id, container); 
      }
      else if (msg.text.equalsIgnoreCase("Debit 2")) {   
          data_to_slave = "d2";      
         myBot.sendMessage(msg.sender.id, container); 
      }
      else if (msg.text.equalsIgnoreCase("Debit 3")) {   
          data_to_slave = "d3";      
         myBot.sendMessage(msg.sender.id, container); 
      }
      else if (msg.text.equalsIgnoreCase("Volume 1")) {   
          data_to_slave = "v1";      
         myBot.sendMessage(msg.sender.id, container); 
      }
      else if (msg.text.equalsIgnoreCase("Volume 2")) {   
          data_to_slave = "v2";      
         myBot.sendMessage(msg.sender.id, container); 
      }
      else if (msg.text.equalsIgnoreCase("Volume 3")) {   
          data_to_slave = "v3";      
         myBot.sendMessage(msg.sender.id, container); 
      }
      else {                                                   
          // generate the message for the sender
          String reply;
          reply = (String)"Welcome " + msg.sender.username + (String)". ketik P1 on / P1 off / Debit 1/ Debit 2/ Debit 3 / Volume 1/ Volume 2 / Volume 3.";
          myBot.sendMessage(msg.sender.id, reply);             // and send it
      }
  }
  delay(1000);
}
void master()
{
   String staging = "";
   Wire.beginTransmission(8);   /* begin with device address 8 */
   Wire.write(data_to_slave);   /* sends to slave */
   Wire.endTransmission();      /* stop transmitting */
  
   Wire.requestFrom(8, 3);     /* request & read data of size 13 from slave */
   while(Wire.available()){
      char c = Wire.read();
    Serial.print(c);
    staging +=c;
   }
   Serial.println();
   container = staging;
   delay(1000);
  }
