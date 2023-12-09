#include "game.h"
#include "dialog.h"
#include <cstdlib>
#include <cstring>

#include <raylib.h>
#include <rlgl.h>

float rotations[5][3]; // SHOULD be from main.cpp

void DoTitle(GameState* state) {
	Camera3D cam = {
		.position = {10, 5, 0},
		.target = {0, 0, 0},
		.up = {0, 1, 0},
		.fovy = 90.0f,
		.projection = CameraProjection::CAMERA_PERSPECTIVE
	};

	// Title and Menu Buttons
	ClearBackground(BLACK);
	BeginMode3D(cam);
		DrawModelEx(state->models.size5Ast,
				{0, 0, 0},
				{rotations[4][0], rotations[4][1], rotations[4][2]},
				GetTime() * 10,
				{2.5, 2.5, 2.5},
				WHITE);
	EndMode3D();

	int len = MeasureText("Asteroid Miner", 48);
	int middle = len - (len/2);
	DrawText("Asteroid Miner", (1920/2) - middle, 200, 48, {255, 0, 0, 255});

	len = MeasureText("by RadsammyT", 24);
	middle = len - (len/2);
	DrawText("by RadsammyT", (WIDTH/2) - middle, 250, 24, RED);
	
	

}



void DrawStation(GameState* state) {
	switch(state->station.stationLevel) {
		case TEST_LEVEL:
			DrawRectangle(-200, 0, 1000, 50, WHITE);
			DrawRectangle(-25, -50, 25, 50, GREEN); // dialogue test
			DrawRectangle(50,-50,25,50, YELLOW); // transition to ship test
			break;
		case TEST_LEVEL_TRANSITION:
			DrawRectangle(-200, 0, 1000, 50, WHITE); 
			DrawRectangle(-100, -50, 100, 50, BROWN); // dialogue test
			DrawRectangle(50, -50, 100, 50, GREEN); //  transition to another room
			break;
	}
}

void DoStation(GameState* state, Camera2D* stationCam) {
			ClearBackground(BLACK);
			BeginMode2D(*stationCam);
				DrawStation(state);
				DrawRectangleV({(float)(state->station.playerPosition.x + 5),
						state->station.playerPosition.y - 32},
						{10, 32},
						{255, 0, 0, 64});
				DrawPlayerTex(state);
				CheckInteract(state);
			EndMode2D();
			if(state->transition.active) {
				if(state->transition.transitionTime > 0)
					DrawRectangle(0, 0, 2000,2000, BLACK);
			}
			HandleState(state);
			if(state->station.stationState == DIALOG) {
				DrawRectangle(300, 800, 1920 - 600, 200, BLACK);
				DrawText(state->station.dialogue.displayMessage, 310, 810, 48, WHITE);
				if(strcmp(state->station.dialogue.speaker, SPEAKER_NARRATOR)) {
					DrawRectangle(300, 800-32, 
							MeasureText(state->station.dialogue.speaker, 32) + 20,
							32, BLACK);
					DrawText(state->station.dialogue.speaker ,310, 800-32, 32, WHITE);
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
void HandleState(GameState * state) {
		if(state->gameState == GAME_STATE::STATE_STATION) {
			if(state->transition.active) {
				state->transition.transitionTime -= GetFrameTime();
				if((int)state->transition.transitionTime == 0
						&& !state->transition.atPeak) {
					state->transition.atPeak = true;
					onTransitionPeak(state);
				} else {
					state->transition.atPeak = false;
				}
				if(state->transition.transitionTime < 
						-state->transition.maxTransitionTime) {
					state->transition.active = false;
				}
			} else if(state->station.stationState == STATION_STATE::WALK) {
				if(IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
					state->station.playerPosition.x -= WALK_SPEED * GetFrameTime();
					state->station.anim.direction = 0;
					state->station.anim.timeToNextCycle -= GetFrameTime();
					if(state->station.anim.timeToNextCycle < 0) {
						state->station.anim.currentCycle++;
						if(state->station.anim.currentCycle > 3) {
							state->station.anim.currentCycle = 0;
						}
						state->station.anim.timeToNextCycle = ANIM_SPEED;
					}
				} else if(IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
					state->station.playerPosition.x += WALK_SPEED * GetFrameTime();
					state->station.anim.direction = 1;
					state->station.anim.timeToNextCycle -= GetFrameTime();
					if(state->station.anim.timeToNextCycle < 0) {
						state->station.anim.currentCycle++;
						if(state->station.anim.currentCycle > 3) {
							state->station.anim.currentCycle = 0;
						}
						state->station.anim.timeToNextCycle = ANIM_SPEED;
					}
				} else {
					state->station.anim.currentCycle = 4;
					state->station.anim.timeToNextCycle = ANIM_SPEED;
				}
			}
			if(state->station.stationState == STATION_STATE::DIALOG) {
				if(state->station.dialogue.timeToNextChar > 0 
						&& !state->station.dialogue.readyToAdvance)
					state->station.dialogue.timeToNextChar -= GetFrameTime();
				else {
					state->station.dialogue.timeToNextChar = 0.03;
					if(SSDP.displayMessage[SSDP.chars + 1] == '.') {
						state->station.dialogue.timeToNextChar = 0.5;
						state->station.dialogue.chars++;
					}
					if(SSDP.displayMessage[SSDP.chars + 1] == '\n') {
						state->station.dialogue.timeToNextChar = 0;
						state->station.dialogue.chars++;
					}
					state->station.dialogue.chars++;
					writeToCharArr(state->station.dialogue.fullMessage,
									state->station.dialogue.displayMessage, 
									state->station.dialogue.chars);
					if(state->station.dialogue.chars == state->station.dialogue.maxChars) {
						state->station.dialogue.readyToAdvance = true;
					}
					if(IsKeyPressed(KEY_E) && SSDP.readyToAdvance) {
						if(SSDP.nextDialog && !SSDP.alreadyDisengaged) {
							SSDP.num++;
							undertale(state, SSDP.num);
						} else {
							state->station.stationState = WALK;
							SSDP.num = -1;
							SSDP.nextDialog = false;
							SSDP.alreadyDisengaged = true;
						}
						SSDP.readyToAdvance = false;
					}
				}
			}
		}
}
void writeToCharArr(const char* in, char * arr, int limit) { // where limit defaults to -1
	if(limit == -1) {
		limit = strlen(in);
	}
	for(int i = 0; i <= limit + 1; i++) {
		if(i == limit)
			arr[i] = '\0';
		else
			arr[i] = in[i];
	}
}

void transitionToShip(GameState *state) {
	state->transition.active = true;
	state->transition.maxTransitionTime = 1;
	state->transition.transitionTime = 1;
	state->transition.onPeak = STATION_TO_SHIP;
}

void transitionToStationLevel(GameState *state, int level) {
	state->transition.active = true;
	state->transition.maxTransitionTime = 1;
	state->transition.transitionTime = 1;
	state->transition.onPeak = STATION_LEVEL_TO_LEVEL;
	state->transition.toLevel = level;
}

void onTransitionPeak(GameState *state) {
	switch(state->transition.onPeak) {
		case ON_TRANSITION_PEAK::STATION_LEVEL_TO_LEVEL:
			state->station.stationLevel = (STATION_LEVEL)state->transition.toLevel;
			state->station.playerPosition = {0,0};
			break;
		case ON_TRANSITION_PEAK::STATION_TO_SHIP:
			state->gameState = STATE_SHIP;
			break;
	}
}


void DrawPlayerTex(GameState *state) {
	auto multi = WALKING_CYCLE_ARR[state->station.anim.currentCycle];
	if(state->station.anim.direction) {
		multi.width *= -1;
	}
	DrawTexturePro(state->textures.basePlayer, multi,
			{state->station.playerPosition.x+10,state->station.playerPosition.y-13.0f, 32, 32},
			{16, 16}, 0.0f, WHITE);
}

void randomizeRotations(float rots[5][3]) {
	for(int i = 0; i < 5; i++) {
		for(int j = 0; j < 3; j++) {
			rots[i][j] = (float)(rand()%100000) / 100000;
		}
	}
}
