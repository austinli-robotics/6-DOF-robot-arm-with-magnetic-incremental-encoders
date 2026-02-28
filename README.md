# 6-DOF Articulated Robot Arm

> ⚠️ **Work In Progress** — Active development ongoing.

A fully custom 6 degree-of-freedom articulated robot arm designed from scratch, featuring custom BLDC actuators, cycloidal gearbox reduction, and custom incremental magnetic encoding. All structural components are 3D-printed with Design for Additive Manufacturing (DfAM) principles in mind.

---

## Goals

1. **Precise positional control** of the end-effector across the full workspace.
2. **Repeatable motion** with minimal deviation between repetitions.

---

## Key Features

- **Custom BLDC Actuators** — Chosen for their superior size-to-torque ratio and low cost compared to other high-torque alternatives.
- **14:1 Cycloidal Gearbox** — Custom-designed gearbox (27 mm height) that reduces backlash and increases output torque in a compact form factor. Brass bushings around roller pins minimize PLA-on-PLA friction.
- **Spherical-Wrist Configuration** — Joint configuration chosen to simplify inverse kinematic equations.
- **Dual Encoder Strategy** — Hybrid use of custom incremental encoders (KY-035 Hall-effect modules) and AS5600 absolute encoders, selected per joint type.
- **Dual-Bearing Elbow Joints** — Each elbow joint is supported by bearings on both sides to balance load and improve actuator efficiency.
- **Inverse Kinematics on Arduino** — IK equations derived from DH parameters and implemented in C++ directly on the Elegoo Mega 2560 R3, with all reachable solutions tested against joint constraints.
- **DfAM-First Construction** — All custom parts are PLA-printed with ≥3 mm wall thickness, M3 fasteners throughout, and no adhesives to enable modular part replacement.

---

## Mechanical Design

All rotating components are outfitted with appropriate thrust or radial bearings, and threaded inserts are used at all repeated-disassembly interfaces. Tapered pins are used to fix small electrical components such as encoder boards, offering customizable clearances without adhesives.

The motor-to-gearbox interface uses a hex adapter system, decoupling the motor and gearbox as independent assemblies to simplify overall robot assembly and maintenance. Tolerances for interference fits were iterated through physical testing to achieve rigid, precise placement of bearings and pins.

---

## Inverse Kinematics

DH (Denavit-Hartenberg) parameters were derived directly from the SolidWorks assembly for each joint. These parameters were used to calculate closed-form IK equations for positional end-effector control. MATLAB's `RigidBodyTree`, Simscape, and Simulink Multibody were used to simulate and validate motion before deployment.

The IK solver is implemented in C++ on the microcontroller, evaluating all valid IK solutions and checking each against joint travel constraints at runtime.

---

## Magnetic Encoding

Two encoder types are used depending on joint type:

| Joint Type | Encoder Type | Sensor |
|---|---|---|
| Elbow joints | Absolute | AS5600 (I²C) |
| Wrist / rotation joints | Custom Incremental | KY-035 Hall-effect |

### Incremental Encoder Complexity

The custom incremental encoders are one of the more nuanced subsystems in this project. Each encoder uses **two KY-035 linear Hall-effect sensors** reading against a ring of **28 alternating-polarity magnets** embedded in the rotating element.

**How it works:** As the joint rotates, each magnet alternation produces a sinusoidal analog signal on each sensor. With two sensors offset from each other, the pair generates a **quadrature-like signal** — analogous to a two-channel optical encoder — allowing both position counting and **direction detection** based on which sensor leads the other.

**Resolution trade-offs:** The theoretical resolution is:

$$\text{Steps} = \text{Poles} \times \text{Gear Ratio} \times \text{Interpolation Factor} = 14{,}366 \text{ steps (theoretical)}$$

However, in practice only **~1,000 steps** are reliably usable due to analog noise, magnet spacing tolerances, and Hall sensor sensitivity limits at speed.

**Homing:** Because incremental encoders have no absolute position reference, a **limit switch** is incorporated into each joint. The switch detects a machined notch in the rotating element, triggering a homing routine on startup to establish the zero-origin before any motion commands are executed.

---

## Electronics

- **Microcontroller:** Elegoo Mega 2560 R3 (Arduino Mega-compatible) — selected for its additional analog input pins required by the custom Hall-effect encoders.
- **Wiring:** Wires are soldered directly to encoder module pads where clearances are tight. Wire management channels are designed into the chassis to route all wiring cleanly.
- **Connectors:** Custom-length 3-pin JST connectors are used throughout to allow easy assembly and disassembly of modular joints.

---

## Tech Stack

| Domain | Tools / Languages |
|---|---|
| Firmware | C++ (Arduino / Elegoo Mega 2560) |
| CAD | SolidWorks |
| Simulation | MATLAB, Simscape, Simulink Multibody |
| Mechanical | PLA (FDM), M3 hardware, bearings, brass bushings |

---

## Status

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

**Austin Li**
[GitHub](https://github.com) · [LinkedIn](https://www.linkedin.com) · a564li@uwaterloo.ca
