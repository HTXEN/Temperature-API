//All the imported stuff// If something doesn't work, it's probably because you don't have it installed
//We use diss libraries
#include <DFRobot_DHT11.h> //https://github.com/DFRobot/DFRobot_DHT11/tree/master
DFRobot_DHT11 DHT; // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include <ESP8266WiFi.h> // Standard in esp8266 library
#include <ArduinoJson.h> //https://arduinojson.org/?utm_source=meta&utm_medium=library.properties
#include <ESP8266HTTPClient.h>// Standard in esp8266 library

const char* ssid = ""; //Your wifi ssid
const char* password = ""; //Your wifi password
const char* apiKey = ""; // Your apikey // docs.htxen.dk
const char* apiEndpoint = "https://api.htxen.dk/v1/sensor/temperature/upload"; // THE API ENDPOINT FOR UPLOAD
int reqtime = 300000; //We highly recommend every 5 minutes // REMEMBER IT IN MILLISECONDS THAT MEANS 1 MINUTE IS 60000 MILLISECONDS
int sensorid = ; // Your sensor id from dashboard
#define DHT11_PIN 16 // Define which pin the sensor's data cable is attached to

void setup() {
    // We are using 115200 because 9600 is too low :/
    Serial.begin(115200);

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
}

void loop() {
    DHT.read(DHT11_PIN);
    //Only use when debuging 
    //Serial.print("temp:");
    //Serial.print(DHT.temperature);
    //Serial.print(" humi:");
    //Serial.println(DHT.humidity);

    // Define json variable to send
    StaticJsonDocument<200> jsonDocument;
    jsonDocument["sensorid"] = sensorid;
    jsonDocument["temperature"] = DHT.temperature;
    jsonDocument["humidity"] = DHT.humidity;

    // Json to a string
    String jsonString;
    serializeJson(jsonDocument, jsonString);

    // WiFi client setup + ssl
    BearSSL::WiFiClientSecure client;

    // DONOT REMOVE THIS 
    client.setInsecure(); // THIS MAKES SSL! WITHOUT IT HTTPS WILL NOT WORK AND THE API WILL REFUSE POST!!!!!!!!!!!!!!!!!!!!
    // DONOT REMOVE THIS 

    if (client.connect("api.htxen.dk", 443)) {
        Serial.println("Connected to server");
        // Send post
        HTTPClient http;
        http.begin(client, apiEndpoint);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("ApiKey", apiKey);

        int httpResponseCode = http.POST(jsonString);

        // Check the result of the post
        if (httpResponseCode > 0) {
            //Only use when debuging 
            //Serial.print("HTTP Response code: ");
            //Serial.println(httpResponseCode);
            //Serial.println(http.getString());
        } else {
            //Only use when debuging 
            //Serial.print("HTTP Request failed, error: ");
            //Serial.println(httpResponseCode);
        }

        // Close the connection
        http.end();
    } else {
        //Only use when debuging
        //Serial.println("Connection failed");
        //Serial.println("Connection error code: " + String(client.getWriteError()));
    }

    // How long for next post, aka sleep (The time is controled by the reqtime variable) 
    delay(reqtime);
}