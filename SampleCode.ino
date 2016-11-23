/*
Arduino Due - ESP 8266 WiFi Module
Serial (Tx/Rx) communicate to PC via USB
Serial3 (Tx3/Rx3) connect to ESP8266
Tx3 - ESP8266 Rx
Rx3 - ESP8266 Tx
pin 53 - ESP8266 CH_PD (Always High)
*/


#define ESP8266 Serial3
String SSID = "testJulia1234";
String PASSWORD = "test1234";


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
    Serial.println("--- Start ---");

    ESP8266.println("AT+RST");
    delay(1000);

    ESP8266.end();
    ESP8266.begin(115200);
    Serial.println("--- Restart ---");
    
    Serial.println("Module is ready\n AT+CWMODE=1");
    ESP8266.println("AT+CWMODE=1");
    ESP8266.println("AT+CWMODE?");

    delay(2000);
      
    //Quit existing AP, for demo
      ESP8266.println("AT+CWQAP");
      Serial.println("=======Disconnect from AP: AT+CWQAP =========");
      delay(1000);

     
      clearESP8266SerialBuffer();
      delay(1000);


      Serial.println("======= AT+CIPMUX=1 =========");
      ESP8266.println("AT+CIPMUX=1");
      delay(1000);
      
      cwJoinAP();
      delay(1000);
      cwGetIP();
      delay(1000);

      // connect to google
      //String cmd= "AT+CIPSTART=4,\"TCP\",\"" + SERVER +"\"" + PORT;
      delay(1000);
      String cmd= "AT+CIPSTART=4,\"TCP\",\"www.google.com\",80";
      Serial.println("\n======= "+cmd);
      ESP8266.println(cmd);

      delay(10000);
      
      
      // Test Nachricht
      Serial.println("======= startString,");
      //String headerGET = "GET /api/feeds?x-aio-key=129f2e69ceed4cc0a71012e5cd1e9202 HTTP/1.1\n"; // FeedListe
      String headerGET = "GET /api/feeds/DHT11?x-aio-key=129f2e69ceed4cc0a71012e5cd1e9202 HTTP/1.1\n"; // DHT Wert abfragen
    
    headerGET  += "Host: io.adafruit.com\n";
    headerGET  += "Connection: close\n";
    headerGET  += "Accept-Charset: ISO-8859-1,UTF-8;q=0.7,*;q=0.7\n ";
    headerGET  += "Cache-Control: no-cache \n ";
    headerGET  += "Accept-Language: de,en;q=0.7,en-us;q=0.3\n";
    headerGET  += "Referer: http://web-sniffer.net/\n\n";
      
      int headerGetLen = headerGET.length();
      Serial.println("======= AT+CIPSEND=4,"+headerGetLen);
      //ESP8266.println("AT+CIPSEND=4,"+headerGetLen);
      //ESP8266.println(headerGET);

    String headerPOST ="POST /api/feeds/welcome-feed/data/send HTTP/1.1\n";
    headerPOST  += "Host: io.adafruit.com\n";
    headerPOST  += "Connection: close\n";
    headerPOST  += "Accept-Charset: ISO-8859-1,UTF-8;q=0.7,*;q=0.7\n ";
    headerPOST  += "Cache-Control: no-cache \n ";
    headerPOST  += "Accept-Language: de,en;q=0.7,en-us;q=0.3\n";
    headerPOST  += "Referer: http://web-sniffer.net/\n";
    headerPOST  += "Content-type: application/x-www-form-urlencoded\n";

    String msgPOST = "x-aio-key=129f2e69ceed4cc0a71012e5cd1e9202\n";
    msgPOST += " {\"feedID\": \"welcome-feed\", \"data\": {\"value\": 586}}";

     int msgPostLen = msgPOST.length();
    headerPOST  += ("Content-length: %d\n\n",msgPostLen);
    
    int headerMsgPostLen = msgPostLen + headerPOST.length();
    Serial.println("======= AT+CIPSEND=4,"+headerMsgPostLen);
      //ESP8266.println("AT+CIPSEND=4,"+headerMsgPostLen);
      //ESP8266.println(headerPOST);
  //ESP8266.println(msgPOST);
  //}while(FAIL_8266);
 
}

void loop() {
    while (Serial.available() > 0) {
    char a = Serial.read();
    Serial3.write(a);
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
  String cmd="AT+CWJAP=\"" + SSID + "\",\"" + PASSWORD + "\"";
  Serial.println("============ Connect to =========");
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
  Serial.println("======= GET IP:  AT+CIFSR ========");
  delay(1000);
  
  while (ESP8266.available() > 0) {
    char a = ESP8266.read();
    Serial.write(a);
  }
}

void clearESP8266SerialBuffer()
{
  Serial.println("= clearESP8266SerialBuffer() =");
      while (ESP8266.available() > 0) {
          char a = ESP8266.read();
           Serial.write(a);
        }
      Serial.println("======== Clear Buffer End ==============");
      delay(1000);
}
