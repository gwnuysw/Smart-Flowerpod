# Smart FlowerPod
This device manage envirenment of crop for **vertical farmming**.
Its service is based on two parts, one is control envirenment of pod, another is store data of envirenment on the Internet.

## 1. Control envirenment
The device sensers temperature, humi, soil moisture and illumination.
Then it control soil-moisture. if we develop further, we can control more.

## 2. Store data on the Internet
At the beginning of project, we designed RasspberryPi to store data on the Internet but we don't know how to do that.
controlboard send data to RasspberryPi through wifi AccessPoint communication.
RasspberrrPi just store it.

## [Running device video](http://www.youtube.com/watch?v=bFWi1mrmgvo)
## [Presentation](https://docs.google.com/presentation/d/10VtbtbHZbr2zLHU4GmD3ix2XDgnNsWMyZWsQEaYHdnU/edit?usp=sharing)
## [report](https://docs.google.com/document/d/12F0riSTwG4JUVup6Jfc6FNtPw7OqBEzBataWK_cbW_U/edit?usp=sharing)

## Contribution
We wellcome contributors. check out our directory that has two sub directory.
- Design beginning (all resources, information)
  - datasheets
  - Schematics
  - expamle projects (use of other devices, sensers)
  - AtmegaLib
- Workingdir
  - ATmega (this contain source code for Atmega2560 controlboard)
  - RaspberryPi (this contain source code for RasspberrrPi as server program)
  - reports (this contain all documents about this project)
  
## License
I don't choice License yet becouse I don't know all resources in 'Design beginning' directory exact come from. the project progressed by lecture named 'Design beginning' during September to December in 2017.
