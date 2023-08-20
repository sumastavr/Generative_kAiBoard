# How to DNS Example
This code is an Arduino project that uses the STM32 Nucleo-144 board and  W5300-TOE to perform a DNS lookup for a domain name.

## Step 1: Prepare software

The following serial terminal programs are required for DNS example test, download and install from below links.

- [**Tera Term**][link-tera_term]


## Step 2: Prepare hardware

1. Combine W5300-TOE Shield with STM32 Nucleo-144 board.

2. Connect ethernet cable to W5300 TOE Shield ethernet port.

3. Connect STM32 Nucleo-144 board to desktop or laptop using 5 pin micro USB cable.



## Step 3: Setup DNS Example

To test the DNS example, minor settings shall be done in code.


1. Setup network configuration.

Setup **network configuration** such as mac in '**DNS.ino**' in '**Ethernet/examples/DNS**' directory.

```cpp
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

```

To set the DNS domain name.
```cpp
char dns_domain[] = "www.wiznet.io";
```

## Step 4: Build and Download

1. After completing the DNS example configuration, build in the IDE you are using.

2. When the build is completed, download the firmware to the STM32 Nucleo-144 board.


## Step 5: Run

1. Connect to the serial COM port of STM32 Nucleo-144 board with Tera Term.
<p align="center"><img src="https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/teraterm_setting.png"></p>
2. If the DNS example works normally on STM32 Nucleo-144 board, you can see the DNS information of domain.
<p align="center"><img src="https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/dns_setting_up.png"></p>

<!--
Link
-->

[link-tera_term]: https://osdn.net/projects/ttssh2/releases/
