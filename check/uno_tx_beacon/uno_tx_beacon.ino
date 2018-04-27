#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(8,10); // define SPI's CE and CS pins 

byte address[][6] = {"1Node","2Node","3Node","4Node","5Node","6Node"};  //pipelines

byte counter=1;

void setup(){
  Serial.begin(9600);
  Serial.print("\n\nInitialization");

  radio.begin(); //enable radio
  radio.printDetails();
  radio.setAutoAck(0);         //disable receive confirm (ACK)
  radio.setRetries(0,15);     //(retries timeout, attempts number)

  radio.setPayloadSize(32);     //payload size Bytes

  radio.openWritingPipe(address[0]);   //0, open transmission pipeline
  radio.setChannel(0x65);  //select channel 

  radio.setPALevel (RF24_PA_MIN); //TX power level. Possible options RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); //Baudrate. RF24_2MBPS, RF24_1MBPS, RF24_250KBPS

  radio.powerUp(); //Start
  radio.stopListening();  //Don't listen. Transmit
}

void loop(void) {
//  Serial.begin(115200);
  radio.write(&counter,1);
  Serial.print(sprintf("\n\nPacket #%d \n\n", (char*)counter));
  delay(5);  
}
