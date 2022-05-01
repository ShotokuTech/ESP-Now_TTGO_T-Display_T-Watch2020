/**
   ESPNOW - Basic communication - Sender
   Date: 26th September 2017
   Author: Arvind Ravulavaru <https://github.com/arvindr21>
   Purpose: ESPNow Communication between a Sender ESP32 and a Client ESP32
   Description: This sketch consists of the code for the Sender module.
   Resources: (A bit outdated)
   a. https://espressif.com/sites/default/files/documentation/esp-now_user_guide_en.pdf
   b. http://www.esploradores.com/practica-6-conexion-esp-now/

   << This Device Sender >>

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

// Global copy of Client
esp_now_peer_info_t Client;
#define CHANNEL 1
#define PRINTSCANRESULTS 0
#define DELETEBEFOREPAIR 0

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

// Scan for Clients in AP mode
void ScanForClient() {
  int8_t scanResults = WiFi.scanNetworks();
  // reset on each scan
  bool ClientFound = 0;
  memset(&Client, 0, sizeof(Client));

  tft.println("");
  if (scanResults == 0) {
    tft.println("No WiFi devices in AP Mode found");
  } else {
    tft.print("Found "); tft.print(scanResults); tft.println(" devices ");
    for (int i = 0; i < scanResults; ++i) {
      // Print SSID and RSSI for each device found
      String SSID = WiFi.SSID(i);
      int32_t RSSI = WiFi.RSSI(i);
      String BSSIDstr = WiFi.BSSIDstr(i);

      if (PRINTSCANRESULTS) {
        tft.print(i + 1);
        tft.print(": ");
        tft.print(SSID);
        tft.print(" (");
        tft.print(RSSI);
        tft.print(")");
        tft.println("");
      }
      delay(10);
      // Check if the current device starts with `Client`
      if (SSID.indexOf("Client") == 0) {
        // SSID of interest
        tft.println("Found a Client.");
        tft.print(i + 1); tft.print(": "); tft.print(SSID); tft.print(" ["); tft.print(BSSIDstr); tft.print("]"); tft.print(" ("); tft.print(RSSI); tft.print(")"); tft.println("");
        // Get BSSID => Mac Address of the Client
        int mac[6];
        if ( 6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x",  &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5] ) ) {
          for (int ii = 0; ii < 6; ++ii ) {
            Client.peer_addr[ii] = (uint8_t) mac[ii];
          }
        }

        Client.channel = CHANNEL; // pick a channel
        Client.encrypt = 0; // no encryption

        ClientFound = 1;
        // we are planning to have only one Client in this example;
        // Hence, break after we find one, to be a bit efficient
        break;
      }
    }
  }

  if (ClientFound) {
    tft.println("Client Found, processing..");
  } else {
    tft.println("Client Not Found, trying again.");
  }

  // clean up ram
  WiFi.scanDelete();
}

// Check if the Client is already paired with the Sender.
// If not, pair the Client with Sender
bool manageClient() {
  if (Client.channel == CHANNEL) {
    if (DELETEBEFOREPAIR) {
      deletePeer();
    }

    tft.print("Client Status: ");
    // check if the peer exists
    bool exists = esp_now_is_peer_exist(Client.peer_addr);
    if ( exists) {
      // Client already paired.
      tft.println("Already Paired");
      return true;
    } else {
      // Client not paired, attempt pair
      esp_err_t addStatus = esp_now_add_peer(&Client);
      if (addStatus == ESP_OK) {
        // Pair success
        tft.println("Pair success");
        return true;
      } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
        // How did we get so far!!
        tft.println("ESPNOW Not Init");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
        tft.println("Invalid Argument");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
        tft.println("Peer list full");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
        tft.println("Out of memory");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
        tft.println("Peer Exists");
        return true;
      } else {
        tft.println("Not sure what happened");
        return false;
      }
    }
  } else {
    // No Client found to process
    tft.println("No Client found to process");
    return false;
  }
}

void deletePeer() {
  esp_err_t delStatus = esp_now_del_peer(Client.peer_addr);
  tft.print("Client Delete Status: ");
  if (delStatus == ESP_OK) {
    // Delete success
    tft.println("Success");
  } else if (delStatus == ESP_ERR_ESPNOW_NOT_INIT) {
    // How did we get so far!!
    tft.println("ESPNOW Not Init");
  } else if (delStatus == ESP_ERR_ESPNOW_ARG) {
    tft.println("Invalid Argument");
  } else if (delStatus == ESP_ERR_ESPNOW_NOT_FOUND) {
    tft.println("Peer not found.");
  } else {
    tft.println("Not sure what happened");
  }
}

uint8_t data = 0;
// send data
void sendData() {
  data++;
  const uint8_t *peer_addr = Client.peer_addr;
  tft.print("Sending: "); tft.println(data);
  esp_err_t result = esp_now_send(peer_addr, &data, sizeof(data));
  tft.print("Send Status: ");
  if (result == ESP_OK) {
    tft.println("Success");
  } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
    // How did we get so far!!
    tft.println("ESPNOW not Init.");
  } else if (result == ESP_ERR_ESPNOW_ARG) {
    tft.println("Invalid Argument");
  } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
    tft.println("Internal Error");
  } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
    tft.println("ESP_ERR_ESPNOW_NO_MEM");
  } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
    tft.println("Peer not found.");
  } else {
    tft.println("Not sure what happened");
  }
}

// callback when data is sent from Sender to Client
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  tft.print("Last Sent to: "); tft.println(macStr);
  tft.print("Last Send Status: "); tft.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
    tft.init();
    tft.setRotation(3);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(1);
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0);  
//  tft.begin(115200);
  //Set device in STA mode to begin with
  WiFi.mode(WIFI_STA);
  tft.println("ESPNow/Basic/Sender Example");
  // This is the mac address of the Sender in Station Mode
  tft.print("STA MAC: "); tft.println(WiFi.macAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
}

void loop() {
    tft.setCursor(0, 25);   
  // In the loop we scan for Client
  ScanForClient();
  // If Client is found, it would be populate in `Client` variable
  // We will check if `Client` is defined and then we proceed further
  if (Client.channel == CHANNEL) { // check if Client channel is defined
    // `Client` is defined
    // Add Client as peer if it has not been added already
    bool isPaired = manageClient();
    if (isPaired) {
      // pair success or already paired
      // Send data to device
      sendData();
    } else {
      // Client pair failed
      tft.println("Client pair failed!");
    }
  }
  else {
    // No Client found to process
  }

  // wait for 3seconds to run the logic again
  delay(3000);
}
