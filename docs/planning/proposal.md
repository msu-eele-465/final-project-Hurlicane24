# Final project proposal

- [x] I have reviewed the project guidelines.
- [x] I will be working alone on this project.
- [x] No significant portion of this project will be (or has been) used in other course work.

## Embedded System Description

At a high level, explain in 2-3 sentences what your embedded system will do.  Then, in 1-2 sentences each, explain each input, the process, and each output. This section is the bulk of the proposal.

My embedded system will allow the user to test strings up to length 16 on 3 predefined NFAs. The user will select the NFA they wish to test using the keypad (press 1, 2, or 3), and then enter their string using the keypad after they have selected an NFA. An LCD screen will display whether or not the string was accepted or rejected by the NFA, and 2 LED light bars will display the user's inputted string in binary (only 0 and 1 will be allowed in the alphabet, as well as the epsilon character).
I will also use an HC-SR04 Ultrasonic Sonar Distance Sensor from Adafruit to check whether or not the user's hand is close to the sensor. If the user's hand is within a particular distance, the LCD screen will display a regular expression equivalent to the NFA the user has selected. It should also be noted that this will be accomplished by communicating with with the non-launchpad MSP430FR2355 using I2C to carry out this logic.

## Hardware Setup

What hardware will you require? Provide a conceptual circuit diagram and/or block diagram to help the reviewers understand your proposal. Be sure to introduce and discuss your figures in the text.

I will require the parallax membrane keypad, 2 LED light bars, the LCD screen, the non-launchpad MSP430FR2355, the MSP430FR2355 launchpad, and the HC-SR04 sonar distance sensor.

![Circuit Diagram](https://github.com/msu-eele-465/final-project-Hurlicane24/blob/main/docs/planning/Circuit_Diagram_Proposal.jpg)

## Software overview

Discuss, at a high level, a concept of how your code will work. Include a *high-level* flowchart. This is a high-level concept that should concisely communicate the project's concept.

The code at a very high level will work something like this:
1. Internally create predefined NFAs. I will most likely have to create my own hashmap or other data structure for this.
2. Poll the keypad for NFA selection, then store result.
3. Poll the keypad for string selection. The polling will automatically end once the string is 16 characters long, or the user has pressed the 'A' button to indicate they want to cut their string short. Then store result.
4. Put correct signals on LED bar depending on user's input for the string.
5. Take the user's string and run it through the NFA.
6. Display accepted or rejected on the LCD screen.
7. For the entire runtime of the program past the point in which the user selects which NFA they want to try, poll the HC-SR04 sensor and display the equivalent expression if the user's hand gets within a particular range.

8. ![Flow Diagram](https://github.com/msu-eele-465/final-project-Hurlicane24/blob/main/docs/planning/Flow_Chart_Proposal.drawio.png)

## Testing Procedure

Briefly describe how you will test and verify that your project is a success. Think about how you will *demo* the final project. If you need extra equipment for the demo, be sure that you can either bring the equipment or convincingly simulate it. For example, if you want to build a system that uses CAN bus to interface with your car, you won't be able to bring your car into Cobleigh for the demo...

For this project, there are no complicated parts or circumstances need to demo it. It will all simply be on a breadboard and wired together with jumper wires. In order to verify and test my project during its development, I will start by verifying that each piece of hardware works independently of the rest of the system first. This eliminates hardware bugs early on which in my experience is 90% of the problems I have in these projects. Then I will slowly integrate each piece into the system one by one verifying that each addition does not break the old system. I will then repeat this process until every piece is integrated.

## Prescaler

Desired Prescaler level: 

- [ ] 100%
- [ ] 95% 
- [x] 90% 
- [ ] 85% 
- [ ] 80% 
- [ ] 75% 

### Prescalar requirements 

**Outline how you meet the requirements for your desired prescalar level**

**The inputs to the system will be:**
1.  Input 1 will be the keypad input from the user.
2.  Input 2 will be the input from the HC-SR04 sonic sensor

**The outputs of the system will be:**
1.  Output 1 will be the LCD screen
2.  Output 2 will be the 2 LED bars
3.  Output three will be an LED indicating an acceptance or rejection of a string

**The project objective is**

The objective of this project is to provide an easy to use interface for user's to see how NFAs and regular expressions work. This project could be used as a teaching aid in a CS theory class or as a string tester for a more experienced student.

**The new hardware or software modules are:**
1. The new hardware module is the HC-SR04 sonar sensor 


**The Master will be responsible for:**

Reading the and processing the inputs.

**The Slave(s) will be responsible for:**

Deciding what to do based on those inputs, like what to display on the LCD screen, LED bars, etc.

### Argument for Desired Prescaler

Consider the guidelines presented in the lecture notes and convince the reviewers that this proposal meets the minimum requirements for your desired prescale level.

Since my project meets the requirements for inputs and outputs, implements a new, non-trivial hardware module, and attempts to serve a real purpose rather than just being an amalgamation of random hardware and software ideas, I think my project meets the amount of rigor required for the low A level. 
