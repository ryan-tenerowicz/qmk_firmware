/* Copyright 2018 James Underwood
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
#include "my_pointing_device.h"

enum layers {
    _COLEMAK,
    _SYMBOL,
    _NAVIGATION
};

enum custom_keys {
    COLEMAK = SAFE_RANGE,
    SCROLL_TOGGLE
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_COLEMAK] = LAYOUT(
    KC_GRV , KC_1, KC_2, KC_3, KC_4, KC_5,   KC_6, KC_7, KC_8   , KC_9  , KC_0   , KC_MINS,
    KC_ESC , KC_Q, KC_W, KC_F, KC_P, KC_B,   KC_J, KC_L, KC_U   , KC_Y  , KC_SCLN, KC_TAB,
    KC_BSPC, KC_A, KC_R, KC_S, KC_T, KC_D,   KC_M, KC_N, KC_E   , KC_I  , KC_O   , KC_ENT,
    KC_DEL , KC_Z, KC_X, KC_C, KC_V, KC_G,   KC_K, KC_H, KC_COMM, KC_DOT, KC_SLSH, KC_INS,

    KC_BTN3, LSFT_T(KC_SPC), KC_BTN1, LCTL_T(KC_ESC), KC_BTN2,   KC_BTN2, LT(_SYMBOL, KC_ESC), KC_BTN1, KC_BTN3, RSFT_T(KC_SPC),
        //Down,             back              Up       in       forw          
    LT(_NAVIGATION, _______), _______, LM(_NAVIGATION, MOD_LALT|MOD_LSFT), _______, LM(_NAVIGATION, MOD_LALT),   
    // forw                   in       down                    back       up
    LM(_NAVIGATION, MOD_LALT), _______, LT(_NAVIGATION, _______), _______, LM(_NAVIGATION, MOD_LALT|MOD_LSFT)
  ),
  [_SYMBOL] = LAYOUT(
    KC_F1  , KC_F2  , KC_F3  , KC_F4  , KC_F5  , KC_F6  ,   KC_F7  , KC_F8  , KC_F9  , KC_F10 , KC_F11 , KC_F12 ,
    _______, _______, _______, _______, _______, _______,   _______, _______, _______, _______, _______, _______,
    _______, _______, KC_QUOT, KC_LBRC, KC_EQL , _______,   _______, KC_SCLN, KC_RBRC, KC_BSLS, _______, _______,
    _______, _______, _______, _______, _______, _______,   _______, KC_LBRC, KC_RBRC, _______, _______, _______,

          KC_BTN3, _______, KC_BTN1, _______, KC_BTN2,   KC_BTN2, _______, KC_BTN1, _______, KC_BTN3,
        //Down,  back  Up   in  forw         forw   in    down   back up
          _______, _______, _______, _______, _______,   _______, _______, _______, _______, _______
  ),
  [_NAVIGATION] = LAYOUT(
    _______, _______, _______, _______, _______, _______,   _______, _______, _______, _______, _______, _______,
    _______, _______, _______, KC_UP  , _______, _______,   _______, _______, KC_UP  , _______, _______, _______,
    _______, _______, KC_LEFT, KC_DOWN, KC_RGHT, _______,   _______, KC_LEFT, KC_DOWN, KC_RGHT, _______, _______,
    _______, _______, _______, _______, _______, _______,   _______, _______, _______, _______, _______, _______,

          KC_BTN3, _______, KC_BTN1, _______, KC_BTN2,   KC_BTN2, _______, KC_BTN1, _______, KC_BTN3,
            //Down,  back  Up   in  forw        forw   in    down   back up
          _______, _______, _______, _______, _______,   _______, _______, _______, _______, _______
  ),
};

int PLOOPY_DRAGSCROLL_SEMAPHORE = 20;
// drag scroll divisor state
int8_t drag_scroll_x_semaphore = 0;
int8_t drag_scroll_y_semaphore = 0;
static bool is_drag_scroll = false;

report_mouse_t scroll_toggle(report_mouse_t mouse_report) {
    if (is_drag_scroll) {
        int16_t mouse_report_x_temp = mouse_report.x;
        int16_t mouse_report_y_temp = mouse_report.y;
        int16_t mouse_report_x_calc = 0;
        int16_t mouse_report_y_calc = 0;
        int16_t valx = (mouse_report_x_temp > 0) ? -1 : 1;
        int16_t valy = (mouse_report_y_temp > 0) ? -1 : 1;

        while (mouse_report_x_temp != 0) {
            mouse_report_x_temp += valx;
            drag_scroll_x_semaphore -= valx;

            if (abs(drag_scroll_x_semaphore) >= PLOOPY_DRAGSCROLL_SEMAPHORE) {
                mouse_report_x_calc -= valx;
                drag_scroll_x_semaphore = 0;
            }
        }

        while (mouse_report_y_temp != 0) {
            mouse_report_y_temp += valy;
            drag_scroll_y_semaphore -= valy;

            if (abs(drag_scroll_y_semaphore) >= PLOOPY_DRAGSCROLL_SEMAPHORE) {
                mouse_report_y_calc -= valy;
                drag_scroll_y_semaphore = 0;
            }
        }

        mouse_report.h = mouse_report_x_calc;
        mouse_report.v = -mouse_report_y_calc;
        mouse_report.x = 0;
        mouse_report.y = 0;
    }
    return mouse_report;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case SCROLL_TOGGLE:
          if (record->event.pressed) {
            is_drag_scroll ^= 1;
          }
          return false;
    }
    return true;
}