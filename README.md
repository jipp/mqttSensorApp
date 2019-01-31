# mqttSensorApp
Sensor App using mqtt as transport protocol based on ESP processor. The ESP8266 can be used as AP or Client.

## topic structure
- publish value: `<macAddress w/o ":">/value`
- subscribe to togle switch: `<macAddress w/o ":">/switch`

eg:
- `5ccf7f3c88a2/value`
- `5ccf7f3c88a2/switch`

## value structure
In case you connect different sensors and multiple values are read for the same topic, they will be listed as csv.

`{"vcc":[<value in volt>],"illuminance":[<value in lux>],"temperature":[<value in degrees celsius>],"humidity":[<value in %>],"pressure":[<value in hPa>]}`

eg: `{"vcc":[3.026],"illuminance":[],"temperature":[25.32,24.02],"humidity":[50.56543],"pressure":[997.304]}`

## switch structure
- '0' switch 'off'
- '1' switch 'on'
- 'x' switch/keep for x milliseconds 'on' then 'off' (x > 1)

## supported Features
- [X] connect to WiFi
- [X] secure WiFi
- [X] publish values using mqtt
- [ ] secure MQTT
- [X] webserver displaying current data
- [X] OTA
- [X] LED shows WiFi status
- [X] act as switch (on/off/count down off)

## needed libraries
 * ArduinoStreaming
 * PubSubClient
 * ArduinoJson
 * i2cSernsorLib
 * Bounce2
 * WifiManager

## Wemos (d1_mini_pro) / Lolin shields default
- i2c: SCL (D1), SDA (D2)
- switch (D5)
- contact sensor (D6/D7)