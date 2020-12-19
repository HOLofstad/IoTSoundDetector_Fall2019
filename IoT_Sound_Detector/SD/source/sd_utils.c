/*
 * sd_utils.c
 *
 * Created: 30.09.2020 12:45:51
 *  Author: hanso
 */ 

#include <stdint.h>
#include <stdio.h>

uint8_t sd_assemble_r1_rsp(uint8_t *sd_return_data, uint8_t array_size)	// The R1 response might begin in the middle of a byte, and must therefore be located and extracted
{
	uint8_t start_val = 0xFF;
	uint8_t shift_counter = 0;

	for (uint8_t i = 0; i < array_size; i++)
	{
		if ((sd_return_data[i] & 0xFF) != 0xFF)	// If byte contains bit-values other than 1
		{
			start_val = sd_return_data[i];
		}
		if (start_val != 0xFF)
		{
			while ((start_val & 0b10000000) != 0x00) // All responses start with a zero. This loop locates that starting point.
			{
				start_val = (start_val << 1);
				shift_counter++;
			}
			uint8_t rsp_high_byte = start_val;
			uint8_t rsp_low_byte = (sd_return_data[i+1] >> (8-shift_counter));
			return (rsp_high_byte | rsp_low_byte);
		}
	}
	return 0xFF;
}

uint8_t * sd_assemble_r7_rsp(uint8_t *sd_return_data, uint8_t array_size)
{
	uint8_t start_val = 0xFF;
	uint8_t shift_counter = 0;
	static uint8_t r7_response[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	
	for (uint8_t i = 0; i < array_size; i++)
	{
		if ((sd_return_data[i] & 0xFF) != 0xFF)	// If byte contains bit-values other than 1
		{
			start_val = sd_return_data[i];
		}
		if (start_val != 0xFF)
		{
			while ((start_val & 0b10000000) != 0x00) // All responses start with a zero. This loop locates that starting point.
			{
				start_val = (start_val << 1);
				shift_counter++;
			}
			
			for (uint8_t j = 0; j < 5; j++)
			{
				r7_response[j] = (sd_return_data[i+j] << shift_counter) | (sd_return_data[i+j+1] >> (8-shift_counter)); 
			}
			
			return r7_response;
		}
	}
	return r7_response;
}


uint8_t * sd_assemble_r3_rsp(uint8_t *sd_return_data, uint8_t array_size)
{
	return sd_assemble_r7_rsp(sd_return_data, array_size);
}

void sd_display_error_message(uint8_t error_id)
{
	printf("Failed\n\r");
	printf("ERROR: CMD%d\n\r", error_id);
	printf("Ensure that your SD-card is properly inserted and restart the system.\n\r");
}

void split_32bit_val(uint32_t data, uint8_t *return_array)	// Takes in a 32bit value, and returns said value in 4 8bit chunks
{
	return_array[0] = (data & 0xff000000) >> 24;
	return_array[1] = (data & 0x00ff0000) >> 16;
	return_array[2] = (data & 0x0000ff00) >> 8;
	return_array[3] = (data & 0x000000ff) >> 0;
}