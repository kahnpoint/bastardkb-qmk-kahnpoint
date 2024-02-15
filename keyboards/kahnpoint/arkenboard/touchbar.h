//#include "i2c_master.h"
#include <platforms/chibios/drivers/i2c_master.h>
#include "T841Defs.h"
#include "ATtiny841Lib.h"

#define EXPECTED_CAPTOUCHWIRELING_FIRMWARE 0x1A
#define TOUCHBAR_ADDRESS (0x31 << 1)
#define numSensors 6
#define DEFAULT_TIMEOUT 100

extern const uint8_t capTouchPins[numSensors];
extern bool IS_KEYBOARD_MASTER;

i2c_status_t writeByte1(uint8_t b1);
i2c_status_t writeByte2(uint8_t b1, uint8_t b2);
i2c_status_t writeByte3(uint8_t b1, uint8_t b2, uint8_t b3);
uint8_t readByte(uint8_t reg);


uint16_t capTouchRead(uint8_t pin);
uint8_t* readAllPins(void);

#define NUM_PINS 6
#define THRESHOLD 700

//#define SPLIT_TRANSACTION_IDS_KB 0x10
//#define SPLIT_TRANSACTION_IDS_USER 0x10
#define READ_ALL_PINS_TRANSACTION_ID 0x10

// Slave-side handler function for readAllPins transaction
void read_all_pins_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data);
