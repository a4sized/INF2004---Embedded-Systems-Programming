#ifndef MOTIONCONTROL_H
#define MOTIONCONTROL_H

#include <stdint.h>
#include <stdbool.h>
#include <tk/tkernel.h>

// declaring motor pins
#define M1A_PIN 8  // Left forward
#define M1B_PIN 9  // Left backward
#define M2A_PIN 10 // Right forward
#define M2B_PIN 11 // Right backward

// declaring encoder pins 
#define LEFT_ENC_A  14  // Left encoder A
#define LEFT_ENC_B  15  // Left encoder B
#define RIGHT_ENC_A 2   // Right encoder A
#define RIGHT_ENC_B 3   // Right encoder B  
#define ENC2_PWR_PIN 16 

extern volatile int32_t left_encoder_ticks;
extern volatile int32_t right_encoder_ticks;

void motor_init(void);
void motor_speed(int16_t left_speed, int16_t right_speed);
void motor_duration(int16_t left_speed, int16_t right_speed, uint32_t duration_ms);
void motor_reset_ticks(void);

#endif 