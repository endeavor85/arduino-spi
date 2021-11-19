#ifndef SPISLAVECLASS_H
#define SPISLAVECLASS_H

#include <Arduino.h>

/* TODO:
    Consider -
        At startup, SPI master can send special command requesting the number of bytes to expect from the spi slave.
        The spi master can use this value to dynamically set the value size and number of transfers per transactions
    Alternatively -
        the SPI slave can respond with the number of bytes needed at the beginning of every transaction.
        The spi master can then issue that many transfers for the spi slave to fully send its value
*/

// number of bytes in each data sample (e.g., long = 4, short = 2, byte = 1)
#define TX_BUFFER_SIZE 4

class SpiSlaveClass {
    public:
        static void begin();

        static void spiTransferISR(const byte masterIn);

        // updates the data value to be sent to the SPI Master
        inline static void setDataValue(unsigned long newDataValue) {
            dataValue = newDataValue;
        }

        // byte value sent by SPI master to notify SPI slave to send its current data value
        static const byte COMMAND_BYTE = 0xA5;

        // byte value to send to SPI master after the last byte of the value is sent
        static const byte STOP_BYTE = 0x5A;

    private:
        // the data value to be sent when requested by the SPI Master
        static unsigned long dataValue;
        
        // sensor value snapshot used to send to the master one byte at a time
        static unsigned long sensorValSnapshot;

        // individual bytes of the snapshot sensor value
        // (we only need to recall 3 bytes since the MSB is tx'd immediately after
        // receiving the COMMAND_BYTE, so index 0 isn't used, just kept for
        // convenience)
        static byte txBuffer[TX_BUFFER_SIZE];

        // current transfer buffer byte index
        static unsigned short txBufferIndex;
};

extern SpiSlaveClass SpiSlave;

#endif