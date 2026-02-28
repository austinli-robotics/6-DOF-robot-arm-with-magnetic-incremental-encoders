# 6-DOF Articulated Robot Arm

> ⚠️ **Work In Progress** — Active development ongoing.

A 6 degree-of-freedom articulated robot arm built entirely from scratch — custom actuators, custom gearboxes, and custom encoders. The project spans mechanical design, firmware, and simulation, with every major subsystem designed and iterated in-house.

---

## Goals

1. Achieve **precise positional control** of the end-effector throughout the full reachable workspace.
2. Produce **repeatable, consistent motion** with minimal deviation between repeated trajectories.

---

## Key Features

- **BLDC Motors** — High torque-to-size ratio with lower cost than comparable servo alternatives.
- **Custom 14:1 Cycloidal Gearbox** — Reduces backlash and multiplies output torque in a 27 mm tall package. Brass bushings on roller pins cut down friction between moving PLA surfaces.
- **Spherical-Wrist Configuration** — Simplifies the inverse kinematic equations by decoupling positional and orientational DOFs.
- **Hybrid Encoding** — Custom incremental encoders on wrist/rotation joints; AS5600 absolute encoders on elbow joints.
- **On-board IK Solver** — Closed-form inverse kinematics derived from DH parameters and executed in C++ directly on the microcontroller.
- **DfAM Construction** — All structural parts are FDM-printed in PLA with ≥3 mm wall thickness. M3 fasteners are used universally and adhesives are avoided entirely, so individual parts can be swapped without affecting the rest of the assembly.

---

## Mechanical Design

Each elbow joint is supported by bearings on both sides, distributing load symmetrically so the actuator only needs to drive rotation rather than resist bending moments. Small components like encoder boards are secured with tapered pins, which provide a repeatable, adhesive-free mount that can be adjusted to meet tight clearances.

The motor connects to the gearbox through a hex adapter, intentionally treating the two as independent modules. This simplifies both assembly and future maintenance — the motor or gearbox can be removed without disturbing the other.

---

## Inverse Kinematics

DH parameters were extracted directly from the SolidWorks assembly and used to derive closed-form IK equations for positional end-effector control. The equations were first validated in MATLAB using `RigidBodyTree`, Simscape, and Simulink Multibody before being ported to C++ for on-board execution.

At runtime, the solver evaluates all valid IK solutions for a target position and filters them against each joint's physical travel limits to determine which configurations are actually reachable.

---

## Magnetic Encoding

Two encoder types are used depending on joint requirements:

| Joint Type | Encoder | Interface |
|---|---|---|
| Elbow joints | AS5600 absolute encoder | Analog |
| Wrist / rotation joints | Custom incremental encoder | Analog |

### How the Incremental Encoder Works

Each custom incremental encoder uses two KY-035 linear Hall-effect sensors reading against a ring of 28 alternating-polarity magnets bonded into the rotating element.

The two sensors are physically positioned **90° out of phase** relative to the magnet ring. As the joint rotates, each sensor produces a continuously varying analog voltage in response to the alternating magnetic field — effectively two sine waves offset by a quarter period. By monitoring how both signals change simultaneously, the firmware can determine **both the magnitude and direction** of rotation: if sensor A leads sensor B, the joint is moving in one direction; if B leads A, it's moving in the other.

**Resolution:** Each magnet transition can theoretically be interpolated up to **1,024 steps**, giving a maximum theoretical resolution of:

\[
28 \text{ magnets} \times 1024 \text{ steps/magnet} = 28{,}672 \text{ steps/revolution (theoretical)}
\]

In practice, analog noise and real-world sensor sensitivity limit reliable interpolation to far fewer usable steps per magnet, bringing the effective resolution down significantly from the theoretical ceiling.

**Homing:** Since incremental encoders track *relative* position with no absolute reference, a limit switch is integrated into each joint. On startup, the joint sweeps until the switch detects a notch machined into the rotating element, establishing a repeatable zero-origin before any motion commands run.

---

## Electronics

- **Microcontroller:** Elegoo Mega 2560 R3 — Arduino Mega-compatible with enough analog input pins to support the multiple Hall-effect encoder channels.
- **Wiring:** Directly soldered to encoder pads where connector clearance isn't feasible. Wire management channels are built into the chassis to keep routing clean and strain-free.
- **Connectors:** Custom-length 3-pin JST connectors used throughout for fast, reliable joint-level assembly and disassembly.

---

## Tech Stack

| Domain | Tools / Languages |
|---|---|
| Firmware | C++ (Arduino / Elegoo Mega 2560) |
| CAD | SolidWorks |
| Simulation | MATLAB, Simscape, Simulink Multibody |
| Mechanical | PLA (FDM), M3 hardware, thrust & radial bearings, brass bushings |

---

## Project Status

| Subsystem | Status |
|---|---|
| Cycloidal Gearbox | ✅ Prototyped & tested |
| Joint Mechanical Assembly | 🔄 In progress |
| Incremental Encoders | ✅ Prototyped |
| Absolute Encoders (AS5600) | ✅ Integrated |
| IK Solver (C++) | ✅ Implemented |
| Full Arm Assembly | 🔄 In progress |

---

## Author

**Austin Li**<br>
[LinkedIn](https://www.linkedin.com/in/austin-c-li/) · a564li@uwaterloo.ca
