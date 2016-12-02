/*
Arduino Due - ESP 8266 WiFi Module
Serial (Tx/Rx) communicate to PC via USB
Serial3 (Tx3/Rx3) connect to ESP8266
Tx3 - ESP8266 Rx
Rx3 - ESP8266 Tx
pin 53 - ESP8266 CH_PD (Always High)
*/
#include <sensor.h>
#include <communication.h>
#include <definitions.h>
#include <zustand.h>

int counter = 0;
int counterfail =0;
int countersend =0;

void setup() {
    Serial.begin(115200);
    startESP8266Connenction ();
}

void loop() {
    while (Serial.available() > 0) 
    {
      char a = Serial.read();
      Serial3.write(a);
   }

  int temperature, humidity;
  readTempSensorData(&temperature, &humidity);

  int duration, inches, cm;
  readUsSensorData (&duration, &cm, &inches);

  int offen = ZustandTuerStatus(cm);
  int Zustand = Zustandsmaschine(offen,temperature,humidity);

  if (!sendValue("present",cm, "temperature", (int)temperature,"humidity", (int)humidity, "NumberState", (int)Zustand))
  {
    Serial.println("Send failed");
  }

  if (counter == 100)
  {
    Serial.print("Send: ");
    Serial.print(counter);
    Serial.print(" Send successed: ");
    Serial.print(countersend);
    Serial.print(" Send failed: ");
    Serial.print(counterfail);
    counter = 0;
    counterfail =0;
    countersend =0;
  }
  delay(500);
}




