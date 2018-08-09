#include "ShiftOut.h"

/**************************************************************************************************/

// I/O Pins
#define P_SH_CLK 5
#define P_SHOUT_DATA 6
#define P_SHOUT_LATCH 7

// Number of shift-registers chips
#define NUM_SHOUT 2

/**************************************************************************************************/

// The ShiftOut object to handle NUM_SHOUT registers
ShiftOut<NUM_SHOUT> Shiftout;

/**************************************************************************************************/

void setup()
{
    // Initialize Serial and ShiftOut object with corresponding pins
    Serial.begin(9600);
    Shiftout.begin(P_SHOUT_LATCH, P_SHOUT_DATA, P_SH_CLK);
}

void loop()
{
    static char to_write[64];

    // Set to HIGH each registers outputs
    for(int i = 0; i < NUM_SHOUT*8; i++)
    {
        Shiftout.digitalWrite(i, HIGH);

        // Print pin change message
        sprintf(to_write, "Expansion port pin %d with value HIGH", i);
        Serial.println(to_write);

        delay(500);
    }
    Serial.println();

    // Set to LOW each even registers outputs
    for(int i = 0; i < NUM_SHOUT*8; i=i+2)
    {
        Shiftout.digitalWrite(i, LOW);

        // Print pin change message
        sprintf(to_write, "Expansion port pin %d with value LOW", i);
        Serial.println(to_write);

        delay(500);
    }
    Serial.println();
	
    // Toggle each registers outputs
    for(int i = 0; i < NUM_SHOUT*8; i++)
    {
        Shiftout.toggle(i);

        // Check the actual output value and determine serial to print message
        if(Shiftout.actualValue(i) == HIGH)
            sprintf(to_write, "Expansion port pin %d with value HIGH", i);
        else
            sprintf(to_write, "Expansion port pin %d with value LOW", i);
        Serial.println(to_write);

        delay(500);
    }
    Serial.println();

    // Set to LOW each even registers outputs
    for(int i = 1; i < NUM_SHOUT*8; i=i+2)
    {
        Shiftout.digitalWrite(i, LOW);

        // Print pin change message
        sprintf(to_write, "Expansion port pin %d with value LOW", i);
        Serial.println(to_write);

        delay(500);
    }
    Serial.println("\n");
}
