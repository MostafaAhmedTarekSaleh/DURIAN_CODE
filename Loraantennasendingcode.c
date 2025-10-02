#include <SPI.h>
#include <LoRa.h>

// Pin definitions for LoRa
#define SS 5
#define RST 17
#define DIO0 4
#define LED_BUILTIN 22

void setup() {
  Serial.begin(115200);
  
  // Initialize LoRa
  pinMode(LED_BUILTIN, OUTPUT);
  LoRa.setPins(SS, RST, DIO0);
  
  while (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    delay(500);
  }
  
  LoRa.setSyncWord(0x34);
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(250E3);
  LoRa.setCodingRate4(5);
  LoRa.enableCrc();
  
  Serial.println("LoRa initialized!");
}

void loop() {
  // Your converted image bytes go here
  uint8_t myImageBytes[] = {
    0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, // JPEG header example
    0x4A, 0x46, 0x49, 0x46, 0x01, 0x01,
    // Replace with your actual image byte array
  };
  
  size_t imageSize = sizeof(myImageBytes);
  
  Serial.print("Sending image: ");
  Serial.print(imageSize);
  Serial.println(" bytes");
  
  // Send the image bytes via LoRa
  sendByteArray(myImageBytes, imageSize);
  
  delay(30000); // Wait 30 seconds before next transmission
}

// Transmission parameters
#define MAX_PACKET_SIZE 240    // LoRa max payload (leave some margin)
#define HEADER_SIZE 6          // Packet header size

struct ImagePacketHeader {
  uint16_t packetId;        // Packet sequence number
  uint16_t totalPackets;    // Total number of packets
  uint16_t dataSize;        // Data size in this packet
};

// Your sendByteArray function with complete logic
void sendByteArray(uint8_t* data, size_t dataSize) {
  Serial.print("Sending ");
  Serial.print(dataSize);
  Serial.println(" bytes via LoRa...");
  
  // Calculate chunking parameters
  size_t payloadSize = MAX_PACKET_SIZE - HEADER_SIZE;
  uint16_t totalPackets = (dataSize + payloadSize - 1) / payloadSize;
  
  Serial.print("Splitting into ");
  Serial.print(totalPackets);
  Serial.println(" packets");
  
  // Send start marker
  sendStartMarker(dataSize, totalPackets);
  delay(100);
  
  // Send all data packets
  for (uint16_t packetId = 0; packetId < totalPackets; packetId++) {
    size_t offset = packetId * payloadSize;
    size_t remaining = dataSize - offset;
    size_t currentPacketSize = (remaining < payloadSize) ? remaining : payloadSize;
    
    sendDataPacket(packetId, totalPackets, &data[offset], currentPacketSize);
    
    Serial.print("Sent packet ");
    Serial.print(packetId + 1);
    Serial.print("/");
    Serial.println(totalPackets);
    
    delay(50); // Small delay between packets
  }
  
  // Send end marker
  sendEndMarker();
  Serial.println("Transmission complete!");
}

// Send start marker with image info
void sendStartMarker(size_t totalSize, uint16_t totalPackets) {
  digitalWrite(LED_BUILTIN, HIGH);
  
  LoRa.beginPacket();
  LoRa.print("IMG_START:");
  LoRa.print(totalSize);
  LoRa.print(":");
  LoRa.print(totalPackets);
  LoRa.endPacket();
  
  digitalWrite(LED_BUILTIN, LOW);
}

// Send individual data packet
void sendDataPacket(uint16_t packetId, uint16_t totalPackets, uint8_t* data, size_t dataSize) {
  digitalWrite(LED_BUILTIN, HIGH);
  
  LoRa.beginPacket();
  
  // Send header
  ImagePacketHeader header;
  header.packetId = packetId;
  header.totalPackets = totalPackets;
  header.dataSize = dataSize;
  
  LoRa.write((uint8_t*)&header, sizeof(header));
  
  // Send data bytes
  LoRa.write(data, dataSize);
  
  LoRa.endPacket();
  digitalWrite(LED_BUILTIN, LOW);
}

// Send end marker
void sendEndMarker() {
  digitalWrite(LED_BUILTIN, HIGH);
  
  LoRa.beginPacket();
  LoRa.print("IMG_END");
  LoRa.endPacket();
  
  digitalWrite(LED_BUILTIN, LOW);
}
