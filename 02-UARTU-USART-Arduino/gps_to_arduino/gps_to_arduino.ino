/* 
* Author: 	Sfrangeu Bogdan-Otniel 
* Purpose: 	Receptie mesaje de la modul GPS si transmiterea lor la al doilea arduino.
* Language: C
*/ 
void setup() 
{ 
	Serial.begin(9600); 
	Serial1.begin(9600); 
} 
void loop() 
{ 
	while (Serial.available()) 
	{ 
		char c = Serial.read(); 
		Serial1.write(c); 
	}
}