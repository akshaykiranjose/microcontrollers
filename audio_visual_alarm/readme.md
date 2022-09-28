## MicroControllers and MicroProcessors Lab

You have a heat source, temperature sensor, Pt-51 board and a speaker. Build a high temperature audio-visual alarm
system which works as follows:
  - [x] When the temperature exceeds the set value and LED starts blinking at 2Hz frequency, indicating a visual alarm
of temperature rise to the operator. The alarm may be acknowledged by the operator by toggling a switch. If
this visual alarm is acknowledged within 5 seconds, the LED stops blinking, but glows continuously until the
temperature falls below the set value.
  - [x] If visual alarm is not acknowledged, the speaker starts generating a sound in any audible frequency, giving an
audio alarm to the operator. On acknowledgement of the audio alarm by the operator by toggling a switch, both
speaker and LED blinking stop, but LED glows continuously till the temperature falls below the set value.

The Code attached is the implementation of the project on Pt-51 kit using ADC, temperature sensor, 
