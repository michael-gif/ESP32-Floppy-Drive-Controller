#include <Arduino.h>
#include <FloppyDiskController.h>

#define DENSITY_PIN 14       // IDC 2  - OUTPUT
#define INDEX_PIN 21         // IDC 8  - INPUT
#define DRIVE_SELECT_PIN 32  // IDC 12 - OUTPUT
#define MOTOR_ENABLE_PIN 15  // IDC 16 - OUTPUT
#define DIR_PIN 33           // IDC 18 - OUTPUT
#define STEP_PIN 27          // IDC 20 - OUTPUT
#define WRDATA_PIN 12        // IDC 22 - OUTPUT
#define WRGATE_PIN 13        // IDC 24 - OUTPUT
#define TRK0_PIN A0          // IDC 26 - INPUT
#define PROT_PIN A3          // IDC 28 - INPUT
#define READ_PIN A1          // IDC 30 - INPUT
#define SIDE_PIN A5          // IDC 32 - OUTPUT
#define READY_PIN A2         // IDC 34 - INPUT

FloppyDrive floppy(DENSITY_PIN, INDEX_PIN, DRIVE_SELECT_PIN, MOTOR_ENABLE_PIN, DIR_PIN,
  STEP_PIN, WRDATA_PIN, WRGATE_PIN, TRK0_PIN, PROT_PIN, READ_PIN, SIDE_PIN, READY_PIN);
uint8_t flux_transitions[MAX_FLUX_PULSE_PER_TRACK];

void setup() {
  Serial.begin(9600);
  floppy.reset();
  pinMode(A4, INPUT);
}

void loop() {
  if (analogRead(A4) > 2048) {
    floppy.prepareDrive();
    delay(1000);
    int pulses = floppy.captureTrack(flux_transitions);
    floppy.reset();
    floppy.decode_mfm(flux_transitions, pulses);
    Serial.println();
  }
}