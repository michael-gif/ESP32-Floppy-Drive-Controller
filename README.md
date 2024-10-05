# ESP32-Floppy-Drive-Controller
Floppy drive controller code for an ESP32 Feather

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
- Connecting Density is optional, but is recommended to ensure the drive uses the right density.
- If direction is LOW, head moves inwards, if HIGH, head moves outwards.
- Step requires a pulse to move the motor. The pulse is 3ms, from LOW to HIGH.
- Track 0 will output LOW if the head is on track 0, otherwise HIGH.
- More info about how the pins works can be found in [this pdf](https://github.com/michael-gif/ESP32-Floppy-Drive-Controller/blob/main/resources/SAMSUNG-SFD321B-070103.pdf)  
The pdf may not be for you're specific drive, but it is good enough.