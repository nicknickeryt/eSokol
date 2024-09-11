# eSokół
The cheapest possible open source electric bike project built around MY1016Z2 motor and STM32.\
This is stil WIP. Images and details soon!\
\
[image]\
\
Bike specs:\
Romet "Sokół"\
Year: 1972\
Wheel size: 20" 24H (?)\
Weight (original):\
Weight (current):\
Battery: 7s4p 11.6Ah 29.4V, BMS 25A 10-MOS\
Motor: MY1016Z2 24V 250W (rear-drive)\
Motor driver: LB27 250W\
Motor speed: ~300 RPM max\
Motor torque: ~6.65Nm\
Gears: 16T (rear hub - pedals), 16T (rear-hub - motor), 9T (motor - rear hub)\
Motor - hub gear ratio:  9:16=0,5625\
Output torque: ~11,82Nm\
Vmax: ~16 km/h\
Note: the original 9T gear will probably be replaced with a 13T one, which will improve the Vmax to ~23km/h, but with ~1,5 less torque. \

## Why?
[image]\
Well, I've really wanted to make an electric bicycle for a long time. My goal was to do it cheap and somewhat unique while I'll be learning something new.\
This old Sokół bike was rarely used by me, so it was perfect for this kind of mod, as I also planned to modify an older bike while keeping it as much original-looking as I could.\
\
There were many challenged though. This 20" inch folding bike was really rusty, heavy and wasn't intented to use with this kind of ebike conversion kit. \
This MY1016Z2 kits are probably the cheapest ebike conversion kits out there. They are however intented to be mounted on a special plate on 22"-and-larger-wheeled bicycles. The motor could be mounted easily using its original plate on my bike, but I couldn't pedal. This bike, being a folding one, has a really short chain with a lot of free space from my foot to front wheel, but very little space to the rear end. That's why I had to mount the motor in a such weird way.\
[image]\
\
The battery will be mounted at the rear rack in some kind of metal box, which will also house the microcontroller and PCBs.\
[image]\
\
The original throttle grip handle has been modified, as the motor is too powerful at the moment (9:16 ratio). I've used this great "voltage delay mod" based on an RC filter, you can find it here [add link]. I used slightly different components which gave the best results after playing a lot with different capacitor and resistor values. The acceleration is lower and the motor starts pretty smooth, but it's still quite snappy. It should reduce the motor and its chain stress a great amount. This mod reduced the maximum speed slightly, though (1-1,5km/h), so I've added a switch so I can use the more snappy "sport mode". The switch will be replaced with a MOSFET which gate will be controlled by STM32. I've also replaced the handle black rubber and used the bike's original handle. This required some more tuning to the handle, as its diameter was a lot larger than the original handle's inner diameter.\
[image]\
\
Apart from throttle-based speed control, the bike will also provide the ability to control by pedalling. The microcontroller will read the pedalling speed and provide the LB27 motor controller with enough voltage so that the motor will spin the wheel w little bit faster than the pedals would. This is to make the bike road-legal, which is also a goal of this project.\
\
The lighting system got a little bit of love too. The original front headlight is still used, but modified. Apart from having a single bulb, it now also has four 3.3V LEDs and one bright 18V warm LED. Rear lamp case was broken and would be coverd by the battery housing, so I bought a new one. It will be modified similarily though. There will be 4 leds minimum and a bulb. It will also be possible to power the bulbs via dynamo, but that's a topic for later.\
[image]\
\
The microcontroller, lights, PAS sensor and everything microcontroller-related will be powered by a single 3.6V lithium cell, charged independently from the main battery. This is for safety and simplicity. I want to split the electronics to two parts:\
• big: motor, motor controller, throttle, main battery;\
• LITTLE: display, lighting, PAS sensor, microcontroller, second battery.\
It is also planned for the two systems to be fully independent. Both of them will work with each other, as the microcontroller will at least control the motor speed when using PAS-based speed control, read battery and motors temperatures and voltages and switch between "sport" and smooth mode. However, when the STM32 battery is discharged, it will still be possible to control the motor using the throttle, which is the default control method. If the main battery is flat though, the lighting system will work flawlessly, the display will still provide you with speed, time and distance information.\
\
The bike will be controlled using a screen mounted on the handle. It will provide all the details of a basic speedometer, but will also allow to:\
• toggle lights (with different "animation" styles),\
• switch between PAS-based speed control and a throttle-based one,\
• switch between "sport mode" and smooth mode,\
• read motor and battery voltage and temperature,\
• more?\
\
This repo is currently divided into firmware and hardware parts. In firmware, you will be able to find everything related to the STM32 microcontroller firmware. In hardware, it will be possible to browse different 3d-printable part designs, KiCad schematics and libraries, various images and additional hardware-related files. This repo will be updated soon.







