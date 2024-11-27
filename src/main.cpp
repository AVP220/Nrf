#include "nRF24L01.h" // библиотека радиомодуля
#include <Arduino.h>
#include <RF24.h>
#include <SPI.h>
#include <Servo.h>

#define V_T_BATTERY 512 // пока так потом через мап уже ридумаю
  int myCommand;
Servo myServo;
RF24 radio(9, 10);

const byte address[6] = "00001"; // ID приёмника
int statusBattery;
int statusFlag;
byte ToSend[2];

void updateToSend() {
  ToSend[0] = statusBattery; // Первый байт: статус батареи
  ToSend[1] = statusFlag;    // Второй байт: флаг состояния
}


void useFlag(int myCommand) {
  switch (myCommand) {
  case 257:
    Serial.println("Command 1");
    myServo.write(180);
    statusFlag = 1;
    updateToSend();
    radio.writeAckPayload(1, &ToSend, sizeof(ToSend));
    break;

  case 99:
  Serial.println("Command 0");
    myServo.write(0);
    statusFlag = 0;
    updateToSend();
    radio.writeAckPayload(1, &ToSend, sizeof(ToSend));
    break;

  case 111: // Обработка запроса статуса
  Serial.println("Command 111");
    if (analogRead(0) <= V_T_BATTERY) {
      statusBattery = 254;
      Serial.println("LowBattery");
    } else {
      statusBattery = 100;
    }
    updateToSend();
    radio.writeAckPayload(1, &ToSend, sizeof(ToSend));
    break;
      default:
    Serial.println("Unknown command received");
  }
}

void setup() {
  Serial.begin(9600);

  radio.begin();
  
  radio.setAutoAck(1); // режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15); //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload(); // разрешить отсылку данных в ответ на входящий// сигнал
  radio.enableDynamicPayloads();

  radio.setChannel(0x60); // выбираем канал (в котором нет шумов!)
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_1MBPS);
  radio.openReadingPipe(1, address);

  radio.powerUp();         // начать работу
  radio.startListening(); // начинаем слушать эфир, мы приёмный модуль

  myServo.attach(8);
  myServo.write(180);
  updateToSend();
}
  unsigned long startTimeBattery = millis();
void loop() {
if (radio.available()) {
  Serial.println("Data available");
  radio.read(&myCommand, sizeof(myCommand));
  Serial.print("Received command: ");
  Serial.println(myCommand);
  useFlag(myCommand);
} else {
  if (millis() - startTimeBattery >= 50){
    startTimeBattery = millis();        
    Serial.println("No data received");
  }
}

}
