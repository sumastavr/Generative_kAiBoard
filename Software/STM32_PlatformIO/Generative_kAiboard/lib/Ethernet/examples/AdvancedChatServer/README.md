# How to Test AdvancedChatServer Example

This is a simple chat server program that can be run on an STM32 Nucleo-144 board with an W5300-TOE shield. The program listens for incoming client connections and allows them to exchange messages with each other.


## Step 1: Prepare software

The following serial terminal programs are required for AdvancedChatServer example test, download and install from below links.

- [**Tera Term**][link-tera_term]
- [**Hercules**][link-hercules]



## Step 2: Prepare hardware

1. Combine W5300-TOE Shield with STM32 Nucleo-144 board.

2. Connect ethernet cable to W5300 TOE Shield ethernet port.

3. Connect STM32 Nucleo-144 board to desktop or laptop using 5 pin micro USB cable.



## Step 3: Setup Advanced Chat Server Example

To test the AdvancedChatServer example, minor settings shall be done in code.


1. Setup network configuration.

Setup **network configuration** such as IP in '**AdvancedChatServer.ino**' in '**Ethernet/examples/AdvancedChatServer**' directory.

```cpp
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 11, 111);
IPAddress myDns(192, 168, 11, 1);
IPAddress gateway(192, 168, 11, 1);
IPAddress subnet(255, 255, 255, 0);
```

2. Setup Chat Server configuration.

Setup Advanced Chat server **port number** in '**AdvancedChatServer.ino**' in '**Ethernet/examples/AdvancedChatServer**' directory.

```cpp
#define SERVER_PORT 5000
```
and And set the maximum number of connected clients.
```cpp
#define MAX_CLIENT  8
```


## Step 4: Build and Download

1. After completing the Advanced Chat Server example configuration, build in the IDE you are using.

2. When the build is completed, download the firmware to the STM32 Nucleo-144 board.


## Step 5: Run

1. Connect to the serial COM port of STM32 Nucleo-144 board with Tera Term.
<p align="center"><img src="https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/teraterm_setting.png"></p>

2. If the AdvancedChatServer example works normally on STM32 Nucleo-144 board, you can see the network information of STM32 Nucleo-144 board and the chat server is open.
<p align="center"><img src="https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/chat_server_setting_up.png"></p>

3. Connect to the open chat server using Hercules TCP client. When connecting to the chat server, you need to enter is the IP that was configured in Step 3, the port is 5000 by default.
<p align="center"><img src="https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/chat_server_connect.png"></p>

4. When a new client connects, the program sends a welcome message and stores the client's connection in an array.

5. When a client sends a message, the program broadcasts the message to all connected clients, except the one that sent the message.
<p align="center"><img src="https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/advancedChatServer_1.jpg"></p>
<p align="center"><img src="https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/advancedChatServer_2.jpg"></p>



<!--
Link
-->

[link-tera_term]: https://osdn.net/projects/ttssh2/releases/
[link-hercules]: https://www.hw-group.com/software/hercules-setup-utility
