# ArduCam Sketch
Sketch for an Arduino motion-controlled camera that wirelessly sends images to a collecting computer via XBee. For a component list and wiring diagram, visit the Documentation folder.

# Summary
This project is intended to be used in conjunction with a computer running the ArduinoCameraImageCollector program, repo located at https://github.com/wodeeken/ArduinoCameraImageCollector. The images captured by the ArduCam shield, triggered by the IR motion sensor, will be sent through a XBee radio to another Xbee connected to the computer running the data collector program. This program listens to the serial port the XBee radio is attached to, and will save the images in a folder located in the same directory as the executable. For a user-friendly way of viewing these images, see the web app here: https://github.com/wodeeken/ArduinoCameraImageWebApp.

# Requirements and Notes
This sketch should be edited, compiled, and uploaded to the Arduino Uno using the standard Arduino IDE. The ArduCam library must be added to the project by going to Tools > Manage Libraries, and searching for and installing ArduCAM. If for some reason this libary cannot be found, its public repository is here: https://github.com/dennis-ard/ArduCAM.

The two XBee radios required in this project must be configured with the same PAN ID in order to communicate with each other. It does not matter what this ID is, just that they are the same in both radios. The Digi XCTU program can be used to accomplish this, and can be downloaded from here: https://www.digi.com/products/embedded-systems/digi-xbee/digi-xbee-tools/xctu.
