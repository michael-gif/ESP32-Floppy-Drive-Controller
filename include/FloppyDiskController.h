#ifndef FLOPPY_DISK_CONTROLLER_H
#define FLOPPY_DISK_CONTROLLER_H

#include <Arduino.h>
#include <Adafruit_SPIDevice.h>

#define FLOPPYDRIVE_HEAD_INWARDS true
#define FLOPPYDRIVE_HEAD_OUTWARDS false
#define MAX_FLUX_PULSE_PER_TRACK 83334

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
        bool readIndex();
        bool readData();
        #ifdef BUSIO_USE_FAST_PINIO
        BusIO_PortReg *indexPort;
        BusIO_PortMask indexMask;
        BusIO_PortReg *dataPort;
        BusIO_PortMask dataMask;
        #endif
    public:
        FloppyDrive(int8_t densityPin, int8_t indexPin,
                    int8_t driveSelectPin, int8_t motorPin,
                    int8_t directionPin, int8_t stepPin,
                    int8_t wrdataPin, int8_t wrgatePin,
                    int8_t track0Pin, int8_t protectPin,
                    int8_t rddataPin, int8_t sidePin,
                    int8_t readyPin);
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
        /**
         * Capture one track worth of data by counting clock cycles during each pulse
         * @param fluxTransitions 
         * @return int 
         */
        int captureTrack(uint8_t* fluxTransitions);
        /**
         * Decode mfm encoded pulses into binary
         * @param fluxTransitions 
         * @param pulses 
         */
        void decode_mfm(uint8_t* fluxTransitions, int pulses);
};

#endif