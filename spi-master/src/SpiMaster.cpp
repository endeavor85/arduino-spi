#include "SpiMaster.h"

// csPin - the chip select pin number
// transferDelay - the number of microseconds to wait between each byte transfer
//    (needed for slower clients to give them time to prepare responses)
SpiMaster::SpiMaster(int8_t csPin, unsigned int spiTransferDelay) : 
  csPin(csPin),
  spiTransferDelay(spiTransferDelay),
  spi_dev(csPin) {}

void SpiMaster::begin() {
  if (!spi_dev.begin()) {
    Serial.print("Could not initialize SPI device at chip select pin: ");
    Serial.println(csPin);
    while (1);
  }
}

// Read a single byte value from the SPI Slave
byte SpiMaster::readByte() {

  // initialize receive value
  byte byteValue = 0;

  bool success = doRead((byte*) &byteValue, sizeof(byteValue));
  // TODO: invalidate byteValue if false, perhaps establish a NaN #define for it?

  return byteValue;
}

// Read a 2-byte short value from the SPI Slave
unsigned short SpiMaster::readShort() {

  // initialize receive value
  unsigned short shortValue = 0;

  bool success = doRead((byte*) &shortValue, sizeof(shortValue));
  // TODO: invalidate shortValue if false, perhaps establish a NaN #define for it?

  return shortValue;
}

// Read a 4-byte long value from the SPI Slave
// Performs 6 SPI transfers (length of long + 2) since the slave doesn't
// respond until the second transfer, and to allow the slave to send its
// STOP_BYTE.
// Bytes are read with LSB first (little-endian).
// As bytes are received, they are shifted into the return long value
unsigned long SpiMaster::readLong() {

  // initialize receive value
  unsigned long longValue = 0;

  bool success = doRead((byte*) &longValue, sizeof(longValue));
  // TODO: invalidate longValue if false, perhaps establish a NaN #define for it?

  return longValue;
}

// Read value with of 'numBytes' length from the SPI Slave
// Performs 6 SPI transfers (length of long + 2) since the slave doesn't
// respond until the second transfer, and to allow the slave to send its
// STOP_BYTE.
// Bytes are read with LSG first (little-endian).
bool SpiMaster::doRead(byte* valueBuffer, uint8_t numBytes) {

  // Send COMMAND_BYTE
  spi_dev.write(&COMMAND_BYTE, 1);

  // delay to give the SPI slave time to buffer the response value
  delayMicroseconds(spiTransferDelay);

  for(int byteNum = 0; byteNum < numBytes; byteNum++) {
    spi_dev.read(valueBuffer + byteNum, 1);
    delayMicroseconds(spiTransferDelay);
  }

  // read last byte (should by STOP_BYTE)
  byte stopByte;
  spi_dev.read(&stopByte, 1);

  // is last received byte STOP_BYTE?
  if(stopByte != STOP_BYTE) {
    // warn about bad stop byte
    Serial.println("ERROR: invalid SPI slave data value (bad STOP_BYTE)");

    return false;
  }

  return true;
}