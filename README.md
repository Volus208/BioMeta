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
- 98 switches, I used designed the PCB for Kailh Box White v2
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
By far the hardest and most expensive part. The gerber files are designed to comply with JLCPCB's tolerances. However, if you upload a single file with a visible cutout that is made to snap the to halves apart, they will charge you more. This is why there are only two printed lines to mark where you should cut using a CNC using the provided SVG file located in `pcb/stencil/pcb-Edge_Cuts.svg`. If you don't mind paying more, you can just replace these lines with "edge.cut" lines. As you can see, their shape is made so the two halves can be joined if you want a more traditionnal layout.
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
