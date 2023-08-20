# How to Test DHCP Example

This program configures the Ethernet connection on an STM32 Nucleo-144 board and maintains the connection. The program uses DHCP to automatically obtain an IP address for the board.


## Step 1: Prepare software

The following serial terminal programs are required for DHCP example test, download and install from below links.

- [**Tera Term**][link-tera_term]


## Step 2: Prepare hardware

1. Combine W5300-TOE Shield with STM32 Nucleo-144 board.

2. Connect ethernet cable to W5300 TOE Shield ethernet port.

3. Connect STM32 Nucleo-144 board to desktop or laptop using 5 pin micro USB cable.



## Step 3: Setup DHCP Example

To test the Dhcp example, minor settings shall be done in code.


1. Setup network configuration.

Setup **network configuration** such as MAC in '**DhcpAddressPrinter.ino**' in '**Ethernet/examples/DhcpAddressPrinter**' directory.

```cpp
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
```

## Step 4: Build and Download

1. After completing the Advanced Chat Server example configuration, build in the IDE you are using.

2. When the build is completed, download the firmware to the STM32 Nucleo-144 board.


## Step 5: Run

1. Connect to the serial COM port of STM32 Nucleo-144 board with Tera Term.
<p align="center"><img src="https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/teraterm_setting.png"></p>
2. If the DHCP example works normally on STM32 Nucleo-144 board, the program automatically configures and maintains the Ethernet connection. The IP address of the board is obtained through DHCP.

<p align="center"><img src="https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/dhcp_setting_up.png"></p>

<!--
Link
-->

[link-tera_term]: https://osdn.net/projects/ttssh2/releases/