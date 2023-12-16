#include "game.h"
#include <raylib.h>
#include <cstring>

void DrawStation(GameState* state) {
	switch(state->station.stationLevel) {
		case TEST_LEVEL:
			DrawTexture(state->textures.testRoom, -112, -112, WHITE);
			DrawRectangle(-25, -50, 25, 50, GREEN); // dialogue test
			DrawRectangle(50,-50,25,50, YELLOW); // transition to ship test
			break;
		case TEST_LEVEL_TRANSITION:
			DrawTexture(state->textures.testRoom, -112, -112, WHITE);
			DrawRectangle(-100, -50, 100, 50, BROWN); // dialogue test
			DrawRectangle(50, -50, 100, 50, GREEN); //  transition to another room
			break;
        case PROTAG_ROOM:
			PLAYERBOUNDS(-80, 90);
			DrawTexture(state->textures.protagRoom, -112, -112, WHITE);
			break;
        case MAIN_HALLWAY:
        case SHIP_BOARDING:
        case CAFETERIA:
        case MANAGERS_OFFICE:
          break;
        case INTRO:
		  DrawRectangle(0, 0, 2000, 2000, BLACK);
          break;
        }
}

void DoStation(GameState* state, Camera2D* stationCam) {

			if(PFLAG[0] && !PFLAG[1] && PDAY == 1) {
				if(!IsSoundPlaying(state->sounds.phoneRing)) {
					PlaySound(state->sounds.phoneRing);
				}
			}

			ClearBackground(BLACK);
			stationCam->target = {0, -30};
			if(state->station.stationLevel == TEST_LEVEL) {
				stationCam->target.x = state->station.playerPosition.x;
			}
			BeginMode2D(*stationCam);
				DrawStation(state);
#if DEBUG_MODE
				DrawRectangleV({(float)(state->station.playerPosition.x-23),
						state->station.playerPosition.y - 32},
						{32,3},
						{255, 0, 0, 64});
				if(state->debug.drawTestRec) {
					DrawRectangleRec(state->debug.testRec, {255, 0, 0, 128});
				}
#endif
				DrawPlayerTex(state);
				CheckInteract(state);
			EndMode2D();
			if(state->transition.active) {
				if(state->transition.transitionTime > 0)
					DrawRectangle(0, 0, 2000,2000, BLACK);
			}
			HandleState(state);
			if(state->station.stationState == DIALOG) {
				DrawRectangle(100, 600, 1200, 200, GRAY);
				DrawText(state->station.dialogue.displayMessage, 110, 610, 48, WHITE);
				if(strcmp(state->station.dialogue.speaker, SPEAKER_NARRATOR)) {
					DrawRectangle(100, 600-32, 
							MeasureText(state->station.dialogue.speaker, 32) + 20,
							32, GRAY);
					DrawText(state->station.dialogue.speaker , 101, 600-32, 32, WHITE);
				}
			}
}

void CheckInteract(GameState* state) {
#define DIALOGCHECK IsKeyPressed(KEY_E) && !SSDP.alreadyDisengaged
	Vector2 stationPlyPos = state->station.playerPosition;
	if(SSDP.alreadyDisengaged || state->station.stationState == DIALOG) {
		SSDP.alreadyDisengaged = false;
		return;
	}
	switch(state->station.stationLevel) {
		case TEST_LEVEL:
			INTERACTABLE(-25, -50, 25, 50, 1);
			INTERACTABLE(50, -50, 25, 50, 8);
			break;
		case TEST_LEVEL_TRANSITION:
			INTERACTABLE(-100, -50, 100, 50, 5);
			INTERACTABLE(50, -50, 100, 50, 6);
			break;
        case PROTAG_ROOM:
			if(PDAY == 1) {
				if(PFLAG[0] && !PFLAG[1]) {
					INTERACTABLE(2, -46.5, 13.5, 35.5, 1'01'100);
				}
			}
			break;
        case MAIN_HALLWAY:
        case SHIP_BOARDING:
        case CAFETERIA:
        case MANAGERS_OFFICE:
          break;
        case INTRO:
          break;
        }
        if(SSDP.alreadyDisengaged) {
		SSDP.alreadyDisengaged = false;
	}
}

void DrawPlayerTex(GameState *state) {
	if(state->station.stationLevel == INTRO)
		return;
	auto multi = WALKING_CYCLE_ARR[state->station.anim.currentCycle];
	if(state->station.anim.direction) {
		multi.width *= -1;
	}
	DrawTexturePro(state->textures.basePlayer, multi,
			{state->station.playerPosition.x,state->station.playerPosition.y-14.0f, 52, 52},
			{32, 32}, 0.0f, WHITE);
}

void transitionToStationLevel(GameState *state, int level, bool dialogAfter, int dialogNum) {
	state->transition.active = true;
	state->transition.maxTransitionTime = 1;
	state->transition.transitionTime = 1;
	state->transition.onPeak = STATION_LEVEL_TO_LEVEL;
	state->transition.toLevel = level;
	state->transition.dialogAfter = dialogAfter;
	state->transition.dialogNum = dialogNum;

	state->station.playerPosition = {0,0};
	ENDDIALOGUEP;
}
