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
volatile bool led_status_GREEN = false; // Estado do LED comum verde
volatile bool led_status_BLUE = false; // Estado do LED comum azul
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

// Função auxiliar para alternar e exibir estado do LED
void toggle_led(uint pin_led, volatile bool *led_status, const char *button, const char *color) {
    *led_status = !*led_status;
    gpio_put(pin_led, *led_status);
    printf("Botão %s pressionado: LED %s %s \n", button, color, *led_status ? MSG_IS_ON : MSG_IS_OFF);
}

// Função de interrupção para o botão A
void button_isr(uint gpio, uint32_t events) {
    uint64_t current_time = time_us_64();
    if (current_time - last_interrupt_time < DEBOUNCE_TIME) return;
    last_interrupt_time = current_time;

    if (gpio == PIN_BUTTON_A) {
        toggle_led(PIN_LED_GREEN, &led_status_GREEN, "A", "VERDE");
    }
    if (gpio == PIN_BUTTON_B) {
        toggle_led(PIN_LED_BLUE, &led_status_BLUE, "B", "AZUL");
    }
}

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

void display_character_on_oled(char character) {
    char text[2] = {character, '\0'}; // Criamos uma string com o caractere

    ssd1306_fill(&ssd, false);  // Limpa o display
    ssd1306_draw_string(&ssd, text, 50, 25); // Desenha o caractere na posição central
    ssd1306_send_data(&ssd);   // Atualiza o display
}

void read_character() {
    if (stdio_usb_connected() && scanf("%c", &character) == 1) {
        printf("Caractere recebido: %c\n", character);

        // Se for um número, exibe na matriz de LED
        if (is_valid_number(character)) {
            int number = character - '0';
            display_number(number);
        }
        
        // Exibir no display OLED
        display_character_on_oled(character);
    }
}

int main() {
    // Inicialização da comunicação serial USB CDC 
    stdio_init_all();

    // Inicialização do display
    init_display_settings();

    // Inicialização das configurações dos pinos
    init_gpio_settings();

    // Configuração de interrupção para os botões
    gpio_set_irq_enabled_with_callback(PIN_BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_isr);
    gpio_set_irq_enabled(PIN_BUTTON_B, GPIO_IRQ_EDGE_FALL, true);

    while (true) {
        read_character();
        sleep_ms(TIME_SLEEP);
    }
}
