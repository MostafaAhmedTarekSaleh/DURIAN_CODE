Ali Murshids Industrial Training Placement (ITP) Progress

This repository documents my learning journey and project development during my Industrial Training Placement (ITP). The focus of my training is on embedded systems, low-power communication (LoRaWAN), and camera integration with STM32 microcontrollers.



📌 What I’ve Learned So Far

LoRaWAN Fundamentals • Understood the basics of LoRa and LoRaWAN communication. • Explored bandwidth limitations and how they affect data transmission. • Studied how to send small packets of sensor and image data over LoRaWAN. • Learned about packet segmentation and reassembly for transmitting images.

Energy Harvesting for IoT Devices • Explored various power sources such as RF harvesting, TEG (Thermoelectric Generators), and solar energy. • Compared efficiency and practicality of different methods. • Decided on solar power as the most reliable energy source for the project.

STM32 Microcontroller • Reviewed STM32 datasheets and peripheral capabilities. • Learned GPIO configuration and tested buttons & LEDs. • Explored UART communication for sending/receiving data. • Began studying DCMI (Digital Camera Memory Interface) and DMA for handling image data.

Image Transmission over LoRaWAN • Researched techniques for image compression and format conversion. • Learned methods such as grayscale, monochrome, and Run-Length Encoding (RLE) to reduce data size. • Experimented with sending very small images as a proof of concept.

Camera Module Integration • Studied camera datasheets to understand pinouts, registers, and communication protocols. • Learned how to connect the camera module to STM32 with proper power and data line mapping. • Began debugging camera initialization, register configuration, and synchronization signals (VSYNC/HSYNC).

⸻

🛠 Tools and Technologies Used • STM32 Microcontroller (STM32F4 series) • STM32CubeIDE for coding and debugging • LoRaWAN communication modules • Camera module (OV2640) • Datasheets and reference manuals • GitHub for documentation and progress tracking
