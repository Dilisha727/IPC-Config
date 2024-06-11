#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <FirebaseESP32.h>
#include <WiFi.h>

// Define your Firebase project credentials
#define FIREBASE_HOST "https://bus-tracking-cf33e.firebaseio.com"
#define FIREBASE_AUTH "nUa6KIPngGs97qMe9jvTqlRtsz9zvc5pqLTkhJ1v" // Replace with your Firebase Database secret

// Your WiFi credentials
#define WIFI_SSID "Dilisha"
#define WIFI_PASSWORD "D19981001"

// Define the GPS and GSM module pins
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17
#define GSM_RX_PIN 26
#define GSM_TX_PIN 27

// Define your serial connections
SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN); // RX, TX
SoftwareSerial gsmSerial(GSM_RX_PIN, GSM_TX_PIN); // RX, TX

TinyGPSPlus gps;
FirebaseData firebaseData;

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600); // GPS module baud rate
  gsmSerial.begin(9600); // GSM module baud rate

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Connect to Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Read data from the GPS module
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  if (gps.location.isUpdated()) {
    float latitude = gps.location.lat();
    float longitude = gps.location.lng();

    // Print coordinates to the Serial Monitor
    Serial.print("Latitude: ");
    Serial.println(latitude, 6);
    Serial.print("Longitude: ");
    Serial.println(longitude, 6);

    // Prepare data to send to Firebase
    String path = "/bus_location";
    FirebaseJson json;
    json.set("lat", latitude);
    json.set("lng", longitude);
    json.set("timestamp", Firebase.getInt(firebaseData, "/serverTime").intValue());

    // Send data to Firebase
    if (Firebase.set(firebaseData, path, json)) {
      Serial.println("Data sent successfully");
    } else {
      Serial.println("Failed to send data");
      Serial.println(firebaseData.errorReason());
    }
  }

  delay(5000); // Delay to avoid spamming Firebase
}
