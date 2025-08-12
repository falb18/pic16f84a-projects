/** 
 * Author: Felipe Arturo López Bonilla
 * Compiler: SDCC v4.5.0
 * 
 * Description: With the push buttons the user sets the timer (0-99 secs). When the timer
 * starts, it counts down till 0.
 */

#define NO_BIT_DEFINES
#include <pic16regs.h>
#include <stdint.h>
#include <stdbool.h>

 /* Set PIC configuration bits */
static __code uint16_t __at (_CONFIG) configword1 = _CP_OFF & _WDT_OFF & _PWRTE_ON & _XT_OSC;

#define TMR0_COUNT 100 // Time overflow set to 5 ms
#define COUNT_TRANSITION_1 100  // 100 * 5ms = 500 ms

volatile uint16_t delay_transition = 0;

/* Table with the values for the decimal numbers that will be displayed on the seven segment
 * display. These values are for a common cathode display, however later on the program 
 * the bits are inverted for the common anode display.
 */
uint8_t tableBCDto7Seg[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

static void interrupt_vector(void) __interrupt (0)
{
    if (INTCONbits.T0IF == 1) { // Check if the interrupt was caused by TMR0
        delay_transition++;
        TMR0 = TMR0_COUNT;      // Reset count for TMR0 register
        INTCONbits.T0IF = 0;    // TMR0 starts counting again
    }
}

static void init_TMR0(void)
{
    OPTION_REG = 0x04;  // Prescaler = 32, Prescaler assign to TMR0
    TMR0 = TMR0_COUNT;  // TMR0 = 256 - (5000 us / (1x4))
    INTCON = 0xA0;      // Enable global interrupts (GIE = 1), enable TMR0 interrupt (T0IE = 1)
}

void main(void)
{
    uint8_t i = 0;
    TRISB = 0x00;       // PORTB as output
    PORTB = 0b11111111; // All pins of PORTB ON. For common anode display all segments are OFF.

    init_TMR0();
    PORTB = ~(tableBCDto7Seg[i]);

    while (true) {
        if (delay_transition == COUNT_TRANSITION_1) {
            (i < 9) ? (i++) : (i = 0);
            PORTB = ~(tableBCDto7Seg[i]);
            delay_transition = 0;
        }
    }
}