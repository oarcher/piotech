# piotech

## external components

```yaml
external_components:
  - source: github://oarcher/piotech/esphome/components
  

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
