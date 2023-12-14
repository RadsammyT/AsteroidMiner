
#include "game.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <raylib.h>



void undertale(GameState* state, int dialog) {
	state->station.stationState = DIALOG;
	writeToCharArr("\0", SSDP.displayMessage, -1);
	state->station.dialogue.num = dialog;
	state->station.dialogue.readyToAdvance = false;
	state->station.dialogue.chars = 0;
	switch(dialog) {
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
			DIALOG("You", "It also smells.                    \n\n\n\n"
									"Like, really bad.", false);
			break;
		default:
			DIALOG("ERROR", TextFormat("Case not found for dialog %d", dialog), false);
			break;
	}
}
