/*
 * Copyright 2020 Christopher Courtney <drashna@live.com> (@drashna)
 * Copyright 2021 Quentin LEBASTARD <qlebastard@gmail.com>
 * Copyright 2021 Charly Delay <charly@codesink.dev> (@0xcharly)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Publicw License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "arkenboard.h"
#include "transactions.h"
#include <string.h>
#include "i2c_master.h"
#include "ATtiny841Lib.h"
#include "mcuconf.h"
#include "i2c.h"


//#include "rgb.h"
//#include "quantum/rgblight.h"

//#include "CapTouchWireling.h"

#ifdef CONSOLE_ENABLE
#    include "print.h"
#endif // CONSOLE_ENABLE

#ifdef POINTING_DEVICE_ENABLE
#    ifndef CHARYBDIS_MINIMUM_DEFAULT_DPI
#        define CHARYBDIS_MINIMUM_DEFAULT_DPI 400
#    endif // CHARYBDIS_MINIMUM_DEFAULT_DPI

#    ifndef CHARYBDIS_DEFAULT_DPI_CONFIG_STEP
#        define CHARYBDIS_DEFAULT_DPI_CONFIG_STEP 200
#    endif // CHARYBDIS_DEFAULT_DPI_CONFIG_STEP

#    ifndef CHARYBDIS_MINIMUM_SNIPING_DPI
#        define CHARYBDIS_MINIMUM_SNIPING_DPI 200
#    endif // CHARYBDIS_MINIMUM_SNIPER_MODE_DPI

#    ifndef CHARYBDIS_SNIPING_DPI_CONFIG_STEP
#        define CHARYBDIS_SNIPING_DPI_CONFIG_STEP 100
#    endif // CHARYBDIS_SNIPING_DPI_CONFIG_STEP

// Fixed DPI for drag-scroll.
#    ifndef CHARYBDIS_DRAGSCROLL_DPI
#        define CHARYBDIS_DRAGSCROLL_DPI 100
#    endif // CHARYBDIS_DRAGSCROLL_DPI

#    ifndef CHARYBDIS_DRAGSCROLL_BUFFER_SIZE
#        define CHARYBDIS_DRAGSCROLL_BUFFER_SIZE 6
#    endif // !CHARYBDIS_DRAGSCROLL_BUFFER_SIZE

typedef union {
    uint8_t raw;
    struct {
        uint8_t pointer_default_dpi : 4; // 16 steps available.
        uint8_t pointer_sniping_dpi : 2; // 4 steps available.
        bool    is_dragscroll_enabled : 1;
        bool    is_sniping_enabled : 1;
    } __attribute__((packed));
} charybdis_config_t;

static charybdis_config_t g_charybdis_config = {0};

/**
 * \brief Set the value of `config` from EEPROM.
 *
 * Note that `is_dragscroll_enabled` and `is_sniping_enabled` are purposefully
 * ignored since we do not want to persist this state to memory.  In practice,
 * this state is always written to maximize write-performances.  Therefore, we
 * explicitly set them to `false` in this function.
 */
static void read_charybdis_config_from_eeprom(charybdis_config_t* config) {
    config->raw                   = eeconfig_read_kb() & 0xff;
    config->is_dragscroll_enabled = false;
    config->is_sniping_enabled    = false;
}

/**
 * \brief Save the value of `config` to eeprom.
 *
 * Note that all values are written verbatim, including whether drag-scroll
 * and/or sniper mode are enabled.  `read_charybdis_config_from_eeprom(…)`
 * resets these 2 values to `false` since it does not make sense to persist
 * these across reboots of the board.
 */
static void write_charybdis_config_to_eeprom(charybdis_config_t* config) {
    eeconfig_update_kb(config->raw);
}

/** \brief Return the current value of the pointer's default DPI. */
static uint16_t get_pointer_default_dpi(charybdis_config_t* config) {
    return (uint16_t)config->pointer_default_dpi * CHARYBDIS_DEFAULT_DPI_CONFIG_STEP + CHARYBDIS_MINIMUM_DEFAULT_DPI;
}

/** \brief Return the current value of the pointer's sniper-mode DPI. */
static uint16_t get_pointer_sniping_dpi(charybdis_config_t* config) {
    return (uint16_t)config->pointer_sniping_dpi * CHARYBDIS_SNIPING_DPI_CONFIG_STEP + CHARYBDIS_MINIMUM_SNIPING_DPI;
}

/** \brief Set the appropriate DPI for the input config. */
static void maybe_update_pointing_device_cpi(charybdis_config_t* config) {
    if (config->is_dragscroll_enabled) {
        pointing_device_set_cpi(CHARYBDIS_DRAGSCROLL_DPI);
    } else if (config->is_sniping_enabled) {
        pointing_device_set_cpi(get_pointer_sniping_dpi(config));
    } else {
        pointing_device_set_cpi(get_pointer_default_dpi(config));
    }
}

/**
 * \brief Update the pointer's default DPI to the next or previous step.
 *
 * Increases the DPI value if `forward` is `true`, decreases it otherwise.
 * The increment/decrement steps are equal to CHARYBDIS_DEFAULT_DPI_CONFIG_STEP.
 */
static void step_pointer_default_dpi(charybdis_config_t* config, bool forward) {
    config->pointer_default_dpi += forward ? 1 : -1;
    maybe_update_pointing_device_cpi(config);
}

/**
 * \brief Update the pointer's sniper-mode DPI to the next or previous step.
 *
 * Increases the DPI value if `forward` is `true`, decreases it otherwise.
 * The increment/decrement steps are equal to CHARYBDIS_SNIPING_DPI_CONFIG_STEP.
 */
static void step_pointer_sniping_dpi(charybdis_config_t* config, bool forward) {
    config->pointer_sniping_dpi += forward ? 1 : -1;
    maybe_update_pointing_device_cpi(config);
}

uint16_t charybdis_get_pointer_default_dpi(void) {
    return get_pointer_default_dpi(&g_charybdis_config);
}

uint16_t charybdis_get_pointer_sniping_dpi(void) {
    return get_pointer_sniping_dpi(&g_charybdis_config);
}

void charybdis_cycle_pointer_default_dpi_noeeprom(bool forward) {
    step_pointer_default_dpi(&g_charybdis_config, forward);
}

void charybdis_cycle_pointer_default_dpi(bool forward) {
    step_pointer_default_dpi(&g_charybdis_config, forward);
    write_charybdis_config_to_eeprom(&g_charybdis_config);
}

void charybdis_cycle_pointer_sniping_dpi_noeeprom(bool forward) {
    step_pointer_sniping_dpi(&g_charybdis_config, forward);
}

void charybdis_cycle_pointer_sniping_dpi(bool forward) {
    step_pointer_sniping_dpi(&g_charybdis_config, forward);
    write_charybdis_config_to_eeprom(&g_charybdis_config);
}

bool charybdis_get_pointer_sniping_enabled(void) {
    return g_charybdis_config.is_sniping_enabled;
}

void charybdis_set_pointer_sniping_enabled(bool enable) {
    g_charybdis_config.is_sniping_enabled = enable;
    maybe_update_pointing_device_cpi(&g_charybdis_config);
}

bool charybdis_get_pointer_dragscroll_enabled(void) {
    return g_charybdis_config.is_dragscroll_enabled;
}

void charybdis_set_pointer_dragscroll_enabled(bool enable) {
    g_charybdis_config.is_dragscroll_enabled = enable;
    maybe_update_pointing_device_cpi(&g_charybdis_config);
}

/**
 * \brief Augment the pointing device behavior.
 *
 * Implement drag-scroll.
 */
static void pointing_device_task_charybdis(report_mouse_t* mouse_report) {
    static int16_t scroll_buffer_x = 0;
    static int16_t scroll_buffer_y = 0;
    if (g_charybdis_config.is_dragscroll_enabled) {
#    ifdef CHARYBDIS_DRAGSCROLL_REVERSE_X
        scroll_buffer_x -= mouse_report->x;
#    else
        scroll_buffer_x += mouse_report->x;
#    endif // CHARYBDIS_DRAGSCROLL_REVERSE_X
#    ifdef CHARYBDIS_DRAGSCROLL_REVERSE_Y
        scroll_buffer_y -= mouse_report->y;
#    else
        scroll_buffer_y += mouse_report->y;
#    endif // CHARYBDIS_DRAGSCROLL_REVERSE_Y
        mouse_report->x = 0;
        mouse_report->y = 0;
        if (abs(scroll_buffer_x) > CHARYBDIS_DRAGSCROLL_BUFFER_SIZE) {
            mouse_report->h = scroll_buffer_x > 0 ? 1 : -1;
            scroll_buffer_x = 0;
        }
        if (abs(scroll_buffer_y) > CHARYBDIS_DRAGSCROLL_BUFFER_SIZE) {
            mouse_report->v = scroll_buffer_y > 0 ? 1 : -1;
            scroll_buffer_y = 0;
        }
    }
}

report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
    if (is_keyboard_master()) {
        pointing_device_task_charybdis(&mouse_report);
        mouse_report = pointing_device_task_user(mouse_report);
    }
    return mouse_report;
}

#    if defined(POINTING_DEVICE_ENABLE) && !defined(NO_CHARYBDIS_KEYCODES)
/** \brief Whether SHIFT mod is enabled. */
static bool has_shift_mod(void) {
#        ifdef NO_ACTION_ONESHOT
    return mod_config(get_mods()) & MOD_MASK_SHIFT;
#        else
    return mod_config(get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT;
#        endif // NO_ACTION_ONESHOT
}
#    endif // POINTING_DEVICE_ENABLE && !NO_CHARYBDIS_KEYCODES


/**
 * \brief Outputs the Charybdis configuration to console.
 *
 * Prints the in-memory configuration structure to console, for debugging.
 * Includes:
 *   - raw value
 *   - drag-scroll: on/off
 *   - sniping: on/off
 *   - default DPI: internal table index/actual DPI
 *   - sniping DPI: internal table index/actual DPI
 */
static void debug_charybdis_config_to_console(charybdis_config_t* config) {
#    ifdef CONSOLE_ENABLE
    dprintf("(charybdis) process_record_kb: config = {\n"
            "\traw = 0x%X,\n"
            "\t{\n"
            "\t\tis_dragscroll_enabled=%u\n"
            "\t\tis_sniping_enabled=%u\n"
            "\t\tdefault_dpi=0x%X (%u)\n"
            "\t\tsniping_dpi=0x%X (%u)\n"
            "\t}\n"
            "}\n",
            config->raw, config->is_dragscroll_enabled, config->is_sniping_enabled, config->pointer_default_dpi, get_pointer_default_dpi(config), config->pointer_sniping_dpi, get_pointer_sniping_dpi(config));
#    endif // CONSOLE_ENABLE
}

bool process_record_kb(uint16_t keycode, keyrecord_t* record) {
    if (!process_record_user(keycode, record)) {
        debug_charybdis_config_to_console(&g_charybdis_config);
        return false;
    }
#    ifdef POINTING_DEVICE_ENABLE
#        ifndef NO_CHARYBDIS_KEYCODES
    switch (keycode) {
        case POINTER_DEFAULT_DPI_FORWARD:
            if (record->event.pressed) {
                // Step backward if shifted, forward otherwise.
                charybdis_cycle_pointer_default_dpi(/* forward= */ !has_shift_mod());
            }
            break;
        case POINTER_DEFAULT_DPI_REVERSE:
            if (record->event.pressed) {
                // Step forward if shifted, backward otherwise.
                charybdis_cycle_pointer_default_dpi(/* forward= */ has_shift_mod());
            }
            break;
        case POINTER_SNIPING_DPI_FORWARD:
            if (record->event.pressed) {
                // Step backward if shifted, forward otherwise.
                charybdis_cycle_pointer_sniping_dpi(/* forward= */ !has_shift_mod());
            }
            break;
        case POINTER_SNIPING_DPI_REVERSE:
            if (record->event.pressed) {
                // Step forward if shifted, backward otherwise.
                charybdis_cycle_pointer_sniping_dpi(/* forward= */ has_shift_mod());
            }
            break;
        case SNIPING_MODE:
            charybdis_set_pointer_sniping_enabled(record->event.pressed);
            break;
        case SNIPING_MODE_TOGGLE:
            if (record->event.pressed) {
                charybdis_set_pointer_sniping_enabled(!charybdis_get_pointer_sniping_enabled());
            }
            break;
        case DRAGSCROLL_MODE:
            charybdis_set_pointer_dragscroll_enabled(record->event.pressed);
            break;
        case DRAGSCROLL_MODE_TOGGLE:
            if (record->event.pressed) {
                charybdis_set_pointer_dragscroll_enabled(!charybdis_get_pointer_dragscroll_enabled());
            }
            break;
    }
#        endif // !NO_CHARYBDIS_KEYCODES
#    endif     // POINTING_DEVICE_ENABLE
    if (IS_QK_KB(keycode) || IS_MOUSEKEY(keycode)) {
        debug_charybdis_config_to_console(&g_charybdis_config);
    }
    return true;
}

void eeconfig_init_kb(void) {
    g_charybdis_config.raw = 0;
    write_charybdis_config_to_eeprom(&g_charybdis_config);
    maybe_update_pointing_device_cpi(&g_charybdis_config);
    eeconfig_init_user();
}


/*
i2c_status_t i2c_writeReg(uint8_t devaddr, uint8_t regaddr, const uint8_t data) {
    return i2c_writeReg(devaddr, regaddr, &data, 1, DEFAULT_TIMEOUT);
}
*/

uint8_t customReadReg(uint8_t devaddr, uint8_t regaddr) {
    uint8_t data;
    i2c_readReg(devaddr, regaddr, &data, 1, DEFAULT_TIMEOUT);
    return data;
}


void matrix_init_kb(void) {
    read_charybdis_config_from_eeprom(&g_charybdis_config);


dprintf("arkenboard matrix_init_user begin\n");


matrix_init_user();

}

#    ifdef CHARYBDIS_CONFIG_SYNC
void charybdis_config_sync_handler(uint8_t initiator2target_buffer_size, const void* initiator2target_buffer, uint8_t target2initiator_buffer_size, void* target2initiator_buffer) {
    if (initiator2target_buffer_size == sizeof(g_charybdis_config)) {
        memcpy(&g_charybdis_config, initiator2target_buffer, sizeof(g_charybdis_config));
    }
}
#    endif
/*
void i2c_init_custom(void)
{
    // Release pins for a short time
    palSetGroupMode(GPIOB, I2C1_SDA_PIN, PAL_MODE_INPUT);
    palSetGroupMode(GPIOB, I2C1_SCL_PIN, PAL_MODE_INPUT);
    chThdSleepMilliseconds(10);

    // Set custom pins for I2C operation
    palSetPadMode(GPIOB, I2C1_SDA_PIN, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_PUPDR_PULLUP);
    palSetPadMode(GPIOB, I2C1_SCL_PIN, PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_PUPDR_PULLUP);
}
*/


i2c_status_t writeByte1(uint8_t b1){
    uint8_t data[] = {b1};
    i2c_start(TOUCHBAR_ADDRESS);
    i2c_status_t status = i2c_transmit(TOUCHBAR_ADDRESS, data, 1, DEFAULT_TIMEOUT);
    i2c_stop();
return status;
}

i2c_status_t writeByte2(uint8_t b1, uint8_t b2){
    uint8_t data[] = {b1, b2};
   i2c_start(TOUCHBAR_ADDRESS);
   i2c_status_t status = i2c_transmit(TOUCHBAR_ADDRESS, data, 2, DEFAULT_TIMEOUT);
   i2c_stop();
    return status;
}

i2c_status_t writeByte3(uint8_t b1, uint8_t b2, uint8_t b3){
    uint8_t data[] = {b1, b2, b3};
   i2c_start(TOUCHBAR_ADDRESS);
   i2c_status_t status = i2c_transmit(TOUCHBAR_ADDRESS, data, 3, DEFAULT_TIMEOUT);
   i2c_stop();
    return status;
}



/*

uint16_t CapTouchWireling::capTouchRead(uint8_t pin) {

  writeByte(COMMAND_CAP_TOUCH, capTouchPins[pin], 5);
  delay(1);
  writeByte(COMMAND_SET_MODE, MODE_REGISTER_DEC);
  int value = read(RETURN_VAL_REG_0);
  value += (int)read(RETURN_VAL_REG_1)<<8;
  writeByte(COMMAND_SET_MODE, MODE_COMMAND);
  return value;
}

*/

uint8_t readByte(uint8_t reg){

    writeByte1(reg);
   i2c_start(TOUCHBAR_ADDRESS);
    uint8_t data;
    i2c_receive(TOUCHBAR_ADDRESS, &data, 1, DEFAULT_TIMEOUT);
   i2c_stop();
    return data;
}


void keyboard_post_init_kb(void) {
    maybe_update_pointing_device_cpi(&g_charybdis_config);
#    ifdef CHARYBDIS_CONFIG_SYNC
    transaction_register_rpc(RPC_ID_KB_CONFIG_SYNC, charybdis_config_sync_handler);
#    endif

  debug_enable=true;
  debug_matrix=true;

   rgblight_enable_noeeprom(); // Enables RGB, without saving settings

/*
uint8_t CapTouchWireling::begin(void)
{
  writeByte(COMMAND_SET_MODE,MODE_REGISTER_DEC);//write to the T841 registers directly
  if(read(FIRMWARE_REVISION_REG)!=EXPECTED_CAPTOUCHWIRELING_FIRMWARE)
    return 1;

  writeByte(T841_ADCSRA, _BV(T841_ADEN) | 4 | 1);
  writeByte(T841_DIDR0, 0xAF);
  writeByte(T841_PRR,/ * _BV(T841_PRADC) | * /_BV(T841_PRSPI) | _BV(T841_PRUSART0) | _BV(T841_PRUSART1));

  writeByte(COMMAND_SET_MODE, MODE_COMMAND);//send interpreted commands- see header file
  writeByte(COMMAND_CLOCK_PRESCALER, T841_CLOCK_PRESCALER_1);

}
*/

// Initialize the I2C driver
    i2c_init();
//i2c_init_custom();

rgblight_sethsv_noeeprom(0,255,255); // Sets the color to red


writeByte2(COMMAND_SET_MODE,MODE_REGISTER_DEC);

//clear the register;
uint8_t buffer[1] = {0};
//try to read the register until there is no data
while (i2c_receive(TOUCHBAR_ADDRESS, buffer, 1, DEFAULT_TIMEOUT) == I2C_STATUS_SUCCESS) {
    // do nothing
}
i2c_stop();

uint8_t data=readByte(FIRMWARE_REVISION_REG);


if (data == 0 || data == 0xFF) {
   rgblight_sethsv_noeeprom(50, 255, 255); // Sets the color to yellow
}else if (data != EXPECTED_CAPTOUCHWIRELING_FIRMWARE) {
   rgblight_sethsv_noeeprom(25, 255, 255); // Sets the color to orange
//testforundefined
}else if (data == EXPECTED_CAPTOUCHWIRELING_FIRMWARE) {
   rgblight_sethsv_noeeprom(220, 255, 255); // Sets the color to blue
}




  writeByte2(T841_ADCSRA, _BV(T841_ADEN) | 4 | 1);
  writeByte2(T841_DIDR0, 0xAF);
  writeByte2(T841_PRR,/* _BV(T841_PRADC) | */_BV(T841_PRSPI) | _BV(T841_PRUSART0) | _BV(T841_PRUSART1));

  writeByte2(COMMAND_SET_MODE, MODE_COMMAND);
  writeByte2(COMMAND_CLOCK_PRESCALER, T841_CLOCK_PRESCALER_1);











/*
i2c_init();
dprintf("arkenboard i2c_init\n");
*/

/*
i2c_status_t i2c_writeReg( index, devaddr, regaddr, const *data, uint16_t length, uint16_t timeout)
*/

/*

   //rgblight_sethsv_noeeprom(50, 255, 255); // Sets the color to red


i2c_start(TOUCHBAR_ADDRESS);
uint8_t mode = MODE_REGISTER_DEC;
//i2c_writeReg(TOUCHBAR_ADDRESS,COMMAND_SET_MODE, &mode, 1, DEFAULT_TIMEOUT);
i2c_transmit(TOUCHBAR_ADDRESS, &mode, sizeof(mode), DEFAULT_TIMEOUT);
i2c_stop();
//   rgblight_sethsv_noeeprom(200, 255, 255); // Sets the color to red

//wait_ms(2000);

i2c_start(TOUCHBAR_ADDRESS);
//dataarrayof1byte
uint8_t data;
//if (customReadReg(TOUCHBAR_ADDRESS,FIRMWARE_REVISION_REG)
//i2c_readReg(TOUCHBAR_ADDRESS,FIRMWARE_REVISION_REG, &data, 1, DEFAULT_TIMEOUT);
uint8_t firmware_revision_reg = FIRMWARE_REVISION_REG;
i2c_transmit(TOUCHBAR_ADDRESS, &firmware_revision_reg, 1, DEFAULT_TIMEOUT);
i2c_receive(TOUCHBAR_ADDRESS, &data, 1, DEFAULT_TIMEOUT);
i2c_stop();

if (data != EXPECTED_CAPTOUCHWIRELING_FIRMWARE) {
    dprintf("CapTouchWireling firmware version mismatch\n");
//printactual firmware version
    dprintf("actual firmware version: %d\n", );
   rgblight_sethsv_noeeprom(200, 255, 255); // Sets the color to red
//testforundefined
}else if (data == 0 || data == 0xFF) {
    dprintf("CapTouchWireling firmware version undefined\n");
   rgblight_sethsv_noeeprom(100, 255, 255); // Sets the color to red

}else{
    dprintf("CapTouchWireling firmware version match\n");
   rgblight_sethsv_noeeprom(0, 255, 255); // Sets the color to red


}
i2c_stop();
*/
/*
i2c_writeReg(TOUCHBAR_ADDRESS,T841_ADCSRA, _BV(T841_ADEN) | 4 | 1, 1, DEFAULT_TIMEOUT);

i2c_writeReg(TOUCHBAR_ADDRESS,T841_DIDR0, 0xAF, 1, DEFAULT_TIMEOUT);

i2c_writeReg(TOUCHBAR_ADDRESS,T841_PRR,/ * _BV(T841_PRADC) | * /_BV(T841_PRSPI) | _BV(T841_PRUSART0) | _BV(T841_PRUSART1), 1, DEFAULT_TIMEOUT);

i2c_writeReg(TOUCHBAR_ADDRESS,COMMAND_SET_MODE, MODE_COMMAND, 1, DEFAULT_TIMEOUT);

i2c_writeReg(TOUCHBAR_ADDRESS,COMMAND_CLOCK_PRESCALER, T841_CLOCK_PRESCALER_1, 1, DEFAULT_TIMEOUT);
*/


    keyboard_post_init_user();
}

#    ifdef CHARYBDIS_CONFIG_SYNC
void housekeeping_task_kb(void) {
    if (is_keyboard_master()) {
        // Keep track of the last state, so that we can tell if we need to propagate to slave.
        static charybdis_config_t last_charybdis_config = {0};
        static uint32_t           last_sync             = 0;
        bool                      needs_sync            = false;

        // Check if the state values are different.
        if (memcmp(&g_charybdis_config, &last_charybdis_config, sizeof(g_charybdis_config))) {
            needs_sync = true;
            memcpy(&last_charybdis_config, &g_charybdis_config, sizeof(g_charybdis_config));
        }
        // Send to slave every 500ms regardless of state change.
        if (timer_elapsed32(last_sync) > 500) {
            needs_sync = true;
        }

        // Perform the sync if requested.
        if (needs_sync) {
            if (transaction_rpc_send(RPC_ID_KB_CONFIG_SYNC, sizeof(g_charybdis_config), &g_charybdis_config)) {
                last_sync = timer_read32();
            }
        }
    }
    // No need to invoke the user-specific callback, as it's been called
    // already.
}
#    endif // CHARYBDIS_CONFIG_SYNC
#endif     // POINTING_DEVICE_ENABLE




#if defined(KEYBOARD_bastardkb_charybdis_3x5_blackpill) || defined(KEYBOARD_bastardkb_charybdis_4x6_blackpill)
void keyboard_pre_init_kb(void) {
   // setPinInputHigh(A0);
    keyboard_pre_init_user();
}


void matrix_scan_kb(void) {
/*
    if (!readPin(A0)) {
        reset_keyboard();
    }
*/



    matrix_scan_user();
}
#endif // KEYBOARD_bastardkb_charybdis_3x5_blackpill || KEYBOARD_bastardkb_charybdis_4x6_blackpill
