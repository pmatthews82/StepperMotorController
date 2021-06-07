# Stepper Motor Controller

## Summary

This project is a complete Arduino sketch (built and tested for the Arduino Uno) which operates a [belt and stepper motor-driven linear actuator](https://openbuildspartstore.com/v-slot-nema-17-linear-actuator-bundle-belt-driven/) with an Arduino Uno via the [Big Easy Driver](https://www.sparkfun.com/products/12859). The system carries out the following functions:

* Receives user input on the target velocity and stop position of the system
* Moves the gantry plate up and down the linear rail while ramping up, holding a target velocity (maximum 30cm/s), and ramping down to a complete stop
* Carries out a homing step before each run, ensuring replicability between runs


## Instructions

_NOTE: Read all the way through the instructions before beginning the process._

1. Build the [linear actuator](https://openbuildspartstore.com/v-slot-nema-17-linear-actuator-bundle-belt-driven/)'s mechanical portion first 
2. Connect the Arduino Uno to the Stepper Motor via the Big Easy Driver. Follow these [instructions](https://learn.sparkfun.com/tutorials/big-easy-driver-hookup-guide?_ga=2.116967247.876811756.1622225245-1078570728.1616535972), but instead of connecting to Arduino pins 2-7, connect the wires to pins 4-9.
3. Connect the positive and ground wires of the limit switch to the Arduino 5V and GND pins (maybe using a breadboard), and connect the limit switch output to pin 2 on the Arduino
4. Connect the positive and negative power supply wires of the Big Easy Driver to a DC power supply. Voltage must be in the 12-24V range, and set max current significantly higher than needed for the motor (~4A max should be fine). The current limit should be as high as possible because the Big Easy driver has a current limiter in place, so if the power supply limits current to the driver, it could damage the driver
5. Connect the Arduino to the computer via a USB cable, and upload the sketch to the Arduino using the [Arduino IDE](https://www.arduino.cc/en/software). Take note of the port number assigned to the Arduino by the computer once the sketch is uploaded (in the top bar, go to Tools -> Port and see which Port # is being used by the connected Arduino)
6. At this point, the homing sequence will automatically start, with the motor set to 1/4 microstepping mode. 
7. Once the homing sequence is complete, open the Serial Monitor in the Arduino IDE ("_Tools_" menu in the top bar). The window should ask you to input your target velocity and desired end point, which can be supplied via the text entry bar in the same window. Before sending commands, make sure the "No line ending" option is selected at the bottom of the window.
    1. If your entered values are outside the acceptable range specified, the system will overwrite your inputted value with more reasonable values.
    2. The default 1/4 microstepping mode should allow belt speeds of upto ~30cm/s, which is also the max actuator bundle speed listed by the vendor on their website. Higher speeds might be possible by changing it to a 1/2 microstepping mode (refer to the "Speed Modification" section), but might lead to unexpected mechanical system behaviour and/or damage since that is an untested microstep level.
7. Once both values are entered correctly, the system will ramp up its speed using the lowest possible acceleration to reach the target velocity in the first 10cm of travel. After the first 10cm of travel, it will enter its constant velocity mode, which it will maintain until reaching the endpoint inputted by the user. Once the endpoint is reached, it will ramp down velocity over the next 15cm before coming to a complete stop
8. At this point, the system will ask the user to enter any keyboard value to re-initiate the homing process. Once this is done, the system reverts to step #7 and it can be re-run an indefinite number of times with different target velocities and endpoints.

## Speed Modification [OPTIONAL]

_NOTE: The system will try its best to reach the target velocity specified by the user, but it is based only on rough estimates. To validate the true speed of the gantry, it is best to other methods, such as high-speed cameras. If the system is bottlenecked by any factor (e.g. current supply, Arduino Uno clock speed, etc.) the speed will be limited to the highest speed allowable by that bottleneck and nothing higher._

_NOTE: The maximum speed listed by the vendor of the actuator bundle is ~20,000mm/min or ~33.33 cm/sec. The motor by itself can spin up to 900rpm at 24V, which translates to ~90cm/sec assuming 6cm of belt is moved per revolution of the motor. However, it is a good idea to stay within the 33cm/sec range listed on the vendor's website to avoid unforeseen issues._


One factor to consider when adjusting the motor's speed is the tradeoff with resolution/smoothness of motion. Stepper motors including this one are capable of an operation mode known as micro-stepping, which results in smaller, more precise steps while sacrificing top speed. This system has been programmed to run at 1/4 microstepping by default, but that can be changed easily in the code to run in the 1/16, 1/8, 1/2 or even full-step mode. At the highest microstepping level (1/16), the max linear belt speed is only ~7.5cm/s, but using full-steps, the theoretical max speed is ~120cm/s. This maximum speed will most likely be bottlenecked by the motor's max listed speed of ~90cm/s, as mentioned above. 

The breakdown of microstepping modes and their estimated max belt speeds are as follows:
* 1/16: 7.5cm/s
* 1/8: 15cm/s
* **1/4: 30cm/s [Default]**
* 1/2: 60cm/s
* 1: 120cm/s

As mentioned before, speeds above 30cm/s are not recommended, since the vendor's max listed speed is ~33cm/s. However, highest speeds may be possible (they just haven't been tested in our lab) through changing the microstepping level to 1/2 or 1.

To change the microstepping mode, the following settings need to be changed in the Arduino sketch/code:
1. At the top of the code, the variable for `steps_per_cm` needs to be set to the correct value (e.g. `sixteenth_steps_per_cm`, `eighth_steps_per_cm`, etc.)
2. Next, the 3 pins corresponding to the `MS1`, `MS2` and `MS3` values need to be set according to the _"Microstep Select Resolution Truth Table"_ found in the [setup instructions](https://learn.sparkfun.com/tutorials/big-easy-driver-hookup-guide?_ga=2.116967247.876811756.1622225245-1078570728.1616535972).

## Troubleshooting Tips

* If the motor stutters or stops, try adjusting the max current potentiometer on the Big Easy Driver using a screwdriver. Reducing the max current (anti-clockwise) setting seems to help prevent this problem. 
* Pressing the reset button on the Arduino can help reset the position of the gantry plate, since that will initiate the homing sequence to run.
* If the actuator is behaving abnormally, moving faster than you expect it to, or is on track to hitting the far end of the rail, turn off the power suppply to stop the unexpected behaviour and prevent damage to the mechanical components. Troubleshoot your system and ensure that everything is setup correctly before re-powering the system and reattempting the run. 
