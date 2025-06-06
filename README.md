# Pomagotchi

Tamagotchi for a certain pommy

## Installation

### Prerequisites
- ESP32-S3 development board (tested with ESP32-S3-DevKitC-1)
- USB-C cable
- Arduino IDE or PlatformIO (recommended)

### Setup with PlatformIO (Recommended)
1. Install [PlatformIO](https://platformio.org/install)
2. Clone this repository
3. Open the project in PlatformIO
4. Connect your ESP32-S3 board via USB-C
5. Click the upload button or run `pio run -t upload`

### Setup with Arduino IDE
1. Install [Arduino IDE](https://www.arduino.cc/en/software)
2. Install ESP32 board support:
   - Open Arduino IDE
   - Go to File > Preferences
   - Add `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json` to Additional Board Manager URLs
   - Go to Tools > Board > Boards Manager
   - Search for "esp32" and install "ESP32 by Espressif Systems"
3. Select "ESP32S3 Dev Module" from Tools > Board menu
4. Set USB CDC On Boot to "Enabled"
5. Set USB Mode to "Hardware CDC and JTAG"
6. Upload the code

## Project Structure
- `src/main.cpp` - Main source code
- `platformio.ini` - PlatformIO configuration
- `CMakeLists.txt` - CMake build configuration

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

