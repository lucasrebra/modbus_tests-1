/*Programa que implementa una conexión MODBUS RTU. En este programa se ha 
habilitado un puerto serial bluetooth desde el que se puede acceder desde 
cualquier dispositivo con esta conexión.*/


#include <Arduino.h>
#include <ModbusRTU.h>
#include "BluetoothSerial.h"

#define REGN 10
#define SLAVE_ID 1

ModbusRTU mb;
BluetoothSerial SerialBT;

 // Callback function to read corresponding DI
uint16_t cbRead(TRegister* reg, uint16_t val) {
  SerialBT.print("Read. Reg RAW#: ");
  SerialBT.print(reg->address.address);
  SerialBT.print(" Old: ");
  SerialBT.print(reg->value);
  SerialBT.print(" New: ");
  SerialBT.println(val);
  return val;
}
  

void setup() {

  /*Configuramos el puerto serial Bluetoth*/

  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");

  /*Congiguramos la comunicación modbus*/

  Serial.begin(9600, SERIAL_8N1);
#if defined(ESP32) || defined(ESP8266)
  mb.begin(&Serial);
#else
  mb.begin(&Serial);
  //mb.begin(&Serial, RXTX_PIN);  //or use RX/TX direction control pin
(if required)
  mb.setBaudrate(9600);
#endif
  //mb.setBaudrate(9600); cosa que cambie
  mb.slave(SLAVE_ID);
  mb.addHreg(REGN);
  mb.Hreg(REGN, 100);
  mb.onGetHreg(10,cbRead,1);
}

void loop() {

  
  mb.task();

  delay(1000);
}