/*
Arduino Due - ESP 8266 WiFi Module
Serial (Tx/Rx) communicate to PC via USB
Serial3 (Tx3/Rx3) connect to ESP8266
Tx3 - ESP8266 Rx
Rx3 - ESP8266 Tx
pin 53 - ESP8266 CH_PD (Always High)
*/
#include <SimpleDHT.h>

// for DHT11, 
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 3


int thr = 30; [cm]

byte temp_up_thr = 30; //[C°]
byte temp_down_thr = 2; //[C°]

byte humidity_up_thr = 70;
byte humidity_down_thr = 20;

//Zustandsautomat:
byte offen = 0;
byte TooHot = 0;
byte TooCold = 0;

byte Zustand = 0;


int pinDHT11 = 3;
SimpleDHT11 dht11;


enum WaitFor { OK ,SEND_OK, READY };

enum Zustand {FensterZu, ZuWarm, ZuKalt, ZuWarmZuFeucht, ZuKaltZuFeucht, ZuWarmZuTrocken, ZuKaltZuTrocken, ZuFeucht, ZuTrocken };


#define ESP8266 Serial3
String SSID = "PES";
String PASSWORD = "arduino_pes";
String IpAddress = "192.168.178.3";


#define SERVER "www.google.com"
#define PORT 80
#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT  1883  
//#define AIO_KEY "xxx" //Julia
#define AIO_KEY "xxx" //Klemens

//always high
int CH_PD_8266 = 53;
int circledValue = 0;
boolean FAIL_8266 = false;

//Ultraschallsensor
const int trigPin = 2;
const int echoPin = 4;
long old_val = 0;
long dif = 0;
char semaphor = 0;
double threshold = 0;

int counter = 0;
int counterfail =0;
int countersend =0;

void setup() {
  pinMode(CH_PD_8266, OUTPUT);
  digitalWrite(CH_PD_8266, HIGH);
  delay(1000);

    pinMode(trigPin, OUTPUT);
    digitalWrite(trigPin, LOW);

    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

 // do{
    Serial.begin(115200);
    ESP8266.begin(74880);
  
    //Wait Serial Monitor to start
    while(!Serial);
    Serial.println("- Start -");

    ESP8266.println("AT+RST");
    delay(1000);

    ESP8266.end();
    ESP8266.begin(115200);
    Serial.println("- Restart -");
    
    Serial.println("Module is ready\n AT+CWMODE=1");
    ESP8266.println("AT+CWMODE=1");
    waitfromESP8266(10, OK);
    ESP8266.println("AT+CWMODE?");

    waitfromESP8266(10, OK);
      
    //Quit existing AP, for demo
      ESP8266.println("AT+CWQAP");
      Serial.println("= Disconnect from AP: AT+CWQAP =");
      waitfromESP8266(10, OK);
      
      clearESP8266SerialBuffer();

      Serial.println("= AT+CIPMUX=1 =");
      ESP8266.println("AT+CIPMUX=1");
      waitfromESP8266(10, OK);
     
      cwJoinAP();
      waitfromESP8266(10, OK);
      cwGetIP();

    // sendValue("test-feed",56);
}

int sem = 0;
unsigned long CT;
unsigned long CR;
unsigned long EP;

void loop() {
    while (Serial.available() > 0) 
    {
      char a = Serial.read();
      Serial3.write(a);
   }

  int duration, inches, cm;
  byte temperature = 0;
  byte humidity = 0;
  while (dht11.read(pinDHT11, &temperature, &humidity, NULL)) {
    if (sem == 0){
      CT = millis();
      sem = 1;
    }
  }
  sem = 0;
  CR = millis();
  EP = CR - CT;
  CT = 0;
  Serial.print("Time since last corect: ");
  Serial.println(EP);
  Serial.print("Sample OK: ");
  Serial.print((int)temperature); Serial.print(" *C, "); 
  Serial.print((int)humidity); Serial.println(" %");
  
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  
  duration = pulseIn(echoPin, HIGH);
  // convert the time into a distance
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);
  
  //Tür offen
  if(cm > thr){
	  offen = 1;
  }else{
	  offen = 0;
	  Zustand = FensterZu;
  }
  
  if (offen == 1){
	  
	  //TEMP
	  if (temp_up_thr > temperature){
				Zustand = ZuWarm; //zu warm (und feuchtigkeit ok)
	  }else{
		  if (temp_down_thr < temperature){
				Zustand = ZuKalt; //zu kalt (und feuchtigkeit ok)
		  }
		}
	  
	  //Humid
	 if (humidity_up_thr > humidity){
			if(Zustand == ZuWarm)
				Zustand = ZuWarmZuFeucht; //zu warm & zu feucht
			else{ 
				if (Zustand == ZuKalt){
					Zustand = ZuKaltZuFeucht; //zu kalt & zu feucht
			    }else{
					Zustand = ZuFeucht; //nur zu feucht
				}
			}
	}else{
		  if(humidity_down_thr < humidity) {
			 if (Zustand == ZuWarm)
				 Zustand = ZuWarmZuTrocken; //zu warm & zu trocken
			 else{
				 if (Zustand == 2){
					Zustand = ZuKaltZuTrocken; //zu kalt & zu trocken
				 }else{
					 Zustand = ZuTrocken; //nur zu trocken
				 }
			 }
		  }
	  }
  }
  /*
  if(Zustand % 2 == 0)
	  Serial.println("Zu kalt...");
  else
	  Serial.println("Zu warm");
  */
  dif = cm - old_val;

  if (dif < 0)
      dif = 0 - dif;

  if (semaphor == 0){
    threshold = (double) dif * 0.01;
    semaphor = 1;
    Serial.println();
    Serial.println("Thresh: ");
    Serial.println(threshold);
  }
  if ((double)dif > threshold){
    Serial.println();
    Serial.print("Great change!");
    Serial.println();  
  }
  old_val = cm;

  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  Serial.print("Difference");
  Serial.println();
  Serial.print(dif);
  Serial.println();  
  

  if (sendValue("present",dif, "temperature", (int)temperature,"humidity", (int)humidity))
  {}
 /* if (sendValue("present",circledValue))
  {
    circledValue += 50;
    delay (5000);//5s
    if (circledValue > 333)
      circledValue = 0;
  }*/
  else
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

bool sendValue (String feed, int usvalue, String feed2,int temp, String feed3,int hum)
{
  String cmd= "AT+CIPSTART=4,\"TCP\",\"io.adafruit.com\",80";
         
  //Serial.println("= "+cmd);
  ESP8266.println(cmd);

  delay(500);
           
  // Test Nachricht
  Serial.println("= startString,");
  //String headerGET = "GET /api/feeds?x-aio-key=xxx HTTP/1.1\r\n"; // FeedListe
  //String headerGET = "GET /api/feeds/twar?x-aio-key=xx HTTP/1.1\n"; // DHT Wert abfragen

   //GET /api/groups/weather/send.json?x-aio-key=xxxx&testfeed=1 HTTP/1.1\r\n
 // String headerGET = "GET /api/groups/project/send.json?x-aio-key=xxx";
  
  String headerGET = "GET /api/groups/embedded/send.json?x-aio-key=";
  headerGET += AIO_KEY; //xxx
  headerGET += "&";
  headerGET += feed;
  headerGET += "=";
  headerGET += usvalue;
  headerGET += "&";
  headerGET += feed2;
  headerGET += "=";
  headerGET += temp;
  headerGET += "&";
  headerGET += feed3;
  headerGET += "=";
  headerGET += hum;
  headerGET += " HTTP/1.1\r\n";
  headerGET  += "Host: io.adafruit.com\r\n\r\n";
      
  int headerGetLen = headerGET.length();
  //Serial.println(headerGET);
  // Serial.println("= AT+CIPSEND=4,"+headerGetLen);
      
  // if (waitOKfromESP8266(1000))
  ESP8266.print("AT+CIPSEND=4,");
  ESP8266.println(headerGetLen);

      
  if (waitfromESP8266(10, OK))
  {
    //Serial.println("send");
    ESP8266.print(headerGET);
    ESP8266.print("+++");
    if(waitfromESP8266(10, SEND_OK))
    {
      Serial.println("true");
      return true;
    }
    else
    {
      Serial.println("false");
      return false;
    }
  }
}

/*
void serialEvent3() {
  while (ESP8266.available() > 0) {
    char a = ESP8266.read();
    Serial.write(a);
  }
}*/


void cwJoinAP()
{
  String cmd ="AT+CIPSTA=\"" + IpAddress + "\"";
      ESP8266.println(cmd);
      delay(1000);

      
   cmd="AT+CWJAP=\"" + SSID + "\",\"" + PASSWORD + "\"";
  Serial.println("= Connect to =");
  Serial.println(cmd);
  ESP8266.println(cmd);
  delay(5000);
  //return true;
}

//show IP address on Serial Monitor
void cwGetIP()
{
  delay(1000);
  //clearESP8266SerialBuffer();
  ESP8266.println("AT+CIFSR");
  Serial.println("= GET IP:  AT+CIFSR =");
  delay(1000);
  
  while (ESP8266.available() > 0) {
    char a = ESP8266.read();
    Serial.write(a);
  }
  delay(500);
}

void clearESP8266SerialBuffer()
{
  Serial.println("= clearESP8266SerialBuffer() =");
      while (ESP8266.available() > 0) {
          char a = ESP8266.read();
           Serial.write(a);
        }
      Serial.println("= Clear Buffer End =");
      delay(1000);
}
boolean waitfromESP8266(int timeout, int type)
{
  do{
    Serial.println("= wait .. ");
    delay(100);
    switch (type)
    {
      case OK:
      {
        if(ESP8266.find((char*)"OK"))
        {
          return true;
          counter ++;
          countersend ++;
        }
        break;
      }
      case SEND_OK:
      {
        if(ESP8266.find((char*)"SEND OK"))
        {
          return true;
        }
        break;
      }
      case READY:
      {
        if(ESP8266.find((char*)"ready"))
        {
          return true;
        }
        break;
      }
    }
    //delay(50);
  }while((timeout--)>0);


  counter ++;
  counterfail ++;
  return false;
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

