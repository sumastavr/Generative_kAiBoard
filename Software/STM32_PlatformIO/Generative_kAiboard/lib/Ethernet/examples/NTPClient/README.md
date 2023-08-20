# How to NTP Client Example
This code is an NTP client example that uses a STM32 Nucleo-144 board and  W5300 TOE Shield. the NTP protocol to connect to an NTP server and retrieve the current time and date.

## Step 1: Prepare software

The following serial terminal programs are required for NTPClient example test, download and install from below links.

- [**Tera Term**][link-tera_term]


## Step 2: Prepare hardware

1. Combine W5300-TOE Shield with STM32 Nucleo-144 board.

2. Connect ethernet cable to W5300 TOE Shield ethernet port.

3. Connect STM32 Nucleo-144 board to desktop or laptop using 5 pin micro USB cable.



## Step 3: Setup NTPClient Example

To test the NTPClient example, minor settings shall be done in code.


1. Setup network configuration.

Setup **network configuration** such as IP in '**NTPClient.ino**' in '**Ethernet/examples/NTPClient**' directory.

```cpp
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 11, 111);
```

To set the NTP Time server.
```cpp
#define TIME_SERVER   "time.nist.gov"
```

## Step 4: Build and Download

1. After completing the NTPClient example configuration, build in the IDE you are using.

2. Download the time lib library for Arduino
 > [Sketch] -> [Include Library] -> [Manage Libraries] -> install **timelib**
<p align="center"><img src="https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/ntp_lib_download.png"></p>

3. When the build is completed, download the firmware to the STM32 Nucleo-144 board.


## Step 5: Run

1. Connect to the serial COM port of STM32 Nucleo-144 board with Tera Term.
<p align="center"><img src="https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/teraterm_setting.png"></p>
2. If the NTPClient example works normally on STM32 Nucleo-144 board, you can see the network information of STM32 Nucleo-144 board.
<p align="center"><img src="https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/ntp_setting_up.png"></p>
3. Connect to the NTP Server. and prints the NTP time every 10 seconds.
<p align="center"><img src="https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/ntp_time_print.png"></p>



<!--
Link
-->

[link-tera_term]: https://osdn.net/projects/ttssh2/releases/

