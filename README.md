# mqttSensorApp
Sensor App using mqtt as transport protocol based on ESP processor. The ESP8266 can be used as AP or Client.

## topic structure
`<macAddress w/o ":">/value`

eg: `5ccf7f3c88a2/value`

## value structure
In case you connect different sensors and multiple values are read for the same topic, they will be listed as csv.

`{"vcc":[<value in volt>],"illuminance":[<value in lux>],"temperature":[<value in degrees celsius>],"humidity":[<value in %>],"pressure":[<value in hPa>]}`

eg: `{"vcc":[3.026],"illuminance":[],"temperature":[25.32,24.02],"humidity":[50.56543],"pressure":[997.304]}`

## WiFi Mode
- Access Point: WIFI_MODE WIFI_AP
- Client: WIFI_MODE WIFI_STA

## Deep Sleep mode
Running with Deep Sleep will configure WiFi as Client.

## supported Features
- [X] connect to WiFi
- [X] act as AP for direct measurement
- [X] publish values using mqtt
- [X] webserver displaying current data
- [X] OTA
- [X] DeepSleep
- [X] LED shows WiFi status

## needed libraries
 * Streaming
 * pubsubclient
 * ArduinoJson
 * i2cSernsorLib
