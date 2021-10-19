# Otto-SetupAssist
Otto-SetupAssist provides an Arduino sketch which assist you to build Otto robots. This sketch provides two features: 
1. Move servos to neutral  (90 degree) position. 
2. calibrate servos

## **Installation**
This is a simple Arduino Sketch, please open it with Arduino IDE and write this program to Otto through USB cable.
If USB connection doesn't find Otto, you may need to install one of the following USB driver for your computer to recognize it:

- [USB serical CH340 driver](https://sparks.gogo.co.nz/ch340.html)
- [FT232 USB UART driver](https://www.ftdichip.com/Drivers/CDM/CDM%20v2.12.28%20WHQL%20Certified.zip)

## **How To Use**
This program requires serial terminal to interact with Otto. Be sure to connect PC to Otto through USB cable and open serial terminal of Arduino IDE to interact with Otto.

This program have two modes. **Neutral mode** and **Trim mode**.
This program start from Neutral mode.
In Neutral mode. Otto does:

### **What this program does in Neutral Mode?** 
1. Move each servos plus and minus 15 degress from the neutral position (90 degree) of the servo and then stop at the neutral position (90 degree). This movements helps you to check if all servos are connected and functioning, and then find the neutral position of each servo. Servos connected to pin2 to pin5 are supported now.

2. In Serial terminal, following message will be displayed. This is a instruction help message for further interaction.

    ```
    Help:
    n key + enter: move all servo (pin 2-5) to neutral (90 degree) position
    t key + enter: go to trim mode
    h key + enter or enter: Show this help message
    ```

3. When you send 't' to terminal, this program goes to **Trim mode**.

### **What this program does in Trim Mode?**
1. It read latest trims from EEPROM and move servos to ther center position with the trim.
2. Further instruction is displayed in the serial terminal. Following is the message which will be shown in the terminal:

    ```
    Help:
    n key + enter: move all servo (pin 2-5) to neutral (90 degree) position
    t key + enter: move all servo (pin 2-5) to the center of the adjusted position
    w key + enter: walk
    x key + enter: show current adjustment
    h key + enter or enter: Show this help message

    Trim your Otto!
                +-----------+
                |  O     O  |
                +-----------+
                |           |
                +-----------+
          1/q    +         +   3/e
                 |         |
          a/z  __|         |__ d/c

    1/q key + enter: Trim right leg  -/+
    a/w key + enter: Trim right foot -/+
    3/e key + enter: Trim left  leg  -/+
    d/c key + enter: Trim left  foot -/+
    s key + enter: Save current position
    ```

**Hints**
- You may want to move servo wider in a single message. You can do it by sending same key multiple time in a single message. For example, you can move left foot +5 degree by sending 'd' five times to terminal in a single message ('dddd' + [Enter]).
- By sending 'w', Otto walks. You can check current adjustment is Ok by this. Same with adjust messages, you can send multiple 'w' to Otto and otto executes walking movements mutiple times. For example, by sending 'wwwwww' to terminal, Otto makes six steps forward.

