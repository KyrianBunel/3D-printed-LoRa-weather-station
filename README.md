# 3D printed LoRa weather station
3D printed solar powered LoRa weather station based on Heltec AB01 developpement board

<img width="724" alt="Capture d’écran 2024-12-03 à 21 28 02" src="https://github.com/user-attachments/assets/d472f429-3b20-416a-8fa1-91aeea1f0b72">

# Features :
- 6v solar panned on a 3D printed ajustable support
- 3.7V 750mAh battery compartement
- LoRaWAN communication protocol
- Good measure of :
  
    temteratue +- 0.5°C

    humidity +- 1%

    pressure

    ambient light

- collect data every 30 minutes

# How to start up :
### Weather station
- Download the code and upload it to the AB01 dev board
- Press the reset button on the PCB
- Print the slt files included ans assemble the weather station

### Receiver
- Download the code and upload it to the AB01 dev board
- Press the reset button on the PCB
- you will see data on serial monitor

# Parts list :
- LTR390 ambient light sensor
- Grove BME680 sensor
- CubeCell – AB01 Dev-Board : https://fr.aliexpress.com/item/1005006996324165.html?spm=a2g0o.order_list.order_list_main.5.493d5e5bFTWf9l&gatewayAdapt=glo2fra
- 72X45mm Solar pannel : https://fr.aliexpress.com/item/1005006312878480.html?spm=a2g0o.order_list.order_list_main.10.19585e5bnmPULu&gatewayAdapt=glo2fra


