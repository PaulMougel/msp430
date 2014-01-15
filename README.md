MSP-430 Demo application
========================


MSP-430 Front-end
-----------------
![](https://raw.github.com/PaulMougel/msp430/master/example.png)

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
