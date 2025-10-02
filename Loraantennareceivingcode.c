#include <SPI.h>
#include <LoRa.h>

// Pin definitions
#define SS 5
#define RST 17
#define DIO0 4
#define LED_BUILTIN 22

// Transmission parameters (must match sender)
#define MAX_PACKET_SIZE 240
#define HEADER_SIZE 6

struct ImagePacketHeader {
  uint16_t packetId;        // Packet sequence number
  uint16_t totalPackets;    // Total number of packets
  uint16_t dataSize;        // Data size in this packet
};

// Image reconstruction variables
uint8_t* imageBuffer = nullptr;
size_t totalImageSize = 0;
uint16_t expectedPackets = 0;
uint16_t receivedPackets = 0;
bool* packetReceived = nullptr;
bool receivingImage = false;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  LoRa.setPins(SS, RST, DIO0);

  while (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    delay(500);
  }
  
  // Match sender settings
  LoRa.setSyncWord(0x34);
  LoRa.setSpreadingFactor(7);      // Match sender for faster transmission
  LoRa.setSignalBandwidth(250E3);  // Match sender
  LoRa.setCodingRate4(5);          // Match sender
  LoRa.enableCrc();

  Serial.println("LoRa Image Receiver Ready");
}

void loop() {
  if (LoRa.parsePacket()) {
    digitalWrite(LED_BUILTIN, HIGH);
    
    String packet = "";
    
    // Check if it's a text packet (start/end markers)
    if (LoRa.peek() < 0x80) { // Likely text packet
      while (LoRa.available()) {
        packet += (char)LoRa.read();
      }
      handleTextPacket(packet);
    } else {
      // Binary data packet
      handleDataPacket();
    }
    
    Serial.printf("RSSI: %d dBm\n", LoRa.packetRssi());
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
  }
}

// Handle text packets (start/end markers)
void handleTextPacket(String packet) {
  Serial.println("Received: " + packet);
  
  if (packet.startsWith("IMG_START:")) {
    // Parse: IMG_START:size:packets
    int firstColon = packet.indexOf(':', 10);
    int secondColon = packet.indexOf(':', firstColon + 1);
    
    if (firstColon > 0 && secondColon > 0) {
      totalImageSize = packet.substring(10, firstColon).toInt();
      expectedPackets = packet.substring(firstColon + 1, secondColon).toInt();
      
      Serial.print("Starting image reception: ");
      Serial.print(totalImageSize);
      Serial.print(" bytes in ");
      Serial.print(expectedPackets);
      Serial.println(" packets");
      
      prepareImageBuffer();
    }
  }
  else if (packet.startsWith("IMG_END")) {
    Serial.println("Image transmission complete!");
    processReceivedImage();
  }
  else {
    // Regular text message
    Serial.println("Text message: " + packet);
  }
}

// Handle binary data packets
void handleDataPacket() {
  if (!receivingImage) {
    Serial.println("Received data packet but not expecting image");
    return;
  }
  
  // Read header
  ImagePacketHeader header;
  if (LoRa.available() >= sizeof(header)) {
    LoRa.readBytes((uint8_t*)&header, sizeof(header));
    
    Serial.print("Packet ");
    Serial.print(header.packetId + 1);
    Serial.print("/");
    Serial.print(header.totalPackets);
    Serial.print(" (");
    Serial.print(header.dataSize);
    Serial.println(" bytes)");
    
    // Validate packet
    if (header.packetId < expectedPackets && header.dataSize <= (MAX_PACKET_SIZE - HEADER_SIZE)) {
      // Read data into buffer
      size_t offset = header.packetId * (MAX_PACKET_SIZE - HEADER_SIZE);
      
      if (offset + header.dataSize <= totalImageSize) {
        LoRa.readBytes(&imageBuffer[offset], header.dataSize);
        
        // Mark packet as received
        if (!packetReceived[header.packetId]) {
          packetReceived[header.packetId] = true;
          receivedPackets++;
          
          Serial.print("Progress: ");
          Serial.print(receivedPackets);
          Serial.print("/");
          Serial.print(expectedPackets);
          Serial.println(" packets received");
        }
        
        // Check if all packets received
        if (receivedPackets == expectedPackets) {
          Serial.println("All packets received!");
          processReceivedImage();
        }
      } else {
        Serial.println("Error: Packet data exceeds buffer size");
      }
    } else {
      Serial.println("Error: Invalid packet header");
    }
  } else {
    Serial.println("Error: Incomplete packet header");
  }
}

// Prepare buffer for image reception
void prepareImageBuffer() {
  // Clean up previous buffer
  if (imageBuffer != nullptr) {
    free(imageBuffer);
    imageBuffer = nullptr;
  }
  if (packetReceived != nullptr) {
    free(packetReceived);
    packetReceived = nullptr;
  }
  
  // Allocate new buffers
  imageBuffer = (uint8_t*)malloc(totalImageSize);
  packetReceived = (bool*)calloc(expectedPackets, sizeof(bool));
  
  if (imageBuffer == nullptr || packetReceived == nullptr) {
    Serial.println("Error: Failed to allocate memory for image buffer");
    receivingImage = false;
    return;
  }
  
  // Initialize variables
  receivedPackets = 0;
  receivingImage = true;
  
  Serial.println("Image buffer prepared");
}

// Process the complete received image
void processReceivedImage() {
  if (!receivingImage || imageBuffer == nullptr) {
    return;
  }
  
  Serial.println("=== IMAGE RECEIVED ===");
  Serial.print("Total size: ");
  Serial.print(totalImageSize);
  Serial.println(" bytes");
  
  // Print first few bytes (for verification)
  Serial.print("First 20 bytes: ");
  for (int i = 0; i < min(20, (int)totalImageSize); i++) {
    Serial.print("0x");
    if (imageBuffer[i] < 0x10) Serial.print("0");
    Serial.print(imageBuffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  // Check if it's a JPEG
  if (totalImageSize >= 2 && imageBuffer[0] == 0xFF && imageBuffer[1] == 0xD8) {
    Serial.println("Image type: JPEG");
  } else if (totalImageSize >= 4 && imageBuffer[0] == 0x89 && imageBuffer[1] == 0x50 && 
             imageBuffer[2] == 0x4E && imageBuffer[3] == 0x47) {
    Serial.println("Image type: PNG");
  } else {
    Serial.println("Image type: Unknown");
  }
  

  
  saveImageToSerial(); // Print as C++ array for debugging
  
  // Clean up
  free(imageBuffer);
  free(packetReceived);
  imageBuffer = nullptr;
  packetReceived = nullptr;
  receivingImage = false;
  
  Serial.println("=== RECEPTION COMPLETE ===\n");
}

// Save image as C++ byte array to Serial (for debugging/copying)
void saveImageToSerial() {
  Serial.println("\n// Received image as C++ byte array:");
  Serial.println("uint8_t receivedImage[] = {");
  
  for (size_t i = 0; i < totalImageSize; i++) {
    if (i % 16 == 0) {
      Serial.print("    ");
    }
    
    Serial.print("0x");
    if (imageBuffer[i] < 0x10) Serial.print("0");
    Serial.print(imageBuffer[i], HEX);
    
    if (i < totalImageSize - 1) {
      Serial.print(", ");
    }
    
    if (i % 16 == 15 || i == totalImageSize - 1) {
      Serial.println();
    }
  }
  
  Serial.println("};");
  Serial.print("// Total size: ");
  Serial.print(totalImageSize);
  Serial.println(" bytes\n");
}

// Check for missing packets
void checkMissingPackets() {
  Serial.println("Missing packets:");
  for (uint16_t i = 0; i < expectedPackets; i++) {
    if (!packetReceived[i]) {
      Serial.print(i);
      Serial.print(" ");
    }
  }
  Serial.println();
}
