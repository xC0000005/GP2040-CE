#ifndef _LegendsPlungerAddon_H
#define _LegendsPlungerAddon_H

#include <string>
#include <stdint.h>
#include "BoardConfig.h"
#include "gpaddon.h"
#include "gamepad.h"
#include "storagemanager.h"

// _LegendsPlungerAddon_H Module Name
#define LegendsPlungerName "LegendsPlunger"

#ifndef LEGENDS_PLUNGER_ENABLED
#define LEGENDS_PLUNGER_ENABLED 0
#endif

#ifndef LEGENDS_PLUNGER_DATA_PIN
#define LEGENDS_PLUNGER_DATA_PIN -1
#endif

#ifndef LEGENDS_PLUNGER_CLOCK_PIN
#define LEGENDS_PLUNGER_CLOCK_PIN -1
#endif

class LegendsPlungerInput : public GPAddon {
public:
    virtual bool available();
    virtual void setup();       // SNESpad Setup
    virtual void process();     // SNESpad Process
    virtual void preprocess() {}
    virtual void postprocess(bool sent) {}
    virtual void reinit() {}
    virtual std::string name() { return LegendsPlungerName; }
private:
    uint32_t uIntervalMS;       // Plunger Interval
    uint32_t nextTimer;         // Plunger Timer
    uint16_t plunger_reading = 0;
    uint16_t rightY = 0;

    uint16_t map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max);
};

#endif  // _LegendsPlungerAddon_H