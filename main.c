#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"

// Cytron Robo Pico Motor Pins
#define M1A_PIN 8   // GP8  (Left Motor Forward)
#define M1B_PIN 9   // GP9  (Left Motor Reverse)
#define M2A_PIN 10  // GP10 (Right Motor Forward)
#define M2B_PIN 11  // GP11 (Right Motor Reverse)

// Encoder Pins (Yahboom Setup)
#define LEFT_ENC_A  14  // Red Wire (GP14)
#define LEFT_ENC_B  15  // Green Wire (GP15)
#define RIGHT_ENC_A 2  // Red Wire (GP2)
#define RIGHT_ENC_B 3  // Green Wire (GP3)

static volatile int32_t left_encoder_ticks = 0;
static volatile int32_t right_encoder_ticks = 0;

// Quadrature Encoder ISR for both motors
static void encoder_callback(uint gpio, uint32_t events) {
    if (gpio == LEFT_ENC_A) {
        if (gpio_get(LEFT_ENC_B)) {
            left_encoder_ticks++;
        } else {
            left_encoder_ticks--;
        }
    } else if (gpio == RIGHT_ENC_A) {
        if (gpio_get(RIGHT_ENC_B)) {
            right_encoder_ticks++;
        } else {
            right_encoder_ticks--;
        }
    }
}

void motor_init(void) {
    // 1. Configure GP8, GP9, GP10, GP11 as PWM
    gpio_set_function(M1A_PIN, GPIO_FUNC_PWM);
    gpio_set_function(M1B_PIN, GPIO_FUNC_PWM);
    gpio_set_function(M2A_PIN, GPIO_FUNC_PWM);
    gpio_set_function(M2B_PIN, GPIO_FUNC_PWM);

    uint slice_m1a = pwm_gpio_to_slice_num(M1A_PIN);
    uint slice_m1b = pwm_gpio_to_slice_num(M1B_PIN);
    uint slice_m2a = pwm_gpio_to_slice_num(M2A_PIN);
    uint slice_m2b = pwm_gpio_to_slice_num(M2B_PIN);

    pwm_set_wrap(slice_m1a, 255);
    pwm_set_wrap(slice_m1b, 255);
    pwm_set_wrap(slice_m2a, 255);
    pwm_set_wrap(slice_m2b, 255);

    pwm_set_enabled(slice_m1a, true);
    pwm_set_enabled(slice_m1b, true);
    pwm_set_enabled(slice_m2a, true);
    pwm_set_enabled(slice_m2b, true);

    // Initial stop state
    pwm_set_gpio_level(M1A_PIN, 0);
    pwm_set_gpio_level(M1B_PIN, 0);
    pwm_set_gpio_level(M2A_PIN, 0);
    pwm_set_gpio_level(M2B_PIN, 0);

    // 2. Configure Encoder Inputs with Pull-ups
    gpio_init(LEFT_ENC_A);  gpio_set_dir(LEFT_ENC_A, GPIO_IN);  gpio_pull_up(LEFT_ENC_A);
    gpio_init(LEFT_ENC_B);  gpio_set_dir(LEFT_ENC_B, GPIO_IN);  gpio_pull_up(LEFT_ENC_B);
    gpio_init(RIGHT_ENC_A); gpio_set_dir(RIGHT_ENC_A, GPIO_IN); gpio_pull_up(RIGHT_ENC_A);
    gpio_init(RIGHT_ENC_B); gpio_set_dir(RIGHT_ENC_B, GPIO_IN); gpio_pull_up(RIGHT_ENC_B);

    // Set callback once, then enable IRQ for both Channel A pins
    gpio_set_irq_enabled_with_callback(LEFT_ENC_A, GPIO_IRQ_EDGE_RISE, true, &encoder_callback);
    gpio_set_irq_enabled(RIGHT_ENC_A, GPIO_IRQ_EDGE_RISE, true);
}

// Separate control for individual motor speeds (-255 to +255)
void motor_set_speeds(int16_t left_speed, int16_t right_speed) {
    // Clamp values
    if (left_speed > 255) left_speed = 255;
    if (left_speed < -255) left_speed = -255;
    if (right_speed > 255) right_speed = 255;
    if (right_speed < -255) right_speed = -255;

    // Left Motor (M1)
    if (left_speed > 0) {
        pwm_set_gpio_level(M1B_PIN, 0);
        pwm_set_gpio_level(M1A_PIN, left_speed);
    } else if (left_speed < 0) {
        pwm_set_gpio_level(M1A_PIN, 0);
        pwm_set_gpio_level(M1B_PIN, -left_speed);
    } else {
        pwm_set_gpio_level(M1A_PIN, 0);
        pwm_set_gpio_level(M1B_PIN, 0);
    }

    // Right Motor (M2)
    if (right_speed > 0) {
        pwm_set_gpio_level(M2B_PIN, 0);
        pwm_set_gpio_level(M2A_PIN, right_speed);
    } else if (right_speed < 0) {
        pwm_set_gpio_level(M2A_PIN, 0);
        pwm_set_gpio_level(M2B_PIN, -right_speed);
    } else {
        pwm_set_gpio_level(M2A_PIN, 0);
        pwm_set_gpio_level(M2B_PIN, 0);
    }
}

int main() {
    stdio_init_all();
    motor_init();

    sleep_ms(3000);

    left_encoder_ticks = 0;
    right_encoder_ticks = 0;

    motor_set_speeds(50, -45);
    for (int i = 0; i < 30; i++) {
        printf("[%02d/30] Left Ticks: %ld | Right Ticks: %ld\n", 
               i + 1, left_encoder_ticks, right_encoder_ticks);
        sleep_ms(100);
    }
    
    motor_set_speeds(0, 0);
    sleep_ms(1000);

    motor_set_speeds(0, -45);
    for (int i = 0; i < 10; i++) {
        printf("[%02d/30] Left Ticks: %ld | Right Ticks: %ld\n", 
               i + 1, left_encoder_ticks, right_encoder_ticks);
        sleep_ms(100);
    }

    motor_set_speeds(0, 0);
    sleep_ms(1000);

    motor_set_speeds(50, -45);
    for (int i = 0; i < 30; i++) {
        printf("[%02d/30] Left Ticks: %ld | Right Ticks: %ld\n", 
               i + 1, left_encoder_ticks, right_encoder_ticks);
        sleep_ms(100);
    }

    motor_set_speeds(0, 0);
    sleep_ms(1000);

    motor_set_speeds(45, 0);
    for (int i = 0; i < 10; i++) {
        printf("[%02d/30] Left Ticks: %ld | Right Ticks: %ld\n", 
               i + 1, left_encoder_ticks, right_encoder_ticks);
        sleep_ms(100);
    }

    motor_set_speeds(0, 0);
    sleep_ms(1000);

    motor_set_speeds(-50, 45);
    for (int i = 0; i < 30; i++) {
        printf("[%02d/30] Left Ticks: %ld | Right Ticks: %ld\n", 
               i + 1, left_encoder_ticks, right_encoder_ticks);
        sleep_ms(100);
    }

    motor_set_speeds(0, 0);

    while (true) {
        tight_loop_contents();
    }

    return 0;
}