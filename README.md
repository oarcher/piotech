# piotech

## external components

### ota_http

ota in client (http) mode: This allow to do a remote ota when the device is far away, behind a firewall, and only reachable with mqtt. (For ex in an other house).

**WARNING**: This repository is not up to date. If you want to use `ota_http`, use the [PR#5586](https://github.com/esphome/esphome/pull/5586)

```
external_components:
  - source: github://pr#5586
      components: [ ota_http ]
```

[Here is the documentation](https://github.com/esphome/esphome-docs/pull/3291), while [#PR3291](https://github.com/esphome/esphome-docs/pull/3291) 

