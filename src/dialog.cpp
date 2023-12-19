// we do the undertale style

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

		// TRANSITIONS 
		case 101: // from PROTAG_ROOM to MAIN_HALLWAY
			transitionToStationLevel(state, MAIN_HALLWAY);
			PSETPLAYERPOS(-65.84);
			break;
		case 201: // from MAIN_HALLWAY to PROTAG_ROOM
			transitionToStationLevel(state, PROTAG_ROOM);
			PSETPLAYERPOS(-65.84);
			break;
		case 202: // from MAIN_HALLWAY to CAFETERIA 
			transitionToStationLevel(state, CAFETERIA);
			PSETPLAYERPOS(-65.84);
			break;
		case 203:
			transitionToStationLevel(state, MANAGERS_OFFICE);
			break;
		case 204:
			transitionToStationLevel(state, SHIP_BOARDING);
			PSETPLAYERPOS(-66.7);
			break;
		case 301: // CAFETERIA to MAIN_HALLWAY
			transitionToStationLevel(state, MAIN_HALLWAY);
			PSETPLAYERPOS(62.51);
			break;
		case 401: // SHIP_BOARDING to MAIN_HALLWAY
			transitionToStationLevel(state, MAIN_HALLWAY);
			PSETPLAYERPOS(317.67);
			break;

		//STORY DIALOG 
		// case num formatting: X = day num, XX = room num (see level enum), XXX = dialog num
		// jumps in the XXX by a 100 indicate a new dialog-line
		case 1'00'000:
			DIALOG(SPEAKER_NARRATOR, "I remember the promo video for this program" LINE
									"like the back of my hand, and it goes for" LINE 
									"everyone else.", true);
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
			DIALOG("Phone", "Alright, cool. Now get yourself something to eat," LINE
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
			DIALOG("Phone (Manager)", "Oh. There's an airlock in your room" LINE
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

		case 1'01'200:
			DIALOG(SPEAKER_NARRATOR, "A composition book is on the table. It's title is" LINE
									 "LITERALLY \"How to do your job for dummies\"." LINE
									 "Authored by a guy named \"Peter\".", true);
			break;
		case 1'01'201:
			DIALOG(
					"Chapter 1/1: How to do your job",
					"Greetings, volunteer! Welcome to the financial" LINE 
					"peril relief program hosted by raycorp! We are" LINE 
					"sincerely honored to have you work for us!", true);
			PFLAG[2] = true;
			break;
		case 1'01'202:
			DIALOG(
					"Chapter 1/1: How to do your job",
					"You will be working with your assigned manager" LINE 
					"in order to excavate resources from the " LINE 
					"station's local resources out here in space!",
					true
				  );
			break;
		case 1'01'203:
			DIALOG(
					"Chapter 1/1: How to do your job",
					"First, you will need to learn the controls of your" LINE 
					"assigned ship. Please be aware that the assigned" LINE 
					"ship is fragile and may collapse under the force",
					true
				  );
			break;

		case 1'01'204:
			DIALOG(
					"Chapter 1/1: How to do your job",
					"of a small collision, so please be careful when" LINE 
					"piloting it. For starters, (A/D) are used to turn" LINE 
					"the ship, but since it is in space, the turn is" LINE, 
					true
				  );
			break;

		case 1'01'205:
			DIALOG(
					"Chapter 1/1: How to do your job", 
					"frictionless, so it is continously turning." LINE
					"(W) is used to propel the ship forward, but" LINE
					"keep in mind, there is no backwards propeller. ",
					true
				  );
			break;

		case 1'01'206:
			DIALOG(
					"Chapter 1/1: How to do your job",
					"As a safety net, this comes modified with" LINE 
					"an airbrake in order to reduce the ship's" LINE 
					"velocities in space, but usage is limited.",
					true
				  );
			break;

		case 1'01'207:
			DIALOG(
					"Chapter 1/1: How to do your job",
					"The ship comes with a laser equipped with a" LINE 
					"laser meant to extract resources out of rock" LINE 
					"material, namely the asteroids littered in space.",
					true
				  );
			break;

		case 1'01'208:
			DIALOG(
					"Chapter 1/1: How to do your job",
					"(SPACE) is used to fire the laser. But like" LINE 
					"the airbrake, the lasers usage is also limited," LINE 
					"along with its range being short.",
					true
				  );

		case 1'01'209:
			DIALOG(
					"Chapter 1/1: How to do your job",
					"This concludes the on-boarding book for the" LINE
					"financial peril relief program. We hope you have " LINE
					"a pleasant experience during your time here.",
					true
				  );
			break;

		case 1'01'210:
			DIALOG(
					SPEAKER_NARRATOR,
					"Theres tiny fine-print handwriting at the end of" LINE 
					"the chapter, saying \"NEXT PAGE ->\", so I turn" LINE 
					"as instructed, and read it.",	
					true
				  );
			break; 

		case 1'01'211:
			DIALOG(
					"Book",
					"Dear whoever-the-fuck-is-going-to-read-this, if" LINE
					"you are reading this as a \"volunteer\" for the" LINE 
					"program they put shitty ads on TV for, then you",
					true);
			break;

		case 1'01'212:
			DIALOG(
					"Book",
					"probably won't be coming back anytime ever." LINE
					"231 days has passed since I signed up." LINE
					"To know how much time holed up in this ",
					true
				  );
			break;

		case 1'01'213:
			DIALOG(
					"Book",
					"says alot about my anticipation fleeing from this" LINE
					"shithole. If you aren't having any second thoughts " LINE
					"even after reading this, then you have something",
					true
				  );
			break;

		case 1'01'214:
			DIALOG(
					"Book",
					"important to go home to, like me. I just want to " LINE 
					"see my wife again. To the managing fucklet reading this, " LINE 
					"keeping me locked up in here: I hope from the bottom of ",
					true
				  );
			break;

		case 1'01'215:
			DIALOG(
					"Book",
					"my heart that whatever lurks in the deepest of space " LINE
					"painfully eats you live, bones chewed on like that of a dog." LINE 
					"because that fate is what YOU FUCKING DESERVE.",
					true
				  );
			break;

		case 1'01'216:
			DIALOG(
					SPEAKER_NARRATOR,
					"The page ends on a... very venomous note." LINE 
					"Flipping through the pages, I notice that there are" LINE 
					"more writings in this book. Peter's diary, perhaps?",
					true
				  );
			break;

		case 1'01'217:
			DIALOG(
					SPEAKER_NARRATOR,
					"I should read more of this once I finish eating" LINE 
					"and do my first shift out of thousands, if Peter is right." LINE 
					"I just hope that I have it easy compared to him...",
					false	
				  );
			break;

		case 1'01'300:
			DIALOG(
					SPEAKER_NARRATOR,
					"Not now, I need to read the book first.",
					false
				  );
			break;



		case 1'02'001: // protag room sign 
			DIALOG(
					SPEAKER_NARRATOR,
					"\"Financial Peril Program Volunteer " 
					"Dormitory\".",
					false	
				);
			break;

		case 1'02'100: // protag room sign 
			DIALOG(
					SPEAKER_NARRATOR,
					"\"Cafeteria\".",
					false
				);
			break;

		case 1'02'200: // protag room sign 
			DIALOG(
					SPEAKER_NARRATOR,
					"\"Financial Peril Program Manager's office and" LINE 
					"dormitory. Authorized personnel only\".",
					false
				);
			break;

		case 1'02'300: // protag room sign 
			DIALOG(
					SPEAKER_NARRATOR,
					"\"Station Spaceport\".", 
					false
				);
			break;

		case 1'04'000: // the food dispenser thing 
			DIALOG(
					SPEAKER_NARRATOR,
					"A set of contraptions are screwed onto the wall." LINE 
					"On the right is a large box with a lid that opens from the" LINE 
					"top. On the left is a small box with only a yellow light.",
					true
				  );
			break;
		case 1'04'001:
			DIALOG(
					SPEAKER_NARRATOR,
					"A note is taped to the left of the small box, reading:" LINE 
					"\"HOLD RIGHT WRIST UP TO RFID READER, LID OPENS " LINE 
					"AUTOMATICALLY\"",
					true
				  );
			PFLAG[3] = true;
			break;

		case 1'04'002:
			DIALOG(
					SPEAKER_NARRATOR,
					"I do as the note instructs, holding my right hand up" LINE 
					"to the reader with the yellow light. It chimes, the" LINE 
					"light turning bright green for a moment, before blinking",
					true
				  );
			break;
		case 1'04'003:
			DIALOG(
					SPEAKER_NARRATOR,
					"yellow. At the same time the lidded box \"hums\", and" LINE 
					"after a brief moment, it opens. Looking into the box," LINE
					"there's a dark-brown bar of what seems to be granola.",
					true
				  );
			break;
		case 1'04'004:
			DIALOG(
					SPEAKER_NARRATOR,
					"The stench of manure hits me soon after, and goes " LINE
					"away as I promptly closed the lid.",
					true
				  );
			break;
		case 1'04'005:
			DIALOG(SPEAKER_NARRATOR, "I'm going to the ship, nevermind the food.", false);
			break;
		case 1'04'006:
			DIALOG(SPEAKER_NARRATOR, "Never again.", false);
			break;
		case 1'04'100:
			DIALOG(
					SPEAKER_NARRATOR,
					"Cafeteria Kitchen. Authorized Personnel Only.",
					false
				  );
			if(PFLAG[3]) {
				state->station.dialogue.nextDialog = true;
				state->station.dialogue.num = 1'04'101 - 1;
			}
			break;
		case 1'04'101:
			DIALOG(
					SPEAKER_NARRATOR,
					"Knowing what I know now, what's behind this door might" LINE 
					"as well not be a kitchen at all if the food it serves" LINE
					"is literal shit.",
					false
				  );
			break;
		case 1'04'200:
			DIALOG(
					SPEAKER_NARRATOR,
					"I try to open the airlock, but it doesn't budge.",
					false
				  );
			break;
		default:
			DIALOG("ERROR", TextFormat("Case not found for dialogNum %d", dialog), false);
			break;
	}
}
