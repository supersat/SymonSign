/*
 * SymonSign.h
 *
 * Created: 12/2/2012 1:04:26 AM
 *  Author: supersat
 */


#ifndef SYMONSIGN_H_
#define SYMONSIGN_H_

#include <avr/io.h>

#define SIGNPORT			PORTB
#define SIGNDDR				DDRB

#define DSRPORT				PORTD
#define DSRDDR				DDRD
#define DSR_PIN				_BV(6)

#define RED_CLK_PIN			0
#define RED_STROBE_PIN		1
#define RED_DATA_PIN		2
#define GREEN_STROBE_PIN	3
#define GREEN_DATA_PIN		4
#define GREEN_CLK_PIN		5

#define RED_CLK				_BV(RED_CLK_PIN)
#define RED_STROBE			_BV(RED_STROBE_PIN)
#define RED_DATA			_BV(RED_DATA_PIN)
#define GREEN_STROBE		_BV(GREEN_STROBE_PIN)
#define GREEN_DATA			_BV(GREEN_DATA_PIN)
#define GREEN_CLK			_BV(GREEN_CLK_PIN)

#define SIGN_WIDTH			200
#define SIGN_HEIGHT			16
#define COLOR_GREEN			1
#define COLOR_RED			0

#define BAUD_RATE			2000000

#endif /* SYMONSIGN_H_ */
