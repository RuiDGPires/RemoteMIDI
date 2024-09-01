# Midi Loopback and Forward

## Client module

The client module is placed on a Raspberry Pi connected to a MIDI controller.
It forwards the messages received from the MIDI controller to a host device, where the server module is placed. Also performs Loopback to the local MIDI driver.

## Server module

The server module is placed on the host device, like a computer running a DAW and it receives MIDI messages from the client module.

## Connection

Connection from client to host is done via TCP/IP on the local network. Aiming for USB and Bluetooth support.
