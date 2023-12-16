
#include "game.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <raylib.h>

#define LINE "\n\n\n\n"

void undertale(GameState* state, int dialog) {
	state->station.stationState = DIALOG;
	writeToCharArr("\0", SSDP.displayMessage, -1);
	state->station.dialogue.num = dialog;
	state->station.dialogue.readyToAdvance = false;
	state->station.dialogue.chars = 0;
	switch(dialog) {
		// DEBUGGING DIALOGUE
		case 1:
			DIALOG(SPEAKER_NARRATOR, "test narrator", true);
			break;
		case 2:
			transitionToStationLevel(state, TEST_LEVEL_TRANSITION);
			PSETPLAYERPOS(0);
			ENDDIALOGUEP;
			break;
		case 5:
			DIALOG("You", "Tis just a big brick of shit...\n\n\n\n"
									 "Nothing special about it.", true);
			state->station.dialogue.num = 10 - 1;
			break;
		case 6: 
			DIALOG(SPEAKER_NARRATOR, "You go through the door.", true);
			break;
		case 7:
			transitionToStationLevel(state, TEST_LEVEL);
			PSETPLAYERPOS(-25);
			ENDDIALOGUEP;
			break;
		case 8:
			DIALOG(SPEAKER_NARRATOR, "You go through the airlock and board the ship.", true);
			break;
		case 9:
			transitionToShip(state, 2);
			break;
		case 10:
			DIALOG("You", "It also smells.                    " LINE
									"Like, really bad.", false);
			break;


			//STORY DIALOG 
			
		// case num formatting: X = day num, XX = room num (see level enum), XXX = dialog num
		// jumps in the XXX by a 100 indicate a new dialog-line
		case 1'00'000:
			DIALOG(SPEAKER_NARRATOR, "I remember the promo video for this program" LINE
									"like the back of my hand, and it goes for" LINE "everyone else.", true);
			break;
		case 1'00'001:
			DIALOG(SPEAKER_NARRATOR, "Think of the most unconvincing advertisement " LINE
									 "for a product that will make you never buy it " LINE
									 "or the advertisers products ever again.", true);
			break;
		case 1'00'002:
			DIALOG(SPEAKER_NARRATOR, "In many cases, people would refer to this advert" LINE
									 "like hes the most ugliest man-thing in high school.", true);
			break;
		case 1'00'003:
			DIALOG(SPEAKER_NARRATOR, "Most of the promo was filled to the brim with legal" LINE
									 "disclaimers, which made it more off-putting for" LINE
									 "this program, even if it was meant to relieve debt.", true);
			break;
		case 1'00'004:
			DIALOG(SPEAKER_NARRATOR, "So why? Why after heeding all of these warnings-" LINE
									 "these red flags...                              " LINE
									 "did I ever sign up for it in the first place...?", true);
			break;
		case 1'00'005:
			transitionToStationLevel(state, PROTAG_ROOM, true, 1'01'006);
			break;
		case 1'01'006:
			PSETPLAYERPOS(80);
			DIALOG("You", "!", true);
			break;
		case 1'01'007:
			DIALOG("You", "Uhm, where am I...?", true);
			break;
		case 1'01'008:
			DIALOG(SPEAKER_NARRATOR, "*RING RING RING*", true);
			state->story.flags[0] = true;
			// day 1: flag 0: is the phone ringing? if flag 1 is on, the ringing stops.
			break;
		case 1'01'009:
			DIALOG("You", "It's coming from the phone on the wall..." LINE 
							"I probably should answer it.", false);
			break;
		case 1'01'100:
			DIALOG(SPEAKER_NARRATOR, "I pickup the phone and hold the reciever" LINE 
									 "next to my ear.", true);
			PFLAG[1] = true;
			break;
		case 1'01'101:
			DIALOG("You", "...                                              " LINE
						  "Hello?", true);
			break;
		case 1'01'102:
			DIALOG("Phone", "\"So you're the new guy that signed up, " LINE
					"am I right or am I right?\"", true);
			break;
		case 1'01'103:
			DIALOG("You", "...Yes, sir...", true);
			break;
		case 1'01'104:
			DIALOG("Phone", "\"Alright, cool. Now get yourself something to eat," LINE
							"because you gotta get shit done right about now.", true);
			break;
		case 1'01'105:
			DIALOG("Phone", "In case you are wondering and it wasn't obvious" LINE
							"to you, I'm your manager that's gonna manage you" LINE 
							"til' the day either of us dies.", true);
			break;
		case 1'01'106:
			DIALOG("You", "That's cool and all, but what *exactly* am I-", true);
			break;
		case 1'01'107:
			DIALOG("Phone (Manager)", "Quit yappin', there's a book on the table, the one" LINE 
									  "to your left. I had the previous \"hire\" write up" LINE
									  "a little 'How to do your job for dummies'.", true);
			break;
		case 1'01'108:
			DIALOG("Phone (Manager)", "Now read it before you eat, or else.", true);
			break;
		case 1'01'109:
			DIALOG("You", "Cool, but where do I eat?", true);
			break;
		case 1'01'110:
			DIALOG("Phone (Manager)", "Oh. There's an airlock in the room" LINE
									  "that leads into the main hall. The cafeteria" LINE
									  "is just nearby- DON'T MISS IT.", true);
			break;
		case 1'01'111:
			DIALOG("Phone", "*click*              " LINE
							"*BEEEEEEEEP* *BEEEEEEEEP*", true);
			break;
		case 1'01'112:
			DIALOG(SPEAKER_NARRATOR, "Well, thats a start of a day." LINE
									 "I should probably read the book, like" LINE 
									 "the manager asked me to.", false);
			break;
		default:
			DIALOG("ERROR", TextFormat("Case not found for dialogNum %d", dialog), false);
			break;
	}
}
