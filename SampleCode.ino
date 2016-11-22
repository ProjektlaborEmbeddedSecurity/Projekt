/*
Arduino Due - ESP 8266 WiFi Module
Serial (Tx/Rx) communicate to PC via USB
Serial3 (Tx3/Rx3) connect to ESP8266
Tx3 - ESP8266 Rx
Rx3 - ESP8266 Tx
pin 53 - ESP8266 CH_PD (Always High)
*/
#include <string.h>

#define ESP8266 Serial3
String SSID = "xxxx";
String PASSWORD = "xxxx";
String IpAddress = "192.168.1.50"; //change to IP-Address in your subsystem!

char buf[10];

#define MAX_SERIAL_MESSAGE 9999
#define DEBUG 1 //Set to 1 to enable all kind of debugging stuff on the serial monitor

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

    WriteToESP("AT+RST",DEBUG);

    delay(1000);

    ESP8266.end();
    ESP8266.begin(115200);

    Serial.println("--- Restart ---");

    WriteToESP("AT+CWMODE=1",DEBUG);
    WriteToESP("AT+CWMODE?",DEBUG);

    delay(2000);
    //Quit existing AP, for demo
    WriteToESP("AT+CWQAP",DEBUG);
  
    Serial.println("=======Disconnect from AP:=========");
    delay(1000);

    clearESP8266SerialBuffer();
    delay(1000);
  
    WriteToESP("AT+CIPMUX=1",DEBUG);
    delay(1000);
      
    cwJoinAP();
    delay(1000);
    cwGetIP();
    delay(1000);

    // connect to google
    //String cmd= "AT+CIPSTART=4,\"TCP\",\"" + SERVER +"\"" + PORT;
    delay(1000);
  //  String cmd= "AT+CIPSTART=4,\"TCP\",\"www.google.com\",80";
 //   String cmd= "AT+CIPSTART=4,\"TCP\",\"https://io.adafruit.com/api/feeds\",1883";
      
      String cmd= "AT+CIPSTART=100,\"TCP\",\"https://io.adafruit.com/api/groups/weather/send.json?x-aio-key=KEY_INSERT&temperature=13&humidity=12&wind=45\",80";
      WriteToESP(cmd,DEBUG);
      WriteToESP("AT+CIPSEND",DEBUG);

    //String postRequest = "POST " + uri + " HTTP/1.0\r\n" + "Host: " + server + "\r\n" + "Accept: *" + "/" + "*\r\n" + "Content-Length: " + data.length() + "\r\n" + "Content-Type: application/x-www-form-urlencoded\r\n" + "\r\n" + data;



    delay(10000);
    
    /*
    // Test Nachricht
    Serial.println("======= startString,");
    String header = "GET /search?q=arduino&ie=utf-8&oe=utf-8&client=firefox-b&gfe_rd=cr&ei=EQEuWJ3xLsSo8wez9aGgCQ HTTP/1.0\n";
    header  += "Connection: close\n";
    header  += "User-Agent: Web-sniffer/1.1.0 (+http://web-sniffer.net/)\n";
    header  += "Accept-Encoding: gzip\n";
    header  += "Accept-Charset: ISO-8859-1,UTF-8;q=0.7,*;q=0.7\n ";
    header  += "Cache-Control: no-cache \n ";
    header  += "Accept-Language: de,en;q=0.7,en-us;q=0.3\n";
    header  += "Referer: http://web-sniffer.net/\n\n";
    
    int headerLen = header.length();
    Serial.println("======= AT+CIPSEND=4,"+headerLen);
    ESP8266.println("AT+CIPSEND=,"+header.length());
    ESP8266.print(header);
      */
  //}while(FAIL_8266);
 
}

void loop() {
  ReadOutToSerial();
}

void serialEvent3() {
  ReadOutToSerial();
}


void cwJoinAP()
{
  String cmd="AT+CWJAP=\"" + SSID + "\",\"" + PASSWORD + "\"";
  Serial.println("============ Connect to =========");
  WriteToESP(cmd,DEBUG);
  delay(5000);
  //return true;
}

//show IP address on Serial Monitor
void cwGetIP()
{
  delay(1000);
  //clearESP8266SerialBuffer();
  WriteToESP("AT+CIFSR",DEBUG);
  Serial.println("======= GET IP=======");
  Serial.print("Changing IP Adress to specific given Address");
  String cmd ="AT+CIPSTA=\"" + IpAddress + "\"";
  WriteToESP(cmd,DEBUG);
  delay(1000);
  ReadOutToSerial();
}

//Reads out messages to the Serial Monitor. 
void ReadOutToSerial(){
  int cnt = 0;
  while (ESP8266.available() > 0) {
    char a = ESP8266.read();
    Serial.write(a);
    cnt = cnt +1;
    if (cnt > MAX_SERIAL_MESSAGE)
      break;
    }    
}

//Writes Commands to the ESP and to the Serial Montor if in debug mode 
bool WriteToESP(String Command, int debug){
  String cmd = Command + "\r\n\"";
  ESP8266.println(cmd); 
  
  if(debug){
    Serial.print(Command);
    Serial.println("\n");
  }
}

void test(char* buf,short maxi){
    int cnt = 0;
    while (ESP8266.available() > 0) {
      char a = ESP8266.read();
      buf[cnt] = a;
      cnt = cnt + 1;
      if (cnt == maxi)
        return;
    }
}

void clearESP8266SerialBuffer()
{
  Serial.println("= clearESP8266SerialBuffer() =");
      while (ESP8266.available() > 0) {
          char a = ESP8266.read();
          if (DEBUG)
            Serial.write(a);
        }
      Serial.println("======== Clear Buffer End ==============");
      delay(1000);
}
