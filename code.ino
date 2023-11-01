
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ezTime.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <DHT_U.h>

// #define DHTTYPE DHT22   // Define the DHT sensor type as DHT22 (AM2302) or AM2321
// #define temp_min 20 //Define the ideal minimum temperature of a certain plant
// #define temp_max 30 //Define the ideal maximum temperature of a certain plant
// #define percentage_h_low 60 //Define the ideal minimum humidity of a certain plant
// #define percentage_h_high 90 //Define the idea maximum humidity of a certain plant
// #define percentage_m_min 30 //Define the ideal minimum moisture moisture of a certain plant
// #define percentage_m_max 60 //Define the ideal maximum moisture moisture of a certain plant

// can be used if additional content is added

// Sensors - DHT22 and Nails
uint8_t DHTPin = 12;         // Connect the DHT22 sensor to Pin 12 on the Huzzah
uint8_t soilPin = 0;      // Use ADC or A0 pin on the Huzzah for the soil moisture sensor
float Temperature;
float Humidity;
int Moisture = 1; // Initialize Moisture with a default value (in case web page loads before readMoisture is called)
int sensorVCC = 13; // Pin to control power to the soil moisture sensor
int blueLED = 2;  // Pin for a blue LED status indicator
DHT dht(DHTPin, DHTTYPE);   // Initialize the DHT sensor.



// Wifi and MQTT
#include "arduino_secrets.h" //import password file


const char* ssid     = SECRET_SSID; // Set the WiFi SSID from the arduino_secrets.h file
const char* password = SECRET_PASS; // Set the WiFi password from the arduino_secrets.h file
const char* mqttuser = SECRET_MQTTUSER; // Set the MQTT username from the arduino_secrets.h file
const char* mqttpass = SECRET_MQTTPASS; // Set the MQTT password from the arduino_secrets.h file


ESP8266WebServer server(80);
const char* mqtt_server = "mqtt.cetools.org"; // MQTT server address
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

// Date and time
Timezone GB;



void setup() {
  // Set up LED to be controllable via broker
  // Initialize the BUILTIN_LED pin as an output
  // Turn the LED off by making the voltage HIGH
  pinMode(BUILTIN_LED, OUTPUT);     
  digitalWrite(BUILTIN_LED, HIGH);  

 // Set up the outputs to control the soil sensor switch and the blue LED for status indicator
  pinMode(sensorVCC, OUTPUT); 
  digitalWrite(sensorVCC, LOW);
  pinMode(blueLED, OUTPUT); 
  digitalWrite(blueLED, HIGH);

 // Open a serial connection for debug information
  Serial.begin(115200);
  delay(100); //

  // start DHT sensor
  pinMode(DHTPin, INPUT);
  dht.begin();

  // Run WIFI and internet initialization functions
  startWifi();
  startWebserver();
  syncDate();

  // Connect to the MQTT server
  client.setServer(mqtt_server, 1884);
  client.setCallback(callback);

}

void loop() {
  // Handle incoming requests to the web server
  server.handleClient();

  if (minuteChanged()) {
    readMoisture();
    sendMQTT();
    Serial.println(GB.dateTime("H:i:s")); // Print the current time in UTC
  }
  
  client.loop();
}

void readMoisture(){
  
  // Power the soil moisture sensor
  digitalWrite(sensorVCC, HIGH);
  digitalWrite(blueLED, LOW);
  delay(100);
  // Read the data from the sensor
  Moisture = analogRead(soilPin);         
  digitalWrite(sensorVCC, LOW);  
  digitalWrite(blueLED, HIGH);
  delay(100);
  Serial.print("Wet ");
  Serial.println(Moisture);   // read the data from the nails
}

void startWifi() {
 // Connect to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

// Check if connected and wait until connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void syncDate() {
  waitForSync();
  Serial.println("UTC: " + UTC.dateTime());
  GB.setLocation("Europe/London");
  Serial.println("London time: " + GB.dateTime());
} // real date and time

void sendMQTT() {//MQTT server reconnect method
  Temperature = dht.readTemperature(); // Gets the temperature data
  snprintf (msg, 50, "%.1f", Temperature);  //data format
  Serial.print("Publish message for t: ");
  Serial.println(msg);
  client.publish("student/CASA0014/plant/ucfniad/temperature", msg); //data publish path

  Humidity = dht.readHumidity(); //humidity(percentage)
  snprintf (msg, 50, "%.0f", Humidity); //data format
  Serial.print("Publish message for h: ");
  Serial.println(msg);
  client.publish("student/CASA0014/plant/ucfniad/humidity", msg); //data publish path

  //Moisture = analogRead(soilPin);   // moisture read by readMoisture()
  snprintf (msg, 50, "%.0i", Moisture); //data format
  Serial.print("Publish message for m: ");
  Serial.println(msg);
  client.publish("student/CASA0014/plant/ucfniad/moisture", msg); //data publish path

  //  if (t < temp_min) {
  //     ;
  //   } else if (t >= temp_max && t <= temp_max) {
  //     ;
  //   } else {
  //     ;
  //   }
  
}

void callback(char* topic, byte* payload, unsigned int length) {
  // show the message 
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Turn on the LED if '1' was received as the first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);  // Turn the LED on
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off
  }

}

void reconnect() {
  // Loop until connected to the MQTT
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
   
    if (client.connect(clientId.c_str(), mqttuser, mqttpass)) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("student/CASA0014/plant/ucfniad/inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);//Wait 5 seconds before retrying
    }
  }
}

void startWebserver() {
  // when connected and IP address obtained start HTTP server
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");  
}

void handle_OnConnect() { //dht22
  Temperature = dht.readTemperature(); //temperature (Celcius)
  Humidity = dht.readHumidity(); //humidity(percentage)
  server.send(200, "text/html", SendHTML(Temperature, Humidity, Moisture));
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found"); //if not found, display 404 error 
}

String SendHTML(float Temperaturestat, float Humiditystat, int Moisturestat) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>ESP8266 DHT22 Report</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr += "p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div id=\"webpage\">\n";
  ptr += "<h1>ESP8266 Huzzah DHT22 Report</h1>\n";

  ptr += "<p>Temperature: ";
  ptr += (int)Temperaturestat;
  ptr += " C</p>";
  ptr += "<p>Humidity: ";
  ptr += (int)Humiditystat;
  ptr += "%</p>";
  ptr += "<p>Moisture: ";
  ptr += Moisturestat;
  ptr += "</p>";
  ptr += "<p>Sampled on: ";
  ptr += GB.dateTime("l,");
  ptr += "<br>";
  ptr += GB.dateTime("d-M-y H:i:s T");
  ptr += "</p>";

  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}