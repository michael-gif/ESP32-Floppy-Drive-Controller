#include <Arduino.h>
#include <FloppyDiskController.h>

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
}

void FloppyDrive::reset() {
    driveSelect(false);
    motorEnable(false);
    motorDirection(true);
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
        int isOnTrack0 = analogRead(_track0Pin);
        if (isOnTrack0 > 2048) break; // pin is active low, so false means the head is on track 0
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
  delay(500); // wait for motor to spin up
  
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

void FloppyDrive::captureTrack(uint8_t* fluxTransitions) {
    while (readIndex()); // wait for falling edge (start of index pulse)
    uint32_t start = millis();
    uint32_t readTime = 0;
    Serial.println("Started reading");

    int pulseCount = 0;
    while (true) {
        // read data here
        uint32_t pulseStart = micros();
        while (readData());
        uint32_t pulseDuration = micros() - pulseStart;
        if (pulseCount < MAX_FLUX_PULSE_PER_TRACK)
            fluxTransitions[pulseCount] = pulseDuration;
        pulseCount++;

        // wait at least 10ms for the index pulse to end
        readTime = millis() - start;
        if (readTime > 10) {
            if (!readIndex()) break; // falling edge of next pulse, meaning end of track
        }
    }
    Serial.println("Finished reading: " + String(readTime) + "ms, Transitions: " + String(pulseCount));
}