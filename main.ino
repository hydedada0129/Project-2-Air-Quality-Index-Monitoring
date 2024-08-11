#include "MQ135.h"
#include <ESP8266WiFi.h>

String apiKey = "";  //this key from ThingSpeak "Write API key"
WiFiClient client;

int sensorValue;
int digitalValue;

const char* ssid = "";
const char* password = "";
const char* server = "api.thingspeak.com";

void setup() {
  Serial.begin(115200); //sets the setial port(serial monitor) to 115200
  pinMode(0, INPUT);  //pin 0(A0) as an input

    // Connect to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("NodeMCU is trying...still attempt connecting...\n");  //if does not establish connection, will loop
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); //will see nodeMCU IP address
}

void loop() {
  MQ135 gasSensor = MQ135(A0);  //create an instance of MQ135 class
  float air_quality = gasSensor.getPPM(); //The ppm of CO2 in the air
  Serial.print("Air quality: ");
  Serial.print(air_quality);
  Serial.print(" PPM");
  Serial.println();

  if (client.connect(server, 80))   //defaut port 80 for HTTP
  {

    String postStr = apiKey;
    postStr += "&field1=";    //concatenate the string &field1= to the postStr
    postStr += String(air_quality); //concatenate PPM
    postStr += "r\n";

    //WiFiclient.print() is used to send data over a network connection
    client.print("POST /update HTTP/1.1\n");      //where data should be sent
    client.print("Host: api.thingspeak.com\n");   //host header
    client.print("Connection: close\n");          //close the connection after completing the request
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");   //to authenticate the request. apiKey : "Write API key" from ThingSpeak 
    client.print("Content-Type: application/x-www-form-urlencoded\n");  //MIME type used to submitting from data in HTTP requests  
    client.print("Content-Length: "); //the length of the request body in bytes
    client.print(postStr.length());
    client.print("\n\n");
    Serial.println(postStr);        //send actual data to ThingSpeak

    Serial.println("Data send to ThingSpeak");
  }
  client.stop();
  Serial.println("Waiting...");

  delay(600000);     //For free ThingSpeak accounts, 
                    //the minimum time interval between updates 
                    //(writes to a channel) is 15 seconds. 

}

