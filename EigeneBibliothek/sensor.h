#ifndef SENSOR_H
#define SENSOR_H

#include <SimpleDHT.h>

// for DHT11, 
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 3
int pinDHT11 = 3;
SimpleDHT11 dht11;

const int trigPin = 2;
const int echoPin = 4;

void readTempSensorData ( int* temp, int* hum);
void readUsSensorData (int* duration, int* cm, int* inches);
long microsecondsToInches(long microseconds);
long microsecondsToCentimeters(long microseconds);


void readTempSensorData ( int* temp, int* hum)
{
  unsigned long CT;
  unsigned long CR;
  unsigned long EP;

  byte temperature = 0;
  byte humidity = 0;
  
  while (dht11.read(pinDHT11, &temperature, &humidity, NULL)) {
 /*   if (sem == 0){
      CT = millis();
      sem = 1;
    }*/
  }
  //sem = 0;
/*  CR = millis();
  EP = CR - CT;
  CT = 0;*/
 // Serial.print("Time since last corect: ");
 // Serial.println(EP);
  Serial.print("Sample OK: ");
  Serial.print((int)temperature); Serial.print(" *C, "); 
  Serial.print((int)humidity); Serial.println(" %");

  *temp = (int)temperature;
  *hum =  (int) humidity;
  
}

void readUsSensorData (int* duration, int* cm, int* inches)
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long d, i, c;
  
  d = pulseIn(echoPin, HIGH);
  // convert the time into a distance
  i = microsecondsToInches(d);
  c = microsecondsToCentimeters(d);

  *duration = (int)d;
  *cm = (int)c;
  *inches = (int)i;
  
}


long microsecondsToInches(long microseconds)
{
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second).  This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}



#endif

