/*------------------------------------------------------------------------------
 * 4190.308 Computer Architecture                                    Spring 2020
 *
 * Bomb Lab
 *
 * Handout: May  6, 2020
 * Due:     May 20, 2020 11:00
 *
 * The binary bomb notifies the bomblab server automatically when you solve a
 * phase. You can look at the current scoreboard at
 *   https://csap.snu.ac.kr/comparch/bomblab/scoreboard
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "support.h"
#include "phases.h"

FILE *infile;

int main(int argc, char *argv[])
{
    char *input;

    // When run with no arguments, the bomb reads its input lines from standard
    // input.
    if (argc == 1) infile = stdin;

    // When run with one argument <file>, the bomb reads from <file> until EOF,
    // and then switches to standard input. Thus, as you defuse each phase, you
    // can add its defusing string to <file> and avoid having to retype it.
    else if (argc == 2) {
      if (!(infile = fopen(argv[1], "r"))) {
        printf("%s: Error: Couldn't open %s\n", argv[0], argv[1]);
        exit(8);
      }
    }

    // You can't run the bomb with more than one command line argument.
    else {
      printf("Usage: %s [<input_file>]\n", argv[0]);
      exit(8);
    }

    // Do all sorts of secret stuff that makes the bomb harder to defuse.
    initialize_bomb();

    printf("Welcome to my fiendish little bomb. You have 6 phases with\n");
    printf("which to blow yourself up. Have a nice day!\n");

    // first phase
    input = read_line();                  // get input
    phase_1(input);                       // run the phase
    phase_defused();                      // phase_1 only comes back when defused
    printf("Phase 1 defused. How about the next one?\n");

    // second phase
    input = read_line();
    phase_2(input);
    phase_defused();
    printf("That's number 2.  Keep going!\n");

    // third phase
    input = read_line();
    phase_3(input);
    phase_defused();
    printf("Halfway there!\n");

    // fourth phase
    input = read_line();
    phase_4(input);
    phase_defused();
    printf("So you got that one.  Try this one.\n");

    // fifth phase
    input = read_line();
    phase_5(input);
    phase_defused();
    printf("Good work!  On to the next...\n");

    // sixth phase
    input = read_line();
    phase_6(input);
    phase_defused();

    // is that really all?

    return 0;
}
