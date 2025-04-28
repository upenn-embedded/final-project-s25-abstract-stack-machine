[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/WXeqVgks)

# final-project-abstract-stack-machine

* Team Number:  4
* Team Name:  Abstract Stack Machine
* Team Members:  Aarti Sabharwal, Sydney Fitzgerald, Maria-Christina Nicolaides
* GitHub Repository URL: https://github.com/upenn-embedded/final-project-s25-abstract-stack-machine.git
* GitHub Pages Website URL: [for final submission]

## Final Project Report

### 1. Video

[Insert final project video here]

* The video must demonstrate your key functionality.
* The video must be 5 minutes or less.
* Ensure your video link is accessible to the teaching team. Unlisted YouTube videos or Google Drive uploads with SEAS account access work well.
* Points will be removed if the audio quality is poor - say, if you filmed your video in a noisy electrical engineering lab.

### 2. Images

[Insert final project images here]

![Alt text](images/IMG_7819.jpeg)

![Alt text](images/IMG_6861.jpeg)

![Alt text](images/IMG_6860.jpeg)

![Alt text](images/IMG_6857.jpeg)

![Alt text](images/IMG_6696.jpeg)

*Include photos of your device from a few angles. If you have a casework, show both the exterior and interior (where the good EE bits are!).*

### 3. Results

*What were your results? Namely, what was the final solution/design to your problem?*

The final solution to our problem of creating a portable musical instrument similar to a piano was to create an Atmega328PB-driven project that also used the Adafruit sound module, a speaker to go along with the sound module, an LCD screen, flex sensors, a pressure sensor, and a buzzer.  The instrument uses ADC on the Atmega to sense whether one of the right hand fingers is bent through the flex resistors attached to the right hand fingers.  When the ADC interrupt picks up on one of the fingers being bent, it sends a signal to the corresponding pin on the Atmega that then sends a GND signal to the corresponding pin on the sound board (which has an audio file for that note attached to it), which then plays the note on the speaker.  On the other hand (the left hand), the pressure sensor, through the same ADC interrupt, is used so that when the interrupt picks up on the pressure sensor being tapped, it sounds the buzzer for a short amount of time (like that of a drum hit).  The gyroscope on the left hand uses I2C communication so that one the left hand is palm-down, the third, fourth, and fifth fingers of the right hand are E, F, and G, and when the left hand is palm-up, they are A, B, and treble C respectively.

#### 3.1 Software Requirements Specification (SRS) Results

*Based on your quantified system performance, comment on how you achieved or fell short of your expected requirements.*

*Did your requirements change? If so, why? Failing to meet a requirement is acceptable; understanding the reason why is critical!*

*Validate at least two requirements, showing how you tested and your proof of work (videos, images, logic analyzer/oscilloscope captures, etc.).*

| ID     | Description                                                                                                                                                                                                                                         | Validation Outcome                                                                                     |
| ------ | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------ |
| SRS-01 | The system shall get the current note being played and print the note out on the screen within 1s.                                                                                                                                                  | EXAMPLE:  Confirmed, logged output from the MCU is saved to "validation" folder in GitHub repository. |
| SRS-02 | There shall be a separate input capture interrupt for each finger on the note-playing hand, so that each can output a note when that finger is pushed down.                                                                                         |                                                                                                        |
| SRS-03 | A timer shall be used to generate a low PWM signal for the buzzer acting as the drum whenever the finger for the drum is down.                                                                                                                      |                                                                                                        |
| SRS-04 | The system shall be able to distinguish from a slight twitch in a finger to actually playing the note using debouncing by only outputting a note if the finger was “down” for longer than a half a second.                                        |                                                                                                        |
| SRS-05 | Voltage across each force resistor shall be analyzed at its own output pin, such that using ADC, we can convert a range of voltages to either on or off, signifying if a finger is fully bent.  If it is, the corresponding sound shall be produced |                                                                                                        |

#### 3.2 Hardware Requirements Specification (HRS) Results

*Based on your quantified system performance, comment on how you achieved or fell short of your expected requirements.*

*Did your requirements change? If so, why? Failing to meet a requirement is acceptable; understanding the reason why is critical!*

*Validate at least two requirements, showing how you tested and your proof of work (videos, images, logic analyzer/oscilloscope captures, etc.).*

| ID     | Description                                                                                                                                                                                     | Validation Outcome                                                                                                                 |
| ------ | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------- |
| HRS-01 | Five flex sensors shall be used, one on each finger of a glove, to detect finger bending for note generating. Each sensor shall detect a range of motion of at least 90 degrees.                | EXAMPLE:  Confirmed, sensed obstacles up to 15cm. Video in "validation" folder, shows tape measure and logged output to terminal. |
| HRS-02 | One force-sensitive resistor shall be used to produce the drum beats and shall detect pressures in the range of 0.2 N to 10 N, only sounding a drum once if a finger is held down continuously. |                                                                                                                                    |
| HRS-03 | An audio breakout board shall be used to generate a PWM signal for a speaker, which shall produce frequencies in the range of 250 Hz to 1000 Hz.                                                |                                                                                                                                    |
| HRS-04 | A buzzer shall be used to create a drum beat. It shall produce sound using low-frequency PWM signals in the range of 30 Hz to 100 Hz.                                                           |                                                                                                                                    |
| HRS-05 | An LCD screen shall be used to display the current note playing and the beats per minute of the current drum beat, based on the average time between the last four drum beats played.           |                                                                                                                                    |
| HRS-06 | A gyroscope shall be used to change the volume of the music produced and shall detect a range of motion of at least 180 degrees.                                                                |                                                                                                                                    |

### 4. Conclusion

Reflect on your project. Some questions to address:

* What did you learn from it?
* What went well?
* What accomplishments are you proud of?
* What did you learn/gain from this experience?
* Did you have to change your approach?
* What could have been done differently?
* Did you encounter obstacles that you didn’t anticipate?
* What could be a next step for this project?

## References

Fill in your references here as you work on your final project. Describe any libraries used here.

LCD Library (Lab 4: Pong)

I2C Library (written based on documentation)

ADC Code (used initialization code from class repository)

UART Library (only used for debugging/validation, from class repository)
