#include <avr/power.h>
#include <util/delay.h>
#include <math.h>
#include "SymonSign.h"

extern void blastRow(unsigned char redBuf[SIGN_WIDTH / 8], unsigned char greenBuf[SIGN_WIDTH / 8]);

// buffer, color, height, width
unsigned char screenBuf[2][2][SIGN_HEIGHT][SIGN_WIDTH / 8];
unsigned char curDispBuf; // Currently displaying buffer
unsigned char *serBufPtr;
int serRecvCnt;
unsigned char swapBuf;

void SET_PIXEL_ON(int b, int x, int y, int c) {
	screenBuf[b][c][y][x/8] |= (1 << (7 - ( x & 7 )));
}
void SET_PIXEL_OFF(int b, int x, int y, int c) {
	screenBuf[b][c][y][x/8] &= ~(1 << (7 - ( x & 7 )));
}


static inline void blinkRow()
{
	SIGNPORT = RED_CLK | RED_STROBE | RED_DATA | GREEN_STROBE | GREEN_DATA | GREEN_CLK;
	_delay_us(15.33);
	SIGNPORT = RED_CLK | RED_STROBE | RED_DATA | GREEN_DATA | GREEN_CLK;
	_delay_us(15.33);
	SIGNPORT = RED_CLK | RED_STROBE | RED_DATA | GREEN_STROBE | GREEN_DATA | GREEN_CLK;
	_delay_us(8.7);
}

int excessCount = 0;
void pollSerialAndSleep()
{
	// Set up polling timer
	OCR1A = F_CPU * (0.000375); // 375 microseconds
	TIFR1 = _BV(OCF1A); // Clear output compare flag
	TCNT1 = 0;
	TCCR1A = 0;
	TCCR1B = _BV(WGM12) | _BV(CS10); // CTC mode

	// Assert DSR
	if (!swapBuf)
		DSRPORT = 0;

	while (!(TIFR1 & _BV(OCF1A))) {
		if ((swapBuf == 0) && (UCSR0A & _BV(RXC0))) {
			unsigned char data = UDR0;
			if (serRecvCnt == -2) {
				if (data == 'S') {
					serRecvCnt = -1;
				}
			} else if (serRecvCnt == -1) {
				if (data == 'C') {
					serRecvCnt = 0;
				} else {
					serRecvCnt = -2;
				}
			} else {
				*serBufPtr++ = data;
				serRecvCnt++;

				if (serRecvCnt == sizeof(screenBuf[0])) {
					DSRPORT = DSR_PIN;
					serRecvCnt = -2;
					swapBuf = 1;
				}
			}
		}
	}

	DSRPORT = DSR_PIN; // De-assert DSR
	TCCR1B = 0; // Shut down timer
	TIFR1 = _BV(OCF1A); // Clear output compare flag
}

void drawScreen()
{
	for (int j = 0; j < SIGN_HEIGHT; j++) {
		SIGNPORT = RED_CLK | RED_DATA | GREEN_STROBE | GREEN_DATA | GREEN_CLK;
		pollSerialAndSleep();
		//_delay_us(375);

		blastRow(screenBuf[curDispBuf][0][j], screenBuf[curDispBuf][1][j]);

		_delay_us(3);
		blinkRow();

		if (j == 0) {
			SIGNPORT = RED_CLK | RED_DATA | GREEN_STROBE | GREEN_DATA | GREEN_CLK;
			_delay_us(15.33);
			SIGNPORT = RED_CLK | RED_DATA | GREEN_DATA | GREEN_CLK;
			_delay_us(15.33);
			SIGNPORT = RED_CLK | RED_DATA | GREEN_STROBE | GREEN_DATA | GREEN_CLK;;
			_delay_us(8.7);
			blinkRow();
		}
	}

	if (swapBuf) {
		serBufPtr = (unsigned char *)screenBuf[curDispBuf];
		curDispBuf ^= 1;
		swapBuf = 0;
	}
}

void setupScreenBuf()
{
	curDispBuf = 0;
	serBufPtr = (unsigned char *)screenBuf[1];
	serRecvCnt = -2;
	swapBuf = 0;
}

void setupSerial()
{
	// DSR signal
	DSRPORT = DSR_PIN;
	DSRDDR = DSR_PIN;

	// FIXME: this assumes the baud divisor will always be >= 255, however
	// this should always be the case in sane scenarios
	UBRR0 =  F_CPU / (8 * BAUD_RATE) - 1;
	UCSR0A = _BV(U2X0); // Double speed mode
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); // 8 bit chars
	UCSR0B = _BV(RXEN0); // Turn on the port
}

int main(void)
{
	clock_prescale_set(clock_div_1);
	setupScreenBuf();
	setupSerial();

	SIGNPORT = RED_CLK | RED_DATA | GREEN_STROBE | GREEN_DATA | GREEN_CLK;
	SIGNDDR = RED_CLK | RED_STROBE | RED_DATA | GREEN_STROBE | GREEN_DATA | GREEN_CLK;

	/*
	for (int y = 0; y < SIGN_HEIGHT; y++) {
		for (int x = 0; x < SIGN_WIDTH; x++) {
			SET_PIXEL_OFF(0, x, y, COLOR_RED);
			SET_PIXEL_ON(0, x, y, COLOR_GREEN);
		}
	}
	*/

	for (int y = 0; y < SIGN_HEIGHT; y++) {
		for (int x = 0; x < y; x++) {
			SET_PIXEL_ON(0, x, y, COLOR_GREEN);
		}
	}

	for (int x = 0; x < SIGN_WIDTH; x++) {
		//SET_PIXEL_ON(0, x, sin(x * 6.28 / SIGN_WIDTH) * (SIGN_HEIGHT / 2) + (SIGN_HEIGHT / 2), COLOR_RED);
		SET_PIXEL_ON(0, x, (int)(sin(x * 2 * 6.28 / SIGN_WIDTH) * (SIGN_HEIGHT / 2) + (SIGN_HEIGHT / 2)), COLOR_RED);
	}

	while (1) {
		drawScreen();
	}
}
