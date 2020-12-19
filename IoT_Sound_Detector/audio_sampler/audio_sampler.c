/*
 * interrupt_test.c
 *
 * Created: 15.10.2020 15:35:26
 *  Author: hanso
 */ 

#include <avr/interrupt.h>
#include "fht/fhtConfig.h"
#include "audio_sampler.h"

uint8_t enable_sampling = 0;
int16_t sampled_signal[FHT_LEN];
volatile uint8_t samples_rdy;
uint16_t number_of_samples = FHT_LEN;	// Currently set to 128


void start_sampling()
{
	enable_sampling = 1;	// On/Off switch for sampler module
}

void stop_sampling()
{
	enable_sampling = 0;
}

int16_t * return_sampled_signal()
{
	while(samples_rdy == 0);
	samples_rdy = 0;
	return sampled_signal;	
}

uint8_t new_sample_batch_ready_bool()
{
	if (samples_rdy)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

ISR(TCA0_OVF_vect)
{
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
	ADC0.COMMAND = 1;	// Start ADC conversion
}

ISR(ADC0_RESRDY_vect)
{
	if (enable_sampling)
	{
		static uint8_t sample_counter = 0;
		
		// The 10-bit ADC results are subtracted 669 to be centered around 0.
		// It is then scaled by a factor of 32 giving it a maximum range of [-16384, 16384]
		// This is a requirement from the FHT algorithm
		
		sampled_signal[sample_counter] = (ADC0.RES-669) * 32;
		
		// Report batch ready and hold off sampling for 0.1 seconds
		if (sample_counter == number_of_samples)
		{
			samples_rdy  = 1;
			TCA0.SINGLE.PER = 12500;	// Wait 0.1 seconds
		}
		
		// Resume sampling after hold-off
		if (sample_counter == 0)
		{
			TCA0.SINGLE.PER = 15;
		}
		
		// Update sample counter
		sample_counter++;
		sample_counter %= (number_of_samples + 1);
	}
	else
	{
		uint8_t adc_res  = ADC0.RES;
		if (adc_res > 200)	// Tweak this threshold if microphone gain is modified
		{
			enable_sampling = 1;
		}
	}
}
