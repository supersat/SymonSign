/*
 * BlastProcessing.s
 *
 * Created: 12/2/2012 12:30:38 AM
 *  Author: supersat
 */

#define __SFR_OFFSET 0
#include "SymonSign.h"
.section .text
.global blastRow

.macro blastBitClkDelay1
	nop
	nop
	nop
	nop
	nop
.endm

.macro blastBitClkDelay2
	nop
	nop
	nop
	nop
.endm

.macro blastBit bit
	ldi r22, (RED_CLK | GREEN_STROBE | GREEN_CLK)
	sbrs r20, \bit
	ori r22, RED_DATA
	sbrs r21, \bit
	ori r22, GREEN_DATA

	out SIGNPORT, r22
	blastBitClkDelay1
	andi r22, ~(RED_CLK | GREEN_CLK)
	out SIGNPORT, r22
	blastBitClkDelay2
.endm

blastRow:
    movw r26, r24
	movw r30, r22

	ldi r19, (SIGN_WIDTH / 8)
col_loop:
	ld r20, X // Load red
	ld r21, Z // Load green

	blastBit 7
	blastBit 6
	blastBit 5
	blastBit 4
	blastBit 3
	blastBit 2
	blastBit 1
	blastBit 0

	// Increment framebuffer pointers
	adiw r26, 1
	adiw r30, 1

	dec r19
	breq blastRowDone
	rjmp col_loop
blastRowDone:
	ret
