# SpiSlave Library

The Arduino SPI library doesn't include support for a SPI Slave device.  This library allows an Arduino board to be a SPI Slave.

The SPI slave device will respond to transfers by a remote SPI Master with a data value that can be set using the `setDataValue` function.

## Example

The example demonstrates SPI communications between two arduino Nanos.  One is setup as the SPI Master, the other as the SPI Slave.

The SPI Master uses the Arduino SPI library.  Since the SPI library doesn't include support for SPI Slave devices, The SPI Slave uses this SpiSlave library instead.

### Example Hardware Setup

Connect two Arduino Nanos as follows:

| Nano 1 (Master)     | Nano 2 (Slave)      |
|---------------------|---------------------|
| D10 (SS / OUTPUT)   | D10 (SS / INPUT)    |
| D11 (MOSI / OUTPUT) | D11 (MOSI / INPUT)  |
| D12 (MISO / INPUT)  | D12 (MISO / OUTPUT) |
| D13 (SCK / OUTPUT)  | D13 (SCK / INPUT)   |

Connect both Arduinos to the PC via the USB Serial port.

### Example Software Setup

Open a serial monitor for each board.

Start the master and the slave boards.

### Example Observations

The Master will periodically send a command to the slave.

The slave will respond with its current data value.

The slave will periodically update its current data value.

## Protocol Details

Protocol uses Network Byte Order (Big-Endian).

Transfers each of the 4 bytes of an unsigned long value separately, starting with MSB.

Transaction takes 1 + 4 + 1 SPI transfers from the SPI Master.

The first is the COMMAND_BYTE from the master.

The next 4 are the data bytes from the slave.

The last is the STOP_BYTE from the slave, notifying the master that the last byte was sent. (Master can check this for sanity to make sure it is not mid-value or communicating to a device using a different protocol).

| transfer # | MOSI | MISO |
|--|--------|-------|
| 1 | COMMAND_BYTE (0xA5) | - |
| 2 | 0x00 | MSB |
| 3 | 0x00 | 2nd byte |
| 4 | 0x00 | 3rd byte |
| 5 | 0x00 | LSB |
| 6 | 0x00 | STOP_BYTE (0x5A) |

### Example Sequence

| Master | Slave |
|--------|-------|
| Master sends COMMAND_BYTE (0xA5) | |
| | Slave receives COMMAND_BYTE |
| | takes snapshot of current data value and loads it into a buffer |
| Master sends empty byte (0x00) | |
| | Slave receives empty byte and responds with first buffer byte (MSB)
| Master reads MSB and stores it in a receive buffer | |
| Master sends empty byte (0x00) | |
| | Slave receives empty byte and responds with next buffer byte
| Master reads next byte and stores it in the receive buffer | |
| Master sends empty byte (0x00) | |
| | Slave receives empty byte and responds with next buffer byte
| Master reads next byte and stores it in the receive buffer | |
| Master sends empty byte (0x00) | |
| | Slave receives empty byte and responds with last buffer byte (LSB)
| Master reads next byte and stores it in the receive buffer | |
| Master sends empty byte (0x00) | |
| | Slave receives empty byte and responds with STOP_BYTE (0x5A) |
| Master reads STOP_BYTE and delivers the unsigned long value | |
| If anything other than STOP_BYTE is received during the last transfer, Master issues a warning | |
