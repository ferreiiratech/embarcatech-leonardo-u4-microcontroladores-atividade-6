#include <stdio.h>
#include "pico/stdlib.h"
#include "src/animation_matriz_led.h"
#include "src/matriz_led_control.h"
#include "hardware/i2c.h"
#include "src/lib/ssd1306.h"

#define PIN_LED_RED 13 // Pino do LED vermelho
#define PIN_LED_GREEN 11 // Pino do LED verde
#define PIN_LED_BLUE 12 // Pino do LED azul
#define TIME_SLEEP 100 // Tempo de espera
#define PIN_MATRIZ_LED 7 // Pino da matriz de LEDs
#define PIN_BUTTON_A 5 // Pino do botão A
#define PIN_BUTTON_B 6 // Pino do botão B
#define DEBOUNCE_TIME 200000 // Tempo de debounce em microssegundos
#define MSG_IS_ON "ACESO"
#define MSG_IS_OFF "APAGADO"
#define I2C_PORT i2c1 // Porta I2C
#define I2C_SDA 14 // Pino SDA
#define I2C_SCL 15 // Pino SCL
#define ENDERECO 0x3C // Endereço do display OLED

volatile uint64_t last_interrupt_time = 0; // Tempo da última interrupção do botão A
volatile bool led_status_GREEN = false; // Estado do LED verde
volatile bool led_status_BLUE = false; // Estado do LED azul
char character; // Caractere a ser exibido
ssd1306_t ssd; // Inicializa a estrutura do display

void init_gpio_settings() {
    // Inicialização do pino vermelho
    gpio_init(PIN_LED_RED);
    gpio_set_dir(PIN_LED_RED, GPIO_OUT);

    // Inicialização do pino verde
    gpio_init(PIN_LED_GREEN);
    gpio_set_dir(PIN_LED_GREEN, GPIO_OUT);

    // Inicialização do pino azul
    gpio_init(PIN_LED_BLUE);
    gpio_set_dir(PIN_LED_BLUE, GPIO_OUT);

    // Inicialização do pino do botão A
    gpio_init(PIN_BUTTON_A);
    gpio_set_dir(PIN_BUTTON_A, GPIO_IN);
    gpio_pull_up(PIN_BUTTON_A);

    // Inicialização do pino do botão B
    gpio_init(PIN_BUTTON_B);
    gpio_set_dir(PIN_BUTTON_B, GPIO_IN);
    gpio_pull_up(PIN_BUTTON_B);

    // Inicialização do pino da matriz de LEDs
    init_led_matrix(PIN_MATRIZ_LED);
}

// Exibe mensagem no display OLED
void display_message_on_oled(const char *message) {
    ssd1306_fill(&ssd, false); // Limpa o display
    ssd1306_draw_string(&ssd, message, 10, 25); // Exibe a mensagem no OLED
    ssd1306_send_data(&ssd); // Atualiza o display
}

// Alterna estado do LED e exibe mensagem no OLED
void toggle_led(uint pin_led, volatile bool *led_status, const char *button, const char *color) {
    *led_status = !*led_status;
    gpio_put(pin_led, *led_status);

    char msg[30];
    snprintf(msg, sizeof(msg), "LED %s %s", color, *led_status ? MSG_IS_ON : MSG_IS_OFF);

    printf("Botão %s pressionado: %s\n", button, msg);
    display_message_on_oled(msg);
}

// Interrupção para os botões
void button_isr(uint gpio, uint32_t events) {
    // Configuração que evita debounce
    uint64_t current_time = time_us_64();
    if (current_time - last_interrupt_time < DEBOUNCE_TIME) return; // Ignora bouncing caso o tempo entre interrupções seja menor que 200ms
    last_interrupt_time = current_time;

    // Verifica qual botão foi pressionado
    if (gpio == PIN_BUTTON_A) {
        toggle_led(PIN_LED_GREEN, &led_status_GREEN, "A", "VERDE");
    }
    if (gpio == PIN_BUTTON_B) {
        toggle_led(PIN_LED_BLUE, &led_status_BLUE, "B", "AZUL");
    }
}

// Verifica se o caractere é um número
bool is_valid_number(char character) {
    return character >= '0' && character <= '9';
}

void init_display_settings() {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
}

// Exibe caractere no display OLED
void display_character_on_oled(char character) {
    char text[2] = {character, '\0'}; // Cria string com o caractere
    display_message_on_oled(text);
}

void read_character() {
    if (stdio_usb_connected() && scanf("%c", &character) == 1) {
        printf("Caractere recebido: %c\n", character);

        if (is_valid_number(character)) {
            int number = character - '0';
            display_number(number);
        }

        display_character_on_oled(character);
    }
}

int main() {
    // Inicialização do GPIO e configuração do display OLED
    stdio_init_all();
    init_display_settings();
    init_gpio_settings();

    // Configuração das interrupções para os botões
    gpio_set_irq_enabled_with_callback(PIN_BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_isr);
    gpio_set_irq_enabled(PIN_BUTTON_B, GPIO_IRQ_EDGE_FALL, true);

    while (true) {
        // Leitura do caractere do teclado
        read_character();
        sleep_ms(TIME_SLEEP);
    }
}
