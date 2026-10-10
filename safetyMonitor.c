#include <stdio.h>
#include "hardware/adc.h"
#include "pico/stdlib.h"
#include <tk/tkernel.h>

#include "systemFaults.h"
#include "motionControl/motionControl.h"

volatile uint32_t active_fault_mask = FAULT_NONE;

static void run_all_safety_checks(void){

    if (get_left_pwm_duty() > 0 && get_left_encoder_ticks() == 0){
        SET_FAULT(active_fault_mask, FAULT_MOTOR_LEFT_STALL);
    }
    if (read_battery_voltage() < 6.4f){
        SET_FAULT(active_fault_mask, FAULT_MOTOR_UNDERVOLTAGE);
    }
















}


void task_safety_monitor(INT stacd, void *param){
    while (1){
        
        run_all_safety_checks();

        if (HAS_FAULT(active_fault_mask, FAULT_SYS_EMERGENCY_STOP) ||
            HAS_FAULT(active_fault_mask, FAULT_US_OBSTACLE_CRITICAL) ||
            HAS_FAULT(active_fault_mask, FAULT_MOTOR_LEFT_STALL)){
            
            motor_stop_all();
        }
        tk_dly_tsk(10); // delay 10ms
    }
}
