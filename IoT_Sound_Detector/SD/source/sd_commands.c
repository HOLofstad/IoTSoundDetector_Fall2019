/*
 * sd_commands.c
 *
 * Created: 30.09.2020 12:34:07
 *  Author: hanso
 */ 

#include "../include/sd_driver.h"
#include "../include/sd_utils.h"
#include "../include/sd_commands.h"
#include <stdio.h>

#define DUMMY_BYTE 0xFF
#define CMD_SIZE 6


uint8_t cmd0[6] = {0x40, 0x00, 0x00, 0x00, 0x00, 0x95}; //  Last byte is CRC (This is only necessary for CMD0 and CMD8. Replace with dummy byte otherwise)
uint8_t cmd1[6] = {0x41, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t cmd8[6] = {0x48, 0x00, 0x00, 0x01, 0xAA, 0x87};
uint8_t cmd55[6] = {0x77, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t cmd58[6] = {0x7A, 0x00, 0x00, 0x00, 0x00, 0x75};
uint8_t acmd41[6] = {0x69, 0x00, 0x00, 0x00, 0x00, 0x00};	// CMD55 needs to be sent before sending ACMD41
	
uint8_t sd_version = 0;		// This variable keeps track of which version the inserted SD-card is (1 or 2)

// Software reset
uint8_t sd_cmd0(uint8_t *error)
{
	uint8_t success = 0;
	uint8_t n_bytes = 2;
	uint8_t return_data[n_bytes];
	uint8_t response = 0xFF;
	uint8_t iter_cnt = 0;
	
	while (!success)
	{
		iter_cnt++;
		sd_send_cmd(cmd0);
		sd_receive_bytes(return_data, n_bytes);
		response = sd_assemble_r1_rsp(return_data, n_bytes);
		if (response == 0x01)
		{
			success = 1;
		}
		else if (iter_cnt == 5)	// For error handling purposes
		{
			error[0] = 0;
			return response;
		}
	}	
	return response;
}

// Initiate initialisation process
uint8_t sd_cmd1(uint8_t *error)
{
	uint8_t success = 0;
	uint8_t n_bytes = 5;
	uint8_t return_data[n_bytes];
	uint8_t response = 0xFF;
	uint8_t iter_cnt = 0;
	
	while (!success)
	{
		iter_cnt++;
		sd_send_cmd(cmd1);
		sd_receive_bytes(return_data, n_bytes);
		response = sd_assemble_r1_rsp(return_data, n_bytes);
		if (response == 0x00)
		{
			success = 1;
		}
		else if (iter_cnt == 5)	// For error handling purposes
		{
			error[0] = 1;
			return response;
		}
	}
	return response;
}

// Check voltage range
uint8_t * sd_cmd8(uint8_t *error)
{
	uint8_t n_bytes = 10;
	uint8_t return_data[n_bytes];
	uint8_t success = 0;
	uint8_t * response;
	uint8_t iter_cnt = 0;
	
	while (!success)
	{
		iter_cnt++;
		sd_send_cmd(cmd8);
		sd_receive_bytes(return_data, n_bytes);
		response = sd_assemble_r7_rsp(return_data, n_bytes);
		if (response[0] == 0x01)
		{
			success = 1;
			sd_version = 2;
		}
		else if (response[0] == 0x05)
		{
			success = 1;
			sd_version = 1;
		}
		else if (iter_cnt == 5)	// For error handling purposes
		{
			error[0] = 8;
			return response;
		}
	}
	
	return response;
}

// Change read/write block size
uint8_t sd_cmd16(uint32_t block_size, uint8_t *error)
{
	uint8_t success = 0;
	uint8_t n_bytes = 5;
	uint8_t return_data[n_bytes];
	uint8_t response = 0xFF;
	uint8_t iter_cnt = 0;
	
	uint8_t chunks[4];
	split_32bit_val(block_size, chunks);
	uint8_t cmd16[6] = {0x50, chunks[0], chunks[1], chunks[2], chunks[3], 0x00};
	
	while (!success)
	{
		iter_cnt++;
		sd_send_cmd(cmd16);
		sd_receive_bytes(return_data, n_bytes);
		response = sd_assemble_r1_rsp(return_data, n_bytes);
		if (response == 0x00)
		{
			success = 1;
		}
		else if (iter_cnt == 5)	// For error handling purposes
		{
			error[0] = 16;
			return response;
		}
	}
	return response;
}

// Read a block
void sd_cmd17(uint32_t address, uint8_t *return_array, uint8_t *error, uint16_t data_block_size)
{
	uint8_t success = 0;
	uint8_t awaiting_response = 1;
	volatile uint8_t response_temp[2] = {0xFF, 0xFF};
	uint8_t outer_loop_counter = 0;

	uint8_t address_chunks[4];
	split_32bit_val(address, address_chunks);	// Address must be divisible with block size
	
	uint8_t cmd17[6] = {0x51, address_chunks[0], address_chunks[1], address_chunks[2], address_chunks[3], 0x00};
	
	while(!success)
	{
		sd_send_dummy_bytes(100);
		sd_send_cmd(cmd17);		
		
		while(awaiting_response)	// While waiting for R1-response
		{
			static uint8_t r1_timeout = 0;
			r1_timeout++;
			awaiting_response = (r1_timeout > 10) ? 0 : 1;	// Exit loop if timeout value reached
			
			response_temp[0] = sd_receive_byte();
			
			if (response_temp[0] == 0x00)
			{
				awaiting_response = 0;
			}
		}
		awaiting_response = 1;
		while(awaiting_response)	// While waiting for data from SD-card
		{
			static uint8_t data_timeout = 0;
			data_timeout++;
			awaiting_response = (data_timeout > 300) ? 0 : 1;	// Exit loop if timeout value reached
			
			response_temp[0] = sd_receive_byte();
			if (response_temp[0] == 0xFE)	// If data token received
			{
				sd_receive_bytes(return_array, data_block_size);
				do 
				{
					response_temp[0] = sd_receive_byte();
				} while (response_temp[0] == 0x00);
				sd_send_dummy_bytes(10);
				success = 1;
				awaiting_response = 0;
			}
			else if ((response_temp[0] != 0xFF) && (response_temp[0] != 0x00))	// If response is something other than NO_DATA (0xFF) and BUSY_FLAG (0x00)
			{
				awaiting_response = 0;
			}
		}
		if (!success)	
		{
			if (outer_loop_counter > 5)	// Timeout
			{
				success = 1;
				error[0] = 17;
			}
			outer_loop_counter++;	
		}
	}
}

// Write a block
void sd_cmd24(uint32_t address, uint8_t *data_block, uint8_t *error)
{
	uint8_t success = 0;
	uint8_t awaiting_response = 1;
	uint8_t response_temp[2] = {0xFF, 0xFF};
	uint8_t outer_loop_counter = 0;
	
	uint8_t address_chunks[4];
	split_32bit_val(address, address_chunks);	// Address must be divisible with block size
	
	uint8_t cmd24[6] = {0x58, address_chunks[0], address_chunks[1], address_chunks[2], address_chunks[3], 0x00};
	
	while(!success)
	{
		sd_send_dummy_bytes(20);
		sd_send_cmd(cmd24);
		
		while(awaiting_response)	// While waiting for R1-response
		{
			static uint8_t r1_timeout = 0;
			r1_timeout++;
			awaiting_response = (r1_timeout > 50) ? 0 : 1;	// Exit loop if timeout value reached
			
			response_temp[0] = sd_receive_byte();
			
			if (response_temp[0] == 0x00)
			{
				awaiting_response = 0;
			}
		}
	
		sd_send_dummy_bytes(3);
		sd_send_byte(0xFE);		// Send data token
		sd_send_bytes(data_block, 512);	// Data block
		sd_send_dummy_bytes(2);	// Send dummy bytes for CRC
		
		awaiting_response = 1;
		while(awaiting_response)
		{
			static uint8_t data_response_timeout = 0;
			data_response_timeout++;
			awaiting_response = (data_response_timeout > 254) ? 0 : 1;	// Exit loop if timeout value reached
			
			response_temp[0] = sd_receive_byte();
			
			if ((response_temp[0] != 0xFF) && (response_temp[0] != 0x00))	// if not NO_DATA and BUSY_FLAG
			{				
				if ((response_temp[0] & 0b10001) == 0x01)	// Check if byte is DataResponse-byte
				{
					if ((response_temp[0] & 0b111) == 0b101)	// Data accepted
					{
						success = 1;
						awaiting_response = 0;
					}
				}
			}
		}
						
		if (!success)
		{			
			if (outer_loop_counter > 5)	// Timeout
			{
				success = 1;
				error[0] = 24;
			}
			outer_loop_counter++;
		}
	}
}

// Alternative initiate initialisation process (Only for SDC)
uint8_t sd_acmd41(uint8_t *error)
{
	uint8_t success = 0;
	uint8_t n_bytes = 5;
	uint8_t return_data[n_bytes];
	uint8_t response = 0xFF;
	uint16_t iter_cnt = 0;
	
	sd_cmd55(error);	// CMD55 needs to be sent before sending ACMD41
	while (!success)
	{
		iter_cnt++;
		sd_send_cmd(acmd41);
		sd_receive_bytes(return_data, n_bytes);
		response = sd_assemble_r1_rsp(return_data, n_bytes);

		if (response == 0x00)
		{
			success = 1;
		}
		if (iter_cnt == 800)	// For error handling purposes
		{
			error[0] = 41;
			return 0xFF;
		}
	}
	return response;
}

// Must send this command before ACMD<n> commands
uint8_t sd_cmd55(uint8_t *error)
{
	uint8_t success = 0;
	uint8_t n_bytes = 5;
	uint8_t return_data[n_bytes];
	uint8_t response = 0xFF;
	uint8_t iter_cnt = 0;
	
	while (!success)
	{
		iter_cnt++;
		sd_send_cmd(cmd55);
		sd_receive_bytes(return_data, n_bytes);
		response = sd_assemble_r1_rsp(return_data, n_bytes);
		if (response == 0x01)
		{
			success = 1;
		}
		else if (iter_cnt == 5)	// For error handling purposes
		{
			error[0] = 55;
			return response;
		}
	}
	return response;
}

// Read Operation Conditions Register (OCR)
uint8_t * sd_cmd58(uint8_t *error)
{
	uint8_t n_bytes = 10;
	uint8_t return_data[n_bytes];
	uint8_t success = 0;
	uint8_t * response;
	uint8_t iter_cnt = 0;
	
	while (!success)
	{
		iter_cnt++;
		sd_send_cmd(cmd58);
		sd_receive_bytes(return_data, n_bytes);
		response = sd_assemble_r3_rsp(return_data, n_bytes);
		if (response[0] == 0x00)
		{
			success = 1;
		}
		else if (iter_cnt == 5)	// For error handling purposes
		{
			error[0] = 58;
			return response;
		}
	}
	
	return response;
}



