#include <SpiSlave.h>

// dummy sensor value used to send to the master
unsigned long sensorVal = 0;

void setup() {
    Serial.begin(115200);

    Serial.println("***** SLAVE *****");

    // initialize SpiSlave
    SpiSlave.begin();
}

void loop() {
    // increment dummy sensor value
    sensorVal += 0x01010101;
    
    // fix last bit after rollover
    if(sensorVal == 0x01010100) {
        sensorVal += 1;
    }
    
    SpiSlave.setDataValue(sensorVal);

    // print current sensor value
    Serial.println(sensorVal, HEX);

    delay(100);
}
