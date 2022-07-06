/* 
* Author:   Sfrangeu Bogdan-Otniel
* Purpose:  Citire sir comanda prin comunicatie seriala UART, procesare sir comanda si citire temperatura de la senzor specific prin i2c, afisarea temperaturii in terminal virtual.
* Language: C 
*/

#include <Wire.h>
// define DS1621 I2C slave address (1001+A2+A1+A0)
// Sensor 1                     --> 1001 0  0  0 = 0x48
// Sensor 2                     --> 1001 0  0  1 = 0x49
// Sensor 3                     --> 1001 0  1  1 = 0x4A
#define DS1621_S1_ADDRESS  0x48
#define DS1621_S2_ADDRESS  0x49
#define DS1621_S3_ADDRESS  0x4A

char commandCharacter;
float temperature;

void setup() 
{
  Serial.begin(9600);
  Wire.begin();                               // join i2c bus
  // initialize DS1621 sensor 1
  Wire.beginTransmission(DS1621_S1_ADDRESS);  // connect to DS1621 (send DS1621 address)
  Wire.write(0xAC);                           // send configuration register address (Access Config)
  Wire.write(0);                              // perform continuous conversion
  Wire.beginTransmission(DS1621_S1_ADDRESS);  // send repeated start condition
  Wire.write(0xEE);                           // send start temperature conversion command
  Wire.endTransmission();                     // stop transmission and release the I2C bus

  // initialize DS1621 sensor 2
  Wire.beginTransmission(DS1621_S2_ADDRESS);  // connect to DS1621 (send DS1621 address)
  Wire.write(0xAC);                           // send configuration register address (Access Config)
  Wire.write(0);                              // perform continuous conversion
  Wire.beginTransmission(DS1621_S2_ADDRESS);  // send repeated start condition
  Wire.write(0xEE);                           // send start temperature conversion command
  Wire.endTransmission();                     // stop transmission and release the I2C bus

  // initialize DS1621 sensor 3
  Wire.beginTransmission(DS1621_S3_ADDRESS);  // connect to DS1621 (send DS1621 address)
  Wire.write(0xAC);                           // send configuration register address (Access Config)
  Wire.write(0);                              // perform continuous conversion
  Wire.beginTransmission(DS1621_S3_ADDRESS);  // send repeated start condition
  Wire.write(0xEE);                           // send start temperature conversion command
  Wire.endTransmission();                     // stop transmission and release the I2C bus
}

void loop() 
{
  if(Serial.available() > 0)
  { 
    commandCharacter = Serial.read(); 
  } 
  if (commandCharacter == '1')
  {
    temperature = get_temperature(DS1621_S1_ADDRESS);
    if(temperature > 125) temperature -= 256;
    Serial.print("Senzor 1, Temperatura citita: ");
    Serial.print(temperature);
    Serial.println(" C."); 
    commandCharacter = '0';
  }

  if (commandCharacter == '2')
  {
    temperature = get_temperature(DS1621_S2_ADDRESS);
    if(temperature > 125) temperature -= 256;
    Serial.print("Senzor 2, Temperatura citita: ");
    Serial.print(temperature);
    Serial.println(" C."); 
    commandCharacter = '0';
  }

  if (commandCharacter == '3')
  {
    temperature = get_temperature(DS1621_S3_ADDRESS);
    if(temperature > 125) temperature -= 256;
    Serial.print("Senzor 3, Temperatura citita: ");
    Serial.print(temperature);
    Serial.println(" C."); 
    commandCharacter = '0';
  }
}

float get_temperature(uint8_t DS1621_S_ADDRESS) 
{
  Wire.beginTransmission(DS1621_S_ADDRESS); // connect to DS1621 (send DS1621 address)
  Wire.write(0xAA);                         // read temperature command
  Wire.endTransmission(false);              // send repeated start condition
  Wire.requestFrom(DS1621_S_ADDRESS, 2);    // request 2 bytes from DS1621 and release I2C bus at end of reading
  uint8_t t_msb = Wire.read();              // read temperature MSB register
  uint8_t t_lsb = Wire.read();              // read temperature LSB register

  float raw_t = t_msb;
  if(t_lsb) raw_t += 0.5;
  return raw_t;
}
