/*
  Repository library
  https://github.com/dancol90/ESP8266Ping
  https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFi.h
  https://github.com/bblanchon/ArduinoJson
*/

#include <ESP8266Ping.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// Variable dan value untuk ssid dan password WiFi
const char* ssid = "One More Shot";
const char* password = "Di9i7alNa7i*ns#";

// Define variable ip dan url www.reqres.in untuk check ping
// IPAddress ip (192, 168, 110, 1); 
const char* serverName = "www.reqres.in";

// Membuat instance WiFiClientSecure
WiFiClientSecure client;

// Variabel untuk menyimpan waktu terakhir request
unsigned long previousMillis = 0;
// Interval waktu untuk jeda antara request dalam milidetik
const long interval = 10000;

void setup() {
  Serial.begin(115200);

  connection();

  // Set waktu untuk validasi sertifikat SSL
  client.setInsecure(); // Menggunakan ini untuk melewati verifikasi sertifikat SSL (jika tidak diperlukan sertifikat yang valid)
}

void loop() {
  // Serial.println(Ping.averageTime()); // averageTime Ping

  // Mendapatkan waktu saat ini
  unsigned long currentMillis = millis();

  // Mengecek apakah interval waktu telah berlalu
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // Memperbarui waktu terakhir request

    // getHttp();
    postHttp();
  }
}

// Method for Connection WiFi
void connection() {
  WiFi.begin(ssid, password);

  // Checking connection WiFi
  Serial.println("Connection to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("WiFi Connected!");
  Serial.println(WiFi.SSID());
  Serial.println(WiFi.RSSI());
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.gatewayIP());
  Serial.println(WiFi.dnsIP());

  bool ret = Ping.ping(serverName);

  if (ret) {
    Serial.println("Ping success\n");
  }
}


// Method POST HTTP
const char* apiRegister = "https://66d6c87a006bfbe2e64e9265.mockapi.io/api/v1/data";
void postHttp() {
  JsonDocument doc;
  String jsonParams;

  // create an object
  JsonObject buff = doc.to<JsonObject>();
  buff["suhu"]=random(20, 35);

  // serializeJson(doc, Serial); // output on the serial monitor
  // serialize the object and send the result to jsonParams
  serializeJson(doc, jsonParams);

   // Pastikan telah terhubung ke Wi-Fi
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(client, apiRegister);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST(jsonParams); // POST Request

    if (httpCode > 0) {
      Serial.printf("HTTP POST... code: %d\n", httpCode);
      // Jika request create berhasil (kode HTTP 201)
      if (httpCode == HTTP_CODE_CREATED) {
        String response = http.getString();
        Serial.print("Response: ");
        Serial.println(response); // Menampilkan respon API
      }
    } else {
      // Jika request gagal
      Serial.printf("POST request failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end(); // Akhiri koneksi HTTP
  }  else {
    Serial.println("Wi-Fi not connected");
  }
}

// Method GET HTTP DATA
const char* apiDetailUser = "https://66d6c87a006bfbe2e64e9265.mockapi.io/api/v1/data/1";
void getHttp() {
  // Pastikan telah terhubung ke Wi-Fi
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(client, apiDetailUser);
    
    int httpCode = http.GET(); // Kirimkan GET request

    // Jika request berhasil (kode HTTP 200)
    if (httpCode > 0) {
        Serial.printf("HTTP GET... code: %d\n", httpCode);
        // Ambil respon dari server
        if (httpCode == HTTP_CODE_OK) {
          String response = http.getString();
          // Serial.print("Response: ");
          // Serial.println(response); // Menampilkan respon API

          /*
            GAMBARAN BENTUK RESPONSE SUCCESS
            {"createdAt":1725352030,"suhu":28,"id":"1"}
          */

          JsonDocument doc;
          // convert string to object
          deserializeJson(doc, response);

          // extract the data
          JsonObject object = doc.as<JsonObject>();
          String suhu = object["suhu"];
          
          Serial.print("Respnonse: ");
          Serial.println(response);
          Serial.println(suhu);
        }
      } else {
        // Jika request gagal
        Serial.printf("GET request failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

    http.end(); // Akhiri koneksi HTTP
  } else {
    Serial.println("Wi-Fi not connected");
  }
}