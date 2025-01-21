# Smart Kitchen Monitoring System

## Project Overview
The **Smart Kitchen Monitoring System** is an IoT-based project designed to ensure kitchen safety by monitoring environmental parameters such as temperature, humidity, and gas levels. The system uses sensors to detect hazardous conditions and provides real-time feedback through local displays and web-based dashboards. It also includes mechanisms to control electrical loads in response to abnormal conditions.

## Features
- **Real-time Monitoring**: Measures temperature, humidity, and gas levels.
- **Alerts and Notifications**: Triggers a buzzer and updates dashboards when hazardous conditions are detected.
- **Local Display**: Uses an OLED screen to show sensor data.
- **Web Interfaces**:
  - **ESP-Dash**: Local real-time monitoring without internet.
  - **ThingsBoard**: Remote monitoring and control via the cloud.
- **Automated Load Control**: A relay disconnects electrical loads in unsafe conditions.

## Components
### Hardware
- **ESP32 Microcontroller**: Handles sensor data processing and communication.
- **DHT11 Sensor**: Measures temperature and humidity.
- **MQ135 Sensor**: Detects gas concentration (PPM).
- **0.96-inch OLED Display**: Shows local data output.
- **5V Single-channel DC Relay**: Controls AC loads.
- **5V Buzzer**: Provides audible alerts.
- **Logic Level Converter**: Ensures proper voltage for relay triggering.
- **Dot Board**: Circuit assembly.

### Software
- **Arduino IDE**: For programming the ESP32.
- **ESP-Dash Library**: Creates a local dashboard.
- **ThingsBoard**: Cloud-based monitoring.
- **Adafruit Libraries**: Interfaces for sensors and OLED display.
- **PubSubClient**: MQTT protocol for data transfer.

## System Workflow
1. **Initialization**:
   - ESP32 connects to Wi-Fi (both local and cloud).
   - Sensors are initialized.
   - Web dashboards are set up.

2. **Data Acquisition**:
   - DHT11 provides temperature and humidity.
   - MQ135 measures gas concentration in PPM.

3. **Data Processing**:
   - Compares sensor values against thresholds.
   - Activates buzzer and relay if conditions exceed safe limits.

4. **Data Output**:
   - **Local**: Displays on the OLED screen.
   - **Web**:
     - ESP-Dash: Accessible locally without the internet.
     - ThingsBoard: Provides remote monitoring with graphical widgets.

5. **Control Mechanism**:
   - Relay disconnects the AC load when thresholds are exceeded.

## Setup Instructions
### Hardware Setup
1. Connect the sensors (DHT11 and MQ135) to the ESP32 as per the pin configurations in the code.
2. Attach the OLED display, relay, and buzzer.
3. Use a logic level converter if required for the relay.
4. Power the circuit and ESP32.

### Software Setup
1. Install the following libraries in Arduino IDE:
   - `WiFi.h`
   - `PubSubClient.h`
   - `Adafruit_GFX.h`
   - `Adafruit_SSD1306.h`
   - `Adafruit_Sensor.h`
   - `DHT.h`
   - `AsyncTCP.h`
   - `ESPAsyncWebServer.h`
   - `ESPDash.h`
2. Configure Wi-Fi credentials for ThingsBoard and ESP-Dash in the code.
3. Upload the code to the ESP32.

### Threshold Values
- **Temperature**: >32°C
- **Humidity**: <50%
- **Gas Level**: >750 PPM

## Troubleshooting
- **Relay Triggering Issue**: Ensure the logic level converter is properly connected as the ESP32 GPIO outputs 3.3V, insufficient for the 5V relay.
- **Wi-Fi Connection**: Verify credentials and network availability.
- **Sensor Errors**: Check wiring and sensor functionality if values are `NaN`.

## Example Dashboards
### ESP-Dash
- Displays:
  - Temperature (°C)
  - Humidity (%)
  - Gas Levels (PPM)
  - Alarm and Relay Status

### ThingsBoard
- Graphical Widgets:
  - Temperature, Humidity, and Gas Levels
  - Real-time Alarm and Relay Status

## Future Improvements
- Integrate additional sensors (e.g., flame detectors).
- Implement mobile app notifications.
- Enhance the graphical interface on ThingsBoard.

---
### Contributors
- **[Your Name/Team Name]**

### License
This project is licensed under the MIT License.
