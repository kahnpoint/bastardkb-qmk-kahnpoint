Here are the qmk i2c docs

I2C Addressing :id=note-on-i2c-addresses

All of the addresses expected by this driver should be pushed to the upper 7 bits of the address byte. Setting the lower bit (indicating read/write) will be done by the respective functions. Almost all I2C addresses listed on datasheets and the internet will be represented as 7 bits occupying the lower 7 bits and will need to be shifted to the left (more significant) by one bit. This is easy to do via the bitwise shift operator << 1.

You can either do this on each call to the functions below, or once in your definition of the address. For example, if your device has an address of 0x18:

#define MY_I2C_ADDRESS (0x18 << 1)

## API :id=api

### `void i2c_init(void)` :id=api-i2c-init

Initialize the I2C driver. This function must be called only once, before any of the below functions can be called.

This function is weakly defined, meaning it can be overridden if necessary for your particular use case:

```c
void i2c_init(void) {
    setPinInput(B6); // Try releasing special pins for a short time
    setPinInput(B7);
    wait_ms(10); // Wait for the release to happen

    palSetPadMode(GPIOB, 6, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_PUPDR_PULLUP); // Set B6 to I2C function
    palSetPadMode(GPIOB, 7, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_PUPDR_PULLUP); // Set B7 to I2C function
}
```

---

### `i2c_status_t i2c_start(uint8_t address, uint16_t timeout)` :id=api-i2c-start

Start an I2C transaction.

#### Arguments :id=api-i2c-start-arguments

 - `uint8_t address`  
   The 7-bit I2C address of the device (ie. without the read/write bit - this will be set automatically).
 - `uint16_t timeout`  
   The time in milliseconds to wait for a response from the target device.

#### Return Value :id=api-i2c-start-return

`I2C_STATUS_TIMEOUT` if the timeout period elapses, `I2C_STATUS_ERROR` if some other error occurs, otherwise `I2C_STATUS_SUCCESS`.

---

### `i2c_status_t i2c_transmit(uint8_t address, uint8_t *data, uint16_t length, uint16_t timeout)` :id=api-i2c-transmit

Send multiple bytes to the selected I2C device.

#### Arguments :id=api-i2c-transmit-arguments

 - `uint8_t address`  
   The 7-bit I2C address of the device.
 - `uint8_t *data`  
   A pointer to the data to transmit.
 - `uint16_t length`  
 The number of bytes to write. Take care not to overrun the length of `data`.
 - `uint16_t timeout`  
   The time in milliseconds to wait for a response from the target device.

#### Return Value :id=api-i2c-transmit-return

`I2C_STATUS_TIMEOUT` if the timeout period elapses, `I2C_STATUS_ERROR` if some other error occurs, otherwise `I2C_STATUS_SUCCESS`.

---

### `i2c_status_t i2c_receive(uint8_t address, uint8_t* data, uint16_t length, uint16_t timeout)` :id=api-i2c-receive

Receive multiple bytes from the selected I2C device.

#### Arguments :id=api-i2c-receive-arguments

 - `uint8_t address`  
   The 7-bit I2C address of the device.
 - `uint8_t *data`  
   A pointer to the buffer to read into.
 - `uint16_t length`  
 The number of bytes to read. Take care not to overrun the length of `data`.
 - `uint16_t timeout`  
   The time in milliseconds to wait for a response from the target device.

#### Return Value :id=api-i2c-receive-return

`I2C_STATUS_TIMEOUT` if the timeout period elapses, `I2C_STATUS_ERROR` if some other error occurs, otherwise `I2C_STATUS_SUCCESS`.

---

### `i2c_status_t i2c_writeReg(uint8_t devaddr, uint8_t regaddr, uint8_t* data, uint16_t length, uint16_t timeout)` :id=api-i2c-writereg

Writes to a register with an 8-bit address on the I2C device.

#### Arguments :id=api-i2c-writereg-arguments

 - `uint8_t devaddr`  
   The 7-bit I2C address of the device.
 - `uint8_t regaddr`  
   The register address to write to.
 - `uint8_t *data`  
   A pointer to the data to transmit.
 - `uint16_t length`  
 The number of bytes to write. Take care not to overrun the length of `data`.
 - `uint16_t timeout`  
   The time in milliseconds to wait for a response from the target device.

#### Return Value :id=api-i2c-writereg-return

`I2C_STATUS_TIMEOUT` if the timeout period elapses, `I2C_STATUS_ERROR` if some other error occurs, otherwise `I2C_STATUS_SUCCESS`.

---

### `i2c_status_t i2c_readReg(uint8_t devaddr, uint8_t regaddr, uint8_t* data, uint16_t length, uint16_t timeout)` :id=api-i2c-readreg

Reads from a register with an 8-bit address on the I2C device.

#### Arguments :id=api-i2c-readreg-arguments

 - `uint8_t devaddr`  
   The 7-bit I2C address of the device.
 - `uint8_t regaddr`  
   The register address to read from.
 - `uint16_t length`  
 The number of bytes to read. Take care not to overrun the length of `data`.
 - `uint16_t timeout`  
   The time in milliseconds to wait for a response from the target device.

#### Return Value :id=api-i2c-readreg-return

`I2C_STATUS_TIMEOUT` if the timeout period elapses, `I2C_STATUS_ERROR` if some other error occurs, otherwise `I2C_STATUS_SUCCESS`.
=
---

### `i2c_status_t i2c_stop(void)` :id=api-i2c-stop

Stop the current I2C transaction.



and here is the arduino library i am trying to port to qmk:



#include <inttypes.h>
#include "Arduino.h"
#include "Wire.h"
#include "T841Defs.h"
#include "ATtiny841Lib.h"

void ATtiny841Lib::writeByte(uint8_t b1){
  Wire.beginTransmission(address);
  Wire.write(b1);
  Wire.endTransmission();
}
  
void ATtiny841Lib::writeByte(uint8_t b1,uint8_t b2){
  Wire.beginTransmission(address);
  Wire.write(b1);
  Wire.write(b2);
  Wire.endTransmission();
}

void ATtiny841Lib::writeByte(uint8_t b1,uint8_t b2,uint8_t b3){
  Wire.beginTransmission(address);
  Wire.write(b1);
  Wire.write(b2);
  Wire.write(b3);
  Wire.endTransmission();
}  

void ATtiny841Lib::writeCommand(uint8_t cmd, uint16_t val){
  int MSB=val>>8;
  Wire.beginTransmission(address);
  Wire.write(cmd);
  Wire.write(val);
  Wire.write(MSB);
  Wire.endTransmission();
  
}

void ATtiny841Lib::writeCommand(uint8_t cmd, uint16_t val1, uint16_t val2){
  int MSB=val1>>8;
  Wire.beginTransmission(address);
  Wire.write(cmd);
  Wire.write(val1);
  Wire.write(MSB);
  MSB=val2>>8;
  Wire.write(val2);
  Wire.write(MSB);
  Wire.endTransmission();
  
}

void ATtiny841Lib::writeCommand(uint8_t cmd, uint16_t val1, uint16_t val2, uint16_t val3, uint16_t val4){
  int MSB=val1>>8;
  Wire.beginTransmission(address);
  Wire.write(cmd);
  Wire.write(val1);
  Wire.write(MSB);
  MSB=val2>>8;
  Wire.write(val2);
  Wire.write(MSB);
  MSB=val3>>8;
  Wire.write(val3);
  Wire.write(MSB);
  MSB=val4>>8;
  Wire.write(val4);
  Wire.write(MSB);
  Wire.endTransmission();
}


uint8_t ATtiny841Lib::read(uint8_t reg){
  writeByte(reg);
  Wire.requestFrom(address,(uint8_t)1);
  return Wire.read();
}



how can i accomplish the following function (checking the firmware version) in qmk?


uint8_t CapTouchWireling::begin(void)
{
  writeByte(COMMAND_SET_MODE,MODE_REGISTER_DEC);//write to the T841 registers directly
  if(read(FIRMWARE_REVISION_REG)!=EXPECTED_CAPTOUCHWIRELING_FIRMWARE)
    return 1;
  
  writeByte(T841_ADCSRA, _BV(T841_ADEN) | 4 | 1);
  writeByte(T841_DIDR0, 0xAF);
  writeByte(T841_PRR,/* _BV(T841_PRADC) | */_BV(T841_PRSPI) | _BV(T841_PRUSART0) | _BV(T841_PRUSART1));
  
  writeByte(COMMAND_SET_MODE, MODE_COMMAND);//send interpreted commands- see header file
  writeByte(COMMAND_CLOCK_PRESCALER, T841_CLOCK_PRESCALER_1);
  
}

