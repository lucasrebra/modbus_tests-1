
/*****************************************************************************/
/*  Programa utilizando libreria modbus a modo de master para ESP32.         */
/*  El programa se encargará de realizar tres acciones:                      */
/*  -->Iniciar las diez primeras direcciones con valor fijo.                 */
/*                                                                           */
/*  -->Enviar el numero de segundos del programa desde el ultimo             */
/*     Reset del controlador a la direccion 40 de discret inputs.            */
/*                                                                           */
/*  -->Fijaremos diez direcciones que se puedan leer y escribir (20 a 30)    */
/*                                                                           */
/*  Autor:Lucas Rey Braga                                                    */
/*  Correo:lucas@ingen10.com                                                 */
/*                                                                           */
/*  Librería utilizada disponible en:                                        */
/*           https://github.com/emelianov/modbus-esp8266                     */
/*                                                                           */
/*****************************************************************************/



#ifdef ESP8266
 #include <ESP8266WiFi.h>
#else //ESP32
 #include <WiFi.h>
#endif
#include <ModbusIP_ESP8266.h>
#include <ModbusRTU.h>
#include <BluetoothSerial.h>
/*
#define REGN 10
#define SLAVE_ID 1
#define PULL_ID 1
#define FROM_REG 20
#define SEGUNDERO 30*/

int REGN=10;
int SLAVE_ID=1;
int PULL_ID=1;
int FROM_REG=20;
int SEGUNDERO=30;

long ts;
int segs;

ModbusRTU mb1;
ModbusIP mb2;
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
  SerialBT.print("Write. Reg RAW#: ");
  SerialBT.print(reg->address.address);
  SerialBT.print(" Old: ");
  SerialBT.print(reg->value);
  SerialBT.print(" New: ");
  SerialBT.println(val);
  return val;
}
  

void setup() {
  //Serial.begin(115200);
  WiFi.begin("twave-24", "KD6rUYrv");
  
  /*Configuramos el puerto serial Bluetoth*/

  SerialBT.begin("ESP32test"); //Bluetooth device name
  SerialBT.print("The device started, now you can pair it with bluetooth!\n");

  /*Congiguramos la comunicación modbus*/

  Serial.begin(9600, SERIAL_8N1);
  mb1.begin(&Serial);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    SerialBT.print(".");
  }
 
  SerialBT.print("\n");
  SerialBT.print("WiFi connected\n");  
  SerialBT.print("IP address:\n");
  SerialBT.print(WiFi.localIP());

  //mb1.begin(&Serial2, 17);  // Specify RE_DE control pin
  mb2.server();
  mb2.addHreg(REGN,100,10);
  mb2.addIreg(SEGUNDERO);


  /*Setup acciones Modbus RTU*/

  mb1.slave(SLAVE_ID);
  //Registros de valores fijos
  mb1.addHreg(REGN,100,10);
  //Registro segundero
  mb1.addIreg(SEGUNDERO);
  //mb.Hreg(REGN, 100);
  mb1.onGetHreg(REGN,cbRead,10);
  mb1.onSetHreg(REGN,cbWrite,10);

  
}

void loop() {
  
  if ( millis() > ts + 2000 ) 
  {
    segs++;
    ts = millis();
    mb1.Ireg(SEGUNDERO,segs);//Escribimos en la direccion RTU los segundos
    mb2.Ireg(SEGUNDERO,segs);//Escribimos en la direccion TCP los segundos
    SerialBT.print(mb1.Ireg(30));
  }

  mb1.task();
  mb2.task();
  delay(50);
}