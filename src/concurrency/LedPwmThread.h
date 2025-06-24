// LedPwmThread.h
#pragma once

#include "concurrency/OSThread.h"
#include <Arduino.h>

namespace concurrency {

class LedPwmThread : public OSThread {
public:
    explicit LedPwmThread(uint8_t pin, meshtastic::PowerStatus* powerStatus)
        : OSThread("LedPWM"), _pin(pin), _brightness(0), _increasing(true), _powerStatus(powerStatus) {
        pinMode(_pin, OUTPUT);
        analogWrite(_pin, 0);
    }

protected:
unsigned long _lastChange = 0;
bool _active = false;
const unsigned long ACTIVE_DURATION = 5000;
const unsigned long INACTIVE_DURATION = 5000;

int32_t runOnce() override {
    int32_t batteryVoltageMv = _powerStatus->getBatteryVoltageMv();
    unsigned long now = millis();

    if (batteryVoltageMv != -1 && batteryVoltageMv < 3000) {
        if (_active && now - _lastChange >= ACTIVE_DURATION) {
            _active = false;
            _lastChange = now;
            analogWrite(_pin, 0);
        } else if (!_active && now - _lastChange >= INACTIVE_DURATION) {
            _active = true;
            _lastChange = now;
            _brightness = 0;
            _increasing = true;
        }

        if (_active) {
            analogWrite(_pin, _brightness);

            if (_increasing) {
                _brightness += 5;
                if (_brightness >= 50) {
                    _brightness = 50;
                    _increasing = false;
                }
            } else {
                _brightness -= 5;
                if (_brightness <= 0) {
                    _brightness = 0;
                    _increasing = true;
                }
            }
        }
    } else {
        analogWrite(_pin, 0);
        _active = false;
        _lastChange = now;
    }

    return 200;  // update tiap 100ms
}


private:
    uint8_t _pin;
    uint8_t _brightness;
    bool _increasing;
    meshtastic::PowerStatus* _powerStatus;
};

} // namespace concurrency
