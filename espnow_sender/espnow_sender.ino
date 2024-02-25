
#include <esp_now.h>
#include <WiFi.h>

// Client MAC Address
uint8_t broadcastAddress[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Packet Definition
union{
    char bytes[8];
    struct{  // The struct and bytes[] share same memory location
        uint16_t start_seq; // 2 bytes
        float tf_range;     // 4 bytes
        uint16_t end_seq;   // 2 bytes
    }unpacked;
}packet;

uint16_t default_start_seq;

esp_now_peer_info_t peerInfo;


void OnSent(const uint8_t *mac_addr, esp_now_send_status_t status) 
// Callback on Sent
{
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
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

  default_start_seq = 0x0210;

  // Register the callback  
  esp_now_register_send_cb(OnSent);
  
  // Register the peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

bool read()
{
    for (int i = 0; i < 2; i++)
    // Get first 2 bytes - start_seq - 16 bit 
    {
        Serial.readBytes(&packet.bytes[i], 1);
    }
    
    if(packet.unpacked.start_seq != default_start_seq)
    // Skips faulty data reading
    {
        return false;
    }

    for (int i = 2; i < 8; i++)
    // Read the remaining data sequence
    {
        Serial.readBytes(&packet.bytes[i], 1);
    }

    return true;
}
 
void loop() {
  if (Serial.available())
  {
    if ( read() ) 
    {
      // Send message via ESP-NOW
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &packet.bytes, sizeof(packet.bytes));
      
      delay(20); 
      
      if (result == ESP_OK) 
      {
        Serial.println("Data Sent");
      } 
      else
      {
        Serial.println("Sending Error");
      }
    }
  }
}
