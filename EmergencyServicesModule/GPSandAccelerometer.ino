/*
 * Senior Design - Spartan Safety System 
 * 
 * Team: Eric Zavala
 *       Joseph De Joya
 *       Cho Chen
 *       Ronak Patel
 *       
 * Purpose:
 *        The purpose of our project is to create devices for motorcyclists to help them be safer on the road. 
 *        The first part of our project is adding a safety feature on a standard helmet such as impact detection. 
 *        In essence the helmet will contact emergency services when the motorcyclist has been in an fatal 
 *        accident on the road, i.e. an impact to the head. 
 *        This is our code:
 * 
 * Note: Need to have downloaded Adafruit_FONA library from github repo before using this code
*/

#include "Adafruit_FONA.h"
#include <string.h>

// inputs for fona GPS module
#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4

// inputs for accelerometer
const int xInput = A0;
const int yInput = A1;
const int zInput = A2;

// We default to using software serial.
#include <SoftwareSerial.h>
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

// Use this one for FONA 3G
Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);

// model of our adafruit fona 
uint8_t type;

// Raw Ranges for accelerometer:
// 0 = 0v, 1023 = 3.3v
int RawMin = 0;
int RawMax = 1023;

// Take multiple samples to reduce noise
const int sampleSize = 10;

void setup() 
{
  while (!Serial);
  Serial.begin(115200);
  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    while (1);
  }
  type = fona.type();

  analogReference(EXTERNAL);
}

void loop() 
{
  // read in from the 3 accelerometers
  int xRaw = ReadAxis(xInput);
  int yRaw = ReadAxis(yInput);
  int zRaw = ReadAxis(zInput);
  
  // Convert raw values to 'milli-Gs"
  long xScaled = map(xRaw, RawMin, RawMax, -200000, 200000);
  long yScaled = map(yRaw, RawMin, RawMax, -200000, 200000);
  long zScaled = map(zRaw, RawMin, RawMax, -200000, 200000);

  // re-scale to fractional Gs
  float xAccel = xScaled / 1000.0;
  float yAccel = yScaled / 1000.0;
  float zAccel = zScaled / 1000.0;

  // compute cumulative Gs
  double total_G = sqrt(xAccel*xAccel + yAccel*yAccel + zAccel*zAccel);
  
  if(total_G >= 2) // threshold is set very low for testing purposes
  {
    // send an SMS
    
    // convert G value to array of chars so we can concatonate with rest of message
    String g_string = String(total_G);
    char gs[10];
    g_string.toCharArray(gs,10);
    strcat(gs, "G.");

    // phone number to send text to 
    char sendto[21] = "6309995053";

    char message[140] = "Driver has been in an accident. Suffered an impact of: ";
    strcat(message,gs);

    if (fona.enableGPS(true)) // turn on GPS - function returns true if successful
    {
      char gpsdata[120];
      fona.getGPS(0, gpsdata, 120);
      fona.sendSMS(sendto, message);
      fona.sendSMS(sendto, "Information of collision in format: mode,fixstatus,utctime(yyyymmddHHMMSS),latitude,longitude,altitude,speed,course,fixmode,reserved1,HDOP,PDOP,VDOP,reserved2,view_satellites,used_satellites,reserved3,C/N0max,HPA,VPA");
      fona.sendSMS(sendto, gpsdata);
    }
    else // GPS did not turn on - send text without coordinates
    {
      strcat(message," Location: UIC SEL.");
      fona.sendSMS(sendto, message);
    }
    
    while(true){} // no need to keep monitoring
  }
  delay(500);
}

//
// Read "sampleSize" samples and report the average
//
int ReadAxis(int axisPin)
{
  long reading = 0;
  analogRead(axisPin);
  delay(1);
  for (int i = 0; i < sampleSize; i++)
  {
    reading += analogRead(axisPin);
  }
  return reading/sampleSize;
}
