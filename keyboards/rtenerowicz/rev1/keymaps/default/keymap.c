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
    SCROLL_TOGGLE_LEFT,
    SCROLL_TOGGLE_RIGHT
};

enum {
  KC_A_SCROLL,
  KC_O_SCROLL,
  KC_T_BACK,
  KC_V_FWD,
  KC_M_BACK,
  KC_N_FWD
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_COLEMAK] = LAYOUT(
    KC_GRV , KC_1, KC_2, KC_3, KC_4, KC_5,   KC_6, KC_7, KC_8   , KC_9  , KC_0   , KC_MINS,
    KC_ESC , KC_Q, KC_W, KC_F, KC_P, KC_B,   KC_J, KC_L, KC_U   , KC_Y  , KC_SCLN, KC_TAB,
    KC_BSPC, TD(KC_A_SCROLL), KC_R, KC_S, TD(KC_T_BACK), TD(KC_V_FWD),   TD(KC_M_BACK), TD(KC_N_FWD), KC_E   , KC_I  , TD(KC_O_SCROLL)   , KC_ENT,
    KC_DEL , KC_Z, KC_X, KC_C, KC_D, KC_G,   KC_K, KC_H, KC_COMM, KC_DOT, KC_SLSH, KC_INS,

    KC_BTN3, LSFT_T(KC_SPC), KC_BTN1, LCTL_T(KC_ESC), KC_BTN2,   KC_BTN2, LT(_SYMBOL, KC_ESC), KC_BTN1, KC_BTN3, RSFT_T(KC_SPC),
        //Down,             back              Up       in       forw          
    LT(_NAVIGATION, _______), _______, LM(_NAVIGATION, MOD_LALT|MOD_LSFT), SCROLL_TOGGLE_LEFT, LM(_NAVIGATION, MOD_LALT),   
    // forw                   in       down                    back       up
    LM(_NAVIGATION, MOD_LALT), SCROLL_TOGGLE_RIGHT, LT(_NAVIGATION, _______), _______, LM(_NAVIGATION, MOD_LALT|MOD_LSFT)
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
    _______, _______, KC_BTN4, KC_UP  , KC_BTN5, _______,   _______, KC_BTN4, KC_UP  , KC_BTN5, _______, _______,
    _______, _______, KC_LEFT, KC_DOWN, KC_RGHT, _______,   _______, KC_LEFT, KC_DOWN, KC_RGHT, _______, _______,
    _______, _______, _______, _______, _______, _______,   _______, _______, _______, _______, _______, _______,

          KC_BTN3, _______, KC_BTN1, _______, KC_BTN2,   KC_BTN2, _______, KC_BTN1, _______, KC_BTN3,
            //Down,  back  Up   in  forw        forw   in    down   back up
          _______, _______, _______, _______, _______,   _______, _______, _______, _______, _______
  ),
};

int PLOOPY_DRAGSCROLL_SEMAPHORE = 20;
// drag scroll divisor state
int8_t drag_scroll_x_semaphore_left = 0;
int8_t drag_scroll_y_semaphore_left = 0;
int8_t drag_scroll_x_semaphore_right = 0;
int8_t drag_scroll_y_semaphore_right = 0;

static bool is_drag_scroll_left = true;
static bool is_drag_scroll_right = false;

report_mouse_t scroll_toggle_left(report_mouse_t mouse_report) {
    if (is_drag_scroll_left) {
        int16_t mouse_report_x_temp = mouse_report.x;
        int16_t mouse_report_y_temp = mouse_report.y;
        int16_t mouse_report_x_calc = 0;
        int16_t mouse_report_y_calc = 0;
        int16_t valx = (mouse_report_x_temp > 0) ? -1 : 1;
        int16_t valy = (mouse_report_y_temp > 0) ? -1 : 1;

        while (mouse_report_x_temp != 0) {
            mouse_report_x_temp += valx;
            drag_scroll_x_semaphore_left -= valx;

            if (abs(drag_scroll_x_semaphore_left) >= PLOOPY_DRAGSCROLL_SEMAPHORE) {
                mouse_report_x_calc -= valx;
                drag_scroll_x_semaphore_left = 0;
            }
        }

        while (mouse_report_y_temp != 0) {
            mouse_report_y_temp += valy;
            drag_scroll_y_semaphore_left -= valy;

            if (abs(drag_scroll_y_semaphore_left) >= PLOOPY_DRAGSCROLL_SEMAPHORE) {
                mouse_report_y_calc -= valy;
                drag_scroll_y_semaphore_left = 0;
            }
        }

        mouse_report.h = mouse_report_x_calc;
        mouse_report.v = -mouse_report_y_calc;
        mouse_report.x = 0;
        mouse_report.y = 0;
    }
    return mouse_report;
}

report_mouse_t scroll_toggle_right(report_mouse_t mouse_report) {
    if (is_drag_scroll_right) {
        int16_t mouse_report_x_temp = mouse_report.x;
        int16_t mouse_report_y_temp = mouse_report.y;
        int16_t mouse_report_x_calc = 0;
        int16_t mouse_report_y_calc = 0;
        int16_t valx = (mouse_report_x_temp > 0) ? -1 : 1;
        int16_t valy = (mouse_report_y_temp > 0) ? -1 : 1;

        while (mouse_report_x_temp != 0) {
            mouse_report_x_temp += valx;
            drag_scroll_x_semaphore_right -= valx;

            if (abs(drag_scroll_x_semaphore_right) >= PLOOPY_DRAGSCROLL_SEMAPHORE) {
                mouse_report_x_calc -= valx;
                drag_scroll_x_semaphore_right = 0;
            }
        }

        while (mouse_report_y_temp != 0) {
            mouse_report_y_temp += valy;
            drag_scroll_y_semaphore_right -= valy;

            if (abs(drag_scroll_y_semaphore_right) >= PLOOPY_DRAGSCROLL_SEMAPHORE) {
                mouse_report_y_calc -= valy;
                drag_scroll_y_semaphore_right = 0;
            }
        }

        mouse_report.h = mouse_report_x_calc;
        mouse_report.v = -mouse_report_y_calc;
        mouse_report.x = 0;
        mouse_report.y = 0;
    }
    return mouse_report;
}

report_mouse_t pointing_device_task_combined_user(report_mouse_t left_report, report_mouse_t right_report) {
    left_report = scroll_toggle_left(left_report);
    right_report = scroll_toggle_right(right_report);
    return pointing_device_combine_reports(left_report, right_report);
}



typedef struct {
    bool is_key;
    bool is_scroll;
} tap_dance_scroll_t;

void tap_dance_tap_hold_finished_right(tap_dance_state_t *state, void *user_data) {
    tap_dance_scroll_t *tap_hold = (tap_dance_scroll_t *)user_data;

    if (state->pressed) {
        if (state->count == 1) {
            is_drag_scroll_right ^= 1;
            tap_hold->is_scroll = true;
        } else {
            register_code16(KC_O);
            tap_hold->is_key = true;
        }
    }
}

void tap_dance_tap_hold_reset_right(tap_dance_state_t *state, void *user_data) {
    tap_dance_scroll_t *tap_hold = (tap_dance_scroll_t *)user_data;

    if (tap_hold->is_key) {
        unregister_code16(KC_O);
        tap_hold->is_key = false;
    }
    if (tap_hold->is_scroll) {
        is_drag_scroll_right ^= 1;
        tap_hold->is_scroll = false;
    }
}

void tap_dance_tap_hold_finished_left(tap_dance_state_t *state, void *user_data) {
    tap_dance_scroll_t *tap_hold = (tap_dance_scroll_t *)user_data;

    if (state->pressed) {
        if (state->count == 1) {
            is_drag_scroll_left ^= 1;
            tap_hold->is_scroll = true;
        } else {
            register_code16(KC_A);
            tap_hold->is_key = true;
        }
    }
}

void tap_dance_tap_hold_reset_left(tap_dance_state_t *state, void *user_data) {
    tap_dance_scroll_t *tap_hold = (tap_dance_scroll_t *)user_data;

    if (tap_hold->is_key) {
        unregister_code16(KC_A);
        tap_hold->is_key = false;
    }
    if (tap_hold->is_scroll) {
        is_drag_scroll_left ^= 1;
        tap_hold->is_scroll = false;
    }
}

typedef struct {
    uint16_t tap;
    uint16_t hold;
    uint16_t held;
} tap_dance_tap_hold_t;

void tap_dance_tap_hold_finished(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (state->pressed) {
        if (state->count == 1
#ifndef PERMISSIVE_HOLD
            && !state->interrupted
#endif
        ) {
            register_code16(tap_hold->hold);
            tap_hold->held = tap_hold->hold;
        } else {
            register_code16(tap_hold->tap);
            tap_hold->held = tap_hold->tap;
        }
    }
}

void tap_dance_tap_hold_reset(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (tap_hold->held) {
        unregister_code16(tap_hold->held);
        tap_hold->held = 0;
    }
}

#define ACTION_TAP_DANCE_TAP_HOLD(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished, tap_dance_tap_hold_reset}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }

tap_dance_action_t tap_dance_actions[] = {
    [KC_O_SCROLL] = { .fn = {NULL, tap_dance_tap_hold_finished_right, tap_dance_tap_hold_reset_right}, .user_data = (void *)&((tap_dance_scroll_t){false,false}), },
    [KC_A_SCROLL] = { .fn = {NULL, tap_dance_tap_hold_finished_left, tap_dance_tap_hold_reset_left}, .user_data = (void *)&((tap_dance_scroll_t){false,false}), },
    [KC_T_BACK] = ACTION_TAP_DANCE_TAP_HOLD(KC_T, KC_BTN4),
    [KC_V_FWD] = ACTION_TAP_DANCE_TAP_HOLD(KC_V, KC_BTN5),
    [KC_M_BACK] = ACTION_TAP_DANCE_TAP_HOLD(KC_M, KC_BTN4),
    [KC_N_FWD] = ACTION_TAP_DANCE_TAP_HOLD(KC_N, KC_BTN5)
};



bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    tap_dance_action_t *action;
    switch (keycode) {
        case SCROLL_TOGGLE_LEFT:
          if (record->event.pressed) {
            is_drag_scroll_left ^= 1;
          }
          return false;
        case SCROLL_TOGGLE_RIGHT:
          if (record->event.pressed) {
            is_drag_scroll_right ^= 1;
          }
          return false;
        case TD(KC_O_SCROLL):
          action = &tap_dance_actions[TD_INDEX(keycode)];
          if (!record->event.pressed && action->state.count && !action->state.finished) {
              tap_code16(KC_O);
          }
          return true;
        case TD(KC_A_SCROLL):
          action = &tap_dance_actions[TD_INDEX(keycode)];
          if (!record->event.pressed && action->state.count && !action->state.finished) {
              tap_code16(KC_A);
          }
          return true;
        case TD(KC_T_BACK):
        case TD(KC_V_FWD):
        case TD(KC_M_BACK):
        case TD(KC_N_FWD):
            action = &tap_dance_actions[TD_INDEX(keycode)];
            if (!record->event.pressed && action->state.count && !action->state.finished) {
                tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
                tap_code16(tap_hold->tap);
            }

    }
    return true;
}



