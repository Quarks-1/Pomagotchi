# Pomagotchi — Engineering

← [Product overview](../README.md)

Technical companion to the README case study: hardware, firmware architecture, build instructions, and developer workflow for the ESP32 e-ink wellness device.

## Hardware

| Component | Details |
|-----------|---------|
| MCU | [Adafruit Feather ESP32 V2](https://www.adafruit.com/product/5400) |
| Enclosure | Custom yellow 3D-printed case |
| Display | 1.54" 200×200 e-ink (`GxEPD2_154_D67`), SPI pins 12–15 |
| Input | Adafruit Seesaw rotary encoder (I2C) |
| Sensors | VCNL4020 proximity/ambient light, LC709203F fuel gauge (STEMMA QT, SDA=23 / SCL=22) |
| Storage | 2MB LittleFS |

**E-ink pin map** (from `../src/core/main.cpp`):

| Signal | GPIO |
|--------|------|
| RST | 13 |
| DC | 12 |
| CS | 14 |
| BUSY | 15 |
| MOSI / SCK | 35 / 36 (default SPI) |

Optional sensors degrade gracefully — if the light sensor or battery monitor is not detected at boot, the firmware continues without them.

## Architecture

Five FreeRTOS tasks coordinate input, display, game logic, persistence, and sleep management. Shared state is protected by `petStateMutex` and `displayMutex`; tasks communicate via queues.

```mermaid
flowchart TB
    subgraph tasks [FreeRTOS Tasks]
        inputTask[InputTask]
        displayTask[DisplayTask]
        storageTask[StorageTask]
        logicTask[LogicTask]
        sleepTask[SleepTask]
    end
    subgraph hw [Hardware]
        encoder[Rotary Encoder]
        eink[E-Ink Display]
        sensors[I2C Sensors]
    end
    subgraph persist [Persistence]
        littlefs[LittleFS pet_state.json]
    end
    encoder --> inputTask
    sensors --> inputTask
    inputTask --> displayTask
    inputTask --> logicTask
    logicTask --> storageTask
    storageTask --> littlefs
    displayTask --> eink
    sleepTask --> logicTask
```

| Task | Role |
|------|------|
| `InputTask` | Encoder polling, serial input, sensor reads |
| `DisplayTask` | Input dispatch and 100ms display refresh |
| `StorageTask` | Async LittleFS writes via queue |
| `LogicTask` | Stat depletion, sunbathing, petting, star grants |
| `SleepTask` | Inactivity detection and light sleep entry |

See [`../src/core/tasks.cpp`](../src/core/tasks.cpp) for task creation, queue sizes, and stack configuration.

## Project Structure

```
src/core/       main.cpp, FreeRTOS task orchestration
src/pet/        state, depletion, petting logic
src/activities/ water logging, sunbathing, store purchases
src/ui/         cursor, components, 5 screen renderers
src/hardware/   encoder, light sensor, battery monitor
src/storage/    LittleFS JSON persistence
src/system/     sleep manager
src/animation/  sprite sequences
src/assets/     embedded sprites + convert_sprites.py
data/           default pet_state.json (flashed to LittleFS)
scripts/        upload_all.sh
docs/images/    README media
```

## Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/install)
- Adafruit Feather ESP32 V2 with the hardware listed above
- USB cable

### Build and flash

```bash
pio run                    # Build
pio run -t upload          # Flash firmware
pio run -t uploadfs        # Flash LittleFS (required on first boot)
pio device monitor         # Serial console @ 115200 baud
```

Or flash firmware and filesystem in one step:

```bash
./scripts/upload_all.sh
```

**First boot:** You must run `uploadfs` at least once so `data/pet_state.json` is written to LittleFS. Without it, persistence will not initialize correctly.

## Developer Workflow

Pomagotchi is designed for fast iteration on constrained hardware.

### Serial debug console

Connect at 115200 baud and type commands followed by Enter. Run `help` for the full list.

| Command | Purpose |
|---------|---------|
| `help` | List all commands |
| `status` | Show current stats and page |
| `debug` | Toggle debug mode (accelerated timers) |
| `home` / `water` / `sun` / `pet` / `store` | Navigate to a screen |
| `left` / `right` / `enter` | Move cursor and select (headless UI control) |
| `set_thirst <0-100>` | Set thirst level |
| `set_sunlight <0-100>` | Set sunlight level |
| `set_pets <0-10>` | Set affection level |
| `set_stars <value>` | Set star count |
| `stars` | Show total stars |
| `proximity` | Show proximity sensor value and threshold |
| `set_proximity <value>` | Set proximity threshold |
| `sleep_enable` / `sleep_disable` | Toggle auto-sleep |
| `sleep_status` | Show sleep state and inactivity timer |
| `sleep_now` | Force light sleep |
| `stack_info` | FreeRTOS stack high-water marks and free heap |

### Debug mode

Toggle with the `debug` command. When active:

- Star checks run every **10 seconds** instead of 24 hours
- Activity multipliers increase **20×** (water, sunbathing)
- Petting increments **2×** per tick

This lets you exercise full game loops in minutes instead of days.

### Asset pipeline

Sprite bitmaps are embedded as C arrays in `src/assets/sprites.h`. To regenerate from source PNGs:

```bash
python src/assets/convert_sprites.py
```

Requires ImageMagick (`convert`) and Python PIL.

## Persistence

Game state is stored as JSON at `/pet_state.json` on LittleFS. The default seed file is [`../data/pet_state.json`](../data/pet_state.json). Affection is stored as `petStatus` in JSON.

```json
{
    "sunlight": 100,
    "thirst": 100,
    "petStatus": 10,
    "stars": 10,
    "lastStarCheckTime": 0,
    "waterLoggedFlag": 0,
    "sunlightLoggedFlag": 0,
    "petLoggedFlag": 0,
    "hats": {
        "topHat":      { "purchased": false, "wearing": false },
        "cowboyHat":   { "purchased": false, "wearing": false },
        "partyHat":    { "purchased": false, "wearing": false },
        "starHat":     { "purchased": false, "wearing": false }
    }
}
```

The storage task saves every 30 seconds and on stat-changing events (activity logged, hat purchased, etc.).

## Troubleshooting

| Problem | Fix |
|---------|-----|
| Blank stats or boot errors | Run `pio run -t uploadfs` to flash the default save file |
| Light sensor not found | Expected if VCNL4020 is disconnected; firmware continues |
| Battery monitor not found | Expected if LC709203F is disconnected; firmware continues |
| Serial not responding | Confirm 115200 baud and correct USB port |

## Tech Stack

C++ · Arduino framework · PlatformIO · FreeRTOS · GxEPD2 · ArduinoJson · LittleFS
