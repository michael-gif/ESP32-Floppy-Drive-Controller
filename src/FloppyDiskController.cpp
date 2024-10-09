#include <Arduino.h>
#include <FloppyDiskController.h>

#ifdef BUSIO_USE_FAST_PINIO
#define readIndexFast() (*indexPort & indexMask)
#define readDataFast() (*dataPort & dataMask)
#endif

FloppyDrive::FloppyDrive(int8_t densityPin, int8_t indexPin, int8_t driveSelectPin, int8_t motorEnablePin,
                        int8_t directionPin, int8_t stepPin, int8_t wrdataPin, int8_t wrgatePin,
                        int8_t track0Pin, int8_t protectPin, int8_t rddataPin, int8_t sidePin,
                        int8_t readyPin) {
  _densityPin = densityPin;
  _indexPin = indexPin;
  _driveSelectPin = driveSelectPin;
  _motorEnablePin = motorEnablePin;
  _directionPin = directionPin;
  _stepPin = stepPin;
  _wrdataPin = wrdataPin;
  _wrgatePin = wrgatePin;
  _track0Pin = track0Pin;
  _protectPin = protectPin;
  _rddataPin = rddataPin;
  _sidePin = sidePin;
  _readyPin = readyPin;
  setupPinModes();
}

void FloppyDrive::setupPinModes() {
    pinMode(_densityPin, OUTPUT);
    pinMode(_indexPin, INPUT_PULLUP);
    pinMode(_driveSelectPin, OUTPUT);
    pinMode(_motorEnablePin, OUTPUT);
    pinMode(_directionPin, OUTPUT);
    pinMode(_stepPin, OUTPUT);
    pinMode(_wrdataPin, OUTPUT);
    pinMode(_wrgatePin, OUTPUT);
    pinMode(_track0Pin, INPUT_PULLUP);
    pinMode(_protectPin, INPUT_PULLUP);
    pinMode(_rddataPin, INPUT_PULLUP);
    pinMode(_sidePin, OUTPUT);
    pinMode(_readyPin, INPUT_PULLUP);

    #ifdef BUSIO_USE_FAST_PINIO
    indexPort = (BusIO_PortReg *)portInputRegister(digitalPinToPort(_indexPin));
    indexMask = digitalPinToBitMask(_indexPin);
    dataPort = (BusIO_PortReg *)portInputRegister(digitalPinToPort(_rddataPin));
    dataMask = digitalPinToBitMask(_rddataPin);
    #endif
}

void FloppyDrive::reset() {
    driveSelect(false);
    motorEnable(false);
    motorDirection(true);
    digitalWrite(_stepPin, LOW);
    digitalWrite(_sidePin, HIGH);
    digitalWrite(_densityPin, LOW);
    digitalWrite(_wrgatePin, HIGH);
    digitalWrite(_wrdataPin, HIGH);
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
        digitalWrite(_stepPin, HIGH);
        delayMicroseconds(3000);
        digitalWrite(_stepPin, LOW);
        steps--;
    }
}

void FloppyDrive::gotoTrack0() {
    motorDirection(FLOPPYDRIVE_HEAD_INWARDS);
    step(10); // step 10 inwards to make sure the head isn't outside the track bounds

    motorDirection(FLOPPYDRIVE_HEAD_OUTWARDS); // track 0 is the outer most track
    int steps = 85; // 3.5" disks have 80 tracks, but we step out 85 times just to make sure
    while (steps != 0) {
        int isOnTrack0 = digitalRead(_track0Pin);
        if (!isOnTrack0) break; // pin is active low, so false means the head is on track 0
        step(1);
        steps--;
    }
    currentTrack = 0;
    Serial.println("Moved head to track 0");
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

void FloppyDrive::spinupMotor() {
  motorEnable(true);
  delay(1000); // wait for motor to spin up
  
  Serial.println("Waiting for index pulse...");
  uint32_t timestamp = micros();
  bool timedOut = false;
  uint32_t timeDiff = 0;
  while (readIndex()) {
    timeDiff = micros() - timestamp;
    if (timeDiff > 1000000) {
      timedOut = true; // timeout after one second of nothing
      break;
    }
    yield();
  }

  if (timedOut) {
    Serial.println("Failed to spin up motor & find index pulse");
    while (1) yield();
  }
  Serial.println("Detected pulse after: " + String(timeDiff) + "us");
}

void FloppyDrive::prepareDrive() {
    Serial.println("Preparing drive...");
    driveSelect(true);
    spinupMotor();
    gotoTrack0();
    Serial.println("Drive is ready");
}

bool FloppyDrive::readIndex() {
    return digitalRead(_indexPin);
}

bool FloppyDrive::readData() {
    return digitalRead(_rddataPin);
}

int FloppyDrive::captureTrack(uint8_t* fluxTransitions) {
    memset(fluxTransitions, 0, MAX_FLUX_PULSE_PER_TRACK);
    int pulseCount = 0;
    int counter = 0;
    while (readIndexFast()); // wait for falling edge (start of index pulse)
    while (!readIndexFast()); // wait for rising edge (end of index pulse)
    while (!readDataFast()); // wait for rising edge (high of data pulse)

    //int prevData = 0; // start with data line high
    //uint8_t transitions = 0;
    uint8_t* fluxEnd = fluxTransitions + MAX_FLUX_PULSE_PER_TRACK;
    while (true) {
        // read data here
        
        // fluxTransitions[0]++; // add clock cycle
        // int dataValue = readDataFast() == 0; // 0 means high, 1 means low
        // transitions += dataValue != prevData; // change in data value means transition
        // prevData = dataValue;
        // uint8_t reachedLimit = (pulseCount < MAX_FLUX_PULSE_PER_TRACK) * 255;
        // uint8_t pulseDetected = (transitions > 0 && transitions % 3 == 0) & reachedLimit; // two transitions means a full data pulse
        // fluxTransitions += pulseDetected;
        // pulseCount += pulseDetected;
        // // if we hit 3 transitions, we are at the start of the next data pulse, so reset. Otherwise keep the current transition count
        // transitions &= (transitions % 3 != 0) * 255;

        // while (readDataFast()); // wait for falling edge (start of data pulse)
        // uint32_t current = micros();
        // uint32_t pulseDuration = current - pulseStart;
        // pulseStart = current;
        // fluxTransitions[pulseCount] = pulseDuration;
        // pulseCount++;
        // delayMicroseconds(1); // wait for data pulse to end

        int counter = 0;
        while (!readDataFast()) counter++;
        while (readDataFast()) counter++;
        fluxTransitions[0] = counter;
        fluxTransitions++;
        pulseCount++;
        if (!readIndexFast()) break; // falling edge of next index pulse, meaning end of track
    }
    Serial.println("Read track: Transitions: " + String(pulseCount));
    return pulseCount;
}

void FloppyDrive::decode_mfm(uint8_t* fluxTransitions, int pulses) {
    std::string temp;
    for (int i = 0; i < pulses; i++) {
        uint8_t pulse = fluxTransitions[i];
        if (pulse <= 25) { // short pulse 10
            Serial.print("10");
        } else if (pulse <= 60) { // medium pulse 100
            Serial.print("100");
        } else { // long pulse 1000
            Serial.print("1000");
        }
    }
    Serial.println();
}