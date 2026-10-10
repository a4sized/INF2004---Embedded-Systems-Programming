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


typedef enum{ // motion states indicated by numbers

    MOTOR_STOPPED = 0,
    MOTOR_RAMPING = 1,
    MOTOR_STEADY_DRIVE = 2,
    MOTOR_REVERSING = 3,
    MOTOR_SAFETY_HALT = 4
} MotionState_t;

void motor_init(void);
void motor_speed(int16_t left_speed, int16_t right_speed);
void motor_duration(int16_t left_speed, int16_t right_speed, uint32_t duration_ms);
void motor_reset_ticks(void);
void motion_fsm_update(int16_t left_speed, int16_t right_speed);
MotionState_t motion_fsm_get_state(void);


/*typedef enum{ // motor fault codes indicated by numbers

    FAULT_NONE = 0x00,
    FAULT_STALL_LEFT = 0x01,
    FAULT_STALL_RIGHT = 0x02,
    FAULT_OVERCURRENT = 0x04,
    FAULT_EMERGENCY_STOP = 0x08,
} MotorFault_t;*/

void motion_trigger_safety_halt(FaultCode_t cause);
bool motion_is_fault_active(void);
FaultCode_t motion_get_fault_code(void);
bool motion_clear_fault_code(void);


int32_t get_left_encoder_ticks(void);
int32_t get_right_encoder_ticks(void);


#endif 