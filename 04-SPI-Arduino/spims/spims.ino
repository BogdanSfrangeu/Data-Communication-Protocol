/*
 * Author:    Sfrangeu Bogdan-Otniel
 * Purpose:   Receptie sir comanda prin UART, dupa care parseaza si comanda operatii de scriere/citire in memoriile eeprom prin spi.
 * Language:  C
 */

#define MOSI 11               //MASTER OUT SLAVE IN
#define MISO 12               //MASTER IN SLAVE OUT
#define SPICLOCK 13           //MASTER CLOCK OUTPUT
#define SLAVESELECTEEPROM1 10 // SLAVE SELECT CHIP EEPROM 1
#define SLAVESELECTEEPROM2  9 // SLAVE SELECT CHIP EEPROM 2

#define WREN 6    //Activare scriere pentru EEPROM
#define WRDI 4    //Dezactivare scriere pentru EEPROM
#define READ 3    //Operatie citire EEPROM
#define WRITE 2   //Operatie scriere EEPROM

#define SSTRANSITIONDELAY 10
#define WRITEDELAY 1000


//Variabile pentru datele de intrare/iesire EEPROM
byte eeprom_output_data = 0;
byte eeprom_input_data = 0;

String inputString = "";        // Variabila pentru stocarea comenzii
boolean stringComplete = false; // Comanda finalizata

uint8_t operation, id;
uint16_t address, value; 

//Functie pentru transferul datelor
char spi_transfer(volatile char data)
{
  SPDR = data;                     // Incarcare date
  while (!(SPSR & (1 << SPIF))) {} // Pooling pentru a asigura transferul complet al datelor
  return SPDR;                     // recuperarea datelor citite
}

//Functie pentru citirea EEPROM 
byte read_eeprom(int EEPROM_address, int EEPROM_slave)
{
  int data;                                   //Variabila temporara pentru stocarea datelor citite
  digitalWrite(EEPROM_slave, LOW);            //Activare circuit eeprom
  delay(SSTRANSITIONDELAY);
  spi_transfer(READ);                         //Transmisie comanda citire
  spi_transfer((char)(EEPROM_address >> 8));  //Transmisie adresa MSB first
  spi_transfer((char)(EEPROM_address));       //Transmisie adresa
  data = spi_transfer(0xFF);                  //Receptie caracter
  delay(SSTRANSITIONDELAY);
  digitalWrite(EEPROM_slave, HIGH);           //Dezactivare eeprom
  return data;
}

//Functie pentru scriere EEPROM 
void write_eeprom(uint8_t eepromSelect, uint16_t eepromAddress, uint16_t eepromValue)
{
  digitalWrite(eepromSelect, LOW);          //enable device
  delay(SSTRANSITIONDELAY);
  spi_transfer(WREN);                       //write enable
  digitalWrite(eepromSelect, HIGH);         //disable device
  delay(SSTRANSITIONDELAY);
  digitalWrite(eepromSelect, LOW);          //enable device
  spi_transfer(WRITE);                      //write instruction
  spi_transfer((char)(eepromAddress >> 8)); //send MSByte address first
  spi_transfer((char)(eepromAddress));      //send LSByte address
  spi_transfer((char)eepromValue);          //write data byte
  digitalWrite(eepromSelect, HIGH);         //disable device
  delay(WRITEDELAY);                        //wait for eeprom to finish writing
  digitalWrite(eepromSelect, LOW);          //enable device
  delay(SSTRANSITIONDELAY);
  spi_transfer(WRDI);                       //write disable
  digitalWrite(eepromSelect, HIGH);         //disable device
}

void handleMemoryOperation()
{

  switch (operation)
  {
    case '0'://write operation
      switch(id)
      {
        case '0':
          Serial.print("Operation: Write; Slave ID: 0; Address: ");
          Serial.print(address); 
          Serial.print(", Value: "); 
          Serial.print(value); 
          Serial.println(".");
          write_eeprom(SLAVESELECTEEPROM1, address, value);
        break;
        case '1':
          Serial.print("Operation: Write; Slave ID: 1; Address: ");
          Serial.print(address); 
          Serial.print(", Value: "); 
          Serial.print(value); 
          Serial.println(".");
          write_eeprom(SLAVESELECTEEPROM2, address, value);
        break;
        default: 
          Serial.println("Error: Invalid Slave ID.");
        break;
      }
      break;
    case '1'://read operation
      switch(id)
      {
        case '0':
          eeprom_output_data = read_eeprom(address, SLAVESELECTEEPROM1);
          Serial.print("Operation: Read; Slave ID: 0; Address: ");
          Serial.print(address); 
          Serial.print(", Value: "); 
          Serial.print(eeprom_output_data, DEC); 
          Serial.println(".");
        break;
        case '1':
          eeprom_output_data = read_eeprom(address, SLAVESELECTEEPROM2);
          Serial.print("Operation: Read; Slave ID: 1; Address: ");
          Serial.print(address); 
          Serial.print(", Value: "); 
          Serial.print(eeprom_output_data, DEC); 
          Serial.println(".");
        break;
        default: 
          Serial.println("Error: Invalid Slave ID.");
        break;
      }
      break;
    default:
      Serial.println("Error: Invalid Operation.");
      break;
  }
}

//Functie pentru receptionarea datelor pe interfata seriala
void serialEvent() 
{
  while (Serial.available() > 0) 
  {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\r') stringComplete = true;
  }
}
void setup()
{
  Serial.begin(9600);                     //Initializarea interfata seriala
  inputString.reserve(10);                //Alocare date comanda
  pinMode(MOSI, OUTPUT);                  //Configurare pin iesire
  pinMode(MISO, INPUT);                   //Configurare pin intrare
  pinMode(SPICLOCK, OUTPUT);              //Configurare pin iesire
  pinMode(SLAVESELECTEEPROM1, OUTPUT);    //Configurare pin iesire
  pinMode(SLAVESELECTEEPROM2, OUTPUT);    //Configurare pin iesire
  digitalWrite(SLAVESELECTEEPROM1, HIGH); //Dezactivare EEPROM 1
  digitalWrite(SLAVESELECTEEPROM2, HIGH); //Dezactivare EEPROM 2
  
  // SPCR = 01010000
  //interrupt disabled,spi enabled,msb 1st,master,clk low when idle,
  //sample on leading edge of clk,system clock/4 rate (fastest)
  SPCR = (1 << SPE) | (1 << MSTR);
  byte clr;   //Variabila temporara pentru curatarea registrelor SPE si MSTR
  clr = SPSR; // "Curatarea" registrului SPSR
  clr = SPDR; // "Curatarea" registrului SPDR
}

void loop()
{
  /*
  if(Serial.available() > 0) 
  {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\r') stringComplete = true;
  }
  */
  serialEvent();
  if (stringComplete) 
  {
    Serial.print("Received command: ");
    Serial.println(inputString);

    char *cmd[4]; 
    char *ptr = NULL;
    byte i = 0; 
    ptr = strtok(inputString.c_str(), "*"); 
    while(ptr != NULL) 
    { 
      cmd[i] = ptr; 
      i++; 
      ptr = strtok(NULL, "*"); 
    }
    operation = *cmd[0];
    id =*cmd[1]; 
    address = atoi(cmd[2]); 
    value = atoi(cmd[3]);

    //Alegere operatie 
    handleMemoryOperation(); 
    // Resetare variabile globale 
    inputString = ""; 
    stringComplete = false;
  }
}
