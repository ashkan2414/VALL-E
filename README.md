# VALL-E: Valve Actuator Lobe-Less Engine
### University of Waterloo | Mechatronics Engineering Capstone (2025-2026)

**VALL-E** is a full-stack development project (Mechanical, Electronics, Firmware) aiming to solve the primary limitation of modern Internal Combustion Engines (ICE): the mechanical camshaft.

### Project Context
Traditional engines rely on a fixed mechanical linkage (the camshaft) to open and close cylinder valves. This forces a compromise: an engine tuned for highway efficiency performs poorly at low speeds. Furthermore, to prevent "valve float" at high RPMs, traditional systems require extremely stiff valve springs. Compressing these springs thousands of times per minute creates significant parasitic drag on the crankshaft, wasting usable power.

**VALL-E** replaces this mechanical constraint with a high-speed **Voice Coil Actuator (VCA)**, enabling a **Camless Engine** architecture.

### Why This Matters
By decoupling valve motion from crankshaft rotation, the VALL-E system offers three distinct efficiency advantages:

1.  **Elimination of Mechanical Parasitic Loss:** Traditional valvetrains consume engine torque to fight the friction of the timing assembly and compress heavy valve springs required to avoid high-RPM valve float. VALL-E removes the physical camshaft connection, reclaiming this energy.
2.  **Infinite Variable Valve Timing (VVT):** Enables precise control over lift, duration, and phasing for every single combustion cycle, independent of engine speed.
3.  **Reduced Pumping Losses:** Allows for "throttle-less" operation by using valve lift height to regulate air intake, significantly increasing thermal efficiency.

To ensure all third-party dependencies (like Eigen, fmt, and the STM32 HAL) are correctly downloaded, you need to initialize the Git submodules.

You can add this section to your **main project README**:

## Getting Started

### Cloning the Repository
This project uses Git submodules for its dependencies. To clone the repository and all dependencies at once, run:

```bash
git clone --recurse-submodules https://github.com/ashkan2414/VALL-E.git
```

### Initializing Submodules (if already cloned)
If you have already cloned the repository without submodules, run the following commands from the project root:

```bash
git submodule update --init --recursive
```

*This will pull in the necessary libraries located in `firmware/Thirdparty`, including Eigen, fmt, and the STM32CubeG4 HAL.*

## Firmware

See the [firmware/README.md](firmware/README.md) for detailed instructions on developing, building, flashing, and debugging the firmware.
