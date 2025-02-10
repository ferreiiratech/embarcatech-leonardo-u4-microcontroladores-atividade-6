#include <stdio.h>
#include "pico/stdlib.h"

#define PIN_LED_RED 13   // Pino do LED vermelho
#define PIN_LED_GREEN 11 // Pino do LED verde
#define PIN_LED_BLUE 12  // Pino do LED azul
#define TIME_SLEEP 100   // Tempo de espera
#define PIN_MATRIZ_LED 7 // Pino da matriz de LEDs
#define PIN_BUTTON_A 5   // Pino do botão A
#define PIN_BUTTON_B 6   // Pino do botão B
#define DEBOUNCE_TIME 200000 // Tempo de debounce em microssegundos
#define MSG_IS_ON "ACESO"
#define MSG_IS_OFF "APAGADO"

volatile uint64_t last_interrupt_time = 0; // Tempo da última interrupção do botão A
volatile bool led_status_GREEN = false; // Estado do LED comum verde
volatile bool led_status_BLUE = false; // Estado do LED comum azul
char character; // Caractere a ser exibido

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

// Função auxiliar para alternar e exibir estado do LED
void toggle_led(uint pin_led, volatile bool *led_status, const char *button, const char *color){
    // Inverte o estado do LED comum
    *led_status = !*led_status;
    gpio_put(pin_led, *led_status);

    // Exibe o estado do LED na saída padrão
    printf("Botão %s pressionado: LED %s %s \n", button, color, *led_status ? MSG_IS_ON : MSG_IS_OFF);
}

void button_A_isr(uint gpio, uint32_t events){
    uint64_t current_time = time_us_64(); // Obtém o tempo atual em microssegundos

    // configuração do debounce
    if (current_time - last_interrupt_time < DEBOUNCE_TIME) return; // Ignora bouncing caso o tempo entre interrupções seja menor que 200ms
    last_interrupt_time = current_time;

    // Lógica para o botão A
    if (gpio == PIN_BUTTON_A) {
        toggle_led(PIN_LED_GREEN, &led_status_GREEN, "A", "VERDE");
        printf("Caractere recebidoo: %c\n", character);
    }

    // Lógica para o botão B
    if (gpio == PIN_BUTTON_B) {
        toggle_led(PIN_LED_BLUE, &led_status_BLUE, "B", "AZUL");
    }
}

void read_character() {
    if(stdio_usb_connected() && scanf("%c", &character) == 1) {
        printf("Caractere recebido: %c\n", character);
    }
}

int main() {
    // Inicialização da comunicação serial USB CDC
    stdio_init_all();
    
    // Inicialização das configurações dos pinos
    init_gpio_settings();

    gpio_set_irq_enabled_with_callback(PIN_BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_A_isr);
    gpio_set_irq_enabled(PIN_BUTTON_B, GPIO_IRQ_EDGE_FALL, true);

    while (true)
    {
        read_character();
        sleep_ms(TIME_SLEEP);
    }
}
