#include <stdio.h>
#include "pico/stdlib.h"

#define PIN_LED_RED 13   // Pino do LED vermelho
#define PIN_LED_GREEN 11 // Pino do LED verde
#define PIN_LED_BLUE 12  // Pino do LED azul
#define TIME_SLEEP 100   // Tempo de espera em milissegundos para o led piscar 5 vezes por segundo, 1000/5 = 200ms e um ciclo leva 100ms*2 = 200ms
#define PIN_MATRIZ_LED 7 // Pino da matriz de LEDs
#define PIN_BUTTON_A 5   // Pino do botão A
#define PIN_BUTTON_B 6   // Pino do botão B

void init_gpio_settings()
{
    // Inicialização do pino vermelho
    gpio_init(PIN_LED_RED);
    gpio_set_dir(PIN_LED_RED, GPIO_OUT);

    // Inicialização do pino verde
    gpio_init(PIN_LED_GREEN);
    gpio_set_dir(PIN_LED_GREEN, GPIO_OUT);

    // Inicialização do pino azul
    gpio_init(PIN_LED_BLUE);
    gpio_set_dir(PIN_LED_BLUE, GPIO_OUT);

    // Configuração do botão A
    gpio_init(PIN_BUTTON_A);
    gpio_set_dir(PIN_BUTTON_A, GPIO_IN);
    gpio_pull_up(PIN_BUTTON_A);

    // Configuração do botão B
    gpio_init(PIN_BUTTON_B);
    gpio_set_dir(PIN_BUTTON_B, GPIO_IN);
    gpio_pull_up(PIN_BUTTON_B);
}

int main()
{
    // Inicialização da comunicação serial
    stdio_init_all();

    // Inicialização das configurações dos pinos
    init_gpio_settings();

    while (true)
    {
        printf("Hello, world!\n");
        sleep_ms(TIME_SLEEP);
    }
}
