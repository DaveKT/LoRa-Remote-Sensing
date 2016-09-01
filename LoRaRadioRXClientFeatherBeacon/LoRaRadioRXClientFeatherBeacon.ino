/*
* Author: David Kolet-Tassara
* Date:   September 1, 2016
* Rev:    0.2
* Description:
*   This sketch uses the RadioHead library to send WX beacon messages via the
*   Adafruit Feather MO with RFM95 LoRa Radio. More information on the radio may
*   be found here: https://www.adafruit.com/product/3178
* Attribution:
*   Parts of the code were taken from the tutorial for the RFM9x LoRa Packet Radio
*   written by Limor Fried (ladyada).
*   RadioHead Libraries were written by Mike McCauley.
*   http://www.airspayce.com/mikem/arduino/RadioHead/index.html
*   
*/

#include <SPI.h>
#include <RH_RF95.h>

//LoRa Settings
#define RFM95_CS 8 // Specific to the feather
#define RFM95_RST 4 // Specific to the feather
#define RFM95_INT 3 // Specific to the feather
#define RF95_FREQ 915.0 //acceptable values are 868 or 915 MHz
#define RF95_PWR 23 //acceptable range includes all integers between 5 and 23 dBm
RH_RF95 rf95(RFM95_CS, RFM95_INT);

//TMP36 Settings
int tempPin = 0; //taking our temp readings from A0

//Global Variables
String message = "";
String StationID = "1";
int tempReading;

void setup() {
  pinMode(13, OUTPUT);
  
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
 
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 
  while (!rf95.init()) {
    digitalWrite(13, HIGH);
    while (1);
  }

  if (!rf95.setFrequency(RF95_FREQ)) {
    while (1);
  }
  
  rf95.setTxPower(RF95_PWR, false);

  for(int i=0; i < 5; i++) {
    digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(500);              // wait for a second
    digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
    delay(500);
  }

}

void loop() {

    //get temperature reading
    float voltage = analogRead(tempPin) * 3.3;
    voltage /= 1024.0;
    float tempC = (voltage - 0.5) * 100;
    float tempF = (tempC * 9.0 / 5.0) + 32.0;

    //build the data message (csv)
    message = StationID+", "+String(tempC, 1)+", "+String(tempF, 1);
     
    //explode string to char array for sending
    int messageSize = message.length()+1;
    char radiopacket[messageSize];
    message.toCharArray(radiopacket, messageSize);

    //send the message OTA
    rf95.send((uint8_t *)radiopacket, messageSize);
    delay(10);
    rf95.waitPacketSent();

    digitalWrite(13, HIGH);   
    delay(500);           
    digitalWrite(13, LOW);
    delay(500);

    //report and clean up
    delay(10000);
    
}
