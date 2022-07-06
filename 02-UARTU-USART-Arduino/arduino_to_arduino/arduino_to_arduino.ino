/* 
* Author: 	Sfrangeu Bogdan-Otniel 
* Purpose: 	Receptie mesaje de la primul arduino, extragerea latitudinii si afisarea acesteia.
* Language: C
*/
#include <TinyGPS.h> 
TinyGPS gps; //Creates a new instance of the TinyGPS object 
void setup() 
{
  Serial.begin(9600); 
  Serial1.begin(9600);
}
void loop() 
{ 
  bool newData = false; 
  unsigned long chars; 
  unsigned short sentences, failed; // For one second we parse GPS data and report some key values 
  for (unsigned long start = millis(); millis() - start < 1000;) 
  { 
    while (Serial.available()) 
    { 
      char c = Serial.read(); 
      if (gps.encode(c)) // Did a new valid sentence come in? 
        newData = true; 
    }
  }
  if (newData) //If newData is true 
  { 
    float flat, flon; 
    unsigned long age; 
    gps.f_get_position(&flat, &flon, &age); 
    Serial1.print("Latitude = "); 
    Serial1.println(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6); 
  }
}
