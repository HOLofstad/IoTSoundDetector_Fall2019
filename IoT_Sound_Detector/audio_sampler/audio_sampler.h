/*
 * audio_sampler.h
 *
 * Created: 15.10.2020 16:07:08
 *  Author: hanso
 */ 


#ifndef AUDIO_SAMPLER_H_
#define AUDIO_SAMPLER_H_


void start_sampling();

void stop_sampling();

int16_t * return_sampled_signal();

uint8_t new_sample_batch_ready_bool();


#endif /* AUDIO_SAMPLER_H_ */