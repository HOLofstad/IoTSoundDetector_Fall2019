/*
 * sd_driver.c
 *
 * Created: 16.09.2020 18:57:02
 *  Author: hanso
 */ 

#include "../include/spi_basic.h"
#include "../include/sd_driver.h"
#include <stdio.h>
#include <avr/io.h>
#include <stdint.h>

#define DUMMY_BYTE 0xFF
#define CMD_SIZE 6

uint8_t cs_enabled = 0;

uint8_t sd_receive_byte()
{
	// Receive a single data byte. Send dummy byte (0xFF) in return. 
	uint8_t data = SPI_0_exchange_byte(DUMMY_BYTE);
	return data;
}

void sd_receive_bytes(uint8_t *return_array, uint16_t n_bytes)
{
	// Receive bytes, one by one. Send dummy byte (0xFF) in return. 
	for (uint16_t i = 0; i < n_bytes; i++)
	{
		return_array[i] = SPI_0_exchange_byte(DUMMY_BYTE);
	}
}

void sd_send_byte(uint8_t data)
{
	// Send data byte to SDC, and discard data received in data exchange
	SPI_0_exchange_byte(data);
}

void sd_send_bytes(uint8_t *data_array, uint16_t n_bytes)
{
	// Send multiple data bytes to SDC, and discard data received in data exchange
	for (uint16_t i = 0; i < n_bytes; i++)
	{
		SPI_0_exchange_byte(data_array[i]);
	}
}

// SDCs have a fixed command size of 6 bytes
void sd_send_cmd(uint8_t *cmd)
{
	for (uint8_t i = 0; i < CMD_SIZE; i++)
	{
		SPI_0_exchange_byte(cmd[i]);
	}
}

void sd_send_dummy_bytes(uint16_t n_bytes)
{
	for (uint16_t i = 0; i < n_bytes; i++)
	{
		SPI_0_exchange_byte(DUMMY_BYTE);
	}
}

void sd_cs_enable()
{
	PORTC.DIR |= (1 << 6);	// SD-module SPI chip select (CS) pin as output
	PORTC.OUT &= !(1 << 6);	// Pull CS pin low to enable chip	
	cs_enabled = 1;	// Variable to keep track of enable/disable status
}

void sd_cs_disable()
{
	PORTC.DIR |= (1 << 6);	// SD-module SPI chip select pin as output
	PORTC.OUT |= (1 << 6);	// Pull CS pin high to disable chip
	cs_enabled = 0;	// Variable to keep track of enable/disable status
}

