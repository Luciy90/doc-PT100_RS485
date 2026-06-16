Technical Documentation: PT100 to RS485 Converter Module (Modbus RTU)
Version: 1.0 (Revised)
Application: Industrial temperature data acquisition, integration with automation systems (e.g., KC868-A16).

1. Hardware Specifications
Interface: RS-485 (two-wire: A+, B-).

Input: PT100 (supports 2-wire and 3-wire configurations).

Power Supply: Please check the markings on your board (typically 5V–24V).

Transceiver: 75P20A03 (or equivalent).

Controller: Modbus RTU Slave emulation.

2. Register Map (Modbus Holding Registers)
Access is provided via standard FC03 (Read) and FC06 (Write) functions.

Note: Parameters marked as (Flash) are saved after power cycling.

3. Communication Protocol
UART Settings: 8-N-1 (8 data bits, no parity, 1 stop bit).

Default Baud Rate: 9600 baud.

Default Address: 1.

4. Configuration and Calibration
Changing the Device Address
To operate multiple sensors on a single RS-485 bus, you must assign a unique address to each module one by one:

Connect only one module to the controller.

Write the new value (e.g., 2) to register 0x0002 using function FC06.

The module adopts the new address immediately; no reboot is required.

Changing the Baud Rate
If a different baud rate is required, write the corresponding code to register 0x0003:

0 — 1200 baud

1 — 2400 baud

2 — 4800 baud

3 — 9600 baud

Temperature Calibration
If the sensor readings deviate from the reference:

Read the current temperature from 0x0100.

Calculate the difference (Offset).

Write the difference to register 0x0004 (e.g., for a +1.5 °C correction, write 15).

5. Important Warnings
Broadcast Address: Avoid writing 0 to register 0x0002 (Address). This may lead to loss of control, as it sets the module to broadcast mode.

System Lockup: Do not write values other than codes 0–4 to the baud rate register, as this may cause the controller's UART to malfunction.

Data Integrity: Since parameters 0x0002, 0x0003, and 0x0004 are stored in non-volatile memory, do not perform writes in your code's loop() function (this will quickly exhaust the Flash memory write cycle limit). Perform writes only during initial configuration.

This documentation was compiled based on reverse-engineering results and is applicable to this specific hardware batch.
