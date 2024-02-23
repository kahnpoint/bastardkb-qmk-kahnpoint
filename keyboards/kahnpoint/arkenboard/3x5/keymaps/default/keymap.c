/**
 * Copyright 2021 Charly Delay <charly@codesink.dev> (@0xcharly)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
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
/*
#include QMK_KEYBOARD_H

enum charybdis_keymap_layers {
    LAYER_BASE = 0,
    LAYER_LOWER,
    LAYER_RAISE,
};

#define LOWER MO(LAYER_LOWER)
#define RAISE MO(LAYER_RAISE)

#define CTL_BSP CTL_T(KC_BSPC)
#define SFT_SPC SFT_T(KC_SPC)
#define GUI_ENT GUI_T(KC_ENT)

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_BASE] = LAYOUT(
  // ╭─────────────────────────────────────────────╮ ╭─────────────────────────────────────────────╮
          KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,       KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
          KC_A,    KC_S,    KC_D,    KC_F,    KC_G,       KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,       KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH,
  // ╰─────────────────────────────────────────────┤ ├─────────────────────────────────────────────╯
                         CTL_BSP, SFT_SPC,   LOWER,      RAISE, GUI_ENT
  //                   ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_LOWER] = LAYOUT(
  // ╭─────────────────────────────────────────────╮ ╭─────────────────────────────────────────────╮
       RGB_TOG, KC_MNXT, KC_MPLY, KC_MPRV, XXXXXXX,    KC_LBRC,    KC_7,    KC_8,    KC_9, KC_RBRC,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
       KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, XXXXXXX,    KC_PPLS,    KC_4,    KC_5,    KC_6, KC_PMNS,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
       XXXXXXX, XXXXXXX, XXXXXXX,  EE_CLR, QK_BOOT,    KC_PAST,    KC_1,    KC_2,    KC_3, KC_PSLS,
  // ╰─────────────────────────────────────────────┤ ├─────────────────────────────────────────────╯
                         XXXXXXX, XXXXXXX, _______,    XXXXXXX, _______
  //                   ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_RAISE] = LAYOUT(
  // ╭─────────────────────────────────────────────╮ ╭─────────────────────────────────────────────╮
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, KC_VOLU, KC_MUTE, KC_VOLD, XXXXXXX,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
       KC_LEFT,   KC_UP, KC_DOWN, KC_RGHT, XXXXXXX,    XXXXXXX, KC_RSFT, KC_RCTL, KC_RALT, KC_RGUI,
  // ├─────────────────────────────────────────────┤ ├─────────────────────────────────────────────┤
       KC_HOME, KC_PGUP, KC_PGDN,  KC_END, XXXXXXX,    QK_BOOT,  EE_CLR, XXXXXXX, XXXXXXX, XXXXXXX,
  // ╰─────────────────────────────────────────────┤ ├─────────────────────────────────────────────╯
                         _______, _______, XXXXXXX,    _______, XXXXXXX
  //                   ╰───────────────────────────╯ ╰──────────────────╯
  ),
};
// clang-format on
*/

/**
 * Copyright 2021 Charly Delay <charly@codesink.dev> (@0xcharly)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
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
#include QMK_KEYBOARD_H
#include "quantum.h"
//#include "transactions.h"
#include <arkenboard/touchbar.h>
#include <string.h>

bool any_key_pressed(void) {
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        matrix_row_t row_state = matrix_get_row(row);
        if (row_state) { // if any key in this row is pressed
            return true;
        }
    }
    return false;
}

#ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
#    include "timer.h"
#endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

enum charybdis_keymap_layers {
    LAYER_BASE = 0, // qwerty
    LAYER_SYMBOLS_AND_ARROWS,
    LAYER_FUNCTIONS_AND_NUMBERS, // function keys across top, number keys on middle row
    LAYER_NAVIGATION_AND_MACROS,
    LAYER_NAVIGATION_AND_MEDIA,
};

/*
// Automatically enable sniping-mode on the pointer layer.
#define CHARYBDIS_AUTO_SNIPING_ON_LAYER LAYER_POINTER

#ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
static uint16_t auto_pointer_layer_timer = 0;

#    ifndef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS
#        define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS 1000
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS

#    ifndef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD
#        define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD 8
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD
#endif     // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
*/

// #define ESC_MED LT(LAYER_MEDIA, KC_ESC)
// #define SPC_NAV LT(LAYER_SYMBOLS_AND_ARROWS, KC_SPC)
// #define TAB_FUN LT(LAYER_FUNCTION, KC_TAB)
// #define ENT_SYM LT(LAYER_NAVIGATION, KC_ENT)
// #define BSP_NUM LT(LAYER_NUMERAL, KC_BSPC)
// #define _L_PTR(KC) LT(LAYER_POINTER, KC)

#ifndef POINTING_DEVICE_ENABLE
#    define DRGSCRL KC_NO
#    define DPI_MOD KC_NO
#    define S_D_MOD KC_NO
#    define SNIPING KC_NO
#endif // !POINTING_DEVICE_ENABLE


/** Convenience row shorthands. */
#define _______________DEAD_HALF_ROW_______________ XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
#define _______________DEAD_FULL_ROW_______________ XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
#define _______________TRNS_HALF_ROW_______________ KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
#define _______________TRNS_FULL_ROW_______________ KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
#define ______________HOME_ROW_GACS_L______________ KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, XXXXXXX
#define ______________HOME_ROW_GACS_R______________ XXXXXXX, KC_LSFT, KC_LCTL, KC_LALT, KC_LGUI
#define _______________DEAD_FUNCTIONS_______________ XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
#define _______________AUDIO_CONTROLS_______________ KC_MEDIA_PLAY_PAUSE, KC_MEDIA_PREV_TRACK, KC_AUDIO_VOL_DOWN, KC_AUDIO_VOL_UP, KC_MEDIA_NEXT_TRACK

#define _______________LEFT_BRACKETS_______________ KC_LEFT_CURLY_BRACE,  KC_LEFT_PAREN,  KC_LEFT_ANGLE_BRACKET,  KC_LEFT_BRACKET
#define _______________RIGHT_BRACKETS_______________ KC_RIGHT_CURLY_BRACE,  KC_RIGHT_PAREN,  KC_RIGHT_ANGLE_BRACKET,  KC_RIGHT_BRACKET
#define _______________BRACKET_MACROS_______________ CURLY_BRACKETS_MACRO,  PARENTHESIS_BRACKETS_MACRO,  ANGLE_BRACKETS_MACRO,  SQUARE_BRACKETS_MACRO
#define _______________QUOTE_MACROS_______________ BACKTICKS_MACRO, SINGLE_QUOTES_MACRO, DOUBLE_QUOTES_MACRO, PYTHON_TRIPLE_QUOTES_MACRO

#define _______________SETTINGS_CONTROLS_______________ CTRL_ALT_DEL_MACRO, _______, EMOJI_MACRO, KC_BRIGHTNESS_UP, KC_BRIGHTNESS_DOWN
#define _______________BROWSER_CONTROLS_______________  KC_WWW_HOME, KC_WWW_BACK, KC_WWW_FORWARD, KC_WWW_SEARCH, KC_WWW_REFRESH
#define _______________APPLICATIONS_______________      KC_MAIL, KC_CALCULATOR, KC_MY_COMPUTER, KC_CONTROL_PANEL, KC_ASSISTANT


#define LAYOUT_LAYER_BLANK                                                   \
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, \
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,\
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,\
   _______________DEAD_FUNCTIONS_______________


// layer 0 - letters - qwerty;
#define LAYOUT_LAYER_BASE                                                                              \
       KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,     KC_P,         \
       KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,     KC_SEMICOLON, \
       KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_SPC,  KC_BTN1,  KC_ESC,       \
                        _______________DEAD_FUNCTIONS_______________

/*
// layer 0 - letters - quoik;
#define LAYOUT_LAYER_BASE                                                                           \
       KC_Q,    KC_U,    KC_O,    KC_I,     KC_K,      KC_J,   KC_T,     KC_H,      KC_S,   KC_P,   \
       KC_Y,    KC_A,    KC_E,    KC_BTN1,  KC_G,      KC_H,   KC_SPC,   KC_ENTER,  KC_L,   KC_W,   \
       KC_Z,    KC_X,    KC_C,    KC_V,     KC_B,      KC_M,   KC_R,     KC_N,      KC_D,   KC_ESC, \
                        _______________DEAD_FUNCTIONS_______________
*/

//layer 1 - symbols
#define LAYOUT_LAYER_SYMBOLS_AND_ARROWS                                                               \
    KC_EXCLAIM, KC_AT, KC_HASH, KC_DOLLAR, KC_PERCENT,          KC_CIRCUMFLEX, KC_AMPERSAND, KC_ASTERISK, KC_UNDERSCORE, KC_PIPE, \
    KC_PLUS, KC_EQUAL, KC_BACKSPACE, KC_DELETE, KC_COLON,      KC_QUESTION, KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, \
    KC_MINUS,  _______________LEFT_BRACKETS_______________,     KC_BACKSLASH, KC_SLASH, KC_COMMA, KC_DOT,  KC_ESC, \
                                _______________DEAD_FUNCTIONS_______________


//layer 2 - numbers
#define LAYOUT_LAYER_FUNCTIONS_AND_NUMBERS                                                   \
    KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,               KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10, \
    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                KC_6,    KC_7,    KC_8,    KC_9,   KC_0, \
    KC_TILDE,  _______________RIGHT_BRACKETS_______________,  KC_F11,  KC_F12, KC_ENTER,  KC_BTN2,  KC_ESC, \
                                  _______________DEAD_FUNCTIONS_______________

//layer 3 - macros
#define LAYOUT_LAYER_NAVIGATION_AND_MACROS \
    HTML_COMMENT_MACRO,   _______________QUOTE_MACROS_______________,                                KC_HOME, CTRL_PGDOWN_MACRO, CTRL_SHIFT_TAB_MACRO, CTRL_TAB_MACRO, CTRL_PGUP_MACRO, \
    TURBOFISH_MACRO,    KC_GRAVE, KC_QUOTE, KC_DOUBLE_QUOTE, DOUBLE_COLON_MACRO,            KC_END, KC_PGDN, KC_MS_WH_DOWN, KC_MS_WH_UP,  KC_PGUP, \
    HELLO_WORLD_MACRO, _______________BRACKET_MACROS_______________,                        JS_DOC_MULTILINE_COMMENT_MACRO, JS_COMMENT_MACRO, KC_TAB, KC_BTN3,  KC_ESC, \
                        _______________DEAD_FUNCTIONS_______________


//layer 4 - navigation/media
#define LAYOUT_LAYER_NAVIGATION_AND_MEDIA                                                  \
    _______________SETTINGS_CONTROLS_______________,   _______________AUDIO_CONTROLS_______________, \
    _______________BROWSER_CONTROLS_______________,   KC_PRINT_SCREEN, WIN_LEFT_MACRO, WIN_DOWN_MACRO, WIN_UP_MACRO, WIN_RIGHT_MACRO, \
    _______________APPLICATIONS_______________,   ALT_TAB_MACRO, SHIFT_TAB_MACRO, KC_TAB, KC_LEFT_ALT, KC_ESC, \
   _______________DEAD_FUNCTIONS_______________





bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case CURLY_BRACKETS_MACRO:
      if (record->event.pressed) {
        // when keycode CURLY_BRACKETS_MACRO is pressed
        SEND_STRING("{}" SS_TAP(X_LEFT));
      }
      break;
    case PARENTHESIS_BRACKETS_MACRO:
        if (record->event.pressed) {
            // when keycode PARENTHESIS_BRACKETS_MACRO is pressed
            SEND_STRING("()" SS_TAP(X_LEFT));
        }
        break;
    case ANGLE_BRACKETS_MACRO:
            if (record->event.pressed) {
                // when keycode ANGLE_BRACKETS_MACRO is pressed
                SEND_STRING("<>" SS_TAP(X_LEFT));
            }
            break;
    case SQUARE_BRACKETS_MACRO:
            if (record->event.pressed) {
                // when keycode SQUARE_BRACKETS_MACRO is pressed
                SEND_STRING("[]" SS_TAP(X_LEFT));
            }
            break;
    case DOUBLE_QUOTES_MACRO:
            if (record->event.pressed) {
                // when keycode DOUBLE_QUOTES_MACRO is pressed
                SEND_STRING("\"\"" SS_TAP(X_LEFT));
            }
            break;
    case SINGLE_QUOTES_MACRO:
            if (record->event.pressed) {
                // when keycode SINGLE_QUOTES_MACRO is pressed
                SEND_STRING("''" SS_TAP(X_LEFT));
            }
            break;
    case BACKTICKS_MACRO:
            if (record->event.pressed) {
                // when keycode BACKTICKS_MACRO is pressed
                SEND_STRING("``" SS_TAP(X_LEFT));
            }
            break;
    case DOUBLE_COLON_MACRO:
            if (record->event.pressed) {
                // when keycode DOUBLE_COLON_MACRO is pressed
                SEND_STRING("::");
            }
            break;
case TURBOFISH_MACRO:
            if (record->event.pressed) {
                // when keycode DOUBLE_COLON_MACRO is pressed
                SEND_STRING("::<>" SS_TAP(X_LEFT));
            }
            break;
    case HTML_COMMENT_MACRO:
            if (record->event.pressed) {
                // when keycode HTML_COMMENT_MACRO is pressed
                SEND_STRING("<!--  -->" SS_TAP(X_LEFT) SS_TAP(X_LEFT) SS_TAP(X_LEFT) SS_TAP(X_LEFT));
            }
            break;
    case JS_COMMENT_MACRO:
            if (record->event.pressed) {
                // when keycode JS_COMMENT_MACRO is pressed
                SEND_STRING("//" SS_TAP(X_LEFT));
            }
            break;
    case JS_DOC_MULTILINE_COMMENT_MACRO:
            if (record->event.pressed) {
                // when keycode JS_DOC_MULTILINE_COMMENT_MACRO is pressed
                //create the string, go back 3 and hit enter;
                SEND_STRING("/**  */" SS_TAP(X_LEFT) SS_TAP(X_LEFT) SS_TAP(X_LEFT) SS_TAP(X_ENTER));
            }
            break;
    case PYTHON_TRIPLE_QUOTES_MACRO:
            if (record->event.pressed) {
                // when keycode PYTHON_TRIPLE_QUOTES_MACRO is pressed
                //create the string, go back 3 and hit enter;
                SEND_STRING("\"\"\"\"\"\"" SS_TAP(X_LEFT) SS_TAP(X_LEFT) SS_TAP(X_LEFT));
            }
            break;
    case WIN_LEFT_MACRO:
      if (record->event.pressed) {
        // when keycode WIN_LEFT_MACRO is pressed
        SEND_STRING(SS_DOWN(X_LGUI) SS_TAP(X_LEFT) SS_UP(X_LGUI));
      }
      break;
    case WIN_RIGHT_MACRO:
      if (record->event.pressed) {
        // when keycode WIN_RIGHT_MACRO is pressed
        SEND_STRING(SS_DOWN(X_LGUI) SS_TAP(X_LEFT) SS_UP(X_LGUI));
      }
      break;
    case WIN_UP_MACRO:
      if (record->event.pressed) {
        // when keycode WIN_UP_MACRO is pressed
        SEND_STRING(SS_DOWN(X_LGUI) SS_TAP(X_UP) SS_UP(X_LGUI));
      }
      break;
    case WIN_DOWN_MACRO:
      if (record->event.pressed) {
        // when keycode WIN_DOWN_MACRO is pressed
        SEND_STRING(SS_DOWN(X_LGUI) SS_TAP(X_DOWN) SS_UP(X_LGUI));
      }
      break;
    case ALT_TAB_MACRO:
      if (record->event.pressed) {
        // when keycode ALT_TAB_MACRO is pressed
        SEND_STRING(SS_DOWN(X_LALT) SS_TAP(X_TAB) SS_UP(X_LALT));
      }
      break;
    case SHIFT_TAB_MACRO:
      if (record->event.pressed) {
        // when keycode SHIFT_TAB_MACRO is pressed
        SEND_STRING(SS_DOWN(X_LSFT) SS_TAP(X_TAB) SS_UP(X_LSFT));
      }
      break;
    case CTRL_ALT_DEL_MACRO:
      if (record->event.pressed) {
        // when keycode CTRL_ALT_DEL_MACRO is pressed
        SEND_STRING(SS_DOWN(X_LCTL) SS_DOWN(X_LALT) SS_TAP(X_DELETE) SS_UP(X_LALT) SS_UP(X_LCTL));
      }
      break;
    case HELLO_WORLD_MACRO:
      if (record->event.pressed) {
        // when keycode HELLO_WORLD_MACRO is pressed
        SEND_STRING("Hello, World!");
      }
      break;
    case EMOJI_MACRO:
      if (record->event.pressed) {
        // when keycode EMOJI_MACRO is pressed
        SEND_STRING(SS_DOWN(X_LGUI) SS_TAP(X_DOT) SS_UP(X_LGUI));
      }
      break;
    case CTRL_PGUP_MACRO:
    if (record->event.pressed) {
        // when keycode CTRL_PGUP_MACRO is pressed
        SEND_STRING(SS_DOWN(X_LCTL) SS_TAP(X_PGUP) SS_UP(X_LCTL));
    }
    break;
    case CTRL_PGDOWN_MACRO:
    if (record->event.pressed) {
        // when keycode CTRL_PGDOWN_MACRO is pressed
        SEND_STRING(SS_DOWN(X_LCTL) SS_TAP(X_PGDN) SS_UP(X_LCTL));
    }
    break;
    case CTRL_TAB_MACRO:
    if (record->event.pressed) {
        // when keycode CTRL_TAB_MACRO is pressed
        SEND_STRING(SS_DOWN(X_LCTL) SS_TAP(X_TAB) SS_UP(X_LCTL));
    }
    break;
    case CTRL_SHIFT_TAB_MACRO:
    if (record->event.pressed) {
        // when keycode CTRL_SHIFT_TAB_MACRO is pressed
        SEND_STRING(SS_DOWN(X_LCTL) SS_DOWN(X_LSFT) SS_TAP(X_TAB) SS_UP(X_LSFT) SS_UP(X_LCTL));
    }
    break;

  }

  return true;
}

/*
#define LAYOUT_LAYER_SYMBOLS_AND_ARROWS                                                               \
    _______________DEAD_HALF_ROW_______________, _______________DEAD_HALF_ROW_______________, \
    ______________HOME_ROW_GACS_L______________, KC_CAPS, KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, \
    _______________DEAD_HALF_ROW_______________,  KC_INS, KC_HOME, KC_PGDN, KC_PGUP,  KC_END, \
                      XXXXXXX, _______, XXXXXXX,  KC_ENT, KC_BSPC
*/
/*

// KC_EXCLAIM 	KC_EXLM 	!
// KC_AT 		@
// KC_HASH 		#
// KC_DOLLAR 	KC_DLR 	$
// KC_PERCENT 	KC_PERC 	%
// KC_CIRCUMFLEX 	KC_CIRC 	^
// KC_AMPERSAND 	KC_AMPR 	&
// KC_ASTERISK 	KC_ASTR 	*
// KC_LEFT_PAREN 	KC_LPRN 	(
// KC_RIGHT_PAREN 	KC_RPRN 	)

// KC_LEFT_CURLY_BRACE 	KC_LCBR 	{
// KC_RIGHT_CURLY_BRACE 	KC_RCBR 	}

//KC_LEFT_ANGLE_BRACKET 	KC_LABK, KC_LT 	<
//KC_RIGHT_ANGLE_BRACKET 	KC_RABK, KC_GT 	>

//KC_LEFT_BRACKET 	KC_LBRC 	[ and {
//KC_RIGHT_BRACKET 	KC_RBRC 	] and }
//KC_QUESTION 	KC_QUES 	?
//KC_COMMA 	KC_COMM 	, and <
//KC_DOT 		. and >
//KC_PIPE 		|
//KC_UNDERSCORE 	KC_UNDS 	_


// KC_TILDE 	KC_TILD 	~
// KC_PLUS 		+
// KC_MINUS 	KC_MINS 	- and _
// KC_EQUAL 	KC_EQL 	= and +
// KC_COLON 	KC_COLN 	:
// KC_SLASH 	KC_SLSH 	/ and ?
//KC_GRAVE 	KC_GRV 	` and ~


KC_DOUBLE_QUOTE 	KC_DQUO, KC_DQT 	"
// KC_BACKSLASH 	KC_BSLS 	\ and |
// KC_QUOTE 	KC_QUOT 	' and "



*/




/**
 * \brief Navigation layer.
 *
 * Secondary left-hand layer has shifted symbols in the same locations to reduce
 * chording when using mods with shifted symbols. `KC_LPRN` is duplicated next to
 * `KC_RPRN`.
 */
/*
#define LAYOUT_LAYER_NAVIGATION_AND_MACROS                                                                  \
    KC_LCBR, KC_AMPR, KC_ASTR, KC_LPRN, KC_RCBR, _______________DEAD_HALF_ROW_______________, \
    KC_COLN,  KC_DLR, KC_PERC, KC_CIRC, KC_PLUS, ______________HOME_ROW_GACS_R______________, \
    KC_TILD, KC_EXLM,   KC_AT, KC_HASH, KC_PIPE, _______________DEAD_HALF_ROW_______________, \
                      KC_LPRN, KC_RPRN, KC_UNDS, _______, XXXXXXX
*/









/**
 * \brief Media layer.
 *
 * Tertiary left- and right-hand layer is media and RGB control.  This layer is
 * symmetrical to accomodate the left- and right-hand trackball.
 */
/*
#define LAYOUT_LAYER_MEDIA                                                                    \
    XXXXXXX,RGB_RMOD, RGB_TOG, RGB_MOD, XXXXXXX, XXXXXXX,RGB_RMOD, RGB_TOG, RGB_MOD, XXXXXXX, \
    KC_MPRV, KC_VOLD, KC_MUTE, KC_VOLU, KC_MNXT, KC_MPRV, KC_VOLD, KC_MUTE, KC_VOLU, KC_MNXT, \
    XXXXXXX, XXXXXXX, XXXXXXX,  EE_CLR, QK_BOOT, QK_BOOT,  EE_CLR, XXXXXXX, XXXXXXX, XXXXXXX, \
                      _______, KC_MPLY, KC_MSTP, KC_MSTP, KC_MPLY
*/



/** \brief Mouse emulation and pointer functions. */
/*
#define LAYOUT_LAYER_POINTER                                                                  \
    XXXXXXX, XXXXXXX, XXXXXXX, DPI_MOD, S_D_MOD, S_D_MOD, DPI_MOD, XXXXXXX, XXXXXXX, XXXXXXX, \
    ______________HOME_ROW_GACS_L______________, ______________HOME_ROW_GACS_R______________, \
    _______, DRGSCRL, SNIPING,  EE_CLR, QK_BOOT, QK_BOOT,  EE_CLR, SNIPING, DRGSCRL, _______, \
                      KC_BTN2, KC_BTN1, KC_BTN3, KC_BTN3, KC_BTN1
*/



/**
 * \brief Add Home Row mod to a layout.
 *
 * Expects a 10-key per row layout.  Adds support for GACS (Gui, Alt, Ctl, Shift)
 * home row.  The layout passed in parameter must contain at least 20 keycodes.
 *
 * This is meant to be used with `LAYER_ALPHAS_QWERTY` defined above, eg.:
 *
 *     HOME_ROW_MOD_GACS(LAYER_ALPHAS_QWERTY)
 */
/*
#define _HOME_ROW_MOD_GACS(                                            \
    L00, L01, L02, L03, L04, R05, R06, R07, R08, R09,                  \
    L10, L11, L12, L13, L14, R15, R16, R17, R18, R19,                  \
    ...)                                                               \
             L00,         L01,         L02,         L03,         L04,  \
             R05,         R06,         R07,         R08,         R09,  \
      LGUI_T(L10), LALT_T(L11), LCTL_T(L12), LSFT_T(L13),        L14,  \
             R15,  RSFT_T(R16), RCTL_T(R17), LALT_T(R18), RGUI_T(R19), \
      __VA_ARGS__
#define HOME_ROW_MOD_GACS(...) _HOME_ROW_MOD_GACS(__VA_ARGS__)
*/

/**
 * \brief Add pointer layer keys to a layout.
 *
 * Expects a 10-key per row layout.  The layout passed in parameter must contain
 * at least 30 keycodes.
 *
 * This is meant to be used with `LAYER_ALPHAS_QWERTY` defined above, eg.:
 *
 *     POINTER_MOD(LAYER_ALPHAS_QWERTY)
 */
/*
#define _POINTER_MOD(                                                  \
    L00, L01, L02, L03, L04, R05, R06, R07, R08, R09,                  \
    L10, L11, L12, L13, L14, R15, R16, R17, R18, R19,                  \
    L20, L21, L22, L23, L24, R25, R26, R27, R28, R29,                  \
    ...)                                                               \
             L00,         L01,         L02,         L03,         L04,  \
             R05,         R06,         R07,         R08,         R09,  \
             L10,         L11,         L12,         L13,         L14,  \
             R15,         R16,         R17,         R18,         R19,  \
      _L_PTR(L20),        L21,         L22,         L23,         L24,  \
             R25,         R26,         R27,         R28,  _L_PTR(R29), \
      __VA_ARGS__
#define POINTER_MOD(...) _POINTER_MOD(__VA_ARGS__)
*/

#define LAYOUT_wrapper(...) LAYOUT(__VA_ARGS__)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_BASE] = LAYOUT_wrapper(
    //POINTER_MOD(HOME_ROW_MOD_GACS(LAYOUT_LAYER_BASE))
    LAYOUT_LAYER_BASE
  ),
  [LAYER_SYMBOLS_AND_ARROWS] = LAYOUT_wrapper(LAYOUT_LAYER_SYMBOLS_AND_ARROWS),
  [LAYER_FUNCTIONS_AND_NUMBERS] = LAYOUT_wrapper(LAYOUT_LAYER_FUNCTIONS_AND_NUMBERS),
  //[LAYER_MEDIA] = LAYOUT_wrapper(LAYOUT_LAYER_MEDIA),
  //[LAYER_NUMERAL] = LAYOUT_wrapper(LAYOUT_LAYER_NUMERAL),
  //[LAYER_POINTER] = LAYOUT_wrapper(LAYOUT_LAYER_POINTER),
  [LAYER_NAVIGATION_AND_MACROS] = LAYOUT_wrapper(LAYOUT_LAYER_NAVIGATION_AND_MACROS),
  [LAYER_NAVIGATION_AND_MEDIA] = LAYOUT_wrapper(LAYOUT_LAYER_NAVIGATION_AND_MEDIA),
};

// clang-format on

/*
#ifdef POINTING_DEVICE_ENABLE
#    ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (abs(mouse_report.x) > CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD || abs(mouse_report.y) > CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD) {
        if (auto_pointer_layer_timer == 0) {
            layer_on(LAYER_POINTER);
#        ifdef RGB_MATRIX_ENABLE
            rgb_matrix_mode_noeeprom(RGB_MATRIX_NONE);
            rgb_matrix_sethsv_noeeprom(HSV_GREEN);
#        endif // RGB_MATRIX_ENABLE
        }
        auto_pointer_layer_timer = timer_read();
    }
    return mouse_report;
}

void matrix_scan_user(void) {
    if (auto_pointer_layer_timer != 0 && TIMER_DIFF_16(timer_read(), auto_pointer_layer_timer) >= CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS) {
        auto_pointer_layer_timer = 0;
        layer_off(LAYER_POINTER);
#        ifdef RGB_MATRIX_ENABLE
        rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
#        endif // RGB_MATRIX_ENABLE
    }
}
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

#    ifdef CHARYBDIS_AUTO_SNIPING_ON_LAYER
layer_state_t layer_state_set_user(layer_state_t state) {
    charybdis_set_pointer_sniping_enabled(layer_state_cmp(state, CHARYBDIS_AUTO_SNIPING_ON_LAYER));
    return state;
}
#    endif // CHARYBDIS_AUTO_SNIPING_ON_LAYER
#endif     // POINTING_DEVICE_ENABLE
*/

#ifdef RGB_MATRIX_ENABLE
// Forward-declare this helper function since it is defined in
// rgb_matrix.c.
void rgb_matrix_update_pwm_buffers(void);
#endif

uint8_t LAYER_COUNT = 5;
bool SHIFT_PRESSED = false;
bool WIN_PRESSED = false;
bool CTRL_PRESSED = false;
bool ALT_PRESSED = false;
bool LOCAL_REST = false;
bool REMOTE_REST = false;

void disable_all_layers_except(uint8_t layer){
    for(int i = 1; i < LAYER_COUNT; i++){
        if (i == layer){
            layer_on(i);
        }else{
            layer_off(i);
        }
    }
}

void disable_all_layers(void){
    disable_all_layers_except(0);
}

int VERY_HIGH_CPI = 2400;
int HIGH_CPI = 1600;
int MEDIUM_CPI = 1000;
int LOW_CPI = 400;

//example: handle_special_key_press(&localHalfTouched[0], &SHIFT_PRESSED, KC_LSFT) ;
void handle_special_key_press(uint8_t value, bool* status, enum qk_keycode_defines keycode) {

    if((value == 1) && !(*status)){
        register_code(keycode);
        *status = true;
    }
    else if((value == 0) && *status){
        unregister_code(keycode);
        *status = false;
    }
}

void set_dragscroll_and_sniping(bool dragscroll, bool sniping){
    if(charybdis_get_pointer_dragscroll_enabled() != dragscroll){
        charybdis_set_pointer_dragscroll_enabled(dragscroll);
    }

    if(charybdis_get_pointer_sniping_enabled() != sniping){
        charybdis_set_pointer_sniping_enabled(sniping);
    }
}

bool handle_touch_layers_and_keys(void){
//check the local first for shift;
handle_special_key_press(localHalfTouched[0], &SHIFT_PRESSED, KC_LSFT) ;

//check the remote first for ctrl;
handle_special_key_press(remoteHalfTouched[0], &CTRL_PRESSED, KC_LCTL) ;


// make sure the rest is not pressed;
//bool localIsNotRest = (localHalfTouched[1] != 1);
//bool remoteIsNotRest = (remoteHalfTouched[1] != 1);

// check the middle 2 for layer shifts;
// there is one extra unused pin;
for(uint8_t i = 1; i < 3; i++) {
    if ((remoteHalfTouched[i] == 1)) {
        disable_all_layers_except((2 * (i-1) + 1));
        //kc_register_code(SNIPING);
        set_dragscroll_and_sniping(true, true);
        //pointing_device_set_cpi(LOW_CPI)
        return true;
    } else if ((localHalfTouched[i] == 1)) {
        disable_all_layers_except((2 * (i-1)) + 2);
        //kc_register_code(SNIPING);
        set_dragscroll_and_sniping(true, true);
        //pointing_device_set_cpi(HIGH_CPI);
        return true;
    }
}

disable_all_layers();
//kc_unregister_code(SNIPING);
set_dragscroll_and_sniping(false, false);


//check the local last for alt;
handle_special_key_press(localHalfTouched[4], &ALT_PRESSED, KC_LALT) ;

//check the remote last for win;
handle_special_key_press(remoteHalfTouched[4], &WIN_PRESSED, KC_LGUI) ;


return false;
}

/*
#define HSV_AZURE       132, 102, 255
#define HSV_BLACK         0,   0,   0
#define HSV_BLUE        170, 255, 255
#define HSV_CHARTREUSE   64, 255, 255
#define HSV_CORAL        11, 176, 255
#define HSV_CYAN        128, 255, 255
#define HSV_GOLD         36, 255, 255
#define HSV_GOLDENROD    30, 218, 218
#define HSV_GREEN        85, 255, 255
#define HSV_MAGENTA     213, 255, 255
#define HSV_ORANGE       21, 255, 255
#define HSV_PINK        234, 128, 255
#define HSV_PURPLE      191, 255, 255
#define HSV_RED           0, 255, 255
#define HSV_SPRINGGREEN 106, 255, 255
#define HSV_TEAL        128, 255, 128
#define HSV_TURQUOISE   123,  90, 112
#define HSV_WHITE         0,   0, 255
#define HSV_YELLOW       43, 255, 255
#define HSV_OFF         HSV_BLACK
*/


int hsv_colors[12][3] = {
    {HSV_AZURE},
    {HSV_BLUE},
    {HSV_CHARTREUSE},
    {HSV_CYAN},
    {HSV_GOLD},
    {HSV_GREEN},
    {HSV_MAGENTA},
    {HSV_ORANGE},
    {HSV_PINK},
    {HSV_PURPLE},
    {HSV_RED},
    {HSV_SPRINGGREEN}
};


int rgb_colors[12][3] = {
    {RGB_AZURE},
    {RGB_BLUE},
    {RGB_CHARTREUSE},
    {RGB_CYAN},
    {RGB_GOLD},
    {RGB_GREEN},
    {RGB_MAGENTA},
    {RGB_ORANGE},
    {RGB_PINK},
    {RGB_PURPLE},
    {RGB_RED},
    {RGB_SPRINGGREEN}
};

int currentColorIndex = 0;


bool set_layer_color_for_debugging( uint8_t value){

for(int i = 0; i < NUM_PINS; i++){
    if (localHalfTouched[i] == 1){
        int scaled_i = 2 * i;
        //if(currentColorIndex != scaled_i){
            currentColorIndex = scaled_i;
            rgb_matrix_sethsv_noeeprom(hsv_colors[scaled_i][0], hsv_colors[scaled_i][1], hsv_colors[scaled_i][2]);
        //}
        return true;
    }else if (remoteHalfTouched[i] == 1){
        int scaled_i = 2 * i + 1;
        //if(currentColorIndex != scaled_i){
            currentColorIndex = scaled_i;
            rgb_matrix_sethsv_noeeprom(hsv_colors[scaled_i][0], hsv_colors[scaled_i][1], hsv_colors[scaled_i][2]);
        //}
        return true;
    }
}
//rgb_matrix_set_color_all(RGB_BLACK);
rgb_matrix_sethsv_noeeprom(0,0,64);
return false;
}


void housekeeping_task_user(void) {
//rgb_matrix_sethsv_noeeprom(0, 0, 64);
//rgb_matrix_set_color_all(RGB_GREEN);

//if any key is pressed, turn the lighs off
// if (any_key_pressed()) {
//     rgb_matrix_set_color_all(RGB_BLACK);
// }else{


if(IS_KEYBOARD_MASTER){
rgb_matrix_sethsv_noeeprom(0,0,64);
//rgb_matrix_set_color_all(RGB_WHITE);

//set_layer_color_for_debugging(255); // doesnt work for some reason;
handle_touch_layers_and_keys();



/*
//left shift;
if (localHalfTouched[0] == 1){
    rgb_matrix_set_color_all(RGB_ORANGE);
register_code(KC_LSFT);
layer_off(1);
layer_off(3);
layer_off(5);
unregister_code(KC_LGUI);

// rest;
}else if (localHalfTouched[1] == 1) {
    rgb_matrix_set_color_all(RGB_GOLDENROD);
unregister_code(KC_LSFT);
layer_off(1);
layer_off(3);
layer_off(5);
unregister_code(KC_LGUI);

// layer 1;
}else if (localHalfTouched[2] == 1) {
    rgb_matrix_set_color_all(RGB_YELLOW);
unregister_code(KC_LSFT);
layer_on(1);
layer_off(3);
layer_off(5);
unregister_code(KC_LGUI);

// layer 3;
}else if (localHalfTouched[3] == 1) {
    rgb_matrix_set_color_all(RGB_CHARTREUSE);
unregister_code(KC_LSFT);
layer_off(1);
layer_on(3);
layer_off(5);
unregister_code(KC_LGUI);

// layer 3;
}else if (localHalfTouched[4] == 1) {
    rgb_matrix_set_color_all(RGB_GREEN);
unregister_code(KC_LSFT);
layer_off(1);
layer_off(2);
layer_on(3);
unregister_code(KC_LGUI);

// windows;
}else if (localHalfTouched[5] == 1) {
    rgb_matrix_set_color_all(RGB_SPRINGGREEN);
unregister_code(KC_LSFT);
layer_off(1);
layer_off(3);
layer_off(5);
register_code(KC_LGUI);

}else{
    //rgb_matrix_set_color_all(RGB_WHITE);
unregister_code(KC_LSFT);
layer_off(1);
layer_off(3);
layer_off(5);
unregister_code(KC_LGUI);

}


//ctrl;
if (remoteHalfTouched[0] == 1){
    rgb_matrix_set_color_all(RGB_PINK);
register_code(KC_LCTL);
layer_off(2);
layer_off(4);
layer_off(6);
unregister_code(KC_LALT);
// rest;
}else if (remoteHalfTouched[1] == 1) {
    rgb_matrix_set_color_all(RGB_MAGENTA);
unregister_code(KC_LCTL);
layer_off(2);
layer_off(4);
layer_off(6);
unregister_code(KC_LALT);
// layer 2;
}else if (remoteHalfTouched[2] == 1) {
    rgb_matrix_set_color_all(RGB_PURPLE);
unregister_code(KC_LCTL);
layer_on(2);
layer_off(4);
layer_off(6);
unregister_code(KC_LALT);
// layer 4;
}else if (remoteHalfTouched[3] == 1) {
    rgb_matrix_set_color_all(RGB_BLUE);
unregister_code(KC_LCTL);
layer_off(2);
layer_on(4);
layer_off(6);
unregister_code(KC_LALT);
// layer 6;
}else if (remoteHalfTouched[4] == 1) {
    rgb_matrix_set_color_all(RGB_AZURE);
unregister_code(KC_LCTL);
layer_off(2);
layer_off(4);
layer_on(6);
unregister_code(KC_LALT);
// alt;
}else if (remoteHalfTouched[5] == 1) {
    rgb_matrix_set_color_all(RGB_CYAN);
unregister_code(KC_LCTL);
layer_off(2);
layer_off(4);
layer_off(6);
register_code(KC_LALT);
}else{
   // rgb_matrix_set_color_all(RGB_WHITE);
unregister_code(KC_LCTL);
layer_off(2);
layer_off(4);
layer_off(6);
unregister_code(KC_LALT);
}
*/
//}

}else{

rgb_matrix_sethsv_noeeprom(0,0,64);
}

}
