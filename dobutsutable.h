#include "tablebase.h"
#include "dobutsu.h"

/*
 * This header contains the low-level interface to the endgame tablebase
 * as well as prototypes for functions used to implement it.
 */

/*
 * the endgame tablebase is organized on four levels:
 *  1. by the cohort, that is, which pieces are on the board
 *  2. by the position of the lions on the board
 *  2. by the position of the other pieces on the board
 *  3. by piece ownership.
 *
 * For each cohort, there is a an array containing the distance-to-mate
 * (dtm) information for this cohort.  The index into this array is
 * is computed by multiplying lion position, map, and piece ownership
 * into one offset.  Separate arrays for each cohort are needed because
 * depending on how many pieces are on the board and which of the pieces
 * are distinguishable, the size of the cohort changes.
 *
 * The following constants describe how many of each encoding level
 * exist.
 */
enum {
	COHORT_COUNT = 63,
	LIONPOS_COUNT = 21,
	LIONPOS_TOTAL_COUNT = 41,
	OWNERSHIP_COUNT = 64,

	/* total positions in the table base */
	POSITION_COUNT = 255280704,

	MAX_PCALIAS = 16,
};

/*
 * cohort_table contains information for each cohort.  The following
 * information is stored:
 *
 *  - the chicken promotion bits
 *  - how many of each kind of piece there are
 *  - how large the encoding space for each piece group is (ignoring
 *    the lions for which the space is always LIONPOS_TOTAL_COUNT).
 *
 * One byte of padding is added to make each entry eight bytes long.
 */
extern const struct cohort_info {
        unsigned char pieces[3]; /* 0: chicks, 1: giraffes, 2: elephants */
        unsigned char status; /* only promotion bits are set */
        unsigned char sizes[3];
        unsigned char padding;
} cohort_info[COHORT_COUNT];

/*
 * cohort_size contains size information for each cohort.  The following
 * information is stored:
 *
 * - the offset of the beginning of data for that cohort in the tablebase
 * - the size of that cohort.
 *
 * this table is separate from cohort_info so that a record in each
 * table is 8 bytes long, allowing an indexed addressing mode to be used
 * on x86.
 */
extern const struct cohort_size {
	unsigned offset, size;
} cohort_size[COHORT_COUNT];

/*
 * The tablebase struct contains a complete tablebase. It is essentially
 * just a huge array of positions.
 */
struct tablebase {
	signed char positions[POSITION_COUNT];
};

extern		size_t			poscode_aliases(poscode[MAX_PCALIAS], const struct position*);
static inline	size_t			position_offset(poscode);

/* inline functions */

/*
 * position_offset() returns the offset of pc in the tablebase.  It is
 * assumed that pc represents a valid position code that is in the table
 * base (i.e. with lionpos < LIONPOS_TOTAL).
 */
static inline size_t
position_offset(poscode pc)
{
	size_t index;

	index = cohort_size[pc.cohort].offset;
	index += cohort_size[pc.cohort].size * (pc.lionpos * OWNERSHIP_COUNT + pc.ownership);
	index += pc.map;

	return (index);
}
