# CASA0016-Smart clothesline

Xiaoling la

GITHUB REPO PATH : <https://github.com/Lalalaconcerto/Automatic-clothesline-system>

## Project Context

Though dyers are widely used in daily life, drying clothes outdoor using a clothesline is also a universal and useful way in some cases. Therefore, a smart clothesline system that is able to automatically collect clothes when it rains and move clothes into the sun when it stops raining is a good and convenient choice, especially for people living in rainy areas and having no time to take care of drying clothes.

 

In this project, a smart clothesline system is designed which provides users with both automatic and manual functions for controlling the clothesline. In addition, an alarm system consisting of LED strips and a buzzer is attached to the main control system, as well as a system for collecting and visualizing rainfall data detected.

## Project Aims

**Target users:** 

Busy workers, solitary, anyone who wants to’ free their hands’, weather observers.

**Goal:** 

1. Automatically collect clothes from outdoor to indoor when it rains/snows and move clothes to dry outdoors when it stops raining/snowing by detecting rainfall and controlling clothesline using a DC motor.

2. Provide users with two manual control switches to control the clothesline.

3. Alert users visually and audibly about the changing outdoor weather(rain/snow) by LED strips and a buzzer.

4. Detect and collect the rainfall values by a rain sensor and upload them online. Provide users with analysed and visualized rainfall data by MQTT server and Grafana.

## Design Rationale

#### Initial sketch and completed product

<img width="511" alt="微信图片_20220111163937" src="https://user-images.githubusercontent.com/92024194/148994181-f76fabba-f0e4-4a38-814d-c2e9dc76892a.png">


#### Video

Youtube path:

#### Components and supplies

|        | Components | Number      | 
|:----   | :----      |    :----:   | 
|**Board**    | Arduino Uno Wi-fi Rev2   | 1 |
|**Sensor**   | Raindrop/snow sensor     | 1 |
|**Actuator** |DC Motor,12V              | 1 |
|             |LED strips                | 1 |
|             |Piezo                     | 1 |
|**Other components**|Switch                      | 2 |
|                    |H-Bridge motor drivers L293D| 1 |
|                    |Resistor 10k ohm            | 2 |
|                    |Raspberry Pi 4 Computer     | 1 |
|                    |Wires
|**Packaging materials**|Cardboard
|                       |String
|                       |Rubber band
|**3D printed device** |Rectangular support frame     | 2 |
|                      |LED lampshade                 | 1 |
|                      |Gear with gear grooves        | 1 |
|                      |Gears with cylindrical grooves| 1 |

#### Apps and online sevices

| **Name**    | **Description and Justification**                            |
| ----------- | ------------------------------------------------------------ |
| Arduino IDE | A basic programming tool for the Arduino Uno Wi-fi Rev2 board |
| MQTT Server | To collect rainfall data and transform data from MQTT to Grafana  for data visualization |
| Putty       | A basic programming tool for the Raspberry pi                |
| Grafana     | A website to visualise collected data of rainfall            |

#### Operation logics

There is one sensor and three actuators in this project. the sensor namely rain sensor and the actuators namely DC motor, LED strips and Piezo. Rain sensor functions to detect the occurrence of rain/not. The DC motor can automatically adjust the clothesline according to the amount of rain, so that the clothes are transported indoors when it rains, and on the contrary, they are transported outside to dry when the rain stops. As actuators, led strips clearly show people the level of the rain by different colours and different display lengths . Piezo, as an alarm, buzzes when it rains to give warnings. Besides, there are two switches in this project, allowing people manually control the system when necessary. 

#### Schematics                 

[![circuit-final.jpg](https://i.postimg.cc/k4Xt3fsk/circuit-final.jpg)](https://postimg.cc/t1wgFdL5)

#### Important libraries 
| **Important library** | **Description  and Justification**  |
| --------------------- | ----------------------------------- |
| Adafruit_NeoPixel.h   | For the flashing animation of LEDs. |
| PubSubClient.h        | To send and receive MQTT data       |
| WiFiNINA.h            | To connect to wi-fi                 |

#### Design diagram

Path: https://lalalaconcerto.files.wordpress.com/2022/01/automatic-clothesline-diagram-1.pdf

## **System Design**

##### 1. Main control system

Rainfall data is detected by the rain sensor [2], then passed to the DC motor to control the movement of the clothesline. The resistance of the uncovered copper traces on the sensing pad of rain sensor is inversely related to the amount of water falling on its surface. H-bridge [3], as a new component, is introduced for changing the direction of motor rotation. Its internal circuit is given in the figure. 

[![3967f962eef18a3944f0c2d9ccbfc06.png](https://i.postimg.cc/500jkjNJ/3967f962eef18a3944f0c2d9ccbfc06.png)](https://postimg.cc/LqcHJ91Q)

When transistors Q1 and Q4 are both switched on and transistors Q2 and Q3 are switched off, the motor will spin either clockwise or counter-clockwise. When the transistors’ states change, the current through the motor will be reversed so that the direction of its rotation will be reversed. On Arduino, ‘digitalWrite(in1, LOW); digitalWrite(in2, HIGH)’ is used to control the states of input pins and output pins.

Two switches were added to manually control the motor to spin either clockwise or counterclockwise.

##### 2. Alarm system

The alarm system consists of two actuators, namely LED strips and a buzzer [4]. Rainfall values are mapped into the frequency of the buzzer sound and the color and flashing a range of the LED strips.  

To make it more visually intuitive, different colors and different ranges of flashing LEDs are designed to vary according to rainfall. Therefore, a new library <NeoPixel> is applied. This library can create various animations and effects on LED strips. The design of the flashing animation of the LED strips in this project was inspired by ‘LEDStrip Effect – Cylon’ example on Tweaking4All [5]. It looks like a beating flame or heartbeat. Two ‘For’ loops are implemented to move the LEDs that are on back and forth. The two LEDs around the LED that is on are dimed by changing their RGB values. Rainfall is classified into three levels from 0 to 900. The LEDs turn blue, yellow, and red depending on the rain level. Also, the height of the display range is limited by using the ‘map’ function so that the height of movement of bright lights will rise when it rains heavily. The figure of LEDs’ effects depending on different rainfall is given.

<img width="422" alt="76b77f245042cd76af072469d66d557" src="https://user-images.githubusercontent.com/92024194/148984603-93bb7411-1e55-49f3-95f1-74e7f3faf3a4.png">

#### Data proccessing and visualisation

In addition to LED strips and buzzers working as actuators to visualize data in practical, rainfall data collected is processed and transferred to MQTT and stored in Raspberry pi, visualized on Grafana.

**·  MQTT**

Rainfall values detected are mapped into a range from 0 to 900 corresponding to the rainfall from small to large. A reminder is also implemented to display whether it rains outdoor by the ‘if, else’ statement. <WIFININA.h> is introduced as a new library to connect to Wi-fi for Arduino Uno Wi-fi Rev2 board [6][7]. 

Given is the screenshot of rainfall topic on MQTT server.
 
 
<img width="375" alt="mqtt" src="https://user-images.githubusercontent.com/92024194/148984710-bc8c047b-e056-415e-993b-618752003273.png">
 
 

 
**·   Grafana and Raspberry pi**

 
Grafana provides users with visualized rainfall data. Rainfall over a period of time is able to be visualized, be counted and be analyzed on Grafana. Rainfall data on MQTT server is transferred and stored in Raspberry pi. This process is based on the workshop of CE [8].

Given is the screenshot of rainfall data on Grafana.

![grafana](https://user-images.githubusercontent.com/92024194/148984768-fb40e2a8-6477-4b5e-bf34-0aaa0cc26519.png)

 
 

#### **Custom parts and enclosures** 

<img width="680" alt="ab3625a001bd2f09c6e2a87b42f4344" src="https://user-images.githubusercontent.com/92024194/148984828-16c0d24f-cd0d-44f1-b7dc-8ae187ad339c.png">

 
 
 Since many pins should be fixed on the main body, a combination of the cartons and 3D printed components was selected instead of 3D printing for the whole body. Two chopsticks were installed as the main support. A notch in the shape of the small gear of the shaft of the motor was cut on the printed pulley A. A cylinder groove was cut in the middle of printed pulley B. The clothesline was inserted in the groove on the side of the two pulleys. Besides, an LED lampshade and a frame supporting the sticks were also printed. A rubber band was added to pulley A to increase the friction and drive the string to rotate. Thus, the friction on the side of pulley B was smaller than that of pulley A, then the clothesline can be driven to move.

 

The whole circuit is reorganized in order to adapt to the position of the components fixed on the box. Tapes are used to fix the wires. It should be noticed that part of the pins of components are inserted in the cardboard so that wires can be connected to them from the bottom of the cardboard. 

 

## Reflection
#### Feedback and future improvements

The connection of the circuit and the components and the writing of the code are completely built. At the same time, the functions of automatic and manual collection and drying of clothes are well realized. Motor was able to be controlled to spin either clockwise or counterclockwise. However, speed of the motor was not in control so that the clothesline is moved quickly by the motor, which caused the clothesline to run erratically, making it easy to derail from the groove in the pulley. The speed of the motor can be controlled using ‘`analogWrite`’ function which allows users to generate a PWM wave in a pin instead of using ‘digitalWrite’ function [9]. 

A new flashing animation was introduced for LED strips. But the strips only have eight LEDs. A longer LED strip could be applied, allowing new flashing animation effects to display more clearly and more obviously. 

 

For programming, map functions were frequently used to map rainfall values into different actuators’ values, which works well. 

Rainfall data was collected and displayed well on MQTT and Grafana. Data displayed on Grafana could be reprocessed more variably. For instance, rainfall over some time can be counted. This allows users to provide information on monthly and daily rainfall in the area where the system is installed.

In addition, more functions could be added to monitor the condition of clothes more comprehensively and allow users to control the system more conveniently. For instance, a miniature camera can be attached to the device. The manual control system can also be installed on the mobile phone by Bluetooth, which allows the user to remotely control the direction of the clothesline to prevent mechanical failure of the automatic control system.

 

#### Key learning and challenges 

‘bool’[10], as a data type, can hold one of two values, true or false. An important variable called ‘raindetected’ is defined in the type of ‘bool’. Its change can terminate the current loop and make it jump to other loops. This solves the problem that the motor rotates intermittently. The logic of this is shown in the [design diagram](#_2.6_Design_diagram). 

The application of H-bridge is important, which is explained in 2.6.

This project requires two pulleys with different coefficients of friction to rotate the clothesline, which is difficult to operate and it is easy to happen that the clothesline is stuck and cannot follow the rotation of the pulley. Though rubber bands can increase friction very well, using two motors to rotate in the same direction will solve this problem and make the clothesline move more smoothly.

 

#### Conclusion

To conclude, this project is lab-scale and can only be used as a simulation. In actual applications, the materials of the parts need to be replaced with harder and more waterproof materials. The packaging also needs to be more complete. An integrating project attached with the real-time monitoring device and remotely control system is expected to realize in the future.



## Reference

[1] Salihi, Irvan & Hulukati, Stephan & Humena, Steven. (2019). Designing an Internet of Things Based Automatic Clothesline. Journal of Sustainable Engineering: Proceedings Series. 1. 240-246. 10.35793/joseps.v1i2.33.

[2] WatElectronics.com: WatElectronics, What is a Rain Sensor : Working & Its Applications (2021) [online] Available at: <https://www.watelectronics.com/rain-sensor/#:~:text=%20What%20is%20a%20Rain%20Sensor%3F%20%201,LM393%20IC%2C%20output%20LED%20%26%20Power...%20More%20> [Accessed 11 January 2022].

[3] Cdn.arduino.cc. 2022. *Arduino Reference*. [online] Available at: <https://cdn.arduino.cc/reference/en/language/functions/advanced-io/tone/> [Accessed 11 January 2022].

[4] Cox, C., 2022. *How to Control DC Motors With an Arduino and an L293D Motor Driver*. [online] Circuit Basics. Available at: <https://www.circuitbasics.com/how-to-control-dc-motor-speed-and-direction-using-l293d-and-arduino/> [Accessed 11 January 2022].

[5] Tweaking4All: Luijten, H., LEDStrip effects for NeoPixel and FastLED (2022) [online] Tweaking4All.com. Available at: <https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/#LEDStripEffectCylon> [Accessed 11 January 2022].

[6] DEV Community: How to connect Arduino UNO Wifi Rev.2 to WiFi (2022) [online] Available at: <https://dev.to/mikulskibartosz/how-to-connect-arduino-uno-wifi-rev-2-to-wifi-59ml> [Accessed 11 January 2022].

[7] Workshops.cetools.org: Sending Soil Data to MQTT,Plant-Monitor (2021) [online] Available at: <https://workshops.cetools.org/codelabs/CASA0014-2-Plant-Monitor/index.html?index=..%2F..casa0014#8> [Accessed 11 January 2022].

[8] Workshops.cetools.org: Installing InfluxDB on RPi,Plant-Monitor (2021) [online] Available at: <https://workshops.cetools.org/codelabs/CASA0014-2-Plant-Monitor/index.html?index=..%2F..casa0014#8> [Accessed 11 January 2022].

[9] Hardware Fun. 2022. *Controlling speed of DC Motors using Arduino*. [online] Available at: <https://hardwarefun.com/tutorials/controlling-speed-of-dc-motors-using-arduino> [Accessed 11 January 2022].

[10] Arduino.cc: bool (2022) [online] Available at: <https://www.arduino.cc/reference/en/language/variables/data-types/bool/> [Accessed 11 January 2022].

 
