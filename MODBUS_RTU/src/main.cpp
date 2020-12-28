/*Programa que implementa una conexión MODBUS RTU. En este programa se ha 
habilitado un puerto serial bluetooth desde el que se puede acceder desde 
cualquier dispositivo con esta conexión.*/


#include <Arduino.h>
#include <ModbusRTU.h>
#include "BluetoothSerial.h"

#define REGN 0
#define SLAVE_ID 1
#define SEGUNDERO 20
int segs=0;
long ts;

ModbusRTU mb;
BluetoothSerial SerialBT;

 // Callback function to read corresponding DI
uint16_t cbRead(TRegister* reg, uint16_t val) {
  SerialBT.print("Registro: ");
  SerialBT.print(reg->address.address);
  SerialBT.print(" Viejo: ");
  SerialBT.print(reg->value);
  SerialBT.print(" Nuevo: ");
  SerialBT.println(val);
  return val;
}

//  Callback cambio de valor
uint16_t cbWrite(TRegister* reg, uint16_t val) {
  Serial.print("Write. Reg RAW#: ");
  Serial.print(reg->address.address);
  Serial.print(" Old: ");
  Serial.print(reg->value);
  Serial.print(" New: ");
  Serial.println(val);
  return val;
}
  


void setup() {

  /*Configuramos el puerto serial Bluetoth*/

  SerialBT.begin("ESP32test"); //Bluetooth device name
  SerialBT.println("The device started, now you can pair it with bluetooth!");

  /*Congiguramos la comunicación modbus*/

  Serial.begin(9600, SERIAL_8N1);

  mb.begin(&Serial);

  //mb.setBaudrate(9600); cosa que cambie
  mb.slave(SLAVE_ID);

  //Registros de valores fijos
  mb.addHreg(REGN,100,10);

  //Registro segundero
  mb.addHreg(SEGUNDERO, 0, 1);

  //mb.Hreg(REGN, 100);
  mb.onGetHreg(REGN,cbRead,10);
  mb.onSetHreg(REGN,cbWrite,10);

}

void loop() {

  if ( millis() > ts + 2000 ) 
  {
    segs++;
    ts = millis();
    mb.Hreg(SEGUNDERO,segs);//Escribimos en la direccion los segundos
    SerialBT.print(mb.Hreg(20));
  }

  mb.task();

  delay(1000);
}