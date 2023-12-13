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
	}
}

void DoStation(GameState* state, Camera2D* stationCam) {
			ClearBackground(BLACK);
			stationCam->target = {0, -30};
			if(state->station.stationLevel == TEST_LEVEL) {
				stationCam->target.x = state->station.playerPosition.x;
			}
			BeginMode2D(*stationCam);
				DrawStation(state);
#if DEBUG_MODE
				DrawRectangleV({(float)(state->station.playerPosition.x-16),
						state->station.playerPosition.y - 32},
						{32,3},
						{255, 0, 0, 64});
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
	}
	if(SSDP.alreadyDisengaged) {
		SSDP.alreadyDisengaged = false;
	}
}

void transitionToStationLevel(GameState *state, int level) {
	state->transition.active = true;
	state->transition.maxTransitionTime = 1;
	state->transition.transitionTime = 1;
	state->transition.onPeak = STATION_LEVEL_TO_LEVEL;
	state->transition.toLevel = level;
}
