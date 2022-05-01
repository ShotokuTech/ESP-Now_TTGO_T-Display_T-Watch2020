/**
   ESPNOW - Basic communication - Client
   Date: 26th September 2017
   Author: Arvind Ravulavaru <https://github.com/arvindr21>
   Purpose: ESPNow Communication between a Sender ESP32 and a Client ESP32
   Description: This sketch consists of the code for the Client module.
   Resources: (A bit outdated)
   a. https://espressif.com/sites/default/files/documentation/esp-now_user_guide_en.pdf
   b. http://www.esploradores.com/practica-6-conexion-esp-now/

   << This Device Client >>

   Flow: Sender
   Step 1 : ESPNow Init on Sender and set it in STA mode
   Step 2 : Start scanning for Client ESP32 (we have added a prefix of `Client` to the SSID of Client for an easy setup)
   Step 3 : Once found, add Client as peer
   Step 4 : Register for send callback
   Step 5 : Start Transmitting data from Sender to Client

   Flow: Client
   Step 1 : ESPNow Init on Client
   Step 2 : Update the SSID of Client with a prefix of `Client`
   Step 3 : Set Client in AP mode
   Step 4 : Register for receive callback and wait for data
   Step 5 : Once data arrives, print it in the serial monitor

   Note: Sender and Client have been defined to easily understand the setup.
         Based on the ESPNOW API, there is no concept of Sender and Client.
         Any devices can act as Sender or salve.
*/

#include <esp_now.h>
#include <WiFi.h>
#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library

#define CHANNEL 1

// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    tft.println("ESPNow Init Success");
  }
  else {
    tft.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}

// config AP SSID
void configDeviceAP() {
  const char *SSID = "Client_1";
  bool result = WiFi.softAP(SSID, "Client_1_Password", CHANNEL, 0);
  if (!result) {
    tft.println("AP Config failed.");
  } else {
    tft.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}

void setup() {
//  tft.begin(115200);

    tft.init();
    tft.setRotation(3);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(1);
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0);
  
  tft.println("ESPNow/Basic/Client Example");
  //Set device in AP mode to begin with
  WiFi.mode(WIFI_AP);
  // configure device AP mode
  configDeviceAP();
  // This is the mac address of the Client in AP Mode
  tft.print("AP MAC: "); tft.println(WiFi.softAPmacAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);
}

// callback when data is recv from Sender
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
//    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 45);  
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  tft.println("Last Recv from: "); tft.println(macStr);
  tft.println("Last Recv Data: "); tft.print(*data); tft.println("   ");
}

void loop() {
  // Chill
}
