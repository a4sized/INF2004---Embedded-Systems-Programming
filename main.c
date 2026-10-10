/**
 * @file main.c
 * @brief Diagnostic test runner for systemFaults bitmask and safety monitoring.
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include <tk/tkernel.h>

#include "systemFaults.h"
#include "motionControl/motionControl.h"

// Master fault register instance
volatile uint32_t active_fault_mask = FAULT_NONE;

/**
 * @brief Test Task: Simulates faults and validates bitwise operations
 */
LOCAL void task_diagnostic_test(INT stacd, void *param) {
    // Wait 3 seconds after boot for USB CDC Serial terminal to connect
    tk_dly_tsk(3000);

    printf("\n==================================================\n");
    printf("   RPI PICO W - SYSTEM FAULTS DIAGNOSTIC TEST     \n");
    printf("==================================================\n\n");

    // -------------------------------------------------------------------------
    // TEST 1: Initial State Check
    // -------------------------------------------------------------------------
    printf("[TEST 1] Initializing system register...\n");
    printf("Initial Mask: 0x%08X\n", active_fault_mask);
    if (!HAS_ANY_FAULT(active_fault_mask)) {
        printf(" -> PASS: System is nominal (0x00000000).\n\n");
    } else {
        printf(" -> FAIL: Initial mask is non-zero!\n\n");
    }

    // -------------------------------------------------------------------------
    // TEST 2: Triggering Motor Faults (Bits 0 & 3)
    // -------------------------------------------------------------------------
    printf("[TEST 2] Raising Motor Left Stall (Bit 0) & Undervoltage (Bit 4)...\n");
    SET_FAULT(active_fault_mask, FAULT_MOTOR_LEFT_STALL);
    SET_FAULT(active_fault_mask, FAULT_MOTOR_UNDERVOLTAGE);

    printf("Active Mask: 0x%08X (Expected: 0x00000011)\n", active_fault_mask);

    if (HAS_FAULT(active_fault_mask, FAULT_MOTOR_LEFT_STALL) &&
        HAS_FAULT(active_fault_mask, FAULT_MOTOR_UNDERVOLTAGE)) {
        printf(" -> PASS: Both motor fault bits successfully set!\n");
    } else {
        printf(" -> FAIL: Bitmask setting failed.\n");
    }

    // Check Subsystem Region Extraction
    uint8_t motor_byte = GET_MOTOR_FAULTS(active_fault_mask);
    printf("Extracted Motor Byte (Bits 0-6): 0x%02X\n\n", motor_byte);

    // -------------------------------------------------------------------------
    // TEST 3: Clearing Specific Fault
    // -------------------------------------------------------------------------
    printf("[TEST 3] Clearing Left Motor Stall (Bit 0)...\n");
    CLEAR_FAULT(active_fault_mask, FAULT_MOTOR_LEFT_STALL);

    printf("Active Mask: 0x%08X (Expected: 0x00000010)\n", active_fault_mask);

    if (!HAS_FAULT(active_fault_mask, FAULT_MOTOR_LEFT_STALL) &&
        HAS_FAULT(active_fault_mask, FAULT_MOTOR_UNDERVOLTAGE)) {
        printf(" -> PASS: Bit 0 cleared while Bit 4 remained active!\n\n");
    } else {
        printf(" -> FAIL: Clearing operation corrupted other bits.\n\n");
    }

    // -------------------------------------------------------------------------
    // TEST 4: Global Reset
    // -------------------------------------------------------------------------
    printf("[TEST 4] Clearing all active faults...\n");
    active_fault_mask = FAULT_NONE;
    printf("Final Mask: 0x%08X\n", active_fault_mask);
    if (!HAS_ANY_FAULT(active_fault_mask)) {
        printf(" -> PASS: Global system reset complete.\n\n");
    }

    printf("==================================================\n");
    printf("        DIAGNOSTIC TEST COMPLETE - READY          \n");
    printf("==================================================\n");

    // Infinite idle loop for RTOS task
    while (1) {
        tk_dly_tsk(1000);
    }
}

/**
 * @brief Main Entry Point
 */
int main(void) {
    // Initialize Pico stdio (USB Serial output)
    stdio_init_all();

    // Start micro T-Kernel OS kernel
    tk_start_kernel();

    return 0;
}