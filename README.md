# EIE568 Mini Project
Based on: [GitHub - OSSLibraries/Arduino_MFRC522v2](https://github.com/OSSLibraries/Arduino_MFRC522v2/tree/master)

Board: Arduino Nano RP2040 Connect

Typical RFID-RC522 pin layout used:

| Signal    | MFRC522 | Uno / 101   | Mega | Nano v3 | Leonardo / Micro | Pro Micro | Yun   |
| --------- | ------- | ----------- | ---- | ------- | ---------------- | --------- | ----- |
| RST/Reset | RST     | 9           | 5    | D9      | RESET / ICSP-5   | RST       | Pin9  |
| SPI SS    | SDA     | 10          | 53   | D10     | 10               | 10        | Pin10 |
| SPI MOSI  | MOSI    | 11 / ICSP-4 | 51   | D11     | ICSP-4           | 16        | ICSP4 |
| SPI MISO  | MISO    | 12 / ICSP-1 | 50   | D12     | ICSP-1           | 14        | ICSP1 |
| SPI SCK   | SCK     | 13 / ICSP-3 | 52   | D13     | ICSP-3           | 15        | ICSP3 |

Relay pin: D3.  In this project, using LED to replace Relay for demonstration.

Some useful information:

```
/* 
A struct used for passing the UID of a PICC.
typedef struct {
  byte		size;			// Number of bytes in the UID. 4, 7 or 10.
  byte		uidByte[10];
  byte		sak;			// The SAK (Select acknowledge) byte returned from the PICC after successful selection.
} Uid;
*/
```

```
/*
For my card test result:
  the white card UID is: 73078AE2
*/
```

