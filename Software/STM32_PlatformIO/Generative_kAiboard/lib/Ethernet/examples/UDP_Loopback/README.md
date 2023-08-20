# How to UDP Loopback Example

This program sets up a UDP communication on an STM32 Nucleo-144 board. The program listens for incoming UDP packets and sends a reply to the sender.

## Step 1: Prepare software

The following serial terminal programs are required for AdvancedChatServer example test, download and install from below links.

- [**Tera Term**][link-tera_term]
- [**Hercules**][link-hercules]


## Step 2: Prepare hardware

1. Combine W5300-TOE Shield with STM32 Nucleo-144 board.

2. Connect ethernet cable to W5300 TOE Shield ethernet port.

3. Connect STM32 Nucleo-144 board to desktop or laptop using 5 pin micro USB cable.



## Step 3: Setup UDP_Loopback Example

To test the UDP_Loopback example, minor settings shall be done in code.


1. Setup network configuration.

Setup **network configuration** such as IP and UDP Port in '**UDP_Loopback.ino**' in '**Ethernet/examples/UDP_Loopback**' directory.

```cpp
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 11, 111);

unsigned int localPort = 8888;
```

## Step 4: Build and Download

1. After completing the UDP_Loopback example configuration, build in the IDE you are using.

2. When the build is completed, download the firmware to the STM32 Nucleo-144 board.


## Step 5: Run

1. Connect to the serial COM port of STM32 Nucleo-144 board with Tera Term.
<p align="center"><img src="https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/teraterm_setting.png"></p>
2. If the UDP_Loopback example works normally on STM32 Nucleo-144 board, you can see the network information of STM32 Nucleo-144 board and the LoopServer is open.
<p align="center"><img src="https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/udp_setting_up.png"></p>
3. Connect to the open UDP client using Hercules UDP. When connecting to the UDP server, you need to enter is the IP that was configured in Step 3, the port is 8888 by default.
<p align="center"><img src="https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/udp_hercules_setting_up.png"></p>
4. The program listens for incoming UDP packets on port 8888. When a packet is received, the program prints the sender's IP address and port, and sends a reply to the sender with the same message.
<p align="center"><img src="https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/udp_teraterm_log.png"><img src="https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/udp_hercules_log.png"></p>



<!--
Link
-->

[link-tera_term]: https://osdn.net/projects/ttssh2/releases/
[link-hercules]: https://www.hw-group.com/software/hercules-setup-utility
