# BioMeta
A split 96% keyboard with fingerprint identification that makes the user look smarter

## What is in this repository
- All the files needed to make the PCB
- The custom source code
- The SVG file to make the back plate
- The 3D files needed to make the fingerprint sensor holder
- Documentation (see below)

## What is not in this repository
- Keycaps, I 3D printed mine but didn't get a good enough result that can be put on the switches without several hours of post processing
- The 3D files needed to make the feet that hold the back plate, I will publish them when I find an easy way to make them durable and grippy enough 

## Parts list
This is a list of every single part in the keyboard
- The PCB
- 98 switches
- 98 keycaps
- 7 sets of stabilizers
- 98 1N4148W-TP diodes
- 98 WS2812B-2020 addressable LEDs
- 98 switches, I designed the PCB for Kailh Box White v2
- FPM10A fingerprint sensor
- Fingerprint sensor holder, made of :
    - the main 3D printed part
    - a lasercut plate on the top
    - 2 M3 bolts + another one for holding it in place
    - a switch to turn the sensor on and off because of it's bright green LED, this can be done in software for some other sensors
- A single capacitor to smooth the supply voltage when turning the FPM10A on and off
- 7 M3 bolts to hold the back plate and the PCB together
- 2 S8B-XH-A-1 both the left and right parts can communicate with each other
- A corresponding cable
- 6 feet to hold the back plates at an angle
- 2 Sparkfun Pro Micros
- Foam between the PCB and the back plate, I just cut mine with scissors

## The PCB
By far the hardest and most expensive part. The gerber files are designed to comply with JLCPCB's tolerances. However, if you upload a single file with a visible cutout that is made to snap the to halves apart, they will charge you more. This is why there are only two printed lines to mark where you should cut using a CNC using the provided SVG file located in `pcb/cnc_cut.svg`. If you don't mind paying more, you can just replace these lines with "edge.cut" lines in the KiCad file. As you can see, their shape is made so the two halves can be joined if you want a more traditional layout.
Soldering the tiny 2x2mm LEDS can be done in 3 ways :
- asking the PCB manufacturer to do it
- the traditionnal SMD way, making a soldering stencil and using thermal paste
- the way cheaper but way longer way, by hand. While it is possible, I wouldn't recommend it without a precise soldering iron, a microscope and enough experience in PCB soldering. Here is how I did it :
    - tin one of the pads
    - add flux
    - while heating the pad, use tweezers to solder the LED (check the orientation !). It should be held in place by a single pad
    - put solder on your iron, and add more flux on the PCB if necessary
    - touch the 3 other pads with your iron, one at a time. The solder should get in between the PCB and the LEDS instantly
    - repeat 97 more times. See why I don't recommend this way to everyone ?

Don't worry, soldering the diodes is easier and can be done in a few minutes. All of the other components are THT.

## The backplate
The SVG file is in `pcb/backplate.svg`. I made mine out of aluminium, but you can really use anything you want. If you are cutting your backplate with a CNC, remember to file the corners where the two parts meet. If you are using a laser cutter, they should fit together without any post processing.

## The firmware
This keyboard doesn't run QMK because that firmware is missing two features : finger print sensor support and a communication protocol between the two arduinos that isn't I²C or UART. The second reason is the biggest problem : the UART is already used for the fingerprint sensor and the I²C protocol seems to have problems with the Adafruit Neopixel library (it has something to do with interrupts). This is why I had to make my own protocol, which runs at an undefined frequency.
### Communication protocol between the two parts
This protocol doesn't support hotplug so you might need to restart the keyboard if one of the two Arduinos reboot.
![How it works](/protocol.png)
COM_DELAY is configurable in the code by modifying COM_SPEED. The communication can probably be a lot faster, feel free to try and push it as fast as possible until you get errors.
### Fingerprint ID
The fingerprint sensor feature types a string of you choice automatically if your fingerprint is identified, basically like a macro. To set that string, enter it in `arduino_code/left_part/password.h`.
To use it, press AltGr + Fn to begin the serial communication with the sensor. When a valid fingerprint is detected, the password will be sent and the serial port will be closed.
### Fn key
Did you know that the Fn key isn't actually a key ? There is no ASCII code for it, so only the keyboard can recognize it for custom actions. Here, it changes the LED lighting mode or enables the fingerprint sensor.
