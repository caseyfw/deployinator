# Deployinator

Simple ESP8266 Arduino script to trigger a Jenkins job. Build your team a box
with missile switch, key switch and LED buttons to show off your deployment
pipeline.

![SOAR Deployinator v2](https://i.imgur.com/URA6ggy.jpg?1)

## Hardware

The build is pretty arbitrary - it could be a single push button with no lights,
but where's the fun in that?

![Deployinator schematic](https://i.imgur.com/3OhKY49.png)

The ESP will only provide 20mA per pin, so make sure you calculate your resistor
values appropriately to not overcook it. The oversized automotive push-buttons I
used for the hold and deploy buttons needed > 5v, so had to be controlled via a
couple of small NPN transistors.

Using an ESP development board like the Wemos D1 mini quite simple. Here's a
suggested protoboard layout:

![Deployinator protoboard](https://i.imgur.com/y0XwMHJ.png)

Note the missile switch is literally a power switch between a battery pack a the
board.

## Software

1. Determine your [Jenkins API token](https://stackoverflow.com/questions/45466090/how-to-get-the-api-token-for-jenkins)
   and base64 encode it.

2. Copy the `credentials.h.dist` file to `credentials.h` and fill in the
   various configuration details (wifi name, password, etc).

3. You'll need the [ESP8266 Arduino library](https://github.com/esp8266/Arduino)
   in order to compile the sketch.
