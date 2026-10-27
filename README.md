# EE109 Speed Trap System

An embedded C speed-measurement system built using an ATmega328P microcontroller. The system measures the speed of an object passing between two sensors and provides visual and audible feedback based on configurable speed limits.

## Features

- **Speed measurement:** Uses two sensors positioned 35 cm apart to calculate speed from elapsed time.
- **Timer-based measurement:** Uses pin-change interrupts and hardware timers to measure elapsed time and handle measurement timeouts.
- **Adjustable speed limits:** Uses buttons and a rotary encoder to configure high and low speed thresholds.
- **Persistent settings:** Stores speed limits in EEPROM so settings can be restored after restarting.
- **LCD display:** Shows measured timing, calculated speed, and configured limits.
- **Visual feedback:** Uses an RGB LED to indicate whether the measured speed is below, within, or above the configured range.
- **Audible alerts:** Triggers a buzzer after three consecutive out-of-range measurements.
- **Servo output:** Adjusts a servo-based indicator according to the measured speed.

## Technologies

- C
- AVR microcontroller programming
- ATmega328P
- AVR-GCC and Make
- Hardware timers and interrupts
- EEPROM
- Embedded hardware interfaces

## Project Structure

| File | Purpose |
|---|---|
| `project.c` / `project.h` | Main application logic, sensor handling, speed calculations, and output control |
| `timer.c` / `timer.h` | Timer configuration, buzzer tones, and servo control |
| `encoder.c` / `encoder.h` | Rotary encoder input and speed-limit adjustment |
| `lcd.c` / `lcd.h` | LCD interface |
| `Makefile` | Build configuration for compiling and generating the microcontroller firmware |
| `.gitignore` | Excludes generated build files and course-specific materials |

## What I Learned

This project gave me experience integrating embedded C software with microcontroller hardware. I practiced working with interrupts and hardware timers, reading sensor inputs, managing persistent settings with EEPROM, and coordinating multiple outputs within a single system.

I also gained experience organizing a multi-file C project and using a Makefile to build firmware for an AVR microcontroller.

## Build

The project targets an ATmega328P running at 16 MHz and uses AVR-GCC and the AVR toolchain.

With the required AVR development tools installed, run:

```bash
make
```

The Makefile builds `main.elf` and generates `main.hex` for the microcontroller.

## Notes

This project was developed as part of USC's EE109 coursework. Some supporting hardware-driver and timer code was adapted from course labs. The repository is intended to document the project and the embedded systems concepts practiced during its development.
