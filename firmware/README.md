
# Firmware

This repository contains the firmware for the **VALL-E** capstone project. The system is built for the STM32G474 microcontroller, utilizing a modern C++20 architecture designed for high-performance control, digital signal processing, and modular hardware abstraction.

## Table of Contents
- [Prerequisites](#prerequisites)
- [Building the Project](#building-the-project)
- [Debugging](#debugging)
- [Flashing](#flashing)
- [Directory Structure](#directory-structure)


## Prerequisites

To build and debug this project, you must install the following tools and ensure they are added to your system's **PATH**.

### 1. ARM GNU Toolchain (Required)
You must install the [**ARM GCC Toolchain**](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads).
*   **Important:** Download the **64-bit version** (x86_64-arm-none-eabi). The 32-bit version has been known to encounter memory limitations during the compilation of complex C++ templates used in this project.
*   Add the `bin` folder of the toolchain to your system **PATH**.

### 2. CMake
*   **Version:** CMake 3.20 or higher is required.
*   Ensure CMake is accessible via terminal.

### 3. STM32CubeIDE & Programmer
*   [**STM32CubeIDE**](https://www.st.com/en/development-tools/stm32cubeide.html): Installed to provide the necessary GDB server backends for debugging. Add the folder containing `ST-LINK_gdbserver.exe` to your **PATH**.
    * NOTE: STM32CubeMx will not work, you must get the IDE.
*   [**STM32CubeProgrammer**](https://www.st.com/en/development-tools/stm32cubeprog.html): Required for standalone flashing via the CLI. Add the `bin` folder containing `STM32_Programmer_CLI.exe` to your **PATH**.


## Building the Project

The recommended workflow is using **Visual Studio Code**.

1.  Install the **CMake Tools** extension in VS Code.
2.  Open the `VALL-E/firmware` folder.
3.  The extension should automatically detect [`CMakeLists.txt`](CMakeLists.txt).
4.  Select the **arm-none-eabi** kit.
5.  Select the build preset for the app and build type (debug or release).
6.  Click **Build** on the bottom status bar or press `F7`.

The build system uses a modular approach, allowing you to target different applications located in the `Apps/` directory.

### Adding New Applications

1. Create a new folder under `Apps/`  (e.g. `my_app/`)
2. Define a new app configure preset in [`CMakePresets.json`](CMakePresets.json)
    ```json
    "configurePresets": [
        ...
        {
            "name": "<my-app>",
            "hidden": true,
            "cacheVariables": {
                "TARGET_APP": "<my_app>" // Must match folder name
            }
        },
        ...
    ]
    ```

3. Create 2 new configure presets for release and debug
    ```json
    "configurePresets": [
        ...
        {
            "name": "my-app-debug",
            "displayName": "My App (Debug)",
            "inherits": [
                "debug",
                "my-app"
            ]
        },
        {
            "name": "my-app-release",
            "displayName": "My App (Release)",
            "inherits": [
                "release",
                "my-app"
            ]
        }
        ...
    ]
    ```

4. Create 2 build presets using
    ```json
    "buildPresets": [
            ...
            {
                "name": "my-app-debug",
                "configurePreset": "my-app-debug"
            },
            {
                "name": "my-app-release",
                "configurePreset": "my-app-release"
            },
            ...
    ]
    ```

### Adding New Thirdparty Libraries
All thirdparty libraries are located in the [`Thirdparty/`](Thirdparty/) folder. To add a new library:
1. Add the library source code to a new folder under `Thirdparty/`.
    - Most libraries should be added as git modules to simplify updates.
    ```bash
    git submodule add <repo-url> Thirdparty/<library-name>
    git submodule update --init --recursive
    ```
2. Modify the [`Thirdparty/CMakeLists.txt`](Thirdparty/CMakeLists.txt) to include the new library.
    - Follow the existing patterns for adding libraries (e.g. `fmt`, `Eigen`).
    - Most libraries can be added using `add_subdirectory()`.
    - You should not need to modify the main `CMakeLists.txt` file.
3. Ensure that the library is correctly linked in application targets.
4. Commit the changes to your repository (including the updated `.gitmodules` file).

## Debugging

Debugging is configured via the **Cortex-Debug** extension in VS Code. The project includes a pre-configured `launch.json`.

*   **Extension:** Install the `marus25.cortex-debug` extension.
*   **Configuration:** Select **"Debug (ST-Link)"** from the Run & Debug side bar. Make sure the target application is selected in the CMake Tools extension build presets (pick the debug variant).
*   **Features:**
    *   **SVD Support:** Peripheral registers can be viewed during a break using the included `STM32G474.svd`.
    *   **Live Watch:** Real-time variable monitoring is supported.
    *   **Automatic Build:** The `preLaunchTask` is set to "CMake: build", ensuring your latest code is always compiled before a session starts.



## Flashing

While debugging automatically flashes the device, you can perform a **"Pure Flash"** without starting a debug session.

1.  Open the VS Code Command Palette (`Ctrl+Shift+P`).
2.  Run `Tasks: Run Task`.
3.  Select the **Flash (ST-Link)** task defined in `tasks.json`.
    - This uses the `STM32_Programmer_CLI.exe` to flash the compiled binary to the device.

> [CAUTION]
> **Antivirus Note:** Some antivirus software (including Windows Defender) may flag or block the `STM32_Programmer_CLI.exe` when triggered via VS Code tasks. If the flash fails or the terminal hangs, you may need to add an exclusion for the STM32CubeProgrammer installation directory.

## Serial Monitor
To view UART output from the device, you can use any serial terminal application (e.g. PuTTY, Tera Term, etc.) however the recommended method is using the VSCode Microsoft [**Serial Monitor** extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.vscode-serial-monitor).


## Lint and Formatting
The project uses **clang-format** for code formatting and **clang-tidy** for linting.

These are integrated into the VS Code workflow via the [**Clang-Format** extension by Xaver Hellauer](https://marketplace.visualstudio.com/items?itemName=xaver.clang-format) and the [**CS 128 Clang-Tidy** extension](https://marketplace.visualstudio.com/items?itemName=CS128.cs128-clang-tidy).

There are pre-configured settings in `.vscode/settings.json` to automatically format code on save and to use the provided [`.clang-format`](.clang-format) and [`.clang-tidy`](.clang-tidy) configuration.

## Directory Structure

The project is organized to separate hardware abstraction, core logic, and specific applications:

*   **`.vscode/`**: Contains build, launch, and task configurations.
*   **`Apps/`**: Contains the main entry points for different project targets.
*   **`Common/`**: The heart of the VALL-E library.
    *   `Inc/valle/core/`: Core system components (Logging, Tasks, Timekeeping).
    *   `Inc/valle/core/device/`: Hardware Abstraction Layer (HAL) traits and registry.
    *   `Inc/valle/platform/drivers/`: High-level drivers for ADC, HRTIM, I2C, and UART.
    *   `Inc/valle/math/`: Control theory components (PID, Biquad filters, Matrix math via Eigen).
    *   `Inc/valle/platform/modules/`: Drivers for specific external hardware (ACS724, LDC1612, VCA).
    *   `Inc/valle/utils/`: Utility functions and classes (RingBuffer, CircularBuffer, CRC).
    *   `Src/`: Implementation files corresponding to the headers in `inc/valle/`.
*   **`Core/`**: Contains CubeMX-generated initialization code and interrupt service routines.
*   **`cmake/`**: Toolchain files and STM32-specific CMake logic.
*   **`Thirdparty/`**: External dependencies including:
    *   **Eigen**: Linear algebra library.
    *   **fmt**: Modern C++ formatting.
    *   **magic_enum**: Static reflection for enums.
    *   **STM32CubeG4**: The standard ST Microelectronics HAL/LL drivers.