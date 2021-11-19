#ifndef SPIMASTER_H
#define SPIMASTER_H

#include <Arduino.h>
#include <Adafruit_SPIDevice.h>

class SpiMaster {
    public:

        SpiMaster(int8_t csPin, unsigned int spiTransferDelay = 0);

        // setup SPI master (includes underlying Arduino SPI setup)
        void begin();

        // Read a single byte value from the SPI Slave
        byte readByte();
        // Read a 2-byte short value from the SPI Slave
        unsigned short readShort();
        // Read a 4-byte long value from the SPI Slave
        unsigned long readLong();
        
        // byte value used to notify SPI slave to send its current data value
        static const byte COMMAND_BYTE = 0xA5;

        // byte value to send to SPI master after the last byte of the value is sent
        static const byte STOP_BYTE = 0x5A;

    private:

        bool doRead(byte* valueBuffer, uint8_t numBytes);

        // the chip select pin number
        int8_t csPin;

        // the number of microseconds to delay between back-to-back SPI transfers
        unsigned int spiTransferDelay;

        // underlying SPI device
        Adafruit_SPIDevice spi_dev;
};

#endif