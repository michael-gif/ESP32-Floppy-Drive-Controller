# ESP32-Floppy-Drive-Controller
Floppy drive controller code for an ESP32 Feather. Can be adapted to other boards.  
Inspired by [this adafruit project](https://github.com/adafruit/Adafruit_Floppy/tree/main)

# Floppy cable IDC connector pinout
![pinout diagram](https://github.com/michael-gif/ESP32-Floppy-Drive-Controller/blob/main/resources/idc_connector_pinout.png)

# General info
### Pins that must be connected
```
08 - Index
12 - Drive Select B
16 - Motor Enable B
18 - Direction
20 - Step
22 - Write Data
24 - Write Enable
26 - Track 0
30 - Read Data
32 - Side Select
```

### Changing GPIO pins
You can change which GPIO pins are used for communication with IDC pins, but be warned that all output pins use `digitalWrite()`, and most input pins use `digitalRead()`.  
If you have no choice and need to use an analog pin for input, change the code accordingly to use `analogRead()` instead of `digitalRead()`.  
To change GPIO pins, change the defines in the code:
```C++
#define DENSITY_PIN 14       // IDC 2  - ESP32 OUTPUT, IDC INPUT
#define INDEX_PIN 21         // IDC 8  - ESP32 INPUT, IDC OUTPUT
#define DRIVE_SELECT_PIN 32  // IDC 12 - ESP32 OUTPUT, IDC INPUT
#define MOTOR_ENABLE_PIN 15  // IDC 16 - ESP32 OUTPUT, IDC INPUT
#define DIR_PIN 33           // IDC 18 - ESP32 OUTPUT, IDC INPUT
#define STEP_PIN 27          // IDC 20 - ESP32 OUTPUT, IDC INPUT
#define WRDATA_PIN 12        // IDC 22 - ESP32 OUTPUT, IDC INPUT
#define WRGATE_PIN 13        // IDC 24 - ESP32 OUTPUT, IDC INPUT
#define TRK0_PIN A0          // IDC 26 - ESP32 INPUT, IDC OUTPUT
#define PROT_PIN A3          // IDC 28 - ESP32 INPUT, IDC OUTPUT
#define READ_PIN A1          // IDC 30 - ESP32 INPUT, IDC OUTPUT
#define SIDE_PIN A5          // IDC 32 - ESP32 OUTPUT, IDC INPUT
#define READY_PIN A2         // IDC 34 - ESP32 INPUT, IDC OUTPUT
```

### Pin info
- Connecting `Density` is optional, but is recommended to ensure the drive uses the right density.
- If `Direction` is LOW, head moves inwards, if HIGH, head moves outwards.
- `Step` requires a pulse to move the motor. The pulse is 3ms, from LOW to HIGH. Every step change is equivalent to a track change. So stepping 10 times will step 10 tracks.
- `Track 0` will output LOW if the head is on track 0, otherwise HIGH. This pin MUST use a digital pin.
- `Index` MUST use a digital pin. Index outputs pulses which the ADC cannot detect.
- `Read Data` pin MUST use a digital pin. It outputs pulses, so you cannot use an analog input for this.
- More info about how the pins works can be found in [this pdf](https://github.com/michael-gif/ESP32-Floppy-Drive-Controller/blob/main/resources/SAMSUNG-SFD321B-070103.pdf)  
The pdf may not be for you're specific drive, but it is good enough.

# MFM Encoding/Decoding

[Wikipedia page](https://en.wikipedia.org/wiki/Modified_frequency_modulation#MFM_coding)

### Encoding
Use the following rules:
- `0` -> `10` if preceded by a `0`
- `0` -> `00` if preceded by a `1`
- `1` -> `01`

Examples:  
`0000` > `10 10 10 10` > `10101010`  
`0101` > `10 01 00 01` > `10010001`  
`00000000` > `10 10 10 10 10 10 10 10` > `1010101010101010`  
`00011011` > `10 10 10 01 01 00 01 01` > `1010100101000101`

### Decoding:  
Read the encoded stream 2 bits at a time and decode using the following rules:
- `10` -> `0`
- `00` -> `0`
- `01` -> `1`

Examples:  
`10101010` > `10 10 10 10` > `0 0 0 0` > `0000`  
`10010001` > `10 01 00 01` > `0 1 0 1` > `0101`  
`1010101010101010` > `10 10 10 10 10 10 10 10` > `0 0 0 0 0 0 0 0` > `00000000`  
`1010100101000101` > `10 10 10 01 01 00 01 01` > `0 0 0 1 1 0 1 1` > `00011011`