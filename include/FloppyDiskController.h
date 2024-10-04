#ifndef FLOPPY_DISK_CONTROLLER_H
#define FLOPPY_DISK_CONTROLLER_H

#include <Arduino.h>

#define FLOPPYDRIVE_HEAD_INWARDS true
#define FLOPPYDRIVE_HEAD_OUTWARDS false

class FloppyDrive {
    private:
        int8_t _wrdataPin, _wrgatePin, _indexPin, _rddataPin;
        int8_t _densityPin, _driveSelectPin, _motorEnablePin, _directionPin, _stepPin, _track0Pin, _protectPin, _sidePin, _readyPin;
        int currentTrack;
        void setupPinModes();
    public:
        FloppyDrive(int8_t densitypin, int8_t indexpin,
                    int8_t driveSelectpin, int8_t motorpin,
                    int8_t directionpin, int8_t steppin,
                    int8_t wrdatapin, int8_t wrgatepin,
                    int8_t track0pin, int8_t protectpin,
                    int8_t rddatapin, int8_t sidepin,
                    int8_t readypin);
        void reset();
        void test();
        void step(int steps);
        void gotoTrack0();
        void gotoTrack(int track);
        void driveSelect(bool driveSelect);
        void motorEnable(bool motorEnable);
        void motorDirection(bool direction);
};

#endif