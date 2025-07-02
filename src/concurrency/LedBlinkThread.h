// LedBlinkThread.h
#pragma once

#include "concurrency/OSThread.h"
#include <Arduino.h>

namespace concurrency {

class LedBlinkThread : public OSThread {
public:
    explicit LedBlinkThread(uint8_t powerLedPin, uint8_t lowBattLedPin, meshtastic::PowerStatus* powerStatus)
        : OSThread("LedBlink"), _powerLedPin(powerLedPin), _lowBattLedPin(lowBattLedPin),
          _lowBattLedState(false), _powerStatus(powerStatus) {
        pinMode(_powerLedPin, OUTPUT);
        pinMode(_lowBattLedPin, OUTPUT);
        digitalWrite(_powerLedPin, HIGH); // Power LED always ON
        digitalWrite(_lowBattLedPin, LOW);
    }

protected:
    int32_t runOnce() override {
        // Check battery voltage for low batt LED
        if (_powerStatus && _powerStatus->getBatteryVoltageMv() < 5000) {
            _lowBattLedState = !_lowBattLedState;
            digitalWrite(_lowBattLedPin, _lowBattLedState ? HIGH : LOW);
        } else {
            _lowBattLedState = false;
            digitalWrite(_lowBattLedPin, LOW);
        }

        return 500; // Update every 500ms
    }

private:
    uint8_t _powerLedPin;
    uint8_t _lowBattLedPin;
    bool _lowBattLedState;
    meshtastic::PowerStatus* _powerStatus;
};

} // namespace concurrency
