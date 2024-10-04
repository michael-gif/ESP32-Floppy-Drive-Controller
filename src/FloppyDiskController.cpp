#include <Arduino.h>
#include <FloppyDiskController.h>

FloppyDrive::FloppyDrive(int8_t densitypin, int8_t indexpin,
                        int8_t driveSelectpin, int8_t motorEnablepin,
                        int8_t directionpin, int8_t steppin,
                        int8_t wrdatapin, int8_t wrgatepin,
                        int8_t track0pin, int8_t protectpin,
                        int8_t rddatapin, int8_t sidepin,
                        int8_t readypin) {
  _densityPin = densitypin;
  _indexPin = indexpin;
  _driveSelectPin = driveSelectpin;
  _motorEnablePin = motorEnablepin;
  _directionPin = directionpin;
  _stepPin = steppin;
  _wrdataPin = wrdatapin;
  _wrgatePin = wrgatepin;
  _track0Pin = track0pin;
  _protectPin = protectpin;
  _rddataPin = rddatapin;
  _sidePin = sidepin;
  _readyPin = readypin;
  setupPinModes();
}

void FloppyDrive::setupPinModes() {
    pinMode(_densityPin, OUTPUT);
    pinMode(_driveSelectPin, OUTPUT);
    pinMode(_motorEnablePin, OUTPUT);
    pinMode(_directionPin, OUTPUT);
    pinMode(_stepPin, OUTPUT);
    pinMode(_wrdataPin, OUTPUT);
    pinMode(_wrgatePin, OUTPUT);
    pinMode(_track0Pin, INPUT_PULLUP);
    pinMode(_protectPin, INPUT_PULLUP);
    pinMode(_rddataPin, INPUT);
    pinMode(_sidePin, OUTPUT);
    pinMode(_readyPin, INPUT_PULLUP);
}

void FloppyDrive::reset() {
    driveSelect(false);
    motorEnable(false);
    motorDirection(true);
    gotoTrack0();
    digitalWrite(_stepPin, LOW);
    digitalWrite(_sidePin, HIGH);
    digitalWrite(_densityPin, LOW);
}

void FloppyDrive::test() {
    gotoTrack0();
    delay(500);
    for (int i = 0; i < 80; i+=4) {
      gotoTrack(i);
      delay(125);
    }
    gotoTrack0();
}

void FloppyDrive::step(int steps) {
    digitalWrite(_stepPin, LOW);
    delayMicroseconds(10);

    while (steps != 0) {
        driveSelect(true);
        digitalWrite(_stepPin, HIGH);
        delayMicroseconds(3000);
        digitalWrite(_stepPin, LOW);
        driveSelect(false);
        steps--;
    }
}

void FloppyDrive::gotoTrack0() {
    motorDirection(FLOPPYDRIVE_HEAD_OUTWARDS); // track 0 is the outer most track
    int steps = 85; // 3.5" disks have 80 tracks, but we step out 85 times just to make sure
    while (steps != 0) {
        int isOnTrack0 = digitalRead(_track0Pin);
        if (!isOnTrack0) break; // pin is active low, so false means the head is on track 0
        step(1);
    }
    currentTrack = 0;
}

void FloppyDrive::gotoTrack(int trackNum) {
    if (trackNum > currentTrack) {
        motorDirection(FLOPPYDRIVE_HEAD_INWARDS);
        step(trackNum - currentTrack);
    } else {
        motorDirection(FLOPPYDRIVE_HEAD_OUTWARDS);
        step(currentTrack - trackNum);
    }
    currentTrack = trackNum;
}

void FloppyDrive::driveSelect(bool driveSelect) {
    digitalWrite(_driveSelectPin, driveSelect ? LOW : HIGH);
}

void FloppyDrive::motorEnable(bool motorEnable) {
    digitalWrite(_motorEnablePin, motorEnable ? LOW : HIGH);
}

void FloppyDrive::motorDirection(bool direction) {
    digitalWrite(_directionPin, direction ? LOW : HIGH);
}