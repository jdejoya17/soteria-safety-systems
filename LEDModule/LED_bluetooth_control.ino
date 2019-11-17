#include <SoftwareSerial.h>
SoftwareSerial BT(1, 0); 
// creates a "virtual" serial port/UART
// connect BT module TX to 0
// connect BT module RX to 1
// connect BT Vcc to 3.3V, GND to GND


void setup()  
{
  // set digital pin to control as an output
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  // set the data rate for the SoftwareSerial port
  BT.begin(9600);
  // Send test message to other device
  BT.println("Hello from Arduino");
}
char a; // stores incoming character from other device
void loop() 
{
  if (BT.available())
  // if text arrived in from BT serial...
  {
    a=(BT.read());
    if (a=='1')
    {
      digitalWrite(13, HIGH);
      BT.println(" left LED on");
      digitalWrite(12, LOW);
    }
    else if (a=='2')
    {
      digitalWrite(12, HIGH);
      BT.println(" right LED on");
      digitalWrite(13, LOW);
      
    }

    else if (a=='3')
    {
      digitalWrite(12, HIGH);
      digitalWrite(13, HIGH);
      BT.println(" both LED on");

    }
    else if (a=='4')
    {
      digitalWrite(11, HIGH);
      BT.println(" bluetooth ready");
    }
    else
    {
      digitalWrite(12, LOW);
      BT.println(" left LED off");
      digitalWrite(13, LOW);
      BT.println(" right LED off");
    }  
    // you can add more "if" statements with other characters to add more commands
  }
}
