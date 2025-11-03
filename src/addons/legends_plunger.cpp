#include "addons/legends_plunger.h"
#include "drivermanager.h"
#include "storagemanager.h"
#include "hardware/gpio.h"
#include "helper.h"

// how long to signal a reset
#define RESET_DELAY 1000

// 8, E, 1 is a start bit, an even parity bit, eight data bites and a stop bit
#define CLOCK_BITS 11
#define PLUNGER_VALUE_INDEX 0
#define VALUES_PER_PACKET 4
#define DATA_MASK 0x1FF

volatile short reading = 0;
short new_reading = 0;

short bit = 0;
int data_index = 0;
int sda_pin = -1;

bool LegendsPlungerInput::available() {
    const LegendsPlungerOptions& legendsPlungerOptions = Storage::getInstance().getAddonOptions().legendsPlungerOptions;

    return (legendsPlungerOptions.enabled &&
        isValidPin(legendsPlungerOptions.clockPin) &&
        isValidPin(legendsPlungerOptions.dataPin));
}

// Interrupt handler function
void clock_pin_callback(unsigned int gpio, uint32_t events) {
  int value = gpio_get(sda_pin) ? 0 : 1;
  new_reading |= (value << bit++);
  //digitalWrite(LED_BUILTIN, value);
  
  if (bit == CLOCK_BITS) {
    if (data_index == PLUNGER_VALUE_INDEX) {
      // we want to mask off the top two bits (the start/parity) and shift away the last (stop);
      new_reading = (new_reading & DATA_MASK) >> 1;
      reading = new_reading;
    }
    
    data_index++;
    
    if (data_index == VALUES_PER_PACKET) {
      data_index = 0;
    }

    bit = 0;
    new_reading = 0;        
  }
}

void LegendsPlungerInput::setup() {
    const LegendsPlungerOptions& legendsPlungerOptions = Storage::getInstance().getAddonOptions().legendsPlungerOptions;

#if LEGENDS_PLUNGER_DEBUG==true
    stdio_init_all();
#endif
    uIntervalMS = 1;
    nextTimer = getMillis();

    sda_pin = legendsPlungerOptions.dataPin;
    gpio_init(legendsPlungerOptions.dataPin);             // Initialize pin
    gpio_set_dir(legendsPlungerOptions.dataPin, GPIO_IN); // Set as OUTPUT

    // The clock pin starts as output and switches to input after
    // the reset signal
    gpio_init(legendsPlungerOptions.clockPin);             // Initialize pin
    gpio_set_dir(legendsPlungerOptions.clockPin, GPIO_OUT); // Set as INPUT

    // Set it high for 100ms to allow the plunger to reset
    gpio_put(legendsPlungerOptions.clockPin, true);
    sleep_ms(100);

    // switch to input and prepare for interrupt
    gpio_init(legendsPlungerOptions.clockPin);             // Initialize pin
    gpio_set_dir(legendsPlungerOptions.clockPin, GPIO_IN); // Set as OUTPUT

    // Now attach the interrupt to read data on clock falling edge (signal is inverted)
    gpio_set_irq_enabled_with_callback(legendsPlungerOptions.clockPin, GPIO_IRQ_EDGE_FALL, true, clock_pin_callback);
}

void LegendsPlungerInput::process() {
    if (nextTimer < getMillis()) {
        plunger_reading = reading;

        uint16_t joystickMid = GAMEPAD_JOYSTICK_MID;
        if ( DriverManager::getInstance().getDriver() != nullptr ) {
            joystickMid = DriverManager::getInstance().getDriver()->GetJoystickMidValue();
        }

        rightY = map(plunger_reading,0,65,GAMEPAD_JOYSTICK_MIN,GAMEPAD_JOYSTICK_MAX);
        nextTimer = getMillis() + uIntervalMS;
    }

    Gamepad * gamepad = Storage::getInstance().GetGamepad();

    gamepad->state.ry = rightY;

    gamepad->hasAnalogTriggers = false;
    gamepad->hasRightAnalogStick = true;
}

uint16_t LegendsPlungerInput::map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
