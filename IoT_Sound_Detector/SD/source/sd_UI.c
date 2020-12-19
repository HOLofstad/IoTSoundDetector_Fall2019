/*
 * sd_UI.c
 *
 * Created: 01.10.2020 16:27:30
 *  Author: hanso
 */ 

#include <stdint.h>
#include <stdio.h>
#include "../include/sd_commands.h"
#include "../include/sd_utils.h"
#include "../include/sd_driver.h"
#include "../include/sd_UI.h"
#include <avr/io.h>

// error == 0xFF implies no error. error == 55 implies error while issuing CMD55
uint8_t error_tracker = 0xFF;

uint8_t sd_write_single_block(uint16_t block_number, uint8_t *write_data, uint8_t recursive_count)
{
	// Reset error_tracker
	error_tracker = 0xFF;
	
	// Abstracts away the need for a memory address
	// The user can instead focus on individual blocks
	uint32_t address = block_number*DATA_BLOCK_SIZE;
	
	// SDC chip select enable
	sd_cs_enable();
	
	// Send single block write command
	sd_cmd24(address, write_data, &error_tracker);
	
	// If an error occurred, try again by recursively calling self
	if (error_tracker != 0xFF)
	{
		recursive_count++;
		// Recursively call self for a total of 10 times
		if (recursive_count < 10)
		{
			error_tracker = 0xFF;
			sd_write_single_block(block_number, write_data, recursive_count);
		}
		// If not successful after 10 attempts, give up and display error message
		else
		{
			sd_display_error_message(error_tracker);
			while(1);	// In case of error, suspend execution (temporary feature)
		}
	}
	
	sd_cs_disable();
	
	return error_tracker;
}

uint8_t sd_read_single_block(uint16_t block_number, uint8_t *return_array, uint8_t recursive_count)
{
	// Reset error_tracker
	error_tracker = 0xFF;
	
	// Abstracts away the need for a memory address
	// The user can instead focus on individual blocks
	uint32_t address = block_number*DATA_BLOCK_SIZE;
	
	sd_cs_enable();
	
	// Send single block read command
	sd_cmd17(address, return_array, &error_tracker, 512);
	
	// If an error occurred, try again by recursively calling self
	if (error_tracker != 0xFF)
	{
		recursive_count++;
		// Recursively call self for a total of 10 times
		if (recursive_count < 10)
		{
			error_tracker = 0xFF;
			sd_read_single_block(block_number, return_array, recursive_count);
		}
		// If not successful after 10 attempts, give up and display error message
		else
		{
			sd_display_error_message(error_tracker);
			//while(1);	// In case of error, suspend execution (temporary feature)
		}
	}
	
	sd_cs_disable();
	
	return error_tracker;
}

// Follows flowchart from elm-chan.org
uint8_t sd_init(uint8_t recursive_count)
{
	error_tracker = 0xFF;
	
	uint8_t r1_response_acmd41 = 0xFF;
	uint8_t *r7_response_cmd8;
	uint8_t *r3_response_cmd58;
	
	sd_cs_disable();
	sd_send_dummy_bytes(70);
	sd_cs_enable();
	
	sd_cmd0(&error_tracker);
	
	r7_response_cmd8 = sd_cmd8(&error_tracker);

	uint16_t temp = (r7_response_cmd8[3] << 8) | (r7_response_cmd8[4]);
	if (temp == 0x1AA)
	{
		r1_response_acmd41 = sd_acmd41(&error_tracker);
		if (r1_response_acmd41 == 0xFF)
		{
			error_tracker = 41;
		}
		r3_response_cmd58 = sd_cmd58(&error_tracker);	// Operation Condition Register (OCR)
		if ((r3_response_cmd58[4] & 0b10) == 0) // Card Capacity Status bit
		{
			sd_cmd16(DATA_BLOCK_SIZE, &error_tracker);	// Set block size to 512 bytes
		}
	}
	else if ((r7_response_cmd8[0] == 0x05) | (r7_response_cmd8[0] == 0xFF))
	{
		r1_response_acmd41 = sd_acmd41(&error_tracker);
		if (r1_response_acmd41 == 0xFF)
		{
			error_tracker = 0xFF;	// Reset error report
			sd_cmd1(&error_tracker);
		}
		sd_cmd16(DATA_BLOCK_SIZE, &error_tracker);	// Set block size to 512 bytes
	}
	else
	{
		error_tracker = 8;
	}
	
	sd_cs_disable();
	
	// If an error occurred, try again by recursively calling self
	if (error_tracker != 0xFF)
	{
		recursive_count++;
		// Recursively call self for a total of 5 times
		if (recursive_count < 5)
		{
			error_tracker = 0xFF;
			sd_init(recursive_count);	
		}
		// If not successful after 5 attempts, give up and display error message
		else
		{
			sd_display_error_message(error_tracker);
			while(1);	// In case of error, suspend execution (temporary feature)
		}
	}
	
	// Increasing SPI speed to 4MHz
	SPI0.CTRLA = SPI0.CTRLA & ~(0x06);	// Setting SPI CLK speed to F_CPU/4
	SPI0.CTRLA |= (1 << 4);		// Enable CLK2X flag, doubling the SPI clock speed	

	
	return error_tracker;
}