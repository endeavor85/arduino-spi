#include "SpiSlave.h"

// the global SpiSlave
SpiSlaveClass SpiSlave;

unsigned long SpiSlaveClass::dataValue = 0;
long SpiSlaveClass::sensorValSnapshot = 0;
byte SpiSlaveClass::txBuffer[TX_BUFFER_SIZE] = { 0, 0, 0, 0 };
unsigned short SpiSlaveClass::txBufferIndex = 0;

void SpiSlaveClass::begin() {
    // set slave output pin to OUTPUT mode
    pinMode(MISO, OUTPUT);

    // turn on SPI in slave mode
    SPCR |= _BV(SPE);

    // turn on interrupts
    SPCR |= _BV(SPIE);
}

// SPI interrupt service routine
// SPI serial transfer complete (STC)
ISR(SPI_STC_vect) {
    // read byte received from master
    // set byte to return to the master during the next SPI transfer
    SpiSlave.spiTransferISR(SPDR);
}

void SpiSlaveClass::spiTransferISR(const byte masterIn) {

    // If COMMAND_BYTE is received, this is the beginning of a new transaction.
    // Take a snapshot of the current sensor value and start responding to SPI
    // transfers with one byte of the value at a time (starting with MSB, network
    // byte order, Big-Endian).
    if(masterIn == COMMAND_BYTE) {
        // take a snapshot of the current sensor value (in case it changes during
        // the transaction)
        cli();
        sensorValSnapshot = dataValue;
        sei();

        // respond with MSB during the next transfer
        SPDR = sensorValSnapshot >> 24;
        // slaveOut = sensorValSnapshot >> 24;

        // load the txBuffer with the sensor value bytes.
        // (except for MSB, it is already primed to send)
        txBuffer[1] = sensorValSnapshot >> 16;
        txBuffer[2] = sensorValSnapshot >> 8;
        txBuffer[3] = sensorValSnapshot;

        // index of txBuffer to send next (technically it is just primed next, it
        // isn't actually sent until the transaction after next)
        txBufferIndex = 1;
    }
    // transaction not started (haven't received COMMAND_BYTE yet)
    else if(txBufferIndex == 0) {
        // send 0 until the next COMMAND_BYTE is received
        SPDR = 0;
    }
    // still mid-transaction
    else if(txBufferIndex < TX_BUFFER_SIZE) {
        // load the next byte to send
        SPDR = txBuffer[txBufferIndex];

        // move to the next byte during the next transfer
        txBufferIndex++;
    }
    // end of transaction
    else if(txBufferIndex == TX_BUFFER_SIZE) {
        // load the next byte to send
        SPDR = STOP_BYTE;

        // clear the sensor snapshot value
        sensorValSnapshot = 0;

        // clear the tx buffer
        txBuffer[1] = 0;
        txBuffer[2] = 0;
        txBuffer[3] = 0;

        // move to the next byte during the next transfer
        txBufferIndex++;
    }
}
