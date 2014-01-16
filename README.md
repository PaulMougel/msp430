MSP-430 Demo application
========================

MSP-430 board code
------------------

*Prerequisite:* a Raspberry Pi, with a user "pi" and IP address 172.16.52, and a connected MSP-430 attached to the USB port.

1. Copy all code inside the `board` folder into the Raspberry Pi, in `~/board/`.
2. On the host machine, `cd board/ez430-applications/demo/`
3. Run `make pido` to download the code to the Raspberry Pi, cross-compile it and download it to the MSP-430

All user code is in `board/ez430-applications/demo/src/`, everything else are drivers and utilities.

MSP-430 Front-end
-----------------
![](https://raw.github.com/PaulMougel/msp430/master/temperature.png)
![](https://raw.github.com/PaulMougel/msp430/master/radar.png)

This reads CSV data from `stdin`, that is supposed to be sent by the MSP-430 board.

Supposed to be run with:

```bash
$ ezconsole | node index.js
```

To run it as a standalone server:

```bash
$ node index.js
```

and write your own commands using the keyboard.

It can also produce fake data, if you set `NODE_ENV=test':

```bash
$ NODE_ENV=test node index.js
```

In this way, the graphs will use generated data.
