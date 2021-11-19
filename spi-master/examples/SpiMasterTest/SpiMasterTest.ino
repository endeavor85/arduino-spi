#include <SpiMaster.h>

SpiMaster spiMaster(
  30, // chip select pin
  0  // SPI transfer delay (needed for slower clients to give them time to prepare responses between each request)
);

void setup() {
  Serial.begin(115200);

  Serial.println("***** MASTER *****");

  spiMaster.begin();
}

void loop() {
  unsigned long longValue = spiMaster.readLong();

  Serial.println(longValue, HEX);

  delay(200);
}