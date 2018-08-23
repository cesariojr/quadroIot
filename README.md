# quadroIot

Esse protótipo tem o objetivo de demonstrar uma aplicação IoT de forma didática, com entrada de dados via sensores, processamento através de um microcontrolador e saída de dados por um display LCD gráfico.

Para isso foram utilizados os seguintes sensores:
1. Sensor de qualidade do ar (http://wiki.seeed.cc/Grove-Air_Quality_Sensor_v1.3/)
2. Sensor de temperatura e umidade (http://wiki.seeed.cc/Grove-TemperatureAndHumidity_Sensor/)
3. Sensor de luminosidade (http://wiki.seeed.cc/Grove-Light_Sensor/)
4. Sensor Bosch BMP180 (https://cdn-shop.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf)
5. Sensor de chuva (similar a http://wiki.seeed.cc/Grove-Water_Sensor/)
6. Sensor de umidade do solo (http://wiki.seeed.cc/Grove-Moisture_Sensor/)

Como microcontrolador, foi utilizado um Arduino Mega 2560 com ethernet shield e 1 display LCD gráfico de 128x64 px para exibição de dados dos sensores e 1 LCD texto, 16x2 para monitoramento do quadro.

![alt text](https://github.com/cesariojr/quadroIot/blob/master/IMG_2211.JPG)

Os dados derivados da medição dos sensores são formatados em JSON e enviados para o IBM Watson IoT Platform, utilizando o protocolo MQTT. Um dashboard criado em NodeRED, utilizando o componente UI foi criado para visualização dos dados em tempo real:

https://internetdascoisas.mybluemix.net/ui/
