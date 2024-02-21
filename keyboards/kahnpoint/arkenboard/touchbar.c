#include "i2c_master.h"
//#include <platforms/chibios/drivers/i2c_master.h>
#include"T841Defs.h"
#include"ATtiny841Lib.h"
#include <string.h>
#include "touchbar.h"

const uint8_t capTouchPins[numSensors] = { 0, 1, 2, 3, 5, 7};
extern uint8_t localHalfTouched[6];
extern uint8_t remoteHalfTouched[6];

i2c_status_t writeByte1(uint8_t b1){
    uint8_t data[] = {b1};
   i2c_start( TOUCHBAR_ADDRESS);
    i2c_status_t status = i2c_transmit( TOUCHBAR_ADDRESS, data, 1, DEFAULT_TIMEOUT);
  i2c_stop();
return status;
}

i2c_status_t writeByte2(uint8_t b1, uint8_t b2){
    uint8_t data[] = {b1, b2};
   i2c_start( TOUCHBAR_ADDRESS);
   i2c_status_t status = i2c_transmit( TOUCHBAR_ADDRESS, data, 2, DEFAULT_TIMEOUT);
 i2c_stop();
    return status;
}

i2c_status_t writeByte3(uint8_t b1, uint8_t b2, uint8_t b3){
    uint8_t data[] = {b1, b2, b3};
   i2c_start( TOUCHBAR_ADDRESS);
   i2c_status_t status = i2c_transmit( TOUCHBAR_ADDRESS, data, 3, DEFAULT_TIMEOUT);
   i2c_stop();
    return status;
}




/*
uint8_t readByte(uint8_t reg){

   i2c_start( TOUCHBAR_ADDRESS);
    i2c_transmit( TOUCHBAR_ADDRESS, &reg, 1, DEFAULT_TIMEOUT);
    uint8_t data;
    i2c_receive( TOUCHBAR_ADDRESS, &data, 1, DEFAULT_TIMEOUT);
   i2c_stop();
    return data;
}
*/

uint8_t readByte(uint8_t reg) {
    uint8_t data;
    //i2c_transmit(TOUCHBAR_ADDRESS, &reg, 1, DEFAULT_TIMEOUT);
    writeByte1(reg);
i2c_start(TOUCHBAR_ADDRESS);
    i2c_receive(TOUCHBAR_ADDRESS, &data, 1, DEFAULT_TIMEOUT);
    return data;
i2c_stop();
}



uint16_t capTouchRead(uint8_t pin) {

    writeByte3(COMMAND_CAP_TOUCH, capTouchPins[pin], 1);
    //wait_ms(1);
    writeByte2(COMMAND_SET_MODE, MODE_REGISTER_DEC);
    uint16_t value = readByte(RETURN_VAL_REG_0);
    value += (uint16_t)readByte(RETURN_VAL_REG_1) << 8;
    writeByte2(COMMAND_SET_MODE, MODE_COMMAND);
    return value;

}


uint8_t* readAllPins(void) {
    static uint8_t results[NUM_PINS];
    for(uint8_t i = 0; i < NUM_PINS; i++) {
        uint16_t value = capTouchRead(i);
        results[i] = (value > THRESHOLD) ? 1 : 0;
    }
    return results;
}

/*
// Slave-side handler function for readAllPins transaction
void read_all_pins_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data) {
    // Call readAllPins function
    uint8_t* results = readAllPins();
    // Copy the results to the out_data buffer
    memcpy(out_data, results, NUM_PINS);
}
*/

// enum custom_transaction_ids {
//     RPC_ID_READ_ALL_PINS = 0x10,
//     // Add more transaction IDs here if needed
// };


/*
void charybdis_config_sync_handler(uint8_t initiator2target_buffer_size, const void* initiator2target_buffer, uint8_t target2initiator_buffer_size, void* target2initiator_buffer) {
    if (initiator2target_buffer_size == sizeof(g_charybdis_config)) {
        memcpy(&g_charybdis_config, initiator2target_buffer, sizeof(g_charybdis_config));
    }
}

*/

void read_all_pins_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data) {
    if (out_buflen == (NUM_PINS * sizeof(uint8_t))) {

    //works
    //uint8_t* pin_results = readAllPins();
    //memcpy(out_data, pin_results, NUM_PINS * sizeof(uint8_t));

    //works
//    uint8_t dummy_data[6] = {0,1,1,1,1,1};
//    memcpy(out_data, &dummy_data, NUM_PINS * sizeof(uint8_t));


    memcpy(out_data, localHalfTouched, NUM_PINS * sizeof(uint8_t));

    // does not work
    //out_data = &localHalfTouched;

    }
}

/*
void send_all_pins_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data) {

    if (in_buflen == NUM_PINS * sizeof(uint8_t)) {
        memcpy(remoteHalfTouched, in_data, NUM_PINS*sizeof(uint8_t));
    }
}
*/

void send_all_pins_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data) {

    if (in_buflen == sizeof(uint8_t)) {
        memcpy(&remoteHalfTouched[0], in_data, NUM_PINS*sizeof(uint8_t));
    }
}
