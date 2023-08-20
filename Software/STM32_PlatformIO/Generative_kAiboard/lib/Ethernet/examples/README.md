# How to Run Sample Examples



## Step 1: Prepare IDE

The **ARDUINO** an **PlatformIO(built-in VSCode)** support the serial monitor. So you need not prepare serial terminal program


## Step 2: Prepare hardware

1. Combine W5300 TOE Shield with STM32 Nucleo-144 board.

2. Connect ethernet cable to W5300 TOE Shield ethernet port.

3. Connect STM32 Nucleo-144 board to desktop or laptop using 5 pin micro USB cable.



## Step 3: Setup Examples

To test the examples, minor settings shall be done in code.

1. Choose the demo application.

< sample >
a. ARDUINO:
To run the ChatServer example, you must double click the **ChatServer.ino** in '**Ethernet/examples/ChatServer**' directory.

b. PlatformIO:
To run the ChatServer example, you have to move the **ChatServer.cpp** in '**Ethernet/examples/PlatformIO_CPP_Examples**' to '**Your PlatformIO Project  Path/src/**' directory to build.


2. Setup network configuration.

Setup **your network configuration** in the examples such as IP of '**ChatServer.ino**' or  '**ChatServer.cpp**'in **project directory**.

```cpp
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 11, 111);
IPAddress myDns(192, 168, 11, 1);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 0, 0);
```



## Step 4: Build and Download

1. After completing the ChatServer example configuration, build in the IDE you are using.

2. When the build is completed, download the firmware to the STM32 Nucleo-144 board.



## Step 5: Run and Samples

1. Connect to the serial COM port of STM32 Nucleo-144 board. Default Serail speed was set to 9600 by example codes.

You can easilly see the network data by **Hercules**.
Download: https://www.hw-group.com/software/hercules-setup-utility

- ChatServer
![][link-Chatserver]


If you send text stream to  Server, you can receive the loopback  stream.

- AdvancedChatServer
![][link-advancedChatServer_1]
![][link-advancedChatServer_2]
If Multi-client connect to Server, each client can chat each other.

- WebServer
![][link-webServer]

If you connect to Server by using **Browser**, you can see the simple web page from server after several seconds.



<!--
Link
-->


[link-Chatserver]: https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/ChatServer.jpg
[link-advancedChatServer_1]: https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/advancedChatServer_1.jpg
[link-advancedChatServer_2]: https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/advancedChatServer_2.jpg
[link-advancedChatServer_serialLogs]: https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/advancedChatServer_serialLogs.jpg
[link-webServer]: https://github.com/Wiznet/W5300-TOE-Arduino/blob/main/Static/images/examples/advancedChatServer_serialLogs.jpg

