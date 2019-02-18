# mqttSensorApp
Sensor App using mqtt as transport protocol based on ESP processor. The ESP8266 can be used as AP or Client.

## WiFi configuraion
In case no WiFi will connect an on-demand AP will be opened and can be used for configuring the WiFi credentials.

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
- [X] OTA updates secured with password hash
- [X] WiFi configuration started when not connecting to WiFi (running AP running till timeout and restarting)
- [X] secure WiFi password storage
- [X] build-in LED shows WiFi status
- [X] data handling: JSON
- [X] data publishing: HTTP, MQTT
- [X] MQTT data transfer: plain or TLS
- [X] act as switch (on/off/timed off - duration in ms)
- [X] two sensor connectors for on/off detection
- [ ] certificate validation

## needed libraries
 * ArduinoJson
 * ArduinoStreaming
 * Bounce2
 * i2cSernsorLib
 * PubSubClient
 * WifiManager

## Wemos (d1_mini_pro) / Lolin shields default
- i2c: SCL (D1), SDA (D2)
- switch (D5)
- contact sensor (D6/D7)

## get security parameter
 - md5 hash for ota: `echo -n "<password>" | md5sum`
 - fingerprint: `echo | openssl s_client -connect localhost:8883 | openssl x509 -fingerprint -noout`
