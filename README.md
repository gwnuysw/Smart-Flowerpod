# RasPiPod
gwnu 2nd grade raspberrypi project.
this device is based on raspberrypi and atmega2560 controlboard.
FlowerPod envirenment is watched by sensers.
atmega2560 controlboard gets Humi, temp, soli moisture data so control AC motor to give water on flowerpod soil.
The data that atmega2560 gained send to raspberrypi using wifi AP comunication.
raspberrypi and atmega2560 controlboard share same wifi AP.
raspberrypi write received Humi, temp, soil moisture data on *.txt file.

