# MQTT Sonoff Switch

The goal is to create a basic implementation of the MQTT protocol for the Sonoff Switch.

## Setting up

### Configuration

First of all you need to create the `sketch/config.h` file and tweak it with your own values.  
You can use the `sketch/config.h.sample` as a base.

```
cp sketch/config.h.sample sketch/config.h
```

### Install Arduino OTA

You can follow this [tutorial](https://github.com/esp8266/Arduino#using-git-version) to install Arduino OTA, it will allow you to update your device over the air.

### Flashing

To flash your Sonoff Switch you can follow [this great tutorial over at randomnerdtutorials.com](https://randomnerdtutorials.com/how-to-flash-a-custom-firmware-to-sonoff/) using the code of this project.

## Usage

This project was created to complete [my home automation project](https://github.com/k2r79/home-automation).

### Channels

You can switch the power on and off with the `COMMAND_TOPIC` using the payloads "ON" and "OFF".  
The switch will then reply on the `STATE_TOPIC` with the new relay state, the payloads being "ON" and "OFF".
