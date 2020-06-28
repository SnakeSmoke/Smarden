
// Include the libraries
#include <SoftwareSerial.h>
#include "WiFiEsp.h"

#define SensorPin A0

// Declare and initialise global arrays for WiFi settings
char ssid[] = "network-name";
char pass[] = "wifi-password";

// Set radio status
int status = WL_IDLE_STATUS;

// Declare and initialise global variables/arrays for Thingspeak connection
const char server[] = "thingspeak.com";
const char thingspeakAPIKey[] = "Your thingspeakAPIKey";
//long postingInterval = 900000;
long postingInterval = 86400000;

// Declare global variable for timing
long lastConnectionTime;

// Declare and initialise data variable
long myData = 0;

// Create client object
WiFiEspClient client;

// Connect WiFi module object on GPIO pin 6 (RX) and 7 (TX)
SoftwareSerial Serial1(6, 7);

float sensorValue = 0;

void setup() {
    
    // Initialize serial for debugging
    Serial.begin(115200);
    
    // Initialize serial for ESP module
    Serial1.begin(9600);
    
    // Initialize ESP module
    WiFi.init(&Serial1);
    
    // Check for the presence of the shield
    if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield not present");
        // Don't continue
        while (true);
    }
    
    // Attempt to connect to WiFi network
    while ( status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        
        // Connect to WPA/WPA2 network
        status = WiFi.begin(ssid, pass);
    }
    
    Serial.println("You're connected to the network");
    printWifiStatus();
}

void loop() {
    
    // Get connection info in Serial monitor
    while(client.available()){
        char c = client.read();
        Serial.write(c);
    }
    
    if (millis() - lastConnectionTime > postingInterval) {
        for (int i = 0; i <= 100; i++){
            sensorValue = sensorValue + analogRead(SensorPin);
            delay(1);
        }
        sensorValue = sensorValue/100.0;
        Serial.println("sensorValue");
        Serial.println(sensorValue);
        sendThingspeak(sensorValue);
        lastConnectionTime = millis();
    }
}

void sendThingspeak(long value){
    if (client.connectSSL(server, 443)) {
        Serial.println("Connected to server");
        client.println("GET /update?api_key=" + String(thingspeakAPIKey) +
                       "&field1=" + String(value) + " HTTP/1.1");
        client.println("Host: api.thingspeak.com");
        client.println("Connection: close");
        client.println();
        Serial.println("Sent to server");
    }
}

void printWifiStatus() {
    
    // Print the SSID of the network
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    
    // Print the IP address
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
}
