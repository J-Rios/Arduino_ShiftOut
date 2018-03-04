#include "ShiftOut.h"

/**************************************************************************************************/

#define P_SH_CLK 5
#define P_SHOUT_DATA 6
#define P_SHOUT_LATCH 7

#define NUM_SHOUT 2

/**************************************************************************************************/

ShiftOut<NUM_SHOUT> Shiftout;

/**************************************************************************************************/

void setup()
{
	Serial.begin(9600);
	Shiftout.begin(P_SHOUT_LATCH, P_SHOUT_DATA, P_SH_CLK);
}

void loop()
{
	static char to_write[64];

	for(int i = 0; i < NUM_SHOUT*8; i++)
	{
		Shiftout.digitalWrite(i, HIGH);

		if(Shiftout.actualValue(i) == HIGH)
			sprintf(to_write, "Expansion port pin %d with value HIGH", i);
		else
			sprintf(to_write, "Expansion port pin %d with value LOW", i);
		Serial.println(to_write);

		delay(500);
	}
	Serial.println();
	for(int i = 0; i < NUM_SHOUT*8; i=i+2)
	{
		Shiftout.digitalWrite(i, LOW);

		if(Shiftout.actualValue(i) == HIGH)
			sprintf(to_write, "Expansion port pin %d with value HIGH", i);
		else
			sprintf(to_write, "Expansion port pin %d with value LOW", i);
		Serial.println(to_write);

		delay(500);
	}
	Serial.println();
	for(int i = 1; i < NUM_SHOUT*8; i=i+2)
	{
		Shiftout.digitalWrite(i, LOW);

		if(Shiftout.actualValue(i) == HIGH)
			sprintf(to_write, "Expansion port pin %d with value HIGH", i);
		else
			sprintf(to_write, "Expansion port pin %d with value LOW", i);
		Serial.println(to_write);

		delay(500);
	}
	Serial.println();
	for(int i = 0; i < NUM_SHOUT*8; i++)
	{
		Shiftout.toggle(i);

		if(Shiftout.actualValue(i) == HIGH)
			sprintf(to_write, "Expansion port pin %d with value HIGH", i);
		else
			sprintf(to_write, "Expansion port pin %d with value LOW", i);
		Serial.println(to_write);

		delay(500);
	}
	Serial.println();
	for(int i = 0; i < NUM_SHOUT*8; i++)
	{
		Shiftout.toggle(i);

		if(Shiftout.actualValue(i) == HIGH)
			sprintf(to_write, "Expansion port pin %d with value HIGH", i);
		else
			sprintf(to_write, "Expansion port pin %d with value LOW", i);
		Serial.println(to_write);

		delay(500);
	}
	Serial.println();
}
