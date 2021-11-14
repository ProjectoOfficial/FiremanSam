# FiremanSam
 ![License](https://badgen.net/badge/license/GPL-3.0/red?icon=github)
 ![Hits](https://hitcounter.pythonanywhere.com/count/tag.svg?url=https://github.com/ProjectoOfficial/FiremanSam)
  
Contents
========

* [Description](#Description)
* [Hardware](#Hardware)
* [Libraries](#Libraries)
* [Machine Learning](#Machine-Learning)
* [Lets Keep in Touch](#Lets-Keep-in-Touch)
* [About](#About)

### Description
 FiremanSam is a project designed and developed for the IOT university exam. It is about combining the IOT and artificial intelligence to detect the presence of a fire and immediately take action. We first of all developed the hardware of the project that requires the use of CO2, TVOC, humidity and temperature sensors, connected to an ARM processor that communicates the score of the predict function of the machine learning model on the firebase. We have in fact created a dataset with the acquisitions by the sensors and we have trained a support vector machine capable of detecting a fire.
 
### Hardware
The following hardware components were used to carry out the project:
- ESP32-WROOM-32
- CCS811 CO2 and TVOC sensor
- DHT11 Humidity and Temperature Sensor

### Libraries
the following libraries are those used to carry out the project:
 - <a href="https://github.com/ProjectoOfficial/Oscup"> Oscup - Open Source Custom UART Protocol By Projecto<a/>
 - <a href="https://www.dropbox.com/sh/or1jzflapzbdepd/AACgGE_AW6nqoYV6fwAW4lx9a/Libraries?dl=0&subfolder_nav_tracking=1">CCS811 Libray </a>
 - <a href="https://github.com/adafruit/DHT-sensor-library"> DHT11 sensor Library by Adafruit</a>
 
### Machine Learning
 As a classifier we used a Support Vector Machine as it provides excellent results, in particular accuracy 100% and f1-score 100% on the testing set. Another advantage of using a support vector machine is that the model only consists of 5 parameters: 4 weights and 1 bias. As an algorithm we used Pegasos, a Support Vector Machine, a stochastic sub-gradient descent algorithm.The model parameters have been loaded onto the ESP32 and the prediction is done on-chip. The forecast score is also saved to a database on Firebase
 
### Lets Keep in Touch
<a href="Https://youtube.com/c/ProjectoOfficial" target="_blank"><img src="https://img.shields.io/badge/YouTube-FF0000?style=for-the-badge&logo=youtube&logoColor=white"/></a>
<a href="https://www.instagram.com/OfficialProjecTo/" target="_blank"><img src="https://img.shields.io/badge/Instagram-E4405F?style=for-the-badge&logo=instagram&logoColor=white"/></a>
<a href="https://www.facebook.com/MiniProjectsOfficial" target="_blank"><img src="https://img.shields.io/badge/Facebook-1877F2?style=for-the-badge&logo=facebook&logoColor=white"/></a>
<a href="https://www.tiktok.com/@officialprojecto" target="_blank"><img src="https://img.shields.io/badge/TikTok-000000?style=for-the-badge&logo=tiktok&logoColor=white"/></a>
<a href="https://github.com/ProjectoOfficial" target="_blank"><img src="https://img.shields.io/badge/GitHub-100000?style=for-the-badge&logo=github&logoColor=white"/></a>
<a href="https://it.linkedin.com/company/officialprojecto" target="_blank"><img src="https://img.shields.io/badge/LinkedIn-0077B5?style=for-the-badge&logo=linkedin&logoColor=white"/></a>

 ### About
 This software is provided as it is, without any warranties. <br/>
 Authors: Dott. Daniel Rossi, Dott. Riccardo Salami both B.Sc. Software Engineers and M.Sc. Artificial Intelligence Engineering students
 
<a href = "https://github.com/Your_GitHub_Username/Oscup/graphs/contributors">
<img src = "https://contrib.rocks/image?repo=ProjectoOfficial/Oscup"/>
</a>

