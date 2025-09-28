/** 
 * Author: Felipe Arturo López Bonilla
 * Compiler: SDCC v4.5.0
 * 
 * Description: This program runs different led animations. The user can change
 * to the next animation when pressing the push button connected to pin (PIN NUMBER)
 */

#define NO_BIT_DEFINES
#include <pic16regs.h>
#include <stdint.h>
#include <stdbool.h>

 /* Set PIC configuration bits */
static __code uint16_t __at (_CONFIG) configword1 = _CP_OFF & _WDT_OFF & _PWRTE_ON & _XT_OSC;

#define TMR0_COUNT 100 // Time overflow set to 5 ms
#define COUNT_PAUSE 60          // 60 * 5ms = 300 ms
#define COUNT_TRANSITION_1 15   // 10 * 5MS = 75 ms
#define COUNT_TRANSITION_2 20   // 20 * 5ms = 100 ms
#define COUNT_TRANSITION_3 50   // 50 * 5ms = 250 ms
#define REPEAT_ANIMATION_COUNT_1 8
#define REPEAT_ANIMATION_COUNT_2 4

volatile uint16_t delay_pause = 0;
volatile uint16_t delay_transition = 0;

static uint8_t animation_repeats = 0;
static uint8_t i = 0;
static uint8_t j = 0;
static uint8_t k = 0;

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

/* ANIMATION 01: blink 4 LEDs and then other 4 LEDs */
static void animation_01(void)
{
    PORTA = 0b00001111;
    PORTB = 0b11110000;
    while (delay_pause < COUNT_PAUSE);
    reset_timers();

    while (animation_repeats < REPEAT_ANIMATION_COUNT_1) {
        if (delay_transition == COUNT_TRANSITION_2) {
            PORTA = ~(PORTA);
            PORTB = ~(PORTB);
            delay_transition = 0;
            animation_repeats++;
        }

        if (PORTAbits.RA4 == 0) {
            break;
        }
    }
}

/* ANIMATION 02: shift LED ON from right to left and vice-versa */
static void animation_02(void)
{
    i = 0b00000000;
    j = 0b00000000;
    PORTA = i;
    PORTB = j;
    while (delay_pause < COUNT_PAUSE);
    reset_timers();

    while (animation_repeats < 2) {
        for (i = 0b00000001; i < 16; i <<= 1) {
            PORTA = i;
            while (delay_transition < COUNT_TRANSITION_1) {
                if (PORTAbits.RA4 == 0) {
                    goto END_ANIMATION_2;
                }
            }
            delay_transition = 0;
        }
        PORTA = 0b00000000;

        for (j = 0b00000001; j != 0; j <<= 1) {
            PORTB = j;
            while (delay_transition < COUNT_TRANSITION_1) {
                if (PORTAbits.RA4 == 0) {
                    goto END_ANIMATION_2;
                }
            }
            delay_transition = 0;
        }
        PORTB = 0b00000000;

        i = 0b00000000;
        j = 0b00000000;
        PORTA = i;
        PORTB = j;
        while (delay_pause < COUNT_PAUSE);
        delay_transition = 0;

        for (j = 0b10000000; j > 0; j >>= 1) {
            PORTB = j;
            while (delay_transition < COUNT_TRANSITION_1) {
                if (PORTAbits.RA4 == 0) {
                    goto END_ANIMATION_2;
                }
            }
            delay_transition = 0;
        }
        PORTB = 0b00000000;

        for (i = 0b00001000; i > 0; i >>= 1) {
            PORTA = i;
            while (delay_transition < COUNT_TRANSITION_1) {
                if (PORTAbits.RA4 == 0) {
                    goto END_ANIMATION_2;
                }
            }
            delay_transition = 0;
        }
        PORTA = 0b00000000;

        animation_repeats++;
    }

END_ANIMATION_2:

}

/* ANIMATION 03: alternate blinking LEDs */
static void animation_03(void)
{
    PORTA = 0b00001010;
    PORTB = 0b10101010;
    while (delay_pause < COUNT_PAUSE);
    reset_timers();

    while (animation_repeats < REPEAT_ANIMATION_COUNT_1) {
        if (delay_transition == COUNT_TRANSITION_3) {
            PORTA = ~(PORTA);
            PORTB = ~(PORTB);
            delay_transition = 0;
            animation_repeats++;
        }

        if (PORTAbits.RA4 == 0) {
            break;
        }
    }
}

/* ANIMATION 04: shift LED OFF from right to left and vice-versa */
static void animation_04(void)
{
    i = 0b00001111;
    j = 0b11111111;
    PORTA = i;
    PORTB = j;
    while (delay_pause < COUNT_PAUSE);
    reset_timers();

    for (i = 0b00000001; i < 16; i <<= 1) {
        PORTA = ~(i);
        while (delay_transition < COUNT_TRANSITION_2) {
            if (PORTAbits.RA4 == 0) {
                goto END_ANIMATION_4;
            }
        }
        delay_transition = 0;
    }
    PORTA = 0b00001111;

    for (j = 0b00000001; j != 0; j <<= 1) {
        PORTB = ~(j);
        while (delay_transition < COUNT_TRANSITION_2) {
            if (PORTAbits.RA4 == 0) {
                goto END_ANIMATION_4;
            }
        }
        delay_transition = 0;
    }
    PORTB = 0b11111111;

    i = 0b00001111;
    j = 0b11111111;
    PORTA = i;
    PORTB = j;
    while (delay_pause < COUNT_PAUSE);
    delay_transition = 0;

    for (j = 0b10000000; j > 0; j >>= 1) {
        PORTB = ~(j);
        while (delay_transition < COUNT_TRANSITION_2) {
            if (PORTAbits.RA4 == 0) {
                goto END_ANIMATION_4;
            }
        }
        delay_transition = 0;
    }
    PORTB = 0b11111111;

    for (i = 0b00001000; i > 0; i >>= 1) {
        PORTA = ~(i);
        while (delay_transition < COUNT_TRANSITION_2) {
            if (PORTAbits.RA4 == 0) {
                goto END_ANIMATION_4;
            }
        }
        delay_transition = 0;
    }
    PORTA = 0b00001111;

END_ANIMATION_4:

}

static void animation_05(void)
{
    i = 0b00000001;
    j = 0b00000000;
    PORTA = i;
    PORTB = j;
    while (delay_pause < COUNT_PAUSE);
    reset_timers();

    for (k = 0; k < 11; k++) {
        if (k < 3) {
            PORTA = i;
            i <<= 1;
            i += 1;
        } else if (k == 3) {
            PORTA = 0b00001111;
            j = 0b00000001;
        } else {
            j <<= 1;
            j += 1;
            PORTB = j;
        }
        while (delay_transition < COUNT_TRANSITION_1) {
            if (PORTAbits.RA4 == 0) {
                goto END_ANIMATION_5;
            }
        }
        delay_transition = 0;
    }

    for (k = 0; k < 12; k++) {
        if (k < 4) {
            PORTA = i;
            i <<= 1;
        } else if (k == 4) {
            PORTA = 0b00000000;
        } else {
            j <<= 1;
            PORTB = j;
        }
        while (delay_transition < COUNT_TRANSITION_1) {
            if (PORTAbits.RA4 == 0) {
                goto END_ANIMATION_5;
            }
        }
        delay_transition = 0;
    }

    while (delay_transition < COUNT_TRANSITION_1) {
        if (PORTAbits.RA4 == 0) {
            goto END_ANIMATION_5;
        }
    }
    delay_transition = 0;

    j = 0b10000000;
    i = 0b00000000;

    for (k = 0; k < 12; k++) {
        while (delay_transition < COUNT_TRANSITION_1) {
            if (PORTAbits.RA4 == 0) {
                goto END_ANIMATION_5;
            }
        }
        delay_transition = 0;

        if (k < 7) {
            j >>= 1;
            j |= 0b10000000;
            PORTB = j;
        } else if (k == 7) {
            PORTB = 0b11111111;
            i = 0b00001000;
            PORTA = i;
        } else {
            i >>= 1;
            i |= 0b00001000;
            PORTA = i;
        }
    }

    for (k = 0; k < 12; k++) {
        if (k < 8) {
            PORTB = j;
            j >>= 1;
        } else if (k == 8) {
            PORTB = 0b00000000;
        } else {
            i >>= 1;
            PORTA = i;
        }
        while (delay_transition < COUNT_TRANSITION_1) {
            if (PORTAbits.RA4 == 0) {
                goto END_ANIMATION_5;
            }
        }
        delay_transition = 0;
    }

END_ANIMATION_5:

}

void main(void)
{
    TRISA = 0b00010000; // PA0 - PA3 as output, PA4 as input
    PORTA = 0b00000000; // All pins of PORTA off
    TRISB = 0x00;       // PORTB as output
    PORTB = 0b00000000; // All pins of PORTB off

    init_TMR0();

    while (true) {

        animation_01();
        reset_timers();

        animation_02();
        reset_timers();
        
        animation_03();
        reset_timers();

        animation_04();

        animation_05();
        reset_timers();

        /* End of animations */
        PORTA = 0b00000000;
        PORTB = 0b00000000;
        while (delay_pause < COUNT_PAUSE);
        reset_timers();
    }
}