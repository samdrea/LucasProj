# ESP32 Soil Sensor Project

This repository contains the source code for an ESP32 Feather project using Arduino. Follow the instructions below to set up your environment, install dependencies, and run the code.

---

## Table of Contents
1. [Project Overview](#project-overview)
2. [Hardware Requirements](#hardware-requirements)
3. [Software Requirements](#software-requirements)
4. [Installation](#installation)
5. [Usage](#usage)
6. [Contributing](#contributing)
7. [License](#license)

---

## Project Overview
This project demonstrates how to use the ESP32 to read the outputs of a custom sensor board and uploads the readings to a Google Spreadsheet

---

## Hardware Requirements
- **ESP32** board from Adafruit
- **davispcbv2** board made by Lucas
---

## Software Requirements
You will need the following software installed:
1. **Arduino IDE**
2. **ESP32 Board Package**  
   - Install it via **Arduino IDE**:  
     `Tools > Board > Boards Manager > ESP32 > Install`

---

## Library Dependencies
This project depends on the following custom libraries:

1. **Library 1: multiplxise**: [GitHub Link to Library 1]
  - Download the multiplexise.zip from this repository
  - `Sketch > Include Library > Add .ZIP Library`
  - Select the multiplexise.zip that you downloaded
2. **Library 2:ESP Google Sheet Client**: [GitHub Link to Library 2]
  - `Sketch > Include Library > Manage Libraries`
  - Search "ESP Google Sheet Client"
  - "Install"

---

## Usage

1. Open the main.ino file in Arduino IDE.
2. Ensure the correct board and port are selected.
  - 'Tools > Board: > ESP32 Arduino > Adafruit ESP32 Feather'
3. Upload the code to your ESP32.
4. Check Serial Monitor

5. Open the Serial Monitor (baud rate: 115200).
  - Monitor the Wi-Fi connection status and sensor readings.
6. View Data in Google Sheets

The data will be uploaded to the specified Google Sheet.

