/*
Arduino Due - ESP 8266 WiFi Module
Serial (Tx/Rx) communicate to PC via USB
Serial3 (Tx3/Rx3) connect to ESP8266
Tx3 - ESP8266 Rx
Rx3 - ESP8266 Tx
pin 53 - ESP8266 CH_PD (Always High)
*/

enum WaitFor { OK ,SEND_OK, READY };

#define ESP8266 Serial3
String SSID = "xxx";
String PASSWORD = "xxx";
String IpAddress = "xxx"; // IP Addresse muss im eigenen Subnetz liegen


#define SERVER "www.google.com"
#define PORT 80
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883  


//always high
int CH_PD_8266 = 53;

boolean FAIL_8266 = false;

void setup() {
  pinMode(CH_PD_8266, OUTPUT);
  digitalWrite(CH_PD_8266, HIGH);
  delay(1000);

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

     sendValue("test-feed",56);
}

void loop() {
    while (Serial.available() > 0) {
    char a = Serial.read();
    Serial3.write(a);
  }
}

bool sendValue (String feed, int value)
{
  String cmd= "AT+CIPSTART=4,\"TCP\",\"io.adafruit.com\",80";
         
  Serial.println("= "+cmd);
  ESP8266.println(cmd);

  delay(500);
           
  // Test Nachricht
  Serial.println("= startString,");
  //String headerGET = "GET /api/feeds?x-aio-key=129f2e69ceed4cc0a71012e5cd1e9202 HTTP/1.1\r\n"; // FeedListe
  //String headerGET = "GET /api/feeds/twar?x-aio-key=b4ef461011dd40a5ba22e39c36d6de87 HTTP/1.1\n"; // DHT Wert abfragen

   //GET /api/groups/weather/send.json?x-aio-key=f27fa137d3934140a44974f37003de00&testfeed=1 HTTP/1.1\r\n
  String headerGET = "GET /api/groups/weather/send.json?x-aio-key=129f2e69ceed4cc0a71012e5cd1e9202&";
  headerGET += feed;
  headerGET += "=";
  headerGET += value;
  headerGET += " HTTP/1.1\r\n";
  headerGET  += "Host: io.adafruit.com\r\n\r\n";
      
  int headerGetLen = headerGET.length();
  //Serial.println(headerGET);
  // Serial.println("= AT+CIPSEND=4,"+headerGetLen);
      
  // if (waitOKfromESP8266(1000))
  ESP8266.print("AT+CIPSEND=4,");
  ESP8266.println(headerGetLen);

      
  if (waitfromESP8266(100, OK))
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


void serialEvent3() {
  while (ESP8266.available() > 0) {
    char a = ESP8266.read();
    Serial.write(a);
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
boolean waitfromESP8266(int timeout, int type)
{
  do{
    Serial.println("= wait .. ");
    delay(50);
    switch (type)
    {
      case OK:
      {
        if(ESP8266.find("OK"))
        {
          return true;
        }
        break;
      }
      case SEND_OK:
      {
        if(ESP8266.find("SEND OK"))
        {
          return true;
        }
        break;
      }
      case READY:
      {
        if(ESP8266.find("ready"))
        {
          return true;
        }
        break;
      }
    }
    //delay(50);
  }while((timeout--)>0);
  return false;
}



