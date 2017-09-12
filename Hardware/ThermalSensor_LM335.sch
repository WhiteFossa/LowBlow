EESchema Schematic File Version 2
LIBS:fossa library
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:ThermalSensor_LM335-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "LM335 Thermal Sensor"
Date "2016-01-27"
Rev "1.0"
Comp ""
Comment1 "Licence: GPLv3+"
Comment2 "aka Artyom Wetroff"
Comment3 "By White Fossa"
Comment4 ""
$EndDescr
$Comp
L LM335 DA1
U 1 1 56A7EAD6
P 1540 1530
F 0 "DA1" H 1270 1570 60  0000 C CNN
F 1 "LM335" H 1750 1570 60  0000 C CNN
F 2 "Housings_TO-92:TO-92_Molded_Narrow_Oval" H 1530 1540 60  0001 C CNN
F 3 "" H 1530 1540 60  0000 C CNN
	1    1540 1530
	1    0    0    -1  
$EndComp
$Comp
L C C1
U 1 1 56A7EBC9
P 890 1950
F 0 "C1" H 915 2050 50  0000 L CNN
F 1 "0.1uF" H 915 1850 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 928 1800 30  0001 C CNN
F 3 "" H 890 1950 60  0000 C CNN
	1    890  1950
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 56A7ECF2
P 1540 940
F 0 "R1" V 1620 940 50  0000 C CNN
F 1 "4k7" V 1540 940 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 1470 940 30  0001 C CNN
F 3 "" H 1540 940 30  0000 C CNN
	1    1540 940 
	1    0    0    -1  
$EndComp
Wire Wire Line
	1540 1330 1540 1090
$Comp
L R R2
U 1 1 56A7EE95
P 2470 1420
F 0 "R2" V 2550 1420 50  0000 C CNN
F 1 "10k" V 2470 1420 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 2400 1420 30  0001 C CNN
F 3 "" H 2470 1420 30  0000 C CNN
	1    2470 1420
	1    0    0    -1  
$EndComp
$Comp
L R R3
U 1 1 56A7EF72
P 2470 1860
F 0 "R3" V 2550 1860 50  0000 C CNN
F 1 "20k" V 2470 1860 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 2400 1860 30  0001 C CNN
F 3 "" H 2470 1860 30  0000 C CNN
	1    2470 1860
	1    0    0    -1  
$EndComp
Wire Wire Line
	2470 1710 2470 1570
Wire Wire Line
	2470 1270 2470 1220
Wire Wire Line
	2470 1220 1540 1220
Connection ~ 1540 1220
Wire Wire Line
	2470 2690 2470 2010
Wire Wire Line
	890  2690 2810 2690
Wire Wire Line
	1540 2690 1540 2630
Wire Wire Line
	890  2100 890  2690
Connection ~ 1540 2690
Wire Wire Line
	890  1800 890  750 
Wire Wire Line
	890  750  2820 750 
Wire Wire Line
	1540 750  1540 790 
$Comp
L CONN_01X01 P3
U 1 1 56A7F455
P 3010 2690
F 0 "P3" H 3010 2790 50  0000 C CNN
F 1 "GND" V 3110 2690 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01" H 3010 2690 60  0001 C CNN
F 3 "" H 3010 2690 60  0000 C CNN
	1    3010 2690
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X01 P2
U 1 1 56A7F662
P 3010 1640
F 0 "P2" H 3010 1740 50  0000 C CNN
F 1 "OUT" V 3110 1640 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01" H 3010 1640 60  0001 C CNN
F 3 "" H 3010 1640 60  0000 C CNN
	1    3010 1640
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X01 P1
U 1 1 56A7F7FB
P 3020 750
F 0 "P1" H 3020 850 50  0000 C CNN
F 1 "VCC" V 3120 750 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01" H 3020 750 60  0001 C CNN
F 3 "" H 3020 750 60  0000 C CNN
	1    3020 750 
	1    0    0    -1  
$EndComp
Connection ~ 1540 750 
Wire Wire Line
	2810 1640 2470 1640
Connection ~ 2470 1640
Connection ~ 2470 2690
Text Notes 3190 1800 0    60   ~ 0
6.(6) mV/°K\n\n0°C		->	1.82V\n100°C	->	2.49V
$EndSCHEMATC
