#include "led_control.h"
#include "ws2818b.pio.h"

npLED_t leds[TOTAL_LEDS]; // Array que representa os LEDs
PIO np_pio; // Objeto representando o periférico PIO usado para controlar os LEDs
uint sm; // Representa o estado da máquina de estados do PIO

// Função que define o estado do led comum
void set_led_color(uint pin, bool status) {
    gpio_put(pin, status);
}

// Função para definir a cor de um LED específico na matriz
void set_led_rgb(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
}

// Inicializa a máquina PIO para controle da matriz de LEDs.
void init_led_matrix(uint pin) {
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0;
    sm = pio_claim_unused_sm(np_pio, true);

    // Inicia programa na máquina PIO obtida.
    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);

    // Limpa buffer de pixels
    for (uint i = 0; i < TOTAL_LEDS; ++i) {
        set_led_rgb(i, 0, 0, 0);
    }
}

// Função para enviar o estado atual dos LEDs ao hardware  - buffer de saída
void buffer() {
    for (uint i = 0; i < TOTAL_LEDS; ++i) {
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
}

// Função para obter o índice do LED com base nas coordenadas x e y.
int get_index(int x, int y) {
    // Se a linha for par (0, 2, 4), percorremos da esquerda para a direita.
    // Se a linha for ímpar (1, 3), percorremos da direita para a esquerda.
    if (y % 2 == 0) {
        return 24-(y * 5 + x);
    } else {
        return 24-(y * 5 + (4 - x));
    }
}
