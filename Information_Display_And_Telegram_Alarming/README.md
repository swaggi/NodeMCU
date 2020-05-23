# Information Display and Telegram Alarming

## Parts needed:

- NodeMCU
- AM2302 (temperature & humidity sensor)
- motion sensor (for Arduino or Raspberry Pi)
- OLED I2C Display
- Connectors (wires, breadboard)

## Replace your data:

- SSID (WiFi name)
- WiFi password
- Telegram Bot Token
- Telegram Chat ID
- IP address of your smartphone

## About the project:

Temperature and humidity data will be displayed no matter the NodeMCU is connected to WiFi or not.
If the NodeMCU is connected to WiFi and your smartphone is reachable via ping: Date & time, temperaure and humidity is displayed.
If your smartphone is not reachable: NodeMCU is in alarming mode! A Telegram message will be fired, if the motion sensor detects any movement. You do not have to turn the alarm in active mode, when leaving the house, nor turn it in inactive mode when you arrive.
Have fun!


