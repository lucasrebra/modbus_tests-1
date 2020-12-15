
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

#include <Arduino.h>
#include <WiFi.h>
#include <ModbusIP_ESP8266.h>

//Offsets de las direcciones registro en los diferentes tipos de protocolo
const int SEGUNDERO = 40; 
const int FIJOS = 0;
const int LEER_ESCRIBIR = 20;
const int nreg=10;

//ModbusIP object
ModbusIP mb;

//Variables utilizadas para el control temporal
long ts;
int segs = 0;


//CALLBACKS--> Cada vez que se produzca una peticion del cliente

//CALLBACK LECTURA
uint16_t cbRead(TRegister* reg, uint16_t val) {
  Serial.print("Read. Reg RAW#: ");
  Serial.print(reg->address.address);
  Serial.print(" Old: ");
  Serial.print(reg->value);
  Serial.print(" New: ");
  Serial.println(val);
  return val;
}

//CALLBACK ESCRITURA
uint16_t cbWrite(TRegister* reg, uint16_t val) {
  Serial.print("Write. Reg RAW#: ");
  Serial.print(reg->address.address);
  Serial.print(" Old: ");
  Serial.print(reg->value);
  Serial.print(" New: ");
  Serial.println(val);
  return val;
}

/*
 * Programa de configuracion: 
 * -Configuracion de la conexion wifi utilizada
 * -Inicializacion de la conexion modbus IP
 * -Configuracion de las direccion segun el tipo de protocolo
 */

void setup() {
    Serial.begin(115200);

    //Conexion WIFI
    //WiFi.begin("MOVISTAR_8380", "paderni9");
    WiFi.begin("twave-24", "KD6rUYrv");
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println( "" );
    Serial.println( "WiFi connected" );  
    Serial.println( "IP address: " );
    Serial.println( WiFi.localIP() );

    //Empieza la conexion modbus como master
    mb.server();		

    //Configuracion de las direcciones de registro
    mb.addIreg( SEGUNDERO );//discret input en direccion segundero
    if (!mb.addHreg(FIJOS, 0xF0F0, nreg)) Serial.println("Error");//init registros fijos
    if (!mb.addHreg(LEER_ESCRIBIR, 0xABCD, nreg)) Serial.println("Error");//init registros rw

    //Callback leer y escribir
    mb.onGetHreg(LEER_ESCRIBIR, cbRead, nreg); // Add callback on Coils value get
    mb.onSetHreg(LEER_ESCRIBIR, cbWrite, nreg);

    //Inicializamos la variable ts para que mida ms
    ts = millis();
}


void loop() {
   //Call once inside loop() - se lanzan las conexiones configuradas
   mb.task();

   //Al estar nosotros mandando señales cada cierto tiempo lo tratare como si
   //fuera una entrada analógica, debido a que es así como funcionar en una 
   //aplicacion real.

    //Cada segundo se actualiza la direccion
  if ( millis() > ts + 2000 ) 
  {
    segs++;
    ts = millis();
    mb.Ireg(SEGUNDERO,segs);//Escribimos en la direccion los segundos
  }

   delay(10);
}