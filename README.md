# ESP32 IoT Environmental Sensor Node

This project implements an ESP32-based IoT node that collects environmental data, connects to WiFi and transmits sensor data to a specified server.
- Temperature
- Humidity
- Air pressure
- Light levels (day/night detection)

  
### Hardware Components
- ESP32 development board (ESP32 Dev Kit V1)
- DHT11 (Temperature and Humidity)
- MPS20N0040D (air pressure sensor) → using HX710B, a 24-bit ADC 
- LDR (Light Dependent Resistor) with 10kΩ resistor

### Pin Connections
- DHT 11  ->  GPIO 4
- HX_SCK  ->  GPIO 18 
- HX_DOUT ->  GPIO 19
- LDR     ->  GPIO 38



### Software Requirements

- Arduino IDE with ESP32 board support
- Libraries:
    - Adafruit DHT sensor library
    - Adafruit Unified Sensor
    - WiFi
    - ArduinoJson (for data formatting-JSON) by Benoît Blanchon
    - HTTPClient (for web requests)
    - Wifi Manager by tzapu (Maintained by tablatronix)



  ### Data Format-jSON

  {
  "device_id": 1,
  "temprature": 33,
  "humidity": 45,
  "pressure": 1.0011,
  "time": "day"
}
