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

#define TMR0_COUNT 100              // Time overflow set to 5 ms
#define DEBOUNCE_TIME 4             // 4 * 5ms = 20 ms
#define LONG_PRESS_TIME 20          // 20 * 5ms = 100 ms
#define INC_BUTTON_SET 0x01         // Index of the Increment button
#define MASK_INC_BUTTON 0x02        // Status bit for the Increment button
#define STATUS_MASK_BUTTONS 0x03    // Mask to get the status of the buttons

volatile bool enableDebounceTimer = false;
volatile uint8_t delayDebounceTime = 0;
volatile bool enableLongPressTimer = false;
volatile uint8_t longPressTimer = 0;

static uint8_t currButtonsStatus = 0;
static uint8_t prevButtonsStatus = 0;
static uint8_t number = 0;
static uint8_t updateDisplay = false;

/* Table with the values for the decimal numbers that will be displayed on the seven segment
 * display. These values are for a common cathode display, however later on the program 
 * the bits are inverted for the common anode display.
 */
uint8_t tableBCDto7Seg[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

static void interrupt_vector(void) __interrupt (0)
{
    if (INTCONbits.T0IF == 1) { // Check if the interrupt was caused by TMR0
        if (enableDebounceTimer == true) {
            delayDebounceTime++;
        }
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

static uint8_t get_buttons_status(void)
{
    uint8_t statusPORTA = (PORTA & 0x1F);
    uint8_t buttonsStatus = 0;

    if ((statusPORTA & STATUS_MASK_BUTTONS) == MASK_INC_BUTTON) {
        buttonsStatus |= INC_BUTTON_SET;
    }

    return buttonsStatus;
}

static void handle_buttons(void)
{
    currButtonsStatus = get_buttons_status();

    /* Compare the current with the previous status to verify if there was a change in any of the
     * buttons. In case there is a change, restart timer variables and previous status to current
     */
    if (currButtonsStatus != prevButtonsStatus) {
        delayDebounceTime = 0;
        enableDebounceTimer = false;
        enableLongPressTimer = false;
        prevButtonsStatus = currButtonsStatus;
    }

    switch (currButtonsStatus)
    {
    case INC_BUTTON_SET:
        /* If is the first time the button is pressed, then enable the debounce timer to confirm
         * that the button is pressed
         */
        if ((enableDebounceTimer == false) && (enableLongPressTimer == false)) {
            enableDebounceTimer = true;
        } else if ((enableDebounceTimer == true) && (delayDebounceTime == DEBOUNCE_TIME)) {
            /* Once the debounce time has elapsed, update the number on the 7-segment display */
            (number < 9) ? (number++) : (number = 0);
            updateDisplay = true;
            enableDebounceTimer = false;
            enableLongPressTimer = true;
        }
        // While the timer for the LongPress is enabled:
            // If the timer has reached the LongPress time:
                // Increment by on the number and restart the LongPress timer
                // Set flag to update the display
        break;
    
    default:
        break;
    }
}

void main(void)
{
    TRISA = 0b00011111; // PORTA pins as input

    TRISB = 0x00;       // PORTB pins as output
    PORTB = 0b11111111; // All pins of PORTB ON. For common anode display all segments are OFF.

    init_TMR0();
    PORTB = ~(tableBCDto7Seg[number]);

    currButtonsStatus = get_buttons_status();
    prevButtonsStatus = currButtonsStatus;

    while (true) {
        handle_buttons();

        if (updateDisplay == true) {
            PORTB = ~(tableBCDto7Seg[number]);
            updateDisplay = false;
        }
    }
}