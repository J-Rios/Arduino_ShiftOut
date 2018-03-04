/**************************************************************************************************/
/* Name: ShiftOut.h                                                                               */
/* Description: Arduino library for easy control serial-in to parallel-out shift registers        */
/*              (i.e. 74hc595) with Mutex-safe for ESP32 FreeRTOS projects.                       */
/* Author: JRios                                                                                  */
/* Creation Date: 04/03/2018                                                                      */
/* Last Modified: 04/03/2018                                                                      */
/* Version: 1.0.0                                                                                 */
/**************************************************************************************************/

#ifndef SHIFTOUT_H_
	#define SHIFTOUT_H_

	/**********************************************************************************************/

	#include <Arduino.h>

	/**********************************************************************************************/

	// Uncomment next line for safe access to Shiftout registers through multiples Tasks in a
	// FreeRTOS project
	//#define SHIFTOUT_USING_FREERTOS
	
	// Number of outputs for each register
	const uint8_t SHIFTOUT_OUTPUTS = 8;

	/**********************************************************************************************/

	template<uint8_t num_chips> class ShiftOut
	{
		public:
			// Constructor
			ShiftOut() : _num_chips(num_chips)
			{
				_pin_latch = 0;
				_pin_data = 0;
				_pin_clk = 0;
				_data_width = num_chips * SHIFTOUT_OUTPUTS;

				#ifdef SHIFTOUT_USING_FREERTOS
					mutex_shout = NULL;
				#endif
			}

			// Configure latch, data and clock pins and initialize outputs
			void begin(const uint8_t pin_latch, const uint8_t pin_data, const uint8_t pin_clk)
			{
				_pin_data = pin_data;
				_pin_clk = pin_clk;
				_pin_latch = pin_latch;

				#ifdef SHIFTOUT_USING_FREERTOS
					mutex_shout = xSemaphoreCreateMutex();

					if(xSemaphoreTake(mutex_shout, (portTickType) 10) == pdTRUE)
					{
						for(uint8_t i = 0; i < _num_chips; i++)
							memset(_data_bin[i], LOW, SHIFTOUT_OUTPUTS);

						pinMode(_pin_data, OUTPUT);
						pinMode(_pin_clk, OUTPUT);
						pinMode(_pin_latch, OUTPUT);
						::digitalWrite(_pin_data, LOW);
						::digitalWrite(_pin_clk, LOW);
						
						::digitalWrite(_pin_latch, LOW);
						for(int8_t i = _num_chips-1; i >= 0; i--)
							shiftOut(_pin_data, _pin_clk, LSBFIRST, 0);
						::digitalWrite(_pin_latch, HIGH);

						xSemaphoreGive(mutex_shout);
					}
				#else
					for(uint8_t i = 0; i < _num_chips; i++)
						memset(_data_bin, LOW, SHIFTOUT_OUTPUTS);

					pinMode(_pin_data, OUTPUT);
					pinMode(_pin_clk, OUTPUT);
					pinMode(_pin_latch, OUTPUT);
					::digitalWrite(_pin_data, LOW);
					::digitalWrite(_pin_clk, LOW);
					
					::digitalWrite(_pin_latch, LOW);
					for(int8_t i = _num_chips-1; i >= 0; i--)
						shiftOut(_pin_data, _pin_clk, LSBFIRST, 0);
					::digitalWrite(_pin_latch, HIGH);
				#endif
			}

			// Set one of the outputs to a specific value (LOW/HIGH)
			int8_t digitalWrite(const uint8_t n, const uint8_t value)
			{
				int8_t write_result = -1;
				uint8_t register_n;
				uint8_t register_output_n;
				uint8_t data_dec;
				
				if(n < _data_width)
				{
					//register_n = int(n/SHIFTOUT_OUTPUTS); // Lets avoid division operation:
					register_n = 0;
					register_output_n = n;
					while(register_output_n >= SHIFTOUT_OUTPUTS)
					{
						register_n = register_n + 1;
						register_output_n = register_output_n-SHIFTOUT_OUTPUTS;
					}

					#ifdef SHIFTOUT_USING_FREERTOS
						if(xSemaphoreTake(mutex_shout, (portTickType) 10) == pdTRUE)
						{
							_data_bin[register_n][register_output_n] = value;
             
							::digitalWrite(_pin_latch, LOW);
							for(int8_t i = _num_chips-1; i >= 0; i--)
							{
								data_dec = bin2dec(_data_bin[i], SHIFTOUT_OUTPUTS);
								shiftOut(_pin_data, _pin_clk, LSBFIRST, data_dec);
							}
							::digitalWrite(_pin_latch, HIGH);

							xSemaphoreGive(mutex_shout);
							
							write_result = 1;
						}
					#else
						_data_bin[register_n][register_output_n] = value;

						::digitalWrite(_pin_latch, LOW);
						for(int8_t i = _num_chips-1; i >= 0; i--)
						{
							data_dec = bin2dec(_data_bin[i], SHIFTOUT_OUTPUTS);
							shiftOut(_pin_data, _pin_clk, LSBFIRST, data_dec);
						}
						::digitalWrite(_pin_latch, HIGH);
						
						write_result = 1;
					#endif
				}

				return write_result;
			}

			// Get the actual value (LOW/HIGH) from one of the outputs
			int8_t actualValue(const uint8_t n)
			{
				int8_t read = -1;
				uint8_t register_n;
				uint8_t register_output_n;

				if(n < _data_width)
				{
					//register_n = int(n/SHIFTOUT_OUTPUTS); // Lets avoid division operation:
					register_n = 0;
					register_output_n = n;
					while(register_output_n >= SHIFTOUT_OUTPUTS)
					{
						register_n = register_n + 1;
						register_output_n = register_output_n-SHIFTOUT_OUTPUTS;
					}
					
					#ifdef SHIFTOUT_USING_FREERTOS
						if(xSemaphoreTake(mutex_shout, (portTickType) 10) == pdTRUE)
						{
							read = _data_bin[register_n][register_output_n];
							xSemaphoreGive(mutex_shout);
						}
					#else
						read = _data_bin[register_n][register_output_n];
					#endif
				}

				return read;
			}

			// Set to LOW a specific output
			int8_t low(const uint8_t n)
			{
				return digitalWrite(n, LOW);
			}

			// Set to HIGH a specific output
			int8_t high(const uint8_t n)
			{
				return digitalWrite(n, HIGH);
			}

			// Toggle the value of a specific outputs
			int8_t toggle(const uint8_t n)
			{
				return digitalWrite(n, !actualValue(n));
			}

			// Set to LOW all the outputs
			void lowAll()
			{
				#ifdef SHIFTOUT_USING_FREERTOS
					if(xSemaphoreTake(mutex_shout, (portTickType) 10) == pdTRUE)
					{
						for(uint8_t i = 0; i < _num_chips; i++)
							memset(_data_bin[i], LOW, SHIFTOUT_OUTPUTS);

						::digitalWrite(_pin_latch, LOW);
						for(int8_t i = _num_chips-1; i >= 0; i--)
							shiftOut(_pin_data, _pin_clk, LSBFIRST, 0);
						::digitalWrite(_pin_latch, HIGH);

						xSemaphoreGive(mutex_shout);
					}
				#else
					for(uint8_t i = 0; i < _num_chips; i++)
						memset(_data_bin[i], LOW, SHIFTOUT_OUTPUTS);

					::digitalWrite(_pin_latch, LOW);
					for(int8_t i = _num_chips-1; i >= 0; i--)
						shiftOut(_pin_data, _pin_clk, LSBFIRST, 0);
					::digitalWrite(_pin_latch, HIGH);
				#endif
			}

			// Set to HIGH all the outputs
			void highAll()
			{
				#ifdef SHIFTOUT_USING_FREERTOS
					if(xSemaphoreTake(mutex_shout, (portTickType) 10) == pdTRUE)
					{
						for(uint8_t i = 0; i < _num_chips; i++)
							memset(_data_bin[i], HIGH, SHIFTOUT_OUTPUTS);

						::digitalWrite(_pin_latch, LOW);
						for(int8_t i = _num_chips-1; i >= 0; i--)
							shiftOut(_pin_data, _pin_clk, LSBFIRST, 255);
						::digitalWrite(_pin_latch, HIGH);

						xSemaphoreGive(mutex_shout);
					}
				#else
					for(uint8_t i = 0; i < _num_chips; i++)
						memset(_data_bin[i], HIGH, SHIFTOUT_OUTPUTS);

					::digitalWrite(_pin_latch, LOW);
					for(int8_t i = _num_chips-1; i >= 0; i--)
						shiftOut(_pin_data, _pin_clk, LSBFIRST, 255);
					::digitalWrite(_pin_latch, HIGH);
				#endif
			}
			
			// Toggle all the outputs
			void toggleAll()
			{
				uint8_t data_dec;

				#ifdef SHIFTOUT_USING_FREERTOS
					if(xSemaphoreTake(mutex_shout, (portTickType) 10) == pdTRUE)
					{
						for(int8_t a = _num_chips-1; a >= 0; a--)
						{
							for(uint8_t b = 0; b < SHIFTOUT_OUTPUTS; b++)
								_data_bin[a][b] = !_data_bin[a][b];
						}
						
						::digitalWrite(_pin_latch, LOW);
						for(int8_t a = _num_chips-1; a >= 0; a--)
						{
							data_dec = bin2dec(_data_bin[a], SHIFTOUT_OUTPUTS);
							shiftOut(_pin_data, _pin_clk, LSBFIRST, data_dec);
						}
						::digitalWrite(_pin_latch, HIGH);

						xSemaphoreGive(mutex_shout);
					}
				#else
					for(int8_t a = _num_chips-1; a >= 0; a--)
					{
						for(uint8_t b = 0; b < SHIFTOUT_OUTPUTS; b++)
							_data_bin[a][b] = !_data_bin[a][b];
					}
						
					::digitalWrite(_pin_latch, LOW);
					for(int8_t a = _num_chips-1; a >= 0; a--)
					{
						data_dec = bin2dec(_data_bin[a], SHIFTOUT_OUTPUTS);
						shiftOut(_pin_data, _pin_clk, LSBFIRST, data_dec);
					}
					::digitalWrite(_pin_latch, HIGH);
				#endif
			}

			// Test all the output pins
			void test()
			{
				// Set to LOW all pins
				lowAll();
				delay(500);

				// Set to HIGH each pin
				for(uint8_t i = 0; i < _data_width; i++)
				{
					digitalWrite(i, HIGH);
					delay(500);
				}

				// Set to LOW each pin
				for(uint8_t i = 0; i < _data_width; i++)
				{
					digitalWrite(i, LOW);
					delay(500);
				}
			}

		/******************************************************************************************/

		private:
			// Attributes
			const uint8_t _num_chips;
			uint8_t _data_width;
			uint8_t _pin_data, _pin_clk, _pin_latch;
			uint8_t _data_bin[num_chips][8];
			#ifdef SHIFTOUT_USING_FREERTOS
				SemaphoreHandle_t mutex_shout;
			#endif

			// Method for convert binary to decimal
			uint8_t bin2dec(uint8_t* bin, uint8_t length)
			{
				uint8_t dec = 0;

				for(uint8_t i = 0; i < length; i++)
				{
					if(bin[i] == 1)
						dec = (dec * 2) + 1;
					else if(bin[i] == 0)
						dec = dec * 2;
				}

				return dec;
			}
	};

	/**********************************************************************************************/

#endif
