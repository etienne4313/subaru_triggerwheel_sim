/*
 * Copyright 2024, Etienne Martineau etienne4313@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <ucos_ii.h>

int debug = 1;

void osdie(int err, int line)
{
	fprintf(stderr, "DIE %d : %d\n", err, line);
	while(1){};
}

#ifdef ARDUINO_AVR_MEGA2560
/* MEGA 2560 */
// Digital PIN 36 and PIN34 ( for scope capture )
#define CFG_OUTPUT() do { \
	DDRC |= _BV(DDC1);\
	DDRC |= _BV(DDC3);\
} while(0)
#define DO_ON() do {PORTC |= (_BV(PC1) | _BV(PC3)) ; } while(0)
#define DO_OFF() do {PORTC &= ~(_BV(PC1) | _BV(PC3)) ; } while(0)

#else

/* MEGA 328 */
// PORTB / PIN 0 ( DIGITAL PIN 8 )
#define CFG_OUTPUT() do { \
	DDRB |= _BV(DDB0); \
} while(0)
#define DO_ON() do {PORTB |= _BV(DDB0) ; } while(0)
#define DO_OFF() do {PORTB &= ~_BV(DDB0) ; } while(0)
#endif

static unsigned char sub_36_2_2[360] = {
	0,0,0,0,0,0,0,0,0,0, /* Missing teeth 1 */
	0,0,0,0,0,0,0,0,0,0, /* Missing teeth 2 */
	1,1,1,1,1,0,0,0,0,0, 
	0,0,0,0,0,0,0,0,0,0, /* Missing teeth 1 */
	0,0,0,0,0,0,0,0,0,0, /* Missing teeth 2 */
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0, /* Missing teeth 1 */
	0,0,0,0,0,0,0,0,0,0, /* Missing teeth 2 */
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0, /* Cyl#1 TDC */
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
};

/* 600 RPM => 10/sec =>100msec/rotation ( 360 deg ) => 277usec/deg */
#define RPM_100 1666 /* usec/deg */
#define RPM_200 833 /* usec/deg */
#define RPM_300 554
#define RPM_400 417
#define RPM_600 277
#define RPM_1200 139
#define RPM_2400 69
#define RPM_4800 30

static OS_STK stack1[STACK_SIZE];
static void t1(void *p)
{
	int x;
	char *name = p;
	OS_CPU_SR cpu_sr = 0;

	OS_ENTER_CRITICAL();
	PRINT("******************************%s\n",name);
	OS_EXIT_CRITICAL();

	CFG_OUTPUT();
	//analogEnable();
	while(1){
		for(x=0; x<360; x++){
			if(sub_36_2_2[x])
				DO_ON();
			else
				DO_OFF();
			_delay_us(RPM_1200);
		}
	}
}

int main(void)
{
	OS_CPU_SR cpu_sr = 0;

	/* Don't let any IRQ come */
	OS_ENTER_CRITICAL();

	lib_init();
	PRINT("Entering main control loop\n");

	OSInit();

	OSTaskCreate(t1, "t1", &stack1[STK_HEAD(STACK_SIZE)], 1);

	/* IRQ are enabled when the first thread is started */
	OSStart();

	/* Never reach */
	DIE(-1);
	OS_EXIT_CRITICAL();
	return 0;
}

