# Blynk-Weather-station
A simple Weather station with Blynk http api

The weather station is the first device that I've realized, it's an esp-8266 battery powered, solar charged and use a DS18B20 temperature sensor to monitor external temperature,
List of sensor:

DS18B20 digital temperature sensor, 16 bit accuracy, onewire protocol.
Resistive rain sensor connected to the ADS1115 external adc.
DHT11 monitor internal box temperature and humidity, poor accuracy, onewire protocol.
LM35 monitor the internal temperature , medium accuracy, connected to the ADS1115.
Battery voltage is measured by both the internal adc and the external ADS1115.
Solar panel voltage is measured by the external ADS1115.
The battery is a Li-ion 1 cell battery (4400 mAh) recharged by a solar panel trough a classic TP4056
