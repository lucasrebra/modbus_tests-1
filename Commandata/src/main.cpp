#include <Arduino.h>
#include <SerialCommand.h>
#include <BluetoothSerial.h>


#define ONBOARD_LED  2


/*
void SerialIO(){

    char c;
    char response[100];
    static char cmd_buffer[100];
    static unsigned int cmd_p = 0;

    while (Serial.available()) {
        c = char(Serial.read());
        if(c == START_CHAR)
          cmd_p = 0;
          cmd_buffer[cmd_p] = c;
          cmd_p += 1;
          cmd_p %= (data_len+16);//This is for discharging the buffer

      
        if(c == END_CHAR) {
            cmd_buffer[cmd_p] = 0;
            cmd_p = 0;
            sCmd.processCommand(cmd_buffer,response);//converts the string in the command
            //eliminating the end and the start_char
            Serial.println(response);
        }
    }  
}
*/

/*
int checkCRC() {//Se chequea el CRC
    unsigned int sumador=0; 
    unsigned int suma = 0;

    if (!crc_enabled)
        return 1;

    for (int i = 2; i < received_bytes; i++)
        sumador= (int)strtol(input_data+i, NULL, 0);//cambiamos a int
        suma += sumador;

    return (my_crc16 == suma);
}*/
/*
void CommDataClass () {
    //Esta va a ser la funcion que nos va a hacer de SERIAL I/O

    //La diferencia con el programa del Opendaq es que nosotros lo vamos
    //a hacer con strings

    char inputData = char(Serial.read());   //leemos char del serial
    input_data[received_bytes] = inputData; //guardamos en input_data
    received_bytes++;                       //Incrementamos en 1 el num de bytes recibidos
    //received_bytes %= MAX_DATA_BYTES;
    //int inputDatadef = (int)strtol(&inputData, NULL, 0); //Cambiamos char a int
    int inputDatadef=0;
    //Vamos a ir almacenando los bytes en la variable input_data
    if (received_bytes == 1)        //Primer byte recibido (CRC1)
        my_crc16 = inputDatadef << 8;

    else if (received_bytes == 2)   //Segundo byte recibido(CRC2)
        my_crc16 += inputDatadef;

    else if (received_bytes == 3)   //Tercer byte recibido(comando)
        *next_command = inputDatadef;   //Comando que utilizaremos a la hora de encontrar nuestra orden

    else if (received_bytes == 4) {  
        wait_for_data = inputDatadef;//numero de bytes argumentos (stream)

        if (wait_for_data > MAX_DATA_BYTES - 4)//Si mayor que el max NO valido
            wait_for_data = 0;

        if (wait_for_data > 10)                //si mayor que 10 no valido
            wait_for_data = 0;

        data_len = wait_for_data;              //Finalmente guardamos en data_len

        if (wait_for_data == 0) {              //si no hay argumentos procesamos el comando
            sCmd.processCommand(input_data,next_command);   //procesamos el comando
        }

    } else {
        if (wait_for_data > 0)
            wait_for_data--;// Restamos hasta que se acaben argumentos //Si pasa de los bytes iniciales, ARGUMENTOS
        if (wait_for_data == 0) {  //Cuando sea 0 se chequea el CRC
            /*if (!checkCRC()) {
                return;
            }
            sCmd.processCommand(input_data,next_command);
        }
    }
}*/
/*
int received_bytes=0;

void CommDataClass(){
    String c;
    char response[100];
    static char cmd_buffer[100];
    unsigned int cmd_p=0;
    

    while(Serial.available()){
        c = char(Serial.readStringUntil('\n'));
    }
    int inputData=(int)strtol(&c,NULL,0);
    cmd_buffer[cmd_p] = c;
    cmd_p++;
    Serial.println(cmd_p);
    Serial.println(inputData);
    input_data[received_bytes]=cmd_buffer;

}*/
unsigned int datalen=100;
unsigned int data_len=0;
static char END_CHAR='\n';
unsigned int input_data_int[100];
byte  input_data_def[100];
static char input_data_char[100];
unsigned int received_bytes=0;
unsigned int mycrc16=0;
static char next_command[10];
String next_command_string;
unsigned int wait_for_data=0;
unsigned int MAX= 64;
int crc_enabled=1;
int ON=0;

SerialCommand sCmd;//objeto para comandos serial
BluetoothSerial SerialBT;



int cmdNack(char *param, uint8_t len, char*response){
    Serial.println("Comando no aceptado");
    return strlen(response);
}

int cmdVersion(char* param, uint8_t len, char* response){
    //char fw_ver[20];
    sprintf(response, "$ACK_VERSION;");
    Serial.printf("Hola soy ESP32");

    delay(1000);
    digitalWrite(ONBOARD_LED,HIGH);
    delay(10000);
    digitalWrite(ONBOARD_LED,LOW);

    //sprintf(fw_ver, ", FW=%02d.02d.%02d;",FW_VER_MAJOR,FW_VER_MINOR, FW_VER_PATCH);
    //strcat(response, fw_ver);
    return strlen(response);
}

int checkCRC() {//Se chequea el CRC
    int sumador=0; 
    unsigned int suma = 0;
    //unsigned int my_crc16;

    if (!crc_enabled)
        return 1;

    for (int i = 2; i < received_bytes; i++){
        sumador= input_data_int[i];//cambiamos a int
    
        suma += sumador;}
        
    Serial.println(mycrc16);
    Serial.println(suma);
    return (mycrc16 == suma);
}

//Programa que va a leer el serial e ir introduciendolo en variable
//global input_data_int[] ---> Valores en crudo
void ReadSerial(){

    /*defino variables*/
    char c;
    static char cmd_buffer[100];    //Aquí guardaremos nuestro cmd serial
    unsigned int cmd_p=0;           //Contador pointer
    char response[100];             //Respuesta command handler
    /*Programa con el que recogeremos comandos para formar info*/

    while (Serial.available()) {

        c = char(Serial.read());//Vamos leyendo caracteres
        cmd_buffer[cmd_p] = c;
        cmd_p += 1;
        cmd_p %= 100;//This is for discharging the buffer
        
      
        if(c == END_CHAR) {
            cmd_buffer[cmd_p] = 0;//En la pos caracter end '\n' un 0
            cmd_p = 0;//Colocamos el puntero a 0
            int inputData=(int)strtol(cmd_buffer,NULL,0);//transformamos a entero
            Serial.println(inputData);
            input_data_int[received_bytes]=inputData;//Guardamos en nuestro buffer de enteros
            Serial.println(input_data_int[received_bytes]);
            //sprintf(&input_data_char[received_bytes],"%x",input_data_int[received_bytes]);//Guardamos en formato hexadecimal char
            Serial.printf("%x",input_data_char[received_bytes]);
            Serial.println(received_bytes);
            received_bytes++;
        }
    }
}

void converseCommand(int* input){

    int escaped=0;
    byte a;


    for (int i=0; i< received_bytes; i++) {
        a=input_data_int[i];
        if ((a == 0x7D) || (a == 0x7E)) {
            escaped++;  // add escaped bytes to the packet length
        }        
    }
    int size=received_bytes+escaped;


    for (int i=0; i< size; i++) {
        a=byte(input_data_int[i]);
        input_data_def[i]=a;
        if ((a == 0x7D) || (a == 0x7E)) {  // add escaped bytes to the packet length
            i++;
            input_data_def[i]= a^0x20;
        }   
    }

    Serial.write(0x7E);

    for (int i=0; i<size;i++){

        Serial.printf("0x%02x ",input_data_def[i]);

    }
    Serial.println();

}

/*
void sendCommand(byte* response, int size) {
    byte a;

    for (byte i = 0; i < size; i++) {
        a = response[i];
        if ((a == 0x7D) || (a == 0x7E)) {
            response[3]++;  // add escaped bytes to the packet length
        }
    }


    Serial.write(0x7E);

    for (byte i = 0; i < size; i++) {
        a = response[i];
        if ((a == 0x7D) || (a == 0x7E)) {
            Serial.write(0x7D);
            Serial.write(a ^ 0x20);
        } else {
            Serial.write(a);
        }
    }
}*/

/*


ESTUDIAR COMANDOS PARA APLICACION

void ProcessSPICommunications(CommandHandler* cmd){
    char cmd_buffer[BUFF_CMD_SIZE];
    unsigned int len=0;
    static char nextcommand[BUFF_CMD_SIZE];
    static BYTE pos = 0;
    char response[40];
    BYTE lenr; 
    BYTE i=0;
    BYTE c;

    if(SpiSlaveAvailable(&SPI2_drv)) {
        len = SpiSlaveGets(&SPI2_drv, cmd_buffer);
        while(i<len){
            c = cmd_buffer[i];
            c &= 0xFF;
            if(c == 0x7D){
                nextcommand[pos] = cmd_buffer[i+1]^0x20;
                i++;
            }
            else{
                nextcommand[pos] = c;
            }
            if(pos<BUFF_CMD_SIZE)
                pos++;
            if(c == cmd->end_char){
                //sprintf(response, "frame(%d) = %d\r\n", pos, check_frame(nextcommand, pos));
                //SerialPuts(&USBSerial, response);
                lenr = processCommand(cmd, nextcommand, response);
                //SerialNPuts(&USBSerial, response, lenr);
                //SpiSlaveNPuts(&SPI2_drv, response, lenr);
                pos = 0;
            }
            i++;
        }    
    }

inline BYTE spi_interrupt_hdr(BYTE c){
    static BYTE scmd, slen, svalue = 0;
    static int count = 0;
    static BOOL escaped = 0;
    BYTE value;
    BYTE resp = c;
    
    if(!escaped)
        value = c;
    else
        value = c^0x20;
        
    if(count == 2){
        scmd = value;
    }
    else if(count == 3){
        slen = value;
    }
    else if(count == 4){
        svalue = value;
        if(scmd == CMDREADPIN)
            resp = digitalRead(svalue);
    }
    
    if(c != 0x7D){
        count ++;
        escaped = 0;
    }else{ 
        escaped = 1;
    }
    
    if(c==0x7E)
        count = 0;
    
    return resp;
}
        
}*/


void CommDataClass(){
    /*defino variables*/

    char c;
    static char cmd_buffer[100];    //Aquí guardaremos nuestro cmd serial
    unsigned int cmd_p=0;           //Contador pointer
    char response[100];             //Respuesta command handler
    
    /*Programa con el que recogeremos comandos para formar info*/
    for(int i=0;i<received_bytes;i++){
        int inputData=input_data_int[i];

        if(received_bytes==1 ){
            mycrc16 = inputData << 8; //primera parte de crc(HEX)
        }

        else if(received_bytes==2 ){
            mycrc16 += inputData;     //Segunda parte de crc(HEX)
        }
        
        else if(received_bytes==3){
            //next_command="$hola;"; //Esta puesto para que tenga caracter inicial y final
            sprintf(next_command,"$%d;",inputData);
            Serial.println("EL comando que envio");
            Serial.println(next_command[0]);
            Serial.println(next_command[1]);
            Serial.println(next_command[2]);
            Serial.println(next_command[3]);
            Serial.println(next_command[4]);
            //Serial.printf("next command received bytes: %02x",next_command);
        }
        else if(received_bytes==4){
            wait_for_data = inputData;//numero de argumentos que vamos a tener

            //ifs en caso de que no se ajuste bien a lo requerido
            if(wait_for_data > MAX){
                wait_for_data=0;
            }
            data_len=wait_for_data;

            if(wait_for_data==0){
                
                Serial.printf("El num de argumentos es 0\n");
                if(checkCRC()){
                    received_bytes=0;
                    Serial.println(next_command);
                    sCmd.processCommand(next_command,response);}
                else{
                    received_bytes=0;
                    Serial.println("El chechcrc no es valido");
                }
            }
        }

        else if(received_bytes>=5){
            Serial.println("Llegamos a 5");
            if(wait_for_data>0){
                wait_for_data--;
                Serial.printf("Wait for data:  %d\n", wait_for_data);
            }
            if(wait_for_data==0){
                
                if(checkCRC()){
                    received_bytes=0;

                    sCmd.processCommand(next_command,response);
                    Serial.printf("PROCESADO\n");}
                else{
                    received_bytes=0;
                    Serial.println("El chechcrc no es valido");
                }
            }
            
            
        }
        //incrementamos bytes recibidos para iniciar el proceso de recoleccion
        //eliminating the end and the start_char
        //Serial.println(response);
    }

}





void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  pinMode(ONBOARD_LED,OUTPUT);

  sCmd.addCommand("1",cmdVersion);
  sCmd.addCommand("2",cmdNack);

  sCmd.setDefaultHandler(cmdNack);
  // put your setup code here, to run once:
}

void loop() {
  ReadSerial();
  converseCommand((int*)input_data_int);
  CommDataClass();
  SerialBT.println("VARIABLES OBTENIDAS");

  SerialBT.printf("mycrc16: %d\n",mycrc16);
  SerialBT.println(next_command);
  SerialBT.printf("wait_for_data: %d\n", wait_for_data);
  
  Serial.printf("mycrc16: %d\n",mycrc16);
  Serial.println(next_command);
  Serial.printf("wait_for_data: %d\n", wait_for_data);
  //SerialIO();
  // put your main code here, to run repeatedly:
  delay(5000);
}

