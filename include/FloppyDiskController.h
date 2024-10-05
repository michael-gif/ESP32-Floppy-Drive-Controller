#ifndef FLOPPY_DISK_CONTROLLER_H
#define FLOPPY_DISK_CONTROLLER_H

#include <Arduino.h>

#define FLOPPYDRIVE_HEAD_INWARDS true
#define FLOPPYDRIVE_HEAD_OUTWARDS false
#define MAX_FLUX_PULSE_PER_TRACK                                               \
  (uint32_t)(500000UL / 5 *                                                    \
             1.5) // 500khz / 5 hz per track rotation, 1.5 rotations

class FloppyDrive {
    private:
        int8_t _wrdataPin, _wrgatePin, _indexPin, _rddataPin;
        int8_t _densityPin, _driveSelectPin, _motorEnablePin, _directionPin, _stepPin, _track0Pin, _protectPin, _sidePin, _readyPin;
        int currentTrack;
        void setupPinModes();
        /**
         * The stepper motor requires a pulse by pulling the _stepPin high for 3ms, then low.
         * 
         * Each step change is equivalent to one track change. There are 80 steps/tracks in total for a 3.5" disk.
         * @param steps 
         */
        void step(int steps);
        void gotoTrack0();
        void gotoTrack(int track);
        void driveSelect(bool driveSelect);
        void motorEnable(bool motorEnable);
        /**
         * Use one of the following for direction:
         * `FLOPPYDRIVE_HEAD_INWARDS` (true)
         * `FLOPPYDRIVE_HEAD_OUTWARDS` (false)
         */
        void motorDirection(bool direction);
        void spinupMotor();
    public:
        FloppyDrive(int8_t densitypin, int8_t indexpin,
                    int8_t driveSelectpin, int8_t motorpin,
                    int8_t directionpin, int8_t steppin,
                    int8_t wrdatapin, int8_t wrgatepin,
                    int8_t track0pin, int8_t protectpin,
                    int8_t rddatapin, int8_t sidepin,
                    int8_t readypin);
        /**
         * Deselect the drive, turn off the motor and turn off any other pins
         */
        void reset();
        /**
         * Put whatever you want in here if you want to mess around with the drive
         */
        void test();
        /**
         * Select the drive, spinup the motor and move the head to track 0
         */
        void prepareDrive();
        uint32_t captureTrack(uint8_t* fluxTransitions, size_t max_pulses, int32_t *falling_index_offset, bool store_greaseweazle);
};

#endif