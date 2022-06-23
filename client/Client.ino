#include "WiFiEsp.h"
#include <MsTimer2.h>
#include <DFRobot_DHT11.h>

DFRobot_DHT11 DHT;
#define DHT11_PIN 8
#define LED_PIN 2
#define FLAME_PIN A5
#define CDS_PIN A0

int cdsValue;
int flameValue;
boolean dht11_flag = true;
char msg[15];

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
#endif

char ssid[] = "iot24kcci";            // your network SSID (name)
char pass[] = "iot40000";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

char server[] = "10.10.141.43";

// Initialize the Ethernet client object
WiFiEspClient client;

/************** setup() *******************/

void setup()
{
  // initialize serial for debugging
  Serial.begin(9600);
  // initialize serial for ESP module
  Serial1.begin(9600);
  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  // you're connected now, so print out the data
  Serial.println("You're connected to the network");
  
  printWifiStatus();

  Serial.println();
  Serial.println("Starting connection to server...");
  // if you get a connection, report back via serial
  if (client.connect(server, 5000)) {
    Serial.println("Connected to server");
  }
  MsTimer2::set(1000, test);
  MsTimer2::start();

  pinMode(LED_PIN, OUTPUT);
  pinMode(FLAME_PIN, INPUT);
  pinMode(CDS_PIN, INPUT);
}
void loop()
{
  // if there are incoming bytes available
  // from the server, read them and print them

  cdsValue = analogRead(CDS_PIN)-250;
  flameValue = digitalRead(FLAME_PIN);
  digitalWrite(LED_PIN, HIGH);

  if(flameValue){
    digitalWrite(LED_PIN, HIGH);
    //flame_state = 1;
  }
  else{
    digitalWrite(LED_PIN, LOW);
    //flame_state = 0;
  }
  
  if(dht11_flag){
    char temp, humi;
    DHT.read(DHT11_PIN);
    temp = DHT.temperature;
    humi = DHT.humidity;
    
    sprintf(msg, "%d:%d:%d:%dL", humi, temp, flameValue, cdsValue);
    Serial.println(msg);
    client.print(msg);
  }
  dht11_flag = false;




  
  /*
  char sensing[10] = {0};
  int index = 0;
  client.print("hello");
  while (client.available()) {
   sensing[index++] = client.read();    
  }
  sensing[index] = '\0';
  Serial.println(sensing);
  delay(100);
  */
  // if the server's disconnected, stop the client
  if (!client.connected()) {
    Serial.println();
    Serial.println("Disconnecting from server...");
    client.stop();

    // do nothing forevermore
    while (true);
  }
}

void test(){
  dht11_flag = !dht11_flag;
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
