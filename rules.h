/*-
 * Copyright (c) 2016--2017 Robert Clausecker. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#ifndef RULES_H
#define RULES_H

/*
 * This header contains the data structures for Doubutsu Shougi position
 * as well as functions to manipulate such a position.  The interface is
 * designed to be somewhat generic in an attempt to reduce the number of
 * changes needed to implement other Shougi variants.  Neverthless it is
 * also designed to perform well, speeding up the table base building as
 * much as possible without sacrificing performance.
 */

#include <stddef.h>

enum {
/*
 * There are eight pieces in Doubutsu Shougi, two of each: lions,
 * giraffes, elephants, and chicks.  Only ownership of the
 * lions is fixed (to Sente and Gote), the other pieces can change
 * owner.  The _S and _G suffixes are to be understood as a mean of
 * distinguishing the two pieces of the same kind in code.
 */
	CHCK_S, CHCK_G,
	GIRA_S, GIRA_G,
	ELPH_S, ELPH_G,
	LION_S, LION_G,

/* see below for explanation of these constants */
	PIECE_COUNT = 8,
	SQUARE_COUNT = 12,
	IN_HAND = 12,
	GOTE_PIECE = 16,

/* the maximal amount of moves that can exist in any given position */
	MAX_MOVES = 40,    /* e.g. for S/---/-L-/--l/-R-/CGGEE */
	MAX_UNMOVES = 77,  /* e.g. for S/---/gel/---/-rL/cge */

/* various buffer lengths */
	MAX_RENDER = 100, /* guess */
	MAX_POSSTR = 25,  /* e.g. S/L--/--l/---/---/ccggee */
	MAX_MOVSTR = 8,   /* e.g. Cb2xb3+ */
	MAX_MOVDSC = 100, /* guess */	

/*
 * Status bits for struct position.  For the promotion status, 0
 * indicates an unpromoted chick, 1 indicates a promoted chick (i.e. a
 * rooster).  For move status, 0 indicates Sente to move, 1 indicates
 * Gote to move.  The indices of these bits have been carefully chosen
 * to enable some optimizations:  One promotion bit is reserved for each
 * piece to simplify the handling of promotions.
 */
	ROST_S =     1 <<  CHCK_S,     /* is CHCK_S a rooster? */
	ROST_G =     1 <<  CHCK_G,     /* is CHCK_G a rooster? */
	GOTE_MOVES = 1 <<  8,          /* is it Gote's move? */
	POS_FLAGS = GOTE_MOVES /* all flags */
};

/*
 * A bitmap representing the current status of the board.  This type
 * is not part of the abstract interface but it is needed for the
 * position type.
 */
typedef unsigned board;

/*
 * A position is described as a vector of where pieces are and some
 * status bits.  Each piece position is a number between 0 and
 * SQUARE_COUNT, with SQUARE_COUNT (== IN_HAND) indicating that the
 * piece is not on the board.  To this, GOTE_PIECE (a power of two) is
 * added if the piece is owned by Gote.  This makes enumerating occupied
 * squares separated by Sente and Gote occupying them easy.  The status
 * bits are explained in the enumeration above.
 */
struct position {
	unsigned char pieces[PIECE_COUNT];
	unsigned status;
	unsigned map;
};

/*
 * This macro expands to the initial board setup.  The setup has the
 * position string S/gle/-c-/-C-/ELG/- and looks like this:
 *
 *      ABC 
 *     +---+
 *    1|gle| 
 *    2| c |
 *    3| C |
 *    4|ELG| *
 *     +---+
 */
#define INITIAL_POSITION (struct position) {\
	.pieces = { 0x04, 0x17, 0x00, 0x1b, 0x02, 0x19, 0x01, 0x1a },\
	.status = 0,\
	.map = 00027 | 07200 << GOTE_PIECE\
}

/*
 * A move is described by a piece and the square it moves from/to.  The
 * constant GOTE_PIECE must be added to both squares if the move is
 * performed by Gote.
 */
struct move {
	unsigned piece, to;
};

/*
 * An unmove describes what needs to be done to undo a move.  This is
 * used for retrograde analysis when building the endgame table.  The
 * field capture contains the piece number of a piece that was captured
 * by the move to be undone.  If no piece was captured, the capture
 * field is filled with -1.  The status field contains promotion bits to
 * be flipped and must be xored into status.  The GOTE_MOVES bit is not
 * set in status.
 */
struct unmove {
	unsigned piece, from, status;
	int capture;
};

/*
 * the following functions perform common operations on positions and
 * moves.  Those functions that update a position do so in-place.  Make
 * a copy beforehand if you need the old position.  See the
 * implementations of these functions for documentation.
 */

/* validation */
extern		int	position_valid(const struct position*);
extern		int	move_valid(const struct position*, const struct move*);

/* status information */
static inline	int	gote_moves(const struct position*);
extern		int	sente_in_check(const struct position*);
extern		int	gote_in_check(const struct position*);
extern		int	position_equal(const struct position*, const struct position*);

/* board modification */
extern		int	play_move(struct position*, const struct move*);
extern		void	undo_move(struct position*, const struct unmove*);
static inline	void	null_move(struct position*);

/* move generation */
extern		size_t	generate_moves(struct move[MAX_MOVES], const struct position*);
extern		size_t	generate_unmoves(struct unmove[MAX_UNMOVES], const struct position*);

/* display */
extern		void	position_render(char[MAX_RENDER], const struct position*);
extern		void	position_string(char[MAX_POSSTR], const struct position*);
extern		void	move_string(char[MAX_MOVSTR], const struct position*, const struct move*);

/* parsing, returns 0 on success, -1 on failure */
extern		int	parse_position(struct position*, const char[MAX_POSSTR]);
extern		int	parse_move(struct move*, const struct position*, const char[MAX_MOVSTR]);

/*
 * Some of these functions are implemented inline for performance.
 * Their implementations can be found below.
 */

/*
 * Return 1 if gote moves in p, 0 otherwise return 1.
 */
static inline
int gote_moves(const struct position *p)
{

	return !!(p->status & GOTE_MOVES);
}

/*
 * Play a null move, that is, flip the bit indicating whose turn it is.
 */
static inline
void null_move(struct position *p)
{

	p->status ^= GOTE_MOVES;
}

#endif /* RULES_H */
