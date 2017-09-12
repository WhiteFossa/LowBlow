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
LIBS:ThermalSensor_LM35-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "LM35 Thermal Sensor"
Date "2016-01-27"
Rev "1.0"
Comp ""
Comment1 "Licence: GPLv3+"
Comment2 "aka Artyom Wetroff"
Comment3 "By White Fossa"
Comment4 ""
$EndDescr
$Comp
L C C1
U 1 1 56A13448
P 1190 1380
F 0 "C1" H 1215 1480 50  0000 L CNN
F 1 "0.1uF" H 1215 1280 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 1228 1230 30  0001 C CNN
F 3 "" H 1190 1380 60  0000 C CNN
	1    1190 1380
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 56A13578
P 2520 1420
F 0 "R1" V 2600 1420 50  0000 C CNN
F 1 "2k" V 2520 1420 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 2450 1420 30  0001 C CNN
F 3 "" H 2520 1420 30  0000 C CNN
	1    2520 1420
	0    1    1    0   
$EndComp
$Comp
L CONN_01X01 P2
U 1 1 56A1379F
P 2940 1420
F 0 "P2" H 2940 1520 50  0000 C CNN
F 1 "OUT" V 3040 1420 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01" H 2940 1420 60  0001 C CNN
F 3 "" H 2940 1420 60  0000 C CNN
	1    2940 1420
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X01 P1
U 1 1 56A13A81
P 2940 920
F 0 "P1" H 2940 1020 50  0000 C CNN
F 1 "VCC" V 3040 920 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01" H 2940 920 60  0001 C CNN
F 3 "" H 2940 920 60  0000 C CNN
	1    2940 920 
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X01 P3
U 1 1 56A13BB7
P 2950 1990
F 0 "P3" H 2950 2090 50  0000 C CNN
F 1 "GND" V 3050 1990 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01" H 2950 1990 60  0001 C CNN
F 3 "" H 2950 1990 60  0000 C CNN
	1    2950 1990
	1    0    0    -1  
$EndComp
Wire Wire Line
	2740 1420 2670 1420
Wire Wire Line
	2740 920  1190 920 
Wire Wire Line
	1190 920  1190 1230
$Comp
L LM35_-_TO-220 DA1
U 1 1 56A14EB0
P 1530 1140
F 0 "DA1" H 1600 1260 60  0000 C CNN
F 1 "LM35_-_TO-220" H 1820 1180 60  0000 C CNN
F 2 "TO_SOT_Packages_THT:TO-220_Neutral123_Vertical" H 1530 1140 60  0001 C CNN
F 3 "" H 1530 1140 60  0000 C CNN
	1    1530 1140
	1    0    0    -1  
$EndComp
Wire Wire Line
	1830 990  1830 920 
Connection ~ 1830 920 
Wire Wire Line
	2370 1420 2320 1420
Wire Wire Line
	2750 1990 1190 1990
Wire Wire Line
	1190 1990 1190 1530
Wire Wire Line
	1830 1920 1830 1990
Connection ~ 1830 1990
Text Notes 3180 1580 0    60   ~ 0
10mV/°C\n\n0°C		->	0.0V\n100°C	->	1.0V
$EndSCHEMATC
