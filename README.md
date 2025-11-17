# Classroom Aura

**Classroom Aura** is a smart Arduino-based air-quality monitor designed for classrooms and indoor environments.

It measures air quality, displays status using an RGB LCD, provides audible alerts through a buzzer, and sends readings to a backend API for remote monitoring.

The device includes a built-in WiFi setup mode accessible through a push button.

## ✨ Features

* **Two operating modes**
  * **Setup Mode:** Access point + web configuration (SSID, password, device ID)
  * **Normal Mode:** Sensor reading, LCD status, buzzer alarm, API reporting
* **Real-time air-quality evaluation**
  * Green = Good
  * Yellow = Moderate
  * Red = Bad (buzzer alarm)
* **Periodic data upload** to an online backend
* **User feedback** via LCD and buzzer
* **WiFi-enabled** using Arduino WiFi Rev2

## 🧰 Components Used

| Component                      | Description                                   |
|--------------------------------|-----------------------------------------------|
| Arduino WiFi Rev2              | Microcontroller with onboard WiFi             |
| Grove Base Shield              | Expansion shield for Grove modules            |
| Grove RGB LCD                  | I²C LCD with RGB backlight                    |
| Grove Button                   | Push button for mode selection                |
| Grove Buzzer                   | Audible alert when air quality is bad         |
| Grove Air Quality Sensor v1.3  | VOC/air-quality analog sensor                 |


## 🔌 Wiring

| Component                      | Grove Port | Arduino Pin |
|-------------------------------|------------|-------------|
| Grove RGB LCD                 | I²C        | SDA/SCL     |
| Grove Button                  | D4         | D4          |
| Grove Buzzer                  | D5         | D5          |
| Grove Air Quality Sensor v1.3 | A0         | A0          |


## 📚 Required Libraries

* **WiFiNINA** – WiFi for Arduino WiFi Rev2
* **ArduinoHttpClient** – Sends HTTP requests to backend
* **Grove LCD RGB Backlight** – Controls the LCD
* **Bounce2** – Button debouncing

## 🛠️ How the Device Works

### 1. Setup Mode (WiFi configuration)

To enter setup mode:

1. Hold the Button while powering on the device.
2. The device starts as a WiFi Access Point.
3. Connect to the device’s AP from a phone or laptop.
4. Open the configuration webpage provided by the device.
5. Set:
   * WiFi SSID
   * WiFi password
   * Device ID (required by backend API)
6. Save settings — A reboot is required for the changes to take effect.

### 2. Normal Sensing Mode

If the button is not held during startup:

* Reads air-quality values from the sensor.
* Displays air-quality and status on the RGB LCD:
* Green = Good
* Yellow = Moderate (also activates buzzer)
* Red = Bad (also activates buzzer)
* Periodically sends sensor readings + device ID to the backend API.
* Continues looping indefinitely.
