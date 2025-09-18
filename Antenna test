#include <SPI.h>
#include <LoRa.h>

#define SS 5      // Chip Select (NSS)
#define RST 14    // Reset
#define DIO0 2    // DIO0 (Interrupt pin)

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa Receiver Init Test...");

  // Setup pins
  LoRa.setPins(SS, RST, DIO0);

  // Try to initialize with your frequency (example: 868E6, 915E6, or 433E6)
  if (!LoRa.begin(915E6)) {   // change to your frequency band
    Serial.println("LoRa init failed. Check connections.");
    while (true); // stop here
  }

  Serial.println("LoRa init succeeded!");
}

void loop() {
  // Do nothing, just idle
}
