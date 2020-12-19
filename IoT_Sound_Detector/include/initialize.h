/*
 * initialize.h
 *
 * Created: 03.03.2020 12:54:16
 *  Author: hanso
 */ 


#ifndef INITIALIZE_H_
#define INITIALIZE_H_


void initialize(void);

void tca_init(void);

void tcb_init(void);

void rtc_init(void);

void adc0_init(void);

void io_pin_init(void);

void spi_init(void);


#endif /* INITIALIZE_H_ */