# LoopPedal

## Author

Milo

## Description

Loop pedal firmware code

## Firmware targets

- `LoopPedal.cpp`: pedal skeleton with `Idle`, `Recording`, and `Playing` states.
  Starts in `Idle` with input 1 monitored on both outputs at unity gain. Button
  events, recording, and playback are TODOs; this target does not record yet.
- `AudioTest.cpp`: working tone, passthrough, and automatic recording/loop tests.

Build the skeleton with `make`, then enter DFU mode and upload with
`make program-dfu`.

Build and upload the test firmware with:

```sh
make TARGET=AudioTest
# Hold BOOT, press/release RESET, then release BOOT to enter DFU mode.
make TARGET=AudioTest program-dfu
```

Both targets use the `build/` directory, producing `LoopPedal.bin` and
`AudioTest.bin` respectively. Select the same target when building and uploading.

## Automatic loop test

`AudioTest::AutoLoop` is selected in `AudioTest.cpp`. After audio starts (on reset):

1. **0–2 seconds:** monitor audio input 1 on both outputs.
2. **2–7 seconds:** record input 1 while monitoring it. The Seed LED lights.
3. **From 7 seconds:** repeat the recorded 5 seconds on both outputs indefinitely.
   Live input is no longer mixed in. Press RESET to start a new recording.

USB serial reports `waiting`, `recording`, or `playing` every 100 ms, along with
the live input peak. Opening the serial monitor does not restart the timer.
Start the source before resetting the Seed so you do not miss the capture window.

The recording is mono, at 48 kHz, stored as raw floats in SDRAM (960,000 bytes).
`kLoopOutputGain` controls both monitoring and playback gain. It retains the
previous test's 70x gain; reduce it toward 1.0 for a line-level source. Output
values are bounded to +/-1, but excessive gain still clips and distorts.
This basic test has no overdubbing or smoothing at the loop boundary, so a
discontinuous recording can click when it wraps. The recording is lost on reset.

## Initial audio wiring tests

Set `kTest` in `AudioTest.cpp`, rebuild, and flash the `AudioTest` target for each test:

| Mode                           | Expected result                                                           |
| ------------------------------ | ------------------------------------------------------------------------- |
| `AudioTest::Tone`             | A 440 Hz square wave at 0.99 amplitude on both outputs. Checks outputs only. |
| `AudioTest::StereoPassthrough` | Input 1 passes to output 1; input 2 passes to output 2.                   |
| `AudioTest::MonoPassthrough`   | Input 1 passes to both outputs with 70x gain, for testing a mono pedal input. |

These tests use the Seed's audio codec inputs and outputs, not the general
purpose `A0` ADC pin used by the previous firmware. Use your audio input/output
circuit and an amplified monitor or audio interface to listen.

The Makefile expects `libDaisy` at `../../libDaisy` and `DaisySP` at
`../../DaisySP`. Install/build those libraries or update the paths before running
`make`. With the Seed connected in DFU mode, run `make program-dfu` to flash.
