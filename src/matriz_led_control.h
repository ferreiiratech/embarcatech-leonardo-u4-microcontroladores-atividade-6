#ifndef MATRIZ_LED_CONTROL_H
#define MATRIZ_LED_CONTROL_H

#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#define TOTAL_LEDS 25

struct pixel_t {
    uint8_t G, R, B;
};
typedef struct pixel_t npLED_t;

void set_led_color(uint pin, bool status);
void init_led_matrix(uint pin);
void set_led_rgb(const uint index, const uint8_t r, const uint8_t g, const uint8_t b);
void buffer(void);
int get_index(int x, int y);

#endif