#include "motionControl.h"
#include <stdio.h>
#include <hardware/pwm.h>
#include <hardware/gpio.h>

volatile int32_t left_encoder_ticks = 0;
volatile int32_t right_encoder_ticks = 0;
static int16_t active_left_speed = 0;
static int16_t active_right_speed = 0;
static uint8_t reverse_delay_counter = 0;


void motor_reset_ticks(void){
    left_encoder_ticks = 0;
    right_encoder_ticks = 0;    
}


static void encoder_callback(uint gpio, uint32_t events){

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


void motor_init(void){ // configure GP8, GP9, GP10, GP11 as PWM
    
    // set motor pins to PWM function
    gpio_set_function(M1A_PIN, GPIO_FUNC_PWM);
    gpio_set_function(M1B_PIN, GPIO_FUNC_PWM);
    gpio_set_function(M2A_PIN, GPIO_FUNC_PWM);
    gpio_set_function(M2B_PIN, GPIO_FUNC_PWM);

    // get PWM slice numbers for each motor pin
    uint slice_m1a = pwm_gpio_to_slice_num(M1A_PIN);
    uint slice_m1b = pwm_gpio_to_slice_num(M1B_PIN);
    uint slice_m2a = pwm_gpio_to_slice_num(M2A_PIN);
    uint slice_m2b = pwm_gpio_to_slice_num(M2B_PIN);

    // clamp values to 255
    pwm_set_wrap(slice_m1a, 255);
    pwm_set_wrap(slice_m1b, 255);
    pwm_set_wrap(slice_m2a, 255);
    pwm_set_wrap(slice_m2b, 255);

    // enable all PWM slices
    pwm_set_enabled(slice_m1a, true);
    pwm_set_enabled(slice_m1b, true);
    pwm_set_enabled(slice_m2a, true);
    pwm_set_enabled(slice_m2b, true);

    // initial stop state
    pwm_set_gpio_level(M1A_PIN, 0);
    pwm_set_gpio_level(M1B_PIN, 0);
    pwm_set_gpio_level(M2A_PIN, 0);
    pwm_set_gpio_level(M2B_PIN, 0);

    // conigure encoder inputs with pull-ups
    gpio_init(LEFT_ENC_A);  gpio_set_dir(LEFT_ENC_A, GPIO_IN);  gpio_pull_up(LEFT_ENC_A);
    gpio_init(LEFT_ENC_B);  gpio_set_dir(LEFT_ENC_B, GPIO_IN);  gpio_pull_up(LEFT_ENC_B);
    gpio_init(RIGHT_ENC_A); gpio_set_dir(RIGHT_ENC_A, GPIO_IN); gpio_pull_up(RIGHT_ENC_A);
    gpio_init(RIGHT_ENC_B); gpio_set_dir(RIGHT_ENC_B, GPIO_IN); gpio_pull_up(RIGHT_ENC_B);

    // set callback once, then enable IRQ for both Channel A pins
    gpio_set_irq_enabled_with_callback(LEFT_ENC_A, GPIO_IRQ_EDGE_RISE, true, &encoder_callback);
    gpio_set_irq_enabled(RIGHT_ENC_A, GPIO_IRQ_EDGE_RISE, true);
}


void motor_speed(int16_t left_speed, int16_t right_speed){ // initialize motor speed function
    
    // clamp values
    if (left_speed > 255) left_speed = 255;
    if (left_speed < -255) left_speed = -255;
    if (right_speed > 255) right_speed = 255;
    if (right_speed < -255) right_speed = -255;

    // left motor
    if (left_speed > 0){
        pwm_set_gpio_level(M1B_PIN, 0);
        pwm_set_gpio_level(M1A_PIN, left_speed);
    } else if (left_speed < 0){
        pwm_set_gpio_level(M1A_PIN, 0);
        pwm_set_gpio_level(M1B_PIN, -left_speed);
    } else{
        pwm_set_gpio_level(M1A_PIN, 0);
        pwm_set_gpio_level(M1B_PIN, 0);
    }

    // right motor
    if (right_speed > 0){
        pwm_set_gpio_level(M2B_PIN, 0);
        pwm_set_gpio_level(M2A_PIN, right_speed);
    } else if (right_speed < 0){
        pwm_set_gpio_level(M2A_PIN, 0);
        pwm_set_gpio_level(M2B_PIN, -right_speed);
    } else{
        pwm_set_gpio_level(M2A_PIN, 0);
        pwm_set_gpio_level(M2B_PIN, 0);
    }
}


static MotionState_t current_state = MOTOR_STOPPED;
static volatile bool system_fault_flag = false;
static volatile FaultCode_t active_fault_code = FAULT_NONE;


void motion_trigger_safety_halt(FaultCode_t cause){

    system_fault_flag = true;
    active_fault_code = cause;
    current_state = MOTOR_SAFETY_HALT; // immediate override
    motor_speed(0, 0); // forcibly cut power
}

bool motion_is_fault_active(void){
    return system_fault_flag;
}

FaultCode_t motion_get_fault_code(void){
    return active_fault_code;
}

bool motion_clear_fault_code(void){
    system_fault_flag = false;
    active_fault_code = FAULT_NONE;
    current_state = MOTOR_STOPPED; // transition to stopped state
    return true;
}


static int16_t ramp_step(int16_t current, int16_t target, int16_t step_size){
    if (current < target){
        current += step_size;
        if (current > target){
            current = target;
        } else if (current > target){
            current -= step_size;
            if (current < target){
                current = target;
            }
        }
    }
    return current;
}





// state handler functions


// state 0
static void handle_motor_stopped(int16_t left_speed, int16_t right_speed){
    
    active_left_speed = 0;
    active_right_speed = 0;

    motor_speed(0, 0);
    if (left_speed != 0 || right_speed != 0){
        current_state = MOTOR_RAMPING;
    }
}

// state 1
static void handle_motor_ramping(int16_t left_speed, int16_t right_speed){

    active_left_speed = ramp_step(active_left_speed, left_speed, 10);
    active_right_speed = ramp_step(active_right_speed, right_speed, 10);
   
    motor_speed(active_left_speed, active_right_speed);
    if (active_left_speed == left_speed && active_right_speed == right_speed){
        current_state = MOTOR_STEADY_DRIVE;
    }
}

// state 2
static void handle_motor_steady_drive(int16_t left_speed, int16_t right_speed){

    bool left_reversing = (left_speed > 0 && active_left_speed < 0) || (left_speed < 0 && active_left_speed > 0);
    bool right_reversing = (right_speed > 0 && active_right_speed < 0) || (right_speed < 0 && active_right_speed > 0);

    if (left_reversing || right_reversing){
        reverse_delay_counter = 0; // reset counter when a new reverse command is invoked
        current_state = MOTOR_REVERSING;
        return;
    }

    if (left_speed == 0 && right_speed == 0){
        current_state = MOTOR_RAMPING; // ramp down to 0
        return;
    }
    
    active_left_speed = left_speed;
    active_right_speed = right_speed;
    motor_speed(active_left_speed, active_right_speed);
}

// state 3
static void handle_motor_reversing(int16_t left_speed, int16_t right_speed){

    active_left_speed = 0;
    active_right_speed = 0;
    motor_speed(0, 0);

    reverse_delay_counter++;
    if (reverse_delay_counter >= 5){
        reverse_delay_counter = 0;
        current_state = MOTOR_RAMPING; // ramp to new speed
    }
}

// state 4
static void handle_motor_safety_halt(int16_t left_speed, int16_t right_speed){

    active_left_speed = 0;
    active_right_speed = 0;
    motor_speed(0, 0);

    if (!system_fault_flag && left_speed == 0 && right_speed == 0){
        current_state = MOTOR_STOPPED;
    }
}





// main state machine update loop

void motion_fsm_update(int16_t left_speed, int16_t right_speed){

    if (system_fault_flag){
        current_state = MOTOR_SAFETY_HALT;
    }

    switch (current_state){

        case MOTOR_STOPPED:
            handle_motor_stopped(left_speed, right_speed);
            break;

        case MOTOR_RAMPING:
            handle_motor_ramping(left_speed, right_speed);
            break;

        case MOTOR_STEADY_DRIVE:
            handle_motor_steady_drive(left_speed, right_speed);
            break;

        case MOTOR_REVERSING:
            handle_motor_reversing(left_speed, right_speed);
            break;

        case MOTOR_SAFETY_HALT:
            handle_motor_safety_halt(left_speed, right_speed);
            break;

        default:
            current_state = MOTOR_STOPPED;
            break;

    }
}

MotionState_t motion_fsm_get_state(void){
    return current_state;
}