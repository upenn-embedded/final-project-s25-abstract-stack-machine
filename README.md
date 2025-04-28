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

| ID     | Description                                                                                               | Validation Outcome                                                                          |
| ------ | --------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------- |
| SRS-01 | The IMU 3-axis acceleration will be measured with 16-bit depth every 100 milliseconds +/-10 milliseconds. | Confirmed, logged output from the MCU is saved to "validation" folder in GitHub repository. |

#### 3.2 Hardware Requirements Specification (HRS) Results

*Based on your quantified system performance, comment on how you achieved or fell short of your expected requirements.*

*Did your requirements change? If so, why? Failing to meet a requirement is acceptable; understanding the reason why is critical!*

*Validate at least two requirements, showing how you tested and your proof of work (videos, images, logic analyzer/oscilloscope captures, etc.).*

| ID     | Description                                                                                                                        | Validation Outcome                                                                                                      |
| ------ | ---------------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------- |
| HRS-01 | A distance sensor shall be used for obstacle detection. The sensor shall detect obstacles at a maximum distance of at least 10 cm. | Confirmed, sensed obstacles up to 15cm. Video in "validation" folder, shows tape measure and logged output to terminal. |
|        |                                                                                                                                    |                                                                                                                         |

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
