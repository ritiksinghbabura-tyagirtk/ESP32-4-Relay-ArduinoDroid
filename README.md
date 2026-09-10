# ESP32-4-Relay-ArduinoDroid
ESP32 4-Relay WiFi Home Automation with ArduinoDroid — beginner-friendly code, wiring, Android setup, and USB OTG upload guide.
ESP32 4-Relay WiFi Home Automation

A beginner-friendly ESP32 4-channel relay control project that can be programmed directly from an Android phone using ArduinoDroid.

This project is designed for people who want to start with ESP32, WiFi, relay modules, and basic home automation without requiring a computer.

✨ Features

- 📱 Program ESP32 using an Android phone
- 🔌 ArduinoDroid support
- 📡 WiFi-based relay control
- ⚡ 4 independent relay channels
- 🌐 Simple mobile-friendly web control
- 🔧 Beginner-friendly wiring and setup
- 🚀 Upload firmware through USB OTG
- 🏠 Suitable as a starting point for IoT and home automation projects

🔌 Relay Pin Configuration

The default relay GPIO configuration is:

Relay| ESP32 GPIO
R1| GPIO 5
R2| GPIO 17
R3| GPIO 16
R4| GPIO 4

«Do not change these GPIO numbers unless your relay board uses different connections.»

📱 Programming ESP32 with Android

You can build and upload this project using an Android phone.

Requirements

- ESP32 development board
- Android phone
- USB OTG adapter/cable
- ArduinoDroid
- USB data cable
- 4-channel relay module
- WiFi network or mobile hotspot

Step 1 — Install ArduinoDroid

Install ArduinoDroid on your Android phone.

Open ArduinoDroid and make sure ESP32 board support is available.

Step 2 — Connect ESP32

Connect the ESP32 to your Android phone using:

Android Phone
     │
   USB OTG
     │
 USB Data Cable
     │
    ESP32

When Android asks for USB permission, select Allow.

Step 3 — Select ESP32 Board

In ArduinoDroid:

1. Open the board selection menu.
2. Select the appropriate ESP32 board.
3. Select the connected USB/serial device if required.
4. Keep the ESP32 connected during compilation and upload.

«Board names and ArduinoDroid menus can vary depending on the ESP32 board and ArduinoDroid version.»

⚙️ Step 4 — Configure WiFi

Open the ".ino" file.

Find:

const char* SSID = "YOUR_WIFI_NAME";
const char* PASSWORD = "YOUR_WIFI_PASSWORD";

Replace them with your WiFi credentials:

const char* SSID = "MyWiFi";
const char* PASSWORD = "MyPassword";

These are the main settings a beginner normally needs to change.

The relay GPIO configuration should remain:

#define R1 5
#define R2 17
#define R3 16
#define R4 4

🛠️ Step 5 — Compile

After pasting the code:

1. Save the sketch.
2. Select the ESP32 board.
3. Press Verify / Compile.
4. Wait for compilation to finish.
5. Fix any library or board-package errors before uploading.

If compilation finishes successfully, continue to the upload step.

🚀 Step 6 — Upload

Keep the ESP32 connected to the Android phone.

Press Upload in ArduinoDroid.

Wait until the upload process completes successfully.

The ESP32 will restart after uploading.

🌐 Step 7 — Open the Control Panel

After the ESP32 connects to WiFi, check the Serial Monitor for its IP address.

For example:

WiFi connected
IP address: 192.168.1.100

Open that IP address in your phone's browser:

http://192.168.1.100

Your ESP32 relay control panel should open.

«Your actual IP address will be different depending on your network.»

🔧 Wiring

Default relay connections:

ESP32             Relay Module

GPIO 5   ───────► R1
GPIO 17  ───────► R2
GPIO 16  ───────► R3
GPIO 4   ───────► R4
GND      ───────► GND

Check your particular relay module's voltage, logic level, and power requirements before connecting it.

⚠️ Electrical Safety

This project can be used for controlling electrical loads, but mains electricity can cause serious injury, fire, or death.

For beginners:

- Test the relay using a safe low-voltage load first.
- Never work on live mains wiring.
- Use an appropriately rated relay/module.
- Use proper insulation and enclosures.
- Disconnect power before changing wiring.
- For household AC wiring, use a qualified electrician.

Do not connect mains electricity just to test whether the ESP32 code works.

🧪 Relay Test

Before connecting any real appliance, verify each channel individually.

Expected mapping:

R1 → GPIO 5
R2 → GPIO 17
R3 → GPIO 16
R4 → GPIO 4

If a relay does not respond:

1. Check GPIO wiring.
2. Check relay-module power.
3. Check GND connection.
4. Check whether the relay module uses active-LOW logic.
5. Check the Serial Monitor.
6. Test with a simple relay sketch.

🐛 Troubleshooting

ESP32 is not detected

Try:

- A different USB data cable.
- Another USB OTG adapter.
- Reconnecting the ESP32.
- Granting USB permission again.
- Checking that the board has power.

Upload fails

Check:

- Correct ESP32 board selection.
- Correct USB/serial device.
- USB permission.
- Data cable.
- ESP32 boot mode requirements.

Some ESP32 boards require holding the BOOT button during part of the upload process.

WiFi does not connect

Check:

const char* SSID = "YOUR_WIFI_NAME";
const char* PASSWORD = "YOUR_WIFI_PASSWORD";

Make sure the ESP32 is within WiFi range and that the network credentials are correct.

Relay does not switch

Check the GPIO mapping:

R1 → GPIO 5
R2 → GPIO 17
R3 → GPIO 16
R4 → GPIO 4

Also check whether your relay module is active LOW or active HIGH.

📂 Project Structure

ESP32-4-Relay-ArduinoDroid/
├── README.md
├── LICENSE
├── .gitignore
├── src/
├── docs/
├── images/
└── examples/

🎯 Project Goals

This project is intended as a simple starting point for learning:

- ESP32 development
- Arduino programming
- ArduinoDroid
- WiFi communication
- Relay control
- IoT
- Mobile-based embedded development
- Home automation

Future versions can be extended with features such as:

- Bluetooth control
- Telegram control
- Voice control
- Sensors
- Scheduling
- More automation rules
- Android application control

🤝 Contributing

Contributions, improvements, bug reports, and beginner-friendly suggestions are welcome.

If you find a problem:

1. Open an Issue.
2. Describe the ESP32 board you are using.
3. Include the ArduinoDroid/Arduino version.
4. Include the error message.
5. Explain what you tried.

⭐ Support

If this project helps you learn ESP32 using only an Android phone, consider giving the repository a ⭐ on GitHub.

📜 License

This project is released under the MIT License.

See ""LICENSE"" (LICENSE) for details.

---

ESP32 • ArduinoDroid • WiFi • Relay • IoT • Home Automation
