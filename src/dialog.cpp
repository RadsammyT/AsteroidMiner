// we do the undertale style

#include "game.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <raylib.h>

#define LINE "\n\n\n\n"

// god save your soul for what you are about to witness
void undertale(GameState *state, int dialog) {
  state->station.stationState = DIALOG;
  writeToCharArr("\0", SSDP.displayMessage, -1);
  state->station.dialogue.num = dialog;
  state->station.dialogue.readyToAdvance = false;
  state->station.dialogue.chars = 0;
  switch (dialog) {
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
    DIALOG("You",
           "Tis just a big brick of shit...\n\n\n\n"
           "Nothing special about it.",
           true);
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
    DIALOG(SPEAKER_NARRATOR, "You go through the airlock and board the ship.",
           true);
    break;
  case 9:
    ENDDIALOGUEP;
    transitionToShip(state, 1, 1, 1);
    break;
  case 10:
    DIALOG("You",
           "It also smells.                    " LINE "Like, really bad.",
           false);
    break;

  // TRANSITIONS
  case 101: // from PROTAG_ROOM to MAIN_HALLWAY
    transitionToStationLevel(state, MAIN_HALLWAY);
    PSETPLAYERPOS(-65.84);
    DOOROPENP;
    break;
  case 201: // from MAIN_HALLWAY to PROTAG_ROOM
    transitionToStationLevel(state, PROTAG_ROOM);
    PSETPLAYERPOS(-65.84);
    DOOROPENP;
    break;
  case 201'1:
    transitionToStationLevel(state, PROTAG_ROOM, true, 1'01'500, true);
    PSETPLAYERPOS(-65.84);
    DOOROPENP;
    break;
  case 202: // from MAIN_HALLWAY to CAFETERIA
    transitionToStationLevel(state, CAFETERIA);
    PSETPLAYERPOS(-65.84);
    DOOROPENP;
    break;
  case 203:
    transitionToStationLevel(state, MANAGERS_OFFICE, false, 0, false);
    PSETPLAYERPOS(-67.51);
    break;
  case 204:
    transitionToStationLevel(state, SHIP_BOARDING);
    PSETPLAYERPOS(-66.7);
    DOOROPENP;
    break;
  case 301: // CAFETERIA to MAIN_HALLWAY
    transitionToStationLevel(state, MAIN_HALLWAY);
    PSETPLAYERPOS(61.65);
    DOOROPENP;
    break;
  case 401: // SHIP_BOARDING to MAIN_HALLWAY
    transitionToStationLevel(state, MAIN_HALLWAY);
    PSETPLAYERPOS(317.67);
    DOOROPENP;
    break;
  case 501:
    transitionToStationLevel(state, MAIN_HALLWAY, false, 0, false);
    PSETPLAYERPOS(190);
    break;

  // STORY DIALOG
  // where case numbers are X'XX'XXX
  //  case num formatting: X = day num, XX = room num (see level enum), XXX =
  //  dialog num jumps in the XXX by a 100 indicate a new dialog-line
  case 1'00'000:
    DIALOG(SPEAKER_NARRATOR,
           "I remember the promo video for this program" LINE
           "like the back of my hand, and it goes for" LINE "everyone else.",
           true);
    break;
  case 1'00'001:
    DIALOG(SPEAKER_NARRATOR,
           "Think of the most unconvincing advertisement " LINE
           "for a product that will make you never buy it " LINE
           "or the advertisers products ever again.",
           true);
    break;
  case 1'00'002:
    DIALOG(SPEAKER_NARRATOR,
           "In many cases, people would refer to this advert" LINE
           "like hes the most ugliest man-thing in high school.",
           true);
    break;
  case 1'00'003:
    DIALOG(SPEAKER_NARRATOR,
           "Most of the promo was filled to the brim with legal" LINE
           "disclaimers, which made it more off-putting for" LINE
           "this program, even if it was meant to relieve debt.",
           true);
    break;
  case 1'00'004:
    DIALOG(SPEAKER_NARRATOR,
           "So why? Why after heeding all of these warnings-" LINE
           "these red flags...                              " LINE
           "did I ever sign up for it in the first place...?",
           true);
    break;
  case 1'00'005:
    transitionToStationLevel(state, PROTAG_ROOM, true, 1'01'006, false);
    break;
  case 1'01'006:
    PSETPLAYERPOS(80);
    DIALOG("You", "!", true);
    break;
  case 1'01'007:
    DIALOG("You", "Uhm, where am I...?", true);
    break;
  case 1'01'008:
    DIALOGS(SPEAKER_NARRATOR, "*RING RING RING*", true, DIALOG_SOUND::NONE);
    state->story.flags[0] = true;
    // day 1: flag 0: is the phone ringing? if flag 1 is on, the ringing stops.
    break;
  case 1'01'009:
    DIALOG("You",
           "It's coming from the phone on the wall..." LINE
           "I probably should answer it.",
           false);
    break;
  case 1'01'100:
    DIALOG(SPEAKER_NARRATOR,
           "I pickup the phone and hold the receiver" LINE "next to my ear.",
           true);
    PFLAG[1] = true;
    break;
  case 1'01'101:
    DIALOG("You",
           "...                                              " LINE "Hello?",
           true);
    break;
  case 1'01'102:
    DIALOGS("Phone",
            "\"So you're the new guy that signed up, " LINE
            "am I right or am I right?\"",
            true, DIALOG_SOUND::MANAGER);
    break;
  case 1'01'103:
    DIALOG("You", "...Yes, sir...", true);
    break;
  case 1'01'104:
    DIALOGS("Phone",
            "Alright, cool. Now get yourself something to eat," LINE
            "because you gotta get shit done right about now.",
            true, DIALOG_SOUND::MANAGER);
    break;
  case 1'01'105:
    DIALOGS("Phone",
            "In case you are wondering and it wasn't obvious" LINE
            "to you, I'm your manager that's gonna manage you" LINE
            "til' the day either of us dies.",
            true, DIALOG_SOUND::MANAGER);
    break;
  case 1'01'106:
    DIALOG("You", "That's cool and all, but what *exactly* am I-", true);
    break;
  case 1'01'107:
    DIALOGS("Phone (Manager)",
            "Quit yappin', there's a book on the table, the one" LINE
            "to your left. I had the previous \"hire\" write up" LINE
            "a little 'How to do your job for dummies'.",
            true, DIALOG_SOUND::MANAGER);
    break;
  case 1'01'108:
    DIALOGS("Phone (Manager)",
            "Before I forget, you have 4 to mine for your first" LINE "day."
            " Now read it before you eat, or else.",
            true, DIALOG_SOUND::MANAGER);
    break;
  case 1'01'109:
    DIALOG("You", "Cool, but where do I eat?", true);
    break;
  case 1'01'110:
    DIALOGS("Phone (Manager)",
            "Oh. There's an airlock in your room" LINE
            "that leads into the main hall. The cafeteria" LINE
            "is just nearby- DON'T MISS IT.",
            true, DIALOG_SOUND::MANAGER);
    break;
  case 1'01'111:
    DIALOGS("Phone", "*click*              " LINE "*BEEEEEEEEP* *BEEEEEEEEP*",
            true, DIALOG_SOUND::NONE);
    break;
  case 1'01'112:
    DIALOG(SPEAKER_NARRATOR,
           "Well, thats a start of a day." LINE
           "I should probably read the book, like" LINE
           "the manager asked me to.",
           false);
    break;

  case 1'01'200:
    DIALOG(SPEAKER_NARRATOR,
           "A composition book is on the table. It's title is" LINE
           "LITERALLY \"How to do your job for dummies\"." LINE
           "Authored by a guy named \"Peter\".",
           true);
    break;
  case 1'01'201:
    DIALOG("Chapter 1/1: How to do your job",
           "Greetings, volunteer! Welcome to the financial" LINE
           "peril relief program offered by raycorp! We are" LINE
           "sincerely honored to have you work for us!",
           true);
    PFLAG[2] = true;
    break;
  case 1'01'202:
    DIALOG("Chapter 1/1: How to do your job",
           "You will be working with your assigned manager" LINE
           "in order to excavate resources from the " LINE
           "station's local resources out here in space!",
           true);
    break;
  case 1'01'203:
    DIALOG("Chapter 1/1: How to do your job",
           "First, you will need to learn the controls of your" LINE
           "assigned ship. Please be aware that the assigned" LINE
           "ship is fragile and may collapse under the force",
           true);
    break;

  case 1'01'204:
    DIALOG("Chapter 1/1: How to do your job",
           "of a small collision, so please be careful when" LINE
           "piloting it. For starters, (A/D) are used to turn" LINE
           "the ship, but since it is in space, the turn is" LINE,
           true);
    break;

  case 1'01'205:
    DIALOG("Chapter 1/1: How to do your job",
           "frictionless, so it is continously turning." LINE
           "(W) is used to propel the ship forward, but" LINE
           "keep in mind, there is no backwards propeller. ",
           true);
    break;

  case 1'01'206:
    DIALOG("Chapter 1/1: How to do your job",
           "As a safety net, this comes modified with" LINE
           "an airbrake in order to reduce the ship's" LINE
           "velocities in space, but usage is limited.",
           true);
    break;

  case 1'01'207:
    DIALOG("Chapter 1/1: How to do your job",
           "The ship comes with a laser equipped with a" LINE
           "laser meant to extract resources out of rock" LINE
           "material, namely the asteroids littered in space.",
           true);
    break;

  case 1'01'208:
    DIALOG("Chapter 1/1: How to do your job",
           "(SPACE) is used to fire the laser. But like" LINE
           "the airbrake, the lasers usage is also limited," LINE
           "along with its range being short.",
           true);

  case 1'01'209:
    DIALOG("Chapter 1/1: How to do your job",
           "This concludes the on-boarding book for the" LINE
           "financial peril relief program. We hope you have " LINE
           "a pleasant experience during your time here.",
           true);
    break;

  case 1'01'210:
    DIALOG(SPEAKER_NARRATOR,
           "Theres tiny fine-print handwriting at the end of" LINE
           "the chapter, saying \"NEXT PAGE ->\", so I turn" LINE
           "as instructed, and read it.",
           true);
    break;

  case 1'01'211:
    DIALOGS("Book",
            "Dear whoever-the-fuck-is-going-to-read-this, if" LINE
            "you are reading this as a \"volunteer\" for the" LINE
            "program they put shitty ads on TV for, then you",
            true, DIALOG_SOUND::PETER);
    break;

  case 1'01'212:
    DIALOGS("Book",
            "probably won't be coming back anytime ever." LINE
            "231 days has passed since I signed up." LINE
            "To know how much time holed up in this ",
            true, DIALOG_SOUND::PETER);
    break;

  case 1'01'213:
    DIALOGS("Book",
            "says alot about my anticipation fleeing from this" LINE
            "shithole. If you aren't having any second thoughts " LINE
            "even after reading this, then you have something",
            true, DIALOG_SOUND::PETER);
    break;

  case 1'01'214:
    DIALOGS("Book",
            "important to go home to, like me. I just want to " LINE
            "see my wife again. To the managing fucklet reading this, " LINE
            "keeping me locked up in here: I hope from the bottom of ",
            true, DIALOG_SOUND::PETER);
    break;

  case 1'01'215:
    DIALOGS("Book",
            "my heart that whatever lurks in the deepest of space " LINE
            "painfully eats you live, bones chewed on like that of a dog." LINE
            "because that fate is what YOU FUCKING DESERVE.",
            true, DIALOG_SOUND::PETER);
    break;

  case 1'01'216:
    DIALOG(SPEAKER_NARRATOR,
           "The page ends on a... very venomous note." LINE
           "Flipping through the pages, I notice that there are" LINE
           "more writings in this book. Peter's diary, perhaps?",
           true);
    break;

  case 1'01'217:
    DIALOG(SPEAKER_NARRATOR,
           "I should read more of this once I finish eating" LINE
           "and do my first shift out of thousands, if Peter is right." LINE
           "I just hope that I have it easy compared to him...",
           false);
    break;

  case 1'01'300:
    DIALOG(SPEAKER_NARRATOR, "Not now, I need to read the book first.", false);
    break;

  case 1'01'400:
    DIALOG(SPEAKER_NARRATOR,
           "I lie myself down onto the metal plate and sleep.", true);
    PFLAG[5] = true;
    break;

  case 1'01'401:
    transitionToSleep(state);
    state->story.day = 2;
    break;

  case 1'01'500:
    DIALOGS(SPEAKER_NARRATOR, "*RING RING RING*", true, DIALOG_SOUND::NONE);
    PFLAG[1] = false;
    break;

  case 1'01'501:
    DIALOG(SPEAKER_NARRATOR, "Its the Manager again. Might as well answer.",
           false);
    break;

  case 1'01'601:
    DIALOG(SPEAKER_NARRATOR,
           "I pickup the phone and hold the receiver next to" LINE "my ear.",
           true);
    PFLAG[1] = true;
    break;
  case 1'01'602:
    DIALOGS("Phone (Manager)", "Finally. Took you long enough.", true,
            DIALOG_SOUND::MANAGER);
    break;
  case 1'01'603:
    DIALOGS("Phone (Manager)",
            "Not bad for your first day. Shifts over for today." LINE
            "So is the day.",
            true, DIALOG_SOUND::MANAGER);
    break;
  case 1'01'604:
    DIALOG(SPEAKER_NARRATOR, "The Manager quickly hung up after that.", true);
    break;

  case 1'01'605:
    DIALOG(SPEAKER_NARRATOR, "Guess its time to sleep.", false);
    break;
  case 1'01'700:
    DIALOG(SPEAKER_NARRATOR, "I'm tired.", false)
    break;
  case 1'02'001: // protag room sign
    DIALOG(SPEAKER_NARRATOR,
           "\"Financial Peril Program Volunteer "
           "Dormitory\".",
           false);
    break;

  case 1'02'100: // cafeteria  room sign
    DIALOG(SPEAKER_NARRATOR, "\"Cafeteria\".", false);
    break;

  case 1'02'200: // manager room sign
    DIALOG(SPEAKER_NARRATOR,
           "\"Financial Peril Program Manager's office and" LINE
           "dormitory. Authorized personnel only\".",
           false);
    break;

  case 1'02'300: // ship room sign
    DIALOG(SPEAKER_NARRATOR, "\"Station Spaceport\".", false);
    break;

  case 1'02'400:
    DIALOG(SPEAKER_NARRATOR, "Not now, I'm hungry.", false);
    break;

  case 1'02'500: // managers office door
    DIALOG(SPEAKER_NARRATOR,
           "I try to open the airlock, but this one wouldn't budge.", false);
    break;

  case 1'03'000:
    DIALOG(SPEAKER_NARRATOR,
           "A locker, labelled \"SPACESUIT\". I open it only to find" LINE
           "nothing inside.",
           false);
    break;

  case 1'03'100:
    DIALOG(SPEAKER_NARRATOR,
           "I peer through the window, and see an expansive interior" LINE
           "with the ship stationed in the middle.",
           false);
    break;

  case 1'03'200:
    DIALOG(SPEAKER_NARRATOR, "I go through the airlock and board the ship.",
           true);
    break;
  case 1'03'201:
    transitionToShip(state, 3, 1, 0);
    DOOROPENP;
    ENDDIALOGUEP;
    break;

  case 1'03'300:
    DIALOG(SPEAKER_NARRATOR,
           "Well, with that now done, I should probably sleep for the" LINE
           "night. Or day.",
           false);

    break;

  case 1'04'000: // the food dispenser thing
    DIALOG(SPEAKER_NARRATOR,
           "A set of contraptions are screwed onto the wall." LINE
           "On the right is a large box with a lid that opens from the" LINE
           "top. On the left is a small box with only a yellow light.",
           true);
    break;
  case 1'04'001:
    DIALOG(SPEAKER_NARRATOR,
           "A note is taped to the left of the small box, reading:" LINE
           "\"HOLD RIGHT WRIST UP TO RFID READER, LID OPENS " LINE
           "AUTOMATICALLY\"",
           true);
    PFLAG[3] = true;
    break;

  case 1'04'002:
    DIALOG(SPEAKER_NARRATOR,
           "I do as the note instructs, holding my right hand up" LINE
           "to the reader with the yellow light. It chimes, the" LINE
           "light turning bright green for a moment, before blinking",
           true);
    break;
  case 1'04'003:
    DIALOG(SPEAKER_NARRATOR,
           "yellow. At the same time the lidded box \"hums\", and" LINE
           "after a brief moment, it opens. Looking into the box," LINE
           "there's a dark-brown bar of what seems to be granola.",
           true);
    break;
  case 1'04'004:
    DIALOG(SPEAKER_NARRATOR,
           "The stench of manure hits me soon after, and goes " LINE
           "away as I promptly closed the lid.",
           true);
    break;
  case 1'04'005:
    DIALOG(SPEAKER_NARRATOR, "I'm going to the ship, nevermind the food.",
           false);
    break;
  case 1'04'006:
    DIALOG(SPEAKER_NARRATOR, "Never again.", false);
    break;
  case 1'04'100:
    DIALOG(SPEAKER_NARRATOR, "Cafeteria Kitchen. Authorized Personnel Only.",
           false);
    if (PFLAG[3]) {
      state->station.dialogue.nextDialog = true;
      state->station.dialogue.num = 1'04'101 - 1;
    }
    break;
  case 1'04'101:
    DIALOG(SPEAKER_NARRATOR,
           "Knowing what I know now, what's behind this door might" LINE
           "as well not be a kitchen at all if the food it serves" LINE
           "is literal shit.",
           false);
    break;
  case 1'04'200:
    DIALOG(SPEAKER_NARRATOR, "I try to open the airlock, but it doesn't budge.",
           false);
    break;

  // DAY 2 DIALOGUE
  case 2'01'001:
    DIALOG(SPEAKER_NARRATOR,
           "..." LINE "When is he calling?" LINE
           "I should get today over with.",
           false);
    break;

  case 2'01'100:
    DIALOG(SPEAKER_NARRATOR, "I lie down on the 'bed' and sleep.", true);
    break;

  case 2'01'101:
    transitionToSleep(state);
    state->story.day = 3;
    break;

  case 2'01'200:
    DIALOG(SPEAKER_NARRATOR,
           "I open the book, flip through the pages" LINE
           "to where I last left off, and read.",
           true);
    break;

  case 2'01'201:
    DIALOGS("Book (Peter)",
            "Writing this here because I couldn't vent" LINE
            "anywhere else- So while I was doing my shifts" LINE
            "mining asteroids for whatever reason, I dunno how-",
            true, DIALOG_SOUND::PETER);
    break;

  case 2'01'202:
    DIALOGS("Book (Peter)",
            "but I saw one of them HALLOW, like a hole burrowed" LINE
            "into the rock and its insides just gone, and only" LINE
            "the 'shell' is left intact.",
            true, DIALOG_SOUND::PETER);
    break;

  case 2'01'203:
    DIALOGS("Book (Peter)",
            "When I talked to the manager over the phone-" LINE
            "who is always in his fucking room doing whatever-" LINE
            "he just brushed it off as me being lazy, somehow.",
            true, DIALOG_SOUND::PETER);
    break;

  case 2'01'204:
    DIALOGS("Book (Peter)",
            "I tried to at least disillusion him, saying that" LINE
            "thats not how any of this works, but he hung up the" LINE
            "phone before I could at least get a valid argument in.",
            true, DIALOG_SOUND::PETER);
    break;

  case 2'01'205:
    DIALOGS("Book (Peter)",
            "But SOMETHING is making these rocks be carved out" LINE
            "hollow, and its obviously not me. So if the manager" LINE
            "is going hands-off on all of this...",
            true, DIALOG_SOUND::PETER);
    break;

  case 2'01'206:
    DIALOGS("Book (Peter)",
            "Then I will simply figure this out myself. And once" LINE
            "I finally do, I want a raise. Even though he's going" LINE
            "to laugh it off like he always has.",
            false, DIALOG_SOUND::PETER);
    break;

  case 2'02'000:
    DIALOG(SPEAKER_NARRATOR, "Something is wrong.", false);
    break;

  case 2'03'000:
    DIALOG(SPEAKER_NARRATOR, "Financial Peril Program Manager's dormitory.",
           false);
    break;

  case 2'03'100:
    DIALOG(SPEAKER_NARRATOR,
           "A laptop lies on a desk, open and on. From what" LINE
           "I see on the screen, only an email client is open" LINE
           "on the laptop. 2 emails catch my eye, both recent.",
           true);
    break;

  case 2'03'101:
    DIALOG(SPEAKER_NARRATOR,
           "One had the subject of 'AUTO: FPRP QUOTA REMINDER'." LINE
           "The contents are self-explanatory. I have 9 asteroids" LINE
           "to mine for today.",
           true);
    break;

  case 2'03'102:
    DIALOG(SPEAKER_NARRATOR,
           "Another had a peculiar subject of 'Immediate" LINE
           "Evacuation Notice'. I try to read its contents" LINE
           "but before I could do so, the screen turns off.",
           true);
    break;

  case 2'03'103:
    DIALOG(SPEAKER_NARRATOR,
           "No amount of rapid-tapping its power button" LINE
           "would bring the laptop back on, not even" LINE
           "closing and opening the laptop worked.",
           true);
    break;

  case 2'03'104:
    DIALOG(SPEAKER_NARRATOR,
           "At least I know my quota for today." LINE
           "I wonder what was up with the Evacuation" LINE "email, though.",
           false);
    PFLAG[0] = true;
    break;

  case 2'03'200:
    DIALOG(SPEAKER_NARRATOR,
           "An airlock is left open. I take a glance inside" LINE
           "to find nothing of interest.",
           false);
    break;

  case 2'04'000:
    DIALOG(SPEAKER_NARRATOR, "I go through the airlock and board the ship.",
           true);
    break;
  case 2'04'001:
    transitionToShip(state, 4, 5, 0);
    DOOROPENP;
    ENDDIALOGUEP;
    break;
  case 2'04'100:
    DIALOG(SPEAKER_NARRATOR,
           "Thats the second shift done for the time being." LINE
           "I should go to bed, got nothing better to do.",
           false);
    // should fix an issue regarding after-shift dialog on day 2
    // state->transition.transitionTime = state->transition.maxTransitionTime;
    // update: breaking news: me is certified dumbass
    // TODO: fix whats described above!!! ^^^
    // UPDATE: fixed with hack
    break;

  case 3'01'000:
    DIALOG(SPEAKER_NARRATOR, "!", true);
    PFLAG[0] = true;
    break;

  case 3'01'001:
    DIALOG(SPEAKER_NARRATOR, "A phone call. Why now?", false);
    break;

  case 3'01'100:
    DIALOG(SPEAKER_NARRATOR,
           "I pick up the receiver and hold it up to" LINE
           "my ear. The next words that came out from the speaker" LINE
           "are obviously synthetic, and not of the manager's.",
           true);
    PFLAG[1] = true;
    break;

  case 3'01'101:
    DIALOGS("Phone",
            "Financial Peril Program Volunteer #89, we at raycorp" LINE
            "thank you for your service thus far in the program." LINE,
            true, DIALOG_SOUND::AI);
    break;

  case 3'01'102:
    DIALOGS("Phone",
            "However, due to suspicious activity detected 2 hours" LINE
            "prior, you are hereby penalized for entering a " LINE
            "restricted area and violating the non-knowledgable",
            true, DIALOG_SOUND::AI);
    break;

  case 3'01'103:
    DIALOGS("Phone",
            "Agreements as discussed prior to your participation." LINE
            "As a result you are to meet an extended quota of" LINE
            "16 additional geological samples prior to the end" LINE,
            true, DIALOG_SOUND::AI);
    break;
  case 3'01'104:
    DIALOGS("Phone",
            "of this current cycle, else your contract and financial" LINE
            "aid will be subject to respective termination and reversal." LINE
            "There will be no compensation for meeting this extension.",
            true, DIALOG_SOUND::AI);
    break;
  case 3'01'105:
    DIALOGS("Phone",
            "Once again, we thank you for participating in this" LINE
            "program and look forward to your continued cooperation.",
            true, DIALOG_SOUND::AI);
    break;
  case 3'01'106:
    DIALOG(SPEAKER_NARRATOR,
           "The phone quickly hangs up after that.           " LINE LINE
           "Fuck.",
           false);
    break;

  case 3'02'000:
    DIALOG(SPEAKER_NARRATOR, "I probably shouldn't go in there again.", false);
    break;

  case 3'04'000:
    DIALOG(SPEAKER_NARRATOR,
           "I once again open the airlock and board the ship.", true);
    break;

  case 3'04'001:
    transitionToShip(state, 5, 10, 1);
    DOOROPENP;
    ENDDIALOGUEP;
    break;

  case 3'04'100:
    DIALOG(SPEAKER_NARRATOR, "Something", false);
    break;

  default:
    DIALOG("ERROR", TextFormat("Case not found for dialogNum %d", dialog),
           false);
    break;
  }
}
