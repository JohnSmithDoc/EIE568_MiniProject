# EIE568 Mini Project
1. **Project Title:** **Shared Conversion Plug**

2. **Project objective:**

   1. To develop a shared plug which is the British to Chinese plug adapter that can be shared among specific groups like students or stuffs in specific campus. It has the following functions:
      1. It can scan the student ID card, and read its information
      2. It can send the student ID card information to the school’s management system or database or school’s cloud (in the mini project I just use MQTT for demonstration because it is impossible for me to get access to the school’s database or cloud)
      3. The MQTT (in real application, this will be the cloud or database or a service software deployed on the server) will send command to the plug to let the plug (the Arduino) know that if this card, which the student possess, is permitted to use this device (shared plug adapter).
      4. If permitted, the plug will automatically connect the wire so the student can use this shared British to Chinese plug adapter. Meantime, there will be green LED blinking indicates the permission is granted.
      5. If not permitted, the plug will blink red LED and will not connect the wire so the plug can not be used.
   2. **Note: this project is only a demonstration, not an actual product!**

3. **More information:**

   1. Based on: [GitHub - OSSLibraries/Arduino_MFRC522v2](https://github.com/OSSLibraries/Arduino_MFRC522v2/tree/master)

   2. Board: Arduino Nano RP2040 Connect

   3. **Some libraries are needed:**

      1. **ArduinoMqttClient**
         1. For connecting with MQTT
      2. **WiFiNINA**
         1. For connecting with WIFI and control on-board RGB LED
      3. **RFID_MFRC522v2**
         1. To drive RFID-RC522
      4. **TickTwo**
         1. For blinking RGB LED without using delay function

   4. Typical RFID-RC522 pin layout used:

      | Signal    | MFRC522 | Uno / 101   | Mega | Nano v3 | Leonardo / Micro | Pro Micro | Yun   |
      | --------- | ------- | ----------- | ---- | ------- | ---------------- | --------- | ----- |
      | RST/Reset | RST     | 9           | 5    | D9      | RESET / ICSP-5   | RST       | Pin9  |
      | SPI SS    | SDA     | 10          | 53   | D10     | 10               | 10        | Pin10 |
      | SPI MOSI  | MOSI    | 11 / ICSP-4 | 51   | D11     | ICSP-4           | 16        | ICSP4 |
      | SPI MISO  | MISO    | 12 / ICSP-1 | 50   | D12     | ICSP-1           | 14        | ICSP1 |
      | SPI SCK   | SCK     | 13 / ICSP-3 | 52   | D13     | ICSP-3           | 15        | ICSP3 |

   5. **Relay pin: D3**.  In this project, using LED to replace Relay for demonstration.

   6. **Other maybe useful information:**

      ```
      /* 
      A struct used for passing the UID of a PICC.
      typedef struct {
        byte		size;			// Number of bytes in the UID. 4, 7 or 10.
        byte		uidByte[10];
        byte		sak;			// The SAK (Select acknowledge) byte returned from the PICC after successful selection.
      } Uid;
      */
      
      /*
      For my card test result:
        the white card UID is: 73078AE2
      */
      ```

