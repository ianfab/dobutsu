Dōbutsu shōgi
=============

This is an engine for the Japanese chess variant
[Dōbutsu shōgi]&#x20;(動物将棋).  It uses a comprehensive endgame table
base to achieve perfect play from any position.

Installation
============

On a UNIX-like operating system, you should be able to compile the
software by typing

    make all

into your terminal.  You might need to adjust the CC variable in case
your system doesn't have a `c99` binary.  On most Linux systems,
`make CC="gcc -std=c11"` should work.  Then type

    make dobutsu.tb.xz

to generate the compressed endgame tablebase.  This may take a while but
you only need to do it once.  Finally, type

    make PREFIX=... install

to install the software into the provided `PREFIX`, defaulting to
`/usr/local` if not provided.

Usage
=====

Once installed, you can start the program by typing `dobutsu` into your
terminal.  You can play moves by typing them.  The following commands
are supported, most of which are similar to those in GNU Chess:

    help        Print a list of commands
    hint        Print what the engine would play
    exit        Quit the program
    version     Print program version
    new         Start a new game
    undo        Undo previous move
    remove      Undo last two moves
    setup       Setup board with specified position string
    show board  Print the current board
    show moves  Print all possible moves
    show eval   Print position evaluation
    show lines  Print all possible moves and their evaluations
    strength    Show/set engine strength
    both        Make engine play both players
    go          Make the engine play the colour that is on the move
    force       Set the engine to play neither colour
    verbose     Print the board after every move
    quiet       Do not print the board after every move

For more details, see **dobutsu**(6).

Rules
=====

Dōbutsu shōgi is played on a board with four ranks 1–4 and three files
A–C.  The farthest rank for each player is called the *promotion zone.*
The player whose pieces start out on ranks 1 and 2 is called *Sente* and
makes the first move.  The other player is called *Gote.*

      ABC
     +---+
    1|gle|
    2| c |
    3| C |
    4|ELG| *
     +---+

Each player starts out with four pieces, one *lion,* one *elephant,* one
*giraffe,* and one *chick.*  The goal of the game is to either capture
the opponents lion or to advance one's own lion into the promotion zone
without moving into check.  In the software, pieces owned by Sente are
represented by uppercase `L`, `E`, `G`, `C`, and `R`; pieces owned by
Gote are represented by lowercase `l`, `e`, `g`, `c`, and `r`.  An
asterisk `*` denotes whose move it is.

The pieces move like this:

* The lion can move one square into every direction (like the king in
  international chess)
* the elephant can move one square diagonally
* the giraffe can move one square horizontally or vertically
* the chick can move one square forwards (towards the promotion zone)
* a rooster can move like a giraffe or one square diagonally forward

All pieces capture the same way they move.  A chick is promoted to a
*rooster* once it enters the promotion zone.  Instead of moving a piece,
a captured piece can be *dropped* into any vacant square.  A captured
rooster loses its promotion when dropped.

[Dōbutsu shōgi]: https://en.wikipedia.org/wiki/D%C5%8Dbutsu_sh%C5%8Dgi

License
=======

Copyright © 2016–2017 Robert Clausecker. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

**THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS “AS IS” AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.**
