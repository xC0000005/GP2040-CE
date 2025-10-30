#include "addons/legends_plunger.h"
#include "drivermanager.h"
#include "storagemanager.h"
#include "hardware/gpio.h"
#include "helper.h"

bool LegendsPlungerInput::available() {
    const LegendsPlungerOptions& legendsPlungerOptions = Storage::getInstance().getAddonOptions().legendsPlungerOptions;

    return (legendsPlungerOptions.enabled &&
        isValidPin(legendsPlungerOptions.clockPin) &&
        isValidPin(legendsPlungerOptions.dataPin));
}

// Interrupt handler function
void clock_pin_callback(unsigned int gpio, uint32_t events) {
}

void LegendsPlungerInput::setup() {
    const LegendsPlungerOptions& legendsPlungerOptions = Storage::getInstance().getAddonOptions().legendsPlungerOptions;

#if LEGENDS_PLUNGER_DEBUG==true
    stdio_init_all();
#endif
    uIntervalMS = 1;
    nextTimer = getMillis();

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
        // snes->poll();

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
