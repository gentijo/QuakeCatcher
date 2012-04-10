Quake Catcher
=============

An open plan earthquake sensor that works with a quake sensor network like QCN (Stanford [Quake Catcher Network](http://qcn.stanford.edu/)).

#### What's an Earthquake Sensor? ####

Accelerometer based earthquake sensors are small devices installed across a large geographic area that send high resolution seismographic data (or vibrations from the ground) to a quake sensor network forming a network analogous to the human nervous system. The central servers look for spikes in readings in real time and with enough sensors, earthquakes can be measured with tremendous precision and speed. 

Currently available sensors require a computer connection (via USB) to process and send data back to the central network. Our project's aim is to eliminate the computer requirement and make a standalone device much like a smoke detector that people can place in parts of their home with little foot traffic. The desktop computer is also quickly disappearing from the modern home.

#### What's in this Earthquake Sensor? ####

<a href="https://github.com/gentijo/QuakeCatcher/blob/master/pics/sensors_02.jpg?raw=true"><img src="https://github.com/gentijo/QuakeCatcher/blob/master/pics/sensors_02.jpg?raw=true" width="512" height="384" alt="Sensor Components"></a>

We are hoping to make the plans of this sensor as accessible as possible. The kinds of electronics we are working with are readily available to buy online or at electronics stores and, to the electronics enthusiast, don't look that different from a list of parts in a modern smartphone:

 - [Microcontroller board](http://en.wikipedia.org/wiki/Microcontroller) - Currently using the [Bahbots Microcontroller](http://bahbots.com/)
 - [MEMS-based accelerometer](http://en.wikipedia.org/wiki/Accelerometer) - Currently prototyping with sensors from [DSS Circuits](http://dsscircuits.com/)
 - Solar Cell and rechargeable battery - currently looking at parts from [The BootstrapSolar project](http://www.bootstrapsolar.com/)
 - Wifi board

#### Coding ####

<a href="https://github.com/gentijo/QuakeCatcher/blob/master/pics/coding_01.jpg?raw=true"><img src="https://github.com/gentijo/QuakeCatcher/blob/master/pics/coding_01.jpg?raw=true" width="50%"></a><a href="https://github.com/gentijo/QuakeCatcher/blob/master/pics/visualizer_01.png?raw=true"><img src="https://github.com/gentijo/QuakeCatcher/blob/master/pics/visualizer_01.png?raw=true" width="50%">

Most of the coding will be done in basic C to tie together all of the electronics components, talk to your local wifi and transmit data. We are also developing a small visualization application written in JavaScript (running on [NodeJS](https://github.com/joyent/node)) that the user can run on their local network to see what data is communicated back to the sensor network.

#### Industrial Design ####

<a href="https://github.com/gentijo/QuakeCatcher/blob/master/pics/milled_case_01.jpg?raw=true"><img src="https://github.com/gentijo/QuakeCatcher/blob/master/pics/milled_case_01.jpg?raw=true" width="50%"></a><a href="https://github.com/gentijo/QuakeCatcher/blob/master/pics/milled_case_02.jpg?raw=true"><img src="https://github.com/gentijo/QuakeCatcher/blob/master/pics/milled_case_02.jpg?raw=true" width="50%"></a>

The construction of the case will be designed for outdoor use. The long term goal is to create a self contained unit that doesn't require constant maintenance and, with the use of improved wireless technology (like 3g), the device could potentially be used in remote locations (ie instrumenting a fault line).

#### Why Are We Doing This? ####

We live in the Bay Area and have friends who live in other earthquake heavy regions (Japan) so this is just our way of getting involved. This is also a great project to learn about electronics, coding and industrial design.

Check out [Ready.gov](http://www.ready.gov/earthquakes) for more great resources on earthquake readiness.