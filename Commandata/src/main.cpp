#include <Arduino.h>
#include <SerialCommand.h>



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
static char input_data_char[100];
unsigned int received_bytes=0;
unsigned int mycrc16;
static char next_command[10];
unsigned int wait_for_data=0;
unsigned int MAX= 64;


SerialCommand sCmd;//objeto para comandos serial

int cmdNack(char *param, uint8_t len, char*response){
    Serial.println("Comando no aceptado");
    return strlen(response);
}

int cmdVersion(char* param, uint8_t len, char* response){
    //char fw_ver[20];
    sprintf(response, "$ACK_VERSION;");
    Serial.printf("Hola soy ESP32");
    //sprintf(fw_ver, ", FW=%02d.02d.%02d;",FW_VER_MAJOR,FW_VER_MINOR, FW_VER_PATCH);
    //strcat(response, fw_ver);
    return strlen(response);
}



void CommDataClass(){
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
        cmd_p %= (datalen+16); //This is for discharging the buffer
        
      
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

            if(received_bytes==1){
                mycrc16 = inputData << 8; //primera parte de crc(HEX)
            }
            else if(received_bytes==2){
                mycrc16 += inputData;     //Segunda parte de crc(HEX)
            }
            else if(received_bytes==3){
                //next_command="$0x01;"; //Esta puesto para que tenga caracter inicial y final
                sprintf(next_command,"$0x%02X;",inputData);
                Serial.println(next_command);
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
                    sCmd.processCommand(next_command,response);
                }
            }

            else if(received_bytes>=5){
                Serial.println("Llegamos a 5");
                if(wait_for_data>0){
                    wait_for_data--;
                    Serial.printf("Wait for data:  %d\n", wait_for_data);
                }
                if(wait_for_data==0){
                    //checkcrc
                    received_bytes=0;
                    sCmd.processCommand(next_command,response);
                    Serial.printf("PROCESADO\n");
                }
                
                
            }


            //incrementamos bytes recibidos para iniciar el proceso de recoleccion
            //eliminating the end and the start_char
            //Serial.println(response);
        }

    

    }




}


void setup() {
  Serial.begin(115200);

  sCmd.addCommand("0x01",cmdVersion);
  sCmd.setDefaultHandler(cmdNack);
  // put your setup code here, to run once:
}

void loop() {
  CommDataClass();
  Serial.println("VARIABLES OBTENIDAS");

  Serial.printf("mycrc16: %d\n",mycrc16);
  Serial.println(next_command);
  Serial.printf("wait_for_data: %d\n", wait_for_data);
  
  //SerialIO();
  // put your main code here, to run repeatedly:
  delay(5000);
}