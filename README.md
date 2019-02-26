master branch: 
[![Build Status](https://travis-ci.org/jipp/mqttSensorApp.svg?branch=master)](https://travis-ci.org/jipp/mqttSensorApp)
[https://ci.appveyor.com/api/projects/status/g3msykeidxhic3y7/branch/master?svg=true](https://ci.appveyor.com/api/projects/status/g3msykeidxhic3y7/branch/master?svg=true)

development branch:
[![Build Status](https://travis-ci.org/jipp/mqttSensorApp.svg?branch=development)](https://ci.appveyor.com/project/jipp/mqttsensorapp)
[https://ci.appveyor.com/api/projects/status/g3msykeidxhic3y7/branch/development?svg=true](https://ci.appveyor.com/project/jipp/mqttsensorapp)


# mqttSensorApp
This app is collecting data using different sensors connected via I2C. In addition two pins are used as input switches. The data will be sent using MQTT to a broker and published on a web server running on the chip.

## IDE
PlatformIO

## Configurataion
Configuration parameters can be changed in the `config.h` file.

### WiFi configuraion
If it is not possible to connect to an existing WiFi network during boot an access point will be started to reconfigure the credentials. After a timeout a reboot will take place to try reconnecting with already stored credentials.

### MQTT
MQTT can be used either plain or over TLS. Server, port and other parameters can be configured manually using `config.h`.

#### topic structure
- publish value: `<macAddress w/o ":">/value`
- subscribe to togle switch: `<macAddress w/o ":">/switch`

eg:
- `5ccf7f3c88a2/value`
- `5ccf7f3c88a2/switch`

#### value structure
In case you connect different sensors and multiple values are read for the same topic, they will be listed as csv.

`{"vcc":[<value in volt>],"illuminance":[<value in lux>],"temperature":[<value in degrees celsius>],"humidity":[<value in %>],"pressure":[<value in hPa>]}`

eg: `{"vcc":[3.026],"illuminance":[],"temperature":[25.32,24.02],"humidity":[50.56543],"pressure":[997.304]}`

#### switch structure
- '0' switch 'off'
- '1' switch 'on'
- 'x' switch/keep for x milliseconds 'on' then 'off' (x > 1)

### Web Server
A http server is started on a configureable port. 

## supported Features
- [X] OTA updates secured with password hash
- [X] WiFi configuration started when not connecting to WiFi (running AP running till timeout and restarting)
- [X] secure WiFi password storage
- [X] build-in LED shows WiFi status
- [X] data handling: JSON
- [X] data publishing: HTTP, MQTT
- [X] configurable HTTP port
- [X] MQTT data transfer: plain or TLS
- [X] act as switch (on/off/timed off - duration in ms)
- [X] publish state change of switch immediately
- [X] two sensor connectors for on/off detection

## needed additional libraries
 * ArduinoJson
 * ArduinoStreaming
 * Bounce2
 * i2cSernsorLib
 * PubSubClient
 * WifiManager

## Wemos/Lolin (d1_mini_pro) shields default
- i2c: SCL (D1), SDA (D2)
- switch (D5)
- contact sensor (D6/D7)

## get security parameter
 - md5 hash for ota: `echo -n "<password>" | md5sum`
 - fingerprint: `echo | openssl s_client -connect localhost:8883 | openssl x509 -fingerprint -noout`
