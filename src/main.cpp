#include <SPI.h>
#include <RF24.h>
#include <Servo.h>
#include "nRF24L01.h"     // библиотека радиомодуля
#include <Arduino.h> 


#define V_T_BATTERY 3.3

Servo myServo;                  
RF24 radio(9, 10);

const byte address[6] = "00001"; // ID приёмника
bool statusBattery;
bool statusFlag;           
         



void useFlag(int myCommand) {
  switch (myCommand) {
    case 1:
      myServo.write(180);
      statusFlag = 1;       
      break;

    case 0:
      myServo.write(0);  
      statusFlag = 0;       
      break;

    case "CHECK":                    // Обработка запроса статуса
      if(analogRead(0) <= V_T_BATTERY){
        statusBattery = false;
      }else{
        statusBattery = true;
      }
      bool ToSend[2]{statusBattery, statusFlag};
      radio.writeAckPayload(&ToSend, sizeof(ToSend)); 
      break;
  }
}



void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_1MBPS);
  radio.openReadingPipe(1, address);
  radio.startListening();  
  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.setPayloadSize(6);     //размер пакета, в байтах
  radio.powerUp(); //начать работу
  radio.startListening();  //начинаем слушать эфир, мы приёмный модуль

  myServo.attach(8);            

}



void loop() {
  int myCommand;
  if (radio.available()) {
    radio.read(&myCommand, sizeof(myCommand));
    useFlag(myCommand);         
  }
}
