/** 
 * Author: Felipe Arturo López Bonilla
 * Compiler: SDCC v4.5.0
 * 
 * Description: This program runs different led animations. The user can change
 * to the next animation when pressing the push button connected to pin (PIN NUMBER)
 */

#define NO_BIT_DEFINES
#include <pic14regs.h>
#include <stdint.h>
#include <stdbool.h>

 /* Set PIC configuration bits */
static __code uint16_t __at (_CONFIG) configword1 = _CP_OFF & _WDT_OFF & _PWRTE_ON & _XT_OSC;

#define TMR0_COUNT 100 // Time overflow set to 5 ms
#define COUNT_DELAY_LED 100 // 100 * 5ms = 500 ms

volatile uint16_t delay_animation = 0;

static void interrupt_vector(void) __interrupt (0)
{
    if (INTCONbits.T0IF == 1) { // Check if the interrupt was caused by TMR0
        delay_animation++;
        TMR0 = TMR0_COUNT;      // Reset count for TMR0 register
        INTCONbits.T0IF = 0;    // TMR0 starts counting again
    }
}

static void init_TMR0(void)
{
    OPTION_REG = 0x04;  // Prescaler = 32, Prescaler assign to TMR0
    TMR0 = TMR0_COUNT;      // TMR0 = 256 - (5000 us / (1x4))
    INTCON = 0xA0;      // Enable global interrupts (GIE = 1), enable TMR0 interrupt (T0IE = 1)
}

void main(void)
{
    TRISBbits.TRISB0 = 0;    // Pin RB0 as output
    PORTBbits.RB0 = 0;      // Pin RB0 off

    init_TMR0();

    while (true) {
        if (delay_animation == COUNT_DELAY_LED) {
            PORTBbits.RB0 ^= 1; // Change the state of the pin
            delay_animation = 0;// Reset timer
        }
    }
}