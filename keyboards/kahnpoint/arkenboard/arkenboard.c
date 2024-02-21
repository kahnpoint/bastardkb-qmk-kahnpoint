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
#include "touchbar.h"
#include <string.h>

uint8_t localHalfTouched[6] = {0};
uint8_t remoteHalfTouched[6]  = {0};
bool IS_KEYBOARD_MASTER;

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

void matrix_init_kb(void) {
    read_charybdis_config_from_eeprom(&g_charybdis_config);
    matrix_init_user();
}

#    ifdef CHARYBDIS_CONFIG_SYNC
void charybdis_config_sync_handler(uint8_t initiator2target_buffer_size, const void* initiator2target_buffer, uint8_t target2initiator_buffer_size, void* target2initiator_buffer) {
    if (initiator2target_buffer_size == sizeof(g_charybdis_config)) {
        memcpy(&g_charybdis_config, initiator2target_buffer, sizeof(g_charybdis_config));
    }
}
#    endif

void keyboard_post_init_kb(void) {

    IS_KEYBOARD_MASTER = is_keyboard_master();
    maybe_update_pointing_device_cpi(&g_charybdis_config);
//#    ifdef CHARYBDIS_CONFIG_SYNC
    transaction_register_rpc(RPC_ID_KB_CONFIG_SYNC, charybdis_config_sync_handler);
//#    endif

// }


//register the transaction handler for the readAllPins transaction
transaction_register_rpc(RPC_ID_READ_ALL_PINS, read_all_pins_handler);
transaction_register_rpc(RPC_ID_SEND_ALL_PINS, send_all_pins_handler);


 //localHalfTouched={0,0,0,0,0,0};
 //remoteHalfTouched={0,0,0,0,0,0};

    // Initialize the I2C driver
i2c_init();
    //i2c_init(&i2c1Driver, I2C1_SCL_PIN, I2C1_SDA_PIN);

//rgb_matrix_enable_noeeprom();
//rgblight_enable_noeeprom();
rgb_matrix_enable_noeeprom();
//rgb_matrix_set_val(BACKLIGHT_BRIGHTNESS);
    rgb_matrix_set_color_all(RGB_RED); // Sets the color to red

//if(!IS_KEYBOARD_MASTER){

    writeByte2(COMMAND_SET_MODE,MODE_REGISTER_DEC);
    uint8_t data=readByte(FIRMWARE_REVISION_REG);

    writeByte2(T841_ADCSRA, _BV(T841_ADEN) | 4 | 1);
    writeByte2(T841_DIDR0, 0xAF);
    writeByte2(T841_PRR,/* _BV(T841_PRADC) | */_BV(T841_PRSPI) | _BV(T841_PRUSART0) | _BV(T841_PRUSART1));
    writeByte2(COMMAND_SET_MODE, MODE_COMMAND);
    writeByte2(COMMAND_CLOCK_PRESCALER, T841_CLOCK_PRESCALER_1);

    data=readByte(FIRMWARE_REVISION_REG);

    if (data == 0 || data == 0xFF) {
    rgb_matrix_set_color_all(RGB_RED);
    }else if (data != EXPECTED_CAPTOUCHWIRELING_FIRMWARE) {
    rgb_matrix_set_color_all(RGB_ORANGE);
    }else if (data == EXPECTED_CAPTOUCHWIRELING_FIRMWARE) {
    rgb_matrix_set_color_all(RGB_GREEN);
    }else{
    rgb_matrix_set_color_all(RGB_YELLOW);
    }

    keyboard_post_init_user();
}


#    ifdef CHARYBDIS_CONFIG_SYNC
void housekeeping_task_kb(void) {
    if (IS_KEYBOARD_MASTER) {
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
                //rgb_matrix_set_color_all(RGB_GREEN);
            }else{
                //rgb_matrix_set_color_all(RGB_RED);
            }
        }
    }
    // No need to invoke the user-specific callback, as it's been called
    // already.



/*

    static bool remoteHalfIsTouched = false;

    if(!IS_KEYBOARD_MASTER){
        for (int i = 0; i < NUM_PINS; i++) {
            if (local_pins[i] == 1) {
                remoteHalfIsTouched = true;
                break;
            }
            if(i==NUM_PINS-1){
                remoteHalfIsTouched = false;
            }
        }

        //works
        if(remoteHalfIsTouched){
            rgb_matrix_set_color_all(RGB_GREEN);
        }else{
            rgb_matrix_set_color_all(RGB_PURPLE);
        }
    }
*/


    if (IS_KEYBOARD_MASTER) {


        //read all local pins
        uint8_t* local_pins = readAllPins();

        //copy the local pins to localHalfTouched;
        memcpy(&localHalfTouched, local_pins, NUM_PINS * sizeof(uint8_t));

        // remote half sync

        // Keep track of the last state
        static uint8_t remote_last_sync= 0;
        bool remote_needs_sync = false;

        // recieve from slave every 100ms regardless of state change.
        if (timer_elapsed32(remote_last_sync) > 10) {
            remote_needs_sync = true;
        }

        // Perform the sync if requested.
        if (remote_needs_sync) {

            remote_last_sync = timer_read32();

            //send_all_pins_slave_to_master_t pin_struct;
            //memcpy(pin_struct.results, local_pins, NUM_PINS * sizeof(uint8_t));

            bool resultRecieved = transaction_rpc_recv(RPC_ID_READ_ALL_PINS, NUM_PINS * sizeof(uint8_t), &remoteHalfTouched);


            //this if statement is required, otherwise the compiler strips out the transaction_rpc_recv call
            if (resultRecieved) {
                //rgb_matrix_set_color_all(RGB_GREEN);
                //rgblight_sethsv( 85, 255, 64);
                dprintf("success");
            }else{
                //rgb_matrix_set_color_all(RGB_MAGENTA);
                //rgblight_sethsv(213, 255, 64);
                dprintf("fail");
            }

        //memcpy(&remoteHalfTouched, local_pins, NUM_PINS * sizeof(uint8_t)); // Use memcpy to copy the elements

         }

    }


    if (!IS_KEYBOARD_MASTER) {


        // Keep track of the last state
        static uint8_t remote_local_last_sync= 0;
        bool remote_local_needs_sync = false;

        // recieve from slave every 100ms regardless of state change.
        if (timer_elapsed32(remote_local_last_sync) > 10) {
            remote_local_needs_sync = true;
        }

if(remote_local_needs_sync){
        remote_local_last_sync=timer_read32();
        //read all local pins
        uint8_t* local_pins = readAllPins();

        //copy the local pins to localHalfTouched;
        memcpy(&localHalfTouched, local_pins, NUM_PINS * sizeof(uint8_t));

}
    }

/*

//old

    if (!IS_KEYBOARD_MASTER) {
        // Keep track of the last state
        static uint32_t           remote_last_sync             = 0;
        bool                      remote_needs_sync            = false;

        // Send to slave every 20ms regardless of state change.
        if (timer_elapsed32(remote_last_sync) > 20) {
            remote_needs_sync = true;
        }

        // Perform the sync if requested.
        if (remote_needs_sync) {

            //send_all_pins_slave_to_master_t pin_struct;
            //memcpy(&pin_struct.results, local_pins, sizeof(pin_struct.results));

            remote_last_sync = timer_read32();

            if (transaction_rpc_exec(RPC_ID_SEND_ALL_PINS, sizeof(uint8_t), &local_pins[0],sizeof(uint8_t), &local_pins[0])) {
                if(remoteHalfIsTouched){
                    rgb_matrix_set_color_all(RGB_GREEN);
                }else{
                    rgb_matrix_set_color_all(RGB_BLUE);
                }
            }else{
                if(remoteHalfIsTouched){
                    rgb_matrix_set_color_all(RGB_RED);
                }else{
                    rgb_matrix_set_color_all(RGB_MAGENTA);
                }
            }


        }

    }
*/

}
#    endif // CHARYBDIS_CONFIG_SYNC
#endif     // POINTING_DEVICE_ENABLE

#if defined(KEYBOARD_bastardkb_charybdis_3x5_blackpill) || defined(KEYBOARD_bastardkb_charybdis_4x6_blackpill)
void keyboard_pre_init_kb(void) {
    setPinInputHigh(A0);
    keyboard_pre_init_user();
}

void matrix_scan_kb(void) {
    if (!readPin(A0)) {
        reset_keyboard();
    }
    matrix_scan_user();
}
#endif // KEYBOARD_bastardkb_charybdis_3x5_blackpill || KEYBOARD_bastardkb_charybdis_4x6_blackpill

bool shutdown_kb(bool jump_to_bootloader) {
    if (!shutdown_user(jump_to_bootloader)) {
        return false;
    }
#ifdef RGBLIGHT_ENABLE
    rgblight_enable_noeeprom();
    rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
    rgblight_setrgb(RGB_RED);
#endif // RGBLIGHT_ENABLE
#ifdef RGB_MATRIX_ENABLE
    void rgb_matrix_update_pwm_buffers(void);
    rgb_matrix_set_color_all(RGB_RED);
    rgb_matrix_update_pwm_buffers();
#endif // RGB_MATRIX_ENABLE
    return true;
}
