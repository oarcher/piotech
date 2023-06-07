# piotech

## external components

### ota_http

ota in client (http) mode: This allow to do a remote ota when the device is far away, behind a firewall, and only reachable with mqtt. (For ex in an other house).



```yaml
external_components:
  - source: github://oarcher/piotech/


ota_http:


button:
  - platform: template
    name: "Firmware update"
    on_press:
      then:
        - ota_http.flash:
            url: http://example.com/firmware.bin
            verify_ssl: false
        - logger.log: "This message should be not displayed(reboot)"
```
