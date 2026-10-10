#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <tk/tkernel.h>
#include "buzzer.h"

void buzzer_init(void){
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_put(BUZZER_PIN, 0);
}

void buzzer_play_tone(uint32_t frequency_hz, uint32_t duration_ms){

    if (frequency_hz == 0) return;

    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);

    uint32_t clock_freq = 125000000U;
    uint32_t divider = 125U; 
    uint32_t wrap = clock_freq / (divider * frequency_hz) - 1U;

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, (float)divider);
    pwm_config_set_wrap(&config, wrap);

    pwm_init(slice_num, &config, true);
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(BUZZER_PIN), wrap / 2U);

    tk_dly_tsk(duration_ms);

    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(BUZZER_PIN), 0U);
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_SIO);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_put(BUZZER_PIN, 0);
}




void buzzer_play_boot_chime(void){
    buzzer_play_tone(523, 80);  // Note C5
    tk_dly_tsk(20);
    buzzer_play_tone(659, 80);  // Note E5
    tk_dly_tsk(20);
    buzzer_play_tone(784, 120); // Note G5
}

void buzzer_play_beep(void){
    buzzer_play_tone(2000, 60); // 2 kHz short chirp
}

void buzzer_play_alarm(void){ // can be used for faults
    for (int i = 0; i < 3; i++){
        buzzer_play_tone(3000, 60); // High alarm
        tk_dly_tsk(30);
        buzzer_play_tone(1500, 60); // Low alarm
        tk_dly_tsk(30);
    }
}