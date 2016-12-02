#ifndef COMMUNICATON_H
#define COMMUNICATON_H

enum WaitFor { OK ,SEND_OK, READY };

#define ESP8266 Serial3
boolean FAIL_8266 = false;

int CH_PD_8266 = 53;

String SSID = "PES";
String PASSWORD = "arduino_pes";
String IpAddress = "192.168.178.3";

#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT  80
#define AIO_KEY "f27fa137d3934140a44974f37003de00" //Klemens


bool startESP8266Connenction ();
void cwJoinAP();
void cwGetIP();
void clearESP8266SerialBuffer();
bool startESP8266Connenction ();
bool waitfromESP8266(int timeout, int type);
bool sendValue (String feed, int usvalue, String feed2,int temp, String feed3,int hum, String feed4, int zustand);

bool startESP8266Connenction ()
{
	pinMode(CH_PD_8266, OUTPUT);
	digitalWrite(CH_PD_8266, HIGH);
	delay(1000);

    pinMode(trigPin, OUTPUT);
    digitalWrite(trigPin, LOW);

    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
	
	Serial.println("- Start ESP -");
	ESP8266.begin(74880);
	Serial.println("- Restart ESP -");
	ESP8266.println("AT+RST");
    delay(1000);

    ESP8266.end();
    ESP8266.begin(115200);
	
	
	  
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

	
}


bool waitfromESP8266(int timeout, int type)
{
	int counter =0;
	int counterfail =0;
    int countersend =0;
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


bool sendValue (String feed, int usvalue, String feed2,int temp, String feed3,int hum, String feed4, int zustand)
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
  headerGET += "&";
  headerGET += feed4;
  headerGET += "=";
  headerGET += zustand;
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


void showESPLog() {
  while (ESP8266.available() > 0) {
    char a = ESP8266.read();
    Serial.write(a);
  }
}




#endif