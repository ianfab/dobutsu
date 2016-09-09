#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "dobutsu.h"

struct gamedb {
	FILE *db_file;
};

/*
 * Open a gamedb file as generated by gendb.  Each byte contains the
 * distance to mate for the position described by the index of that
 * byte.  An odd number means a win for gote, an even number a win for
 * sente.  0xfe indicates a draw, 0xff an invalid position.
 */
extern GAMEDB*
open_gamedb(const char *path)
{
	GAMEDB *db;

	db = malloc(sizeof *db);
	if (db == NULL)
		return (NULL);

	db->db_file = fopen(path, "rb");
	if (db->db_file == NULL) {
		free(db);
		return (NULL);
	}

	return (db);
}

/*
 * Lookup pos in db and tell the distance to mate.  On draw, POS_DRAW is
 * returned.  On error, another negative number is returned and errno is
 * set to indicate the issue.
 */
extern int
distance_to_mate(GAMEDB *db, const struct position *pos, int to_move)
{
	pos_code pc;
	int distance;

	if (to_move == TURN_GOTE) {
		struct position p = *pos;
		turn_position(&p);
		pc = encode_pos_check(&p);
	} else
		pc = encode_pos_check(pos);

	if (pc == POS_SENTE)
		return (0);

	if (pc == POS_INVALID) {
		errno = EINVAL;
		return (pc);
	}

	if (fseeko(db->db_file, (off_t)pc, SEEK_SET) != 0)
		return (POS_IOERROR);

	distance = getc(db->db_file);
	switch (distance) {
	case EOF:
		if (!ferror(db->db_file))
			errno = EINVAL;

		return (POS_IOERROR);

	case 0xfe:
		return (POS_DRAW);

	case 0xff:
		/* means "position invalid," should never happen */
		errno = EINVAL;
		return (POS_IOERROR);

	default:
		return (distance);
	}
}

/*
 * Close a gamedb and release all associated resources.
 */
extern void
close_gamedb(GAMEDB *db)
{
	fclose(db->db_file);
	free(db);
}