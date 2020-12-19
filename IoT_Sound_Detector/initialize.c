/*
 * initialize.c
 *
 * Created: 03.03.2020 12:54:01
 *  Author: hanso
 */ 

#include "include/driver_init.h"
#include "include/initialize.h"
#include "sd/include/sd_driver.h"
#include "SD/include/sd_UI.h"

void initialize(void)
{
	// System clock set at 8 MHz (16 Mhz / prescaler = 2)
	
	system_init();						// Atmel start driver init
	tca_init();
	tcb_init();
	adc0_init();						// ADC
		
	io_pin_init();
	fdevopen(CDC_USART_3_write, NULL);	// Initialize standard out
	sd_init(0);							// Initialize SD-card communication
	
	sei();								// Enable global interrupts
}


void tca_init(void)
{	
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;	// Enable Overflow interrupt
	
	TCA0.SINGLE.PER = 15;	// Period set to 15 to realize sampling rate = 8333 kHz
	
	// Enable Timer Counter A, and set prescaler = 64, F_TCA = 125 kHz
	TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm | TCA_SINGLE_CLKSEL_DIV64_gc;  
}

void tcb_init(void)
{
	TCB0.CTRLA = 0b101; // Enable timer and set clk source to TCA
}

void adc0_init(void)
{
	ADC0.CTRLC = ADC_PRESC_DIV16_gc | ADC_SAMPCAP_bm;
	ADC0.MUXPOS = ADC_MUXPOS_AIN2_gc;
	ADC0.SAMPCTRL = 19;
	//ADC0.EVCTRL = ADC_STARTEI_bm;
	ADC0.INTCTRL = ADC_RESRDY_bm;
	ADC0.CTRLA = ADC_ENABLE_bm | ADC_RESSEL_10BIT_gc;

	// Disable digital input buffers on analog input
	PORTD.PIN2CTRL = PORT_ISC_INPUT_DISABLE_gc;

	// Configure ADC0 voltage reference
	VREF.CTRLA = VREF_ADC0REFSEL_2V5_gc;

	ADC0.INTCTRL = 0x01;	// RESRDY interrupt enable

}

//void spi_init()
//{
	//PORTA.DIR |= PIN4_bm | PIN6_bm;		// MOSI and SCK
	//PORTC.DIR |= PIN6_bm;	// Chip select
	//
	//SPI0.CTRLA = 0 << SPI_CLK2X_bp     /* Enable Double Speed: disabled */
	//| 0 << SPI_DORD_bp    /* Data Order Setting: disabled */
	//| 1 << SPI_ENABLE_bp  /* Enable Module: enabled */
	//| 1 << SPI_MASTER_bp  /* SPI module in master mode */
	//| SPI_PRESC_DIV64_gc; /* System Clock / 64 */
//
	//SPI0.CTRLB = 1 << SPI_SSD_bp; 
//}

void io_pin_init(void)
{
	PORTF.DIR |= (1 << PIN5_bp);		// Built-in LED OUTPUT
}



