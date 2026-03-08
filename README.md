# piotech

## modem package

This repository is related to esphome PR \#6721 [[modem] pppos tcp/ip for UART modem (esp32)](https://github.com/esphome/esphome/pull/6721) and \#7147 [[wifi, ethernet] Allow Wifi AP to reuse existing network interface (ethernet or modem) for NAT](https://github.com/esphome/esphome/pull/6721) 


It simplify the modem configuration, from well known developpement board, by enabling most of the features.

Supported boad are:

  - Lilygo TSIM 7600 : https://lilygo.cc/en-us/products/t-sim7600 
  - Waveshare ESP32-S3-A7670E-4G-EN : https://docs.waveshare.com/ESP32-S3-A7670E-4G



### Lilygo TSIM 7600

GNSS NMEA support: Yes, by modem URC

### Sim 7670

GNSS NMEA support: Yes, but tricky (need to decode/encode from AT+GNSSINFO)

### Waveshare ESP32-S3-A7670E-4G-EN

GNSS NMEA support: No (Buggy firmware that brake gnss when a sim card is inserted.)

Note on buggy gnss:

The bug can be reproduced by connecting to the modem by minicom or pyserial:
Every command related to gnss or gps are broken and return empty token when a sim is inserted, even AT+CGNSSPROD, that should return firmware version.


With a sim inserted:
```
AT+CGNSSPWR=1
OK
+CGNSSPWR: READY!
AT+CGNSSPROD
AT+CGNSSPROD ,,
```

With no sim:
```
AT+CGNSSPWR=1
OK
+CGNSSPWR: READY!
AT+CGNSSPROD
AT+CGNSSPROD PRODUCT: UNICORECOMM,UC6228CI,R3.4.21.0Build16211
```







