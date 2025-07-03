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
#define COUNT_PAUSE 60          // 60 * 5ms = 300 ms
#define COUNT_TRANSITION_1 20   // 20 * 5ms = 100 ms
#define COUNT_TRANSITION_2 50   // 50 * 5ms = 250 ms
#define REPEAT_ANIMATION_COUNT_1 8
#define REPEAT_ANIMATION_COUNT_2 4

volatile uint16_t delay_pause = 0;
volatile uint16_t delay_transition = 0;

static uint8_t animation_repeats = 0;

static void interrupt_vector(void) __interrupt (0)
{
    if (INTCONbits.T0IF == 1) { // Check if the interrupt was caused by TMR0
        delay_pause++;
        delay_transition++;
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

static void reset_timers(void)
{
    delay_pause = 0;
    delay_transition = 0;
    animation_repeats = 0;
}

void main(void)
{
    uint8_t i = 0;
    TRISB = 0x00;       // PORTB as output
    PORTB = 0b00000000; // All pins of PORTB off

    init_TMR0();

    while (true) {
        /* ANIMATION 1: blink 4 LEDs and then other 4 LEDs */
        PORTB = 0b11110000;
        while (delay_pause < COUNT_PAUSE);
        reset_timers();

        while (animation_repeats < REPEAT_ANIMATION_COUNT_1) {
            if (delay_transition == COUNT_TRANSITION_1) {
                PORTB = ~(PORTB);
                delay_transition = 0;
                animation_repeats++;
            }
        }
        reset_timers();
        
        /* ANIMATION 2: alternate blinking LEDs */
        PORTB = 0b10101010;
        while (delay_pause < COUNT_PAUSE);
        reset_timers();

        while (animation_repeats < REPEAT_ANIMATION_COUNT_1) {
            if (delay_transition == COUNT_TRANSITION_2) {
                PORTB = ~(PORTB);
                delay_transition = 0;
                animation_repeats++;
            }
        }
        reset_timers();

        /* ANIMATION 3: shift LED ON to the right */
        PORTB = 0b00000001;
        i = PORTB;
        while (delay_pause < COUNT_PAUSE);
        reset_timers();

        while (animation_repeats < REPEAT_ANIMATION_COUNT_2) {
            if (delay_transition == COUNT_TRANSITION_1) {
                i <<= 1;
                if (i == 16) {
                    i = 0b00000001;
                    animation_repeats++;
                }
                PORTB = i;
                delay_transition = 0;
            }
        }
        reset_timers();

        /* ANIMATION 4: shift LED ON to the left */
        PORTB = 0b00001000;
        i = PORTB;
        while (delay_pause < COUNT_PAUSE);
        reset_timers();

        while (animation_repeats < REPEAT_ANIMATION_COUNT_2) {
            if (delay_transition == COUNT_TRANSITION_1) {
                i >>= 1;
                if (i == 0) {
                    i = 0b00001000;
                    animation_repeats++;
                }
                PORTB = i;
                delay_transition = 0;
            }
        }
        reset_timers();

        /* End of animations */
        PORTB = 0b00000000;
        while (delay_pause < COUNT_PAUSE);
        reset_timers();
    }
}