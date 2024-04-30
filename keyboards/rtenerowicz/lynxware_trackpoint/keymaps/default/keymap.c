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
    _ENGRAM,
    _NUMBERS,
    _NAVIGATION,
    _MOUSE,
};

enum custom_keys {
    ENGRAM = SAFE_RANGE,
    SCROLL_TOGGLE_LEFT,
    SCROLL_TOGGLE_RIGHT,
    SCROLL_TOGGLE_LEFT_TEMP,
    SCROLL_TOGGLE_RIGHT_TEMP,
};

enum {
  KC_COMM_SCROLL,
  KC_DOT_SCROLL,
  KC_O_BACK,
  KC_U_FWD,
  KC_L_BACK,
  KC_D_FWD,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_ENGRAM] = LAYOUT(
            KC_B, KC_Y, TD(KC_O_BACK), TD(KC_U_FWD), KC_QUOT,
    KC_Z,   KC_C, KC_I, KC_E, KC_A, TD(KC_COMM_SCROLL),
    KC_GRV, KC_G, KC_J, KC_X, KC_K, KC_MINS,
               KC_DEL, KC_BSPC, KC_SPC,

    KC_LCTL, KC_LSFT,
    _______, MO(_NUMBERS), 
    _______, KC_ESC ,
    KC_BTN2, KC_BTN1,
    TG(_MOUSE), KC_BTN3,

    KC_SCLN, TD(KC_L_BACK), TD(KC_D_FWD), KC_W, KC_V,
    TD(KC_DOT_SCROLL), KC_H, KC_T, KC_S, KC_N, KC_Q,
    KC_SLASH, KC_R, KC_M, KC_F, KC_P, KC_EQL,
          KC_SPC, KC_ENT, KC_TAB,

    KC_RSFT, KC_RCTL,
    MO(_NUMBERS), _______, 
    KC_ESC , _______,
    KC_BTN1, KC_BTN2,
    KC_BTN3, TG(_MOUSE)
  ),
  [_NUMBERS] = LAYOUT(
             KC_F1  , KC_F2  , KC_F3  , KC_F4  , KC_F5  ,
    _______, KC_1   , KC_2   , KC_3   , KC_4   , KC_5   ,
    _______, _______, _______, _______, _______, KC_LBRC,
                      _______, _______, _______,

    _______, _______, 
    _______, _______,
    _______, _______,
    _______, _______,
    _______, _______,

    KC_F6  , KC_F7  , KC_F8  , KC_F9  , KC_F10 ,
    KC_6   , KC_7   , KC_8   , KC_9   , KC_0   , KC_F11,
    KC_RBRC, _______, _______, _______, _______, KC_F12,
             _______, _______, _______,

    _______, _______, 
    _______, _______,
    _______, _______,
    _______, _______,
    _______, _______
  ),
  [_NAVIGATION] = LAYOUT(
             _______, _______, KC_UP  , _______, _______,
    _______, _______, KC_LEFT, KC_DOWN, KC_RGHT, _______,
    _______, _______, _______, _______, _______, _______,
                      _______, _______, _______,

    _______, _______, 
    _______, _______,
    _______, _______,
    _______, _______,
    _______, _______,

    _______, _______, KC_UP  , _______, _______,
    _______, KC_LEFT, KC_DOWN, KC_RGHT, _______, _______,
    _______, _______, _______, _______, _______, _______,
             _______, _______, _______,

    _______, _______, 
    _______, _______,
    _______, _______,
    _______, _______,
    _______, _______
  ),
  [_MOUSE] = LAYOUT(
             _______, _______, SCROLL_TOGGLE_LEFT_TEMP, _______, _______,
    _______, _______, KC_BTN2, KC_BTN3, KC_BTN1, SCROLL_TOGGLE_LEFT,
    _______, _______, _______, _______, _______, _______,
                      _______, _______, _______,

    _______, _______, 
    _______, _______,
    _______, _______,
    _______, _______,
    _______, _______,

    _______, _______, SCROLL_TOGGLE_RIGHT_TEMP, _______, _______,
    SCROLL_TOGGLE_RIGHT, KC_BTN1, KC_BTN3, KC_BTN2, _______, _______,
    _______, _______, _______, _______, _______, _______,
             _______, _______, _______,

    _______, _______, 
    _______, _______,
    _______, _______,
    _______, _______,
    _______, _______
  ),
};

int PLOOPY_DRAGSCROLL_SEMAPHORE = 20;
// drag scroll divisor state
int8_t drag_scroll_x_semaphore_left = 0;
int8_t drag_scroll_y_semaphore_left = 0;
int8_t drag_scroll_x_semaphore_right = 0;
int8_t drag_scroll_y_semaphore_right = 0;

static bool is_drag_scroll_left = false;
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

        mouse_report.h = 0;
        if (mouse_report_x_calc > 5) {
            mouse_report.h = mouse_report_x_calc;
        }
        mouse_report.v = -mouse_report_y_calc;
        mouse_report.x = 0;
        mouse_report.y = 0;
    }
    return mouse_report;
}

report_mouse_t scroll_toggle_right(report_mouse_t mouse_report) {
    if (is_drag_scroll_right){
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

        // TODO figure out deadzone for horizontal scroll
        mouse_report.h = mouse_report_x_calc;
        mouse_report.v = -mouse_report_y_calc;
        mouse_report.x = 0;
        mouse_report.y = 0;
    }
    return mouse_report;
}

uint16_t mouse_rotation_angle = 10;

void rotate_mouse_coordinates(int angle, report_mouse_t *mouse_report) {
    // because pi/180 = 0.017453
    static const float degree = 0.017453f;

    float radians = angle * degree;

    // Need to save these values because we rewrite mouse_report->x immediately but reuse the value to find the rotated y value
    int current_x = mouse_report->x;
    int current_y = mouse_report->y;

    // Calculate rotated x & y, convert back to an int
    mouse_report->x = round(cos(radians) * current_x - sin(radians) * current_y);
    mouse_report->y = round(sin(radians) * current_x + cos(radians) * current_y);
}


report_mouse_t pointing_device_task_combined_user(report_mouse_t left_report, report_mouse_t right_report) {
    rotate_mouse_coordinates(-mouse_rotation_angle, &left_report);
    left_report = scroll_toggle_left(left_report);
    rotate_mouse_coordinates(mouse_rotation_angle, &right_report);
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
            register_code16(KC_DOT);
            tap_hold->is_key = true;
        }
    }
}

void tap_dance_tap_hold_reset_right(tap_dance_state_t *state, void *user_data) {
    tap_dance_scroll_t *tap_hold = (tap_dance_scroll_t *)user_data;

    if (tap_hold->is_key) {
        unregister_code16(KC_DOT);
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
            register_code16(KC_COMM);
            tap_hold->is_key = true;
        }
    }
}

void tap_dance_tap_hold_reset_left(tap_dance_state_t *state, void *user_data) {
    tap_dance_scroll_t *tap_hold = (tap_dance_scroll_t *)user_data;

    if (tap_hold->is_key) {
        unregister_code16(KC_COMM);
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
    [KC_DOT_SCROLL] = { .fn = {NULL, tap_dance_tap_hold_finished_right, tap_dance_tap_hold_reset_right}, .user_data = (void *)&((tap_dance_scroll_t){false,false}), },
    [KC_COMM_SCROLL] = { .fn = {NULL, tap_dance_tap_hold_finished_left, tap_dance_tap_hold_reset_left}, .user_data = (void *)&((tap_dance_scroll_t){false,false}), },
    [KC_O_BACK] = ACTION_TAP_DANCE_TAP_HOLD(KC_O, KC_BTN4),
    [KC_U_FWD] = ACTION_TAP_DANCE_TAP_HOLD(KC_U, KC_BTN5),
    [KC_L_BACK] = ACTION_TAP_DANCE_TAP_HOLD(KC_L, KC_BTN4),
    [KC_D_FWD] = ACTION_TAP_DANCE_TAP_HOLD(KC_D, KC_BTN5),
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
        case SCROLL_TOGGLE_LEFT_TEMP:
          if (record->event.pressed) {
            is_drag_scroll_left ^= 1;
          } else {
            is_drag_scroll_left ^= 1;
          }
          return false;
        case SCROLL_TOGGLE_RIGHT_TEMP:
          if (record->event.pressed) {
            is_drag_scroll_right ^= 1;
          } else {
            is_drag_scroll_right ^= 1;
          }
          return false;
        case TD(KC_DOT_SCROLL):
          action = &tap_dance_actions[TD_INDEX(keycode)];
          if (!record->event.pressed && action->state.count && !action->state.finished) {
              tap_code16(KC_DOT);
          }
          return true;
        case TD(KC_COMM_SCROLL):
          action = &tap_dance_actions[TD_INDEX(keycode)];
          if (!record->event.pressed && action->state.count && !action->state.finished) {
              tap_code16(KC_COMM);
          }
          return true;
        case TD(KC_O_BACK):
        case TD(KC_U_FWD):
        case TD(KC_L_BACK):
        case TD(KC_D_FWD):
            action = &tap_dance_actions[TD_INDEX(keycode)];
            if (!record->event.pressed && action->state.count && !action->state.finished) {
                tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
                tap_code16(tap_hold->tap);
            }

    }
    return true;
}



