# LoopPedal

## Author

Milo

## Description

Loop pedal firmware code

## Initial audio wiring tests

Set `kTest` in `LoopPedal.cpp`, rebuild, and flash for each test:

| Mode                           | Expected result                                                           |
| ------------------------------ | ------------------------------------------------------------------------- |
| `AudioTest::Tone` (default)    | A 440 Hz sine wave at 0.1 amplitude on both outputs. Checks outputs only. |
| `AudioTest::StereoPassthrough` | Input 1 passes to output 1; input 2 passes to output 2.                   |
| `AudioTest::MonoPassthrough`   | Input 1 passes to both outputs, for testing a mono pedal input.           |

These tests use the Seed's audio codec inputs and outputs, not the general
purpose `A0` ADC pin used by the previous firmware. Use your audio input/output
circuit and an amplified monitor or audio interface to listen.

The Makefile expects `libDaisy` at `../../libDaisy` and `DaisySP` at
`../../DaisySP`. Install/build those libraries or update the paths before running
`make`. With the Seed connected in DFU mode, run `make program-dfu` to flash.
