# mBot A-maze-ing Race Project

This repo consists of the report and code to my team's mBot A-maze-ing Race project for the CG1111 module.

The code was written and uploaded via the Arduino IDE. In this project, we were given the task to build an mBot that can find its own way through a random maze given by our profs. Unlike previous academic years, this mBot only has IR and Colour Sensing capabilities without Sound Sensors.

In the maze, this mBot has IR sensors to keep it moving straight so that it does not bump into the side walls of the maze. When it detects a black strip on the floor, it will call its colour challenge function to read the colour given on top of the mBot. The colour will determine which direction the mBot will turn. The mBot will continue finding its way until it detects the black paper on top of it, indicating the end of the maze. It will then activate its buzzer and play a celebratory tune as it has completed the maze. The functions are implemented using conditional statements in an infinite loop often used for Arduinos.
