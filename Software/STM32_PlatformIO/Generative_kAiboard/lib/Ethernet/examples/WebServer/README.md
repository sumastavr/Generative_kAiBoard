# How to Web Server Example
This example uses a STM32 Nucleo-144 board with W5300-TOE to create a web server that responds to client requests.

## Step 1: Prepare software

The following serial terminal programs are required for WebServer example test, download and install from below links.

- [**Tera Term**][link-tera_term]


## Step 2: Prepare hardware

1. Combine W5300-TOE Shield with STM32 Nucleo-144 board.

2. Connect ethernet cable to W5300 TOE Shield ethernet port.

3. Connect STM32 Nucleo-144 board to desktop or laptop using 5 pin micro USB cable.



## Step 3: Setup WebServer Example

To test the WebServer example, minor settings shall be done in code.


1. Setup network configuration.

Setup **network configuration** such as IP and UDP Port in '**WebServer.ino**' in '**Ethernet/examples/WebServer**' directory.

```cpp
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 11, 111);
```

To set the port for the web server.
```cpp
#define SERVER_PORT 5000
```

## Step 4: Build and Download

1. After completing the WebServer example configuration, build in the IDE you are using.

2. When the build is completed, download the firmware to the STM32 Nucleo-144 board.


## Step 5: Run

1. Connect to the serial COM port of STM32 Nucleo-144 board with Tera Term.
<p align="center"><img src="https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/teraterm_setting.png"></p>
2. If the WebServer example works normally on STM32 Nucleo-144 board, you can see the network information of STM32 Nucleo-144 board and the WebServer is open.
<p align="center"><img src="https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/web_server_setting_up.png"></p>
3. Connect to the WebServer using Web browser. When connecting to the WebServer, you need to enter is the IP and port that was configured in Step 3, the port is 5000 by default.
<p align="center"><img src="https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/web_server_browser.png"></p>





<!--
Link
-->

[link-tera_term]: https://osdn.net/projects/ttssh2/releases/
