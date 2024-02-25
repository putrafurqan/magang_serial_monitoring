#include <esp_now.h>
#include <WiFi.h>

// Packet Definition
union {
    char bytes[8];
    struct {  // The struct and bytes[] share the same memory location
        uint16_t start_seq; // 2 bytes
        float tf_range;     // 4 bytes
        uint16_t end_seq;   // 2 bytes
    } unpacked;
} packet;

esp_now_peer_info_t peerInfo;

void OnRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&packet.bytes, incomingData, sizeof(packet.bytes));
 
  Serial.print("Range: ");
  Serial.print(packet.unpacked.tf_range);
  Serial.println(" cm");
}

void setup() {

  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the callback  
  esp_now_register_recv_cb(OnRecv);
  
}
 
void loop() {
 
}
