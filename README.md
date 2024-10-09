# ESP32-Floppy-Drive-Controller
Floppy drive controller code for an ESP32 Feather  
Inspired by [this adafruit project](https://github.com/adafruit/Adafruit_Floppy/tree/main)

# IDC Connector pinout
![pinout diagram](https://github.com/michael-gif/ESP32-Floppy-Drive-Controller/blob/main/resources/idc_connector_pinout.png)

# General info
### Pins that must be connected
- 8 - Index
- 12 - Drive Select B
- 16 - Motor Enable B
- 18 - Direction
- 20 - Step
- 22 - Write Data
- 24 - Write Enable
- 26 - Track 0
- 28 - Write Protect
- 30 - Read Data
- 32 - Side Select
- 34 - Ready/Disk Change

### Pin info
- You can change which GPIO pins are used for communication with IDC pins, but be warned that all output pins use digitalWrite(), and most input pins use digitalRead().  
If you have no choice and need to use an analog pin for input, change the code accordingly to use analogRead() instead of digitalRead().
- Connecting `Density` is optional, but is recommended to ensure the drive uses the right density.
- If `Direction` is LOW, head moves inwards, if HIGH, head moves outwards.
- `Step` requires a pulse to move the motor. The pulse is 3ms, from LOW to HIGH.
- `Track 0` will output LOW if the head is on track 0, otherwise HIGH. This pin MUST use a digital pin.
- `Index` MUST use a digital pin. Index outputs pulses which the ADC cannot detect.
- `Read Data` pin MUST use a digital pin. It outputs pulses, so you cannot use an analog input for this.
- More info about how the pins works can be found in [this pdf](https://github.com/michael-gif/ESP32-Floppy-Drive-Controller/blob/main/resources/SAMSUNG-SFD321B-070103.pdf)  
The pdf may not be for you're specific drive, but it is good enough.