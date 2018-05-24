/**************************************************************************/
/*! 
    Author: Michael Zhang
    
    Based on work by: KTOWN (Adafruit Industries) 
*/
/**************************************************************************/



#include <avr/pgmspace.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <Adafruit_MCP4725.h>

//Adafruit Library and Device setup

#define PN532_SS   (6)
#define PN532_IRQ   (4)
#define PN532_RESET (8) //Working even though this is not connected.  Must be defined.

#define PWM_out 9

Adafruit_PN532 nfc(PN532_SS);

//DAC
Adafruit_MCP4725 DAC_1;

char Puck_Names[6][6] = {"Puck_A","Puck_B","Puck_C","Puck_D","Puck_E","Puck_F"};
int Output_values[6] ={4095,3685,3276,2866,2457,2047};

const byte Pucks[6][7] PROGMEM = 
{
    {0x04, 0xE6, 0xF2, 0x32, 0xED, 0x4C, 0x80}, //A
    {0x04, 0x49, 0xED, 0x32, 0xED, 0x4C, 0x81}, //B
    {0x04, 0x9B, 0xF2, 0x32, 0xED, 0x4C, 0x80}, //C
    {0x04, 0x51, 0xED, 0x32, 0xED, 0x4C, 0x81}, //D
    {0x04, 0xF6, 0xF2, 0x32, 0xED, 0x4C, 0x80}, //E
    {0x04, 0x4B, 0xEC, 0x32, 0xED, 0x4C, 0x81} //F

};


void setup(void) {
    Serial.begin(115200);
    nfc.begin();
    uint32_t versiondata = nfc.getFirmwareVersion();
    
    if (! versiondata) {
        while (1); // halt
    }
    // configure board to read RFID tags
    nfc.SAMConfig();
    Serial.println("Waiting for an ISO14443A Card ...");
    DAC_1.begin(0x60);
    DAC_1.setVoltage(0, false);
}

void loop(void) {
    
    Serial.println("Reading....");
    read_RFID();
    delay(1000);

}

void read_RFID() {

    uint8_t success;
    byte uid[7] = { 0, 0, 0, 0, 0, 0, 0};  // Buffer to store the returned UID
    uint8_t uidLength;                // Length of the UID (4 or 7 bytes depending on ISO14443A card type).  For robot, it's 7.

    // Wait for an NTAG203 card.  When one is found 'uid' will be populated with
    // the UID, and uidLength will indicate the size of the UUID (normally 7)
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
    
    if (success) {
        nfc.PrintHex(uid, uidLength);

        Serial.println("Checking...");

        for (int i=0;i<6;i++){
            //Serial.println(Pucks[i]);
            if (memcmp_P(uid, Pucks[i], 7) == 0){
                Serial.print("\n");
                for (int k = 0;k<6;k++){
                    Serial.print(Puck_Names[i][k]);
                }                
                Serial.print(" found!\n");
                DAC_1.setVoltage(Output_values[i], false);
            }
        }
    }

}