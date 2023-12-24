#include "game.h"
#include <cmath>
#include <raylib.h>
#include <cstring>
#include <raymath.h>

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
			PLAYERBOUNDS(-80, 383);
			DrawTexture(state->textures.mainHall, -112, -112, WHITE);
			break;
        case SHIP_BOARDING:
			DrawTexture(state->textures.shipBoarding, -112, -112, WHITE);
			break;
        case CAFETERIA:
			PLAYERBOUNDS(-80, 190);
			DrawTexture(state->textures.cafeteria, -112, -112, WHITE);
			break;
        case MANAGERS_OFFICE:
			DrawTexture(state->textures.shipBoarding, -112, -112, WHITE);
    		break;
        case INTRO:
		  DrawRectangle(0, 0, 2000, 2000, BLACK);
          break;
	}
}

void DoStation(GameState* state, Camera2D* stationCam, Camera3D* shipCam) {

			if(PFLAG[0] && !PFLAG[1] && PDAY == 1) {
				if(!IsSoundPlaying(state->sounds.phoneRing)) {
					PlaySound(state->sounds.phoneRing);
				}
			}

			ClearBackground(BLACK);
			stationCam->target = {0, -30};
			if(state->station.stationLevel == TEST_LEVEL ||
				state->station.stationLevel == MAIN_HALLWAY ||
				state->station.stationLevel == CAFETERIA) {
				stationCam->target.x = state->station.playerPosition.x;
			}
			if(state->station.stationLevel == SHIP_BOARDING) {
				stationCam->target.x = 25;
			}
			if(state->station.stationState != STATION_SUBSTATE::SLEEPING) {
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
					if(!state->transition.active)
						CheckInteract(state);
				EndMode2D();
			} else {
				state->station.sleep.time -= GetFrameTime();
				if(state->station.sleep.time <= 0) {
					state->station.stationState = STATION_SUBSTATE::WALK;
					for(int i = 0; i < 32; i++)
						state->story.flags[i] = 0;

				}
				auto sleepCam = state->station.sleep.cam;
				float x = sin(GetTime());
				float y = cos(GetTime());
				state->station.sleep.cam.target = Vector3Add(sleepCam.position, {x, 0, y});
				BeginMode3D(sleepCam);
					for(auto i: state->station.sleep.asteroids) {
						i.DrawAst(state, &sleepCam);
					}
				EndMode3D();
				float meas = MeasureText("YOU ARE SLEEPING", 48);
				DrawRectangle((WIDTH/2) - (meas/2), (HEIGHT/2)-24, meas, 48, BLACK);
				DrawText("YOU ARE SLEEPING", (WIDTH/2)-(meas/2), (HEIGHT/2)-24, 48, {255,0,0,255});

			}
			if(state->transition.active) {
				if(state->transition.transitionTime > 0)
					DrawRectangle(0, 0, 2000,2000, BLACK);
			}

			HandleState(state, shipCam);
			if(state->station.stationState == DIALOG) {
				DrawRectangle(00, 600, WIDTH, 200, GRAY);
				DrawText(state->station.dialogue.displayMessage, 10, 610, 48, WHITE);
				if(strcmp(state->station.dialogue.speaker, SPEAKER_NARRATOR)) { // if ssdp != SPEAKER_NARRATOR
					DrawRectangle(00, 600-32, 
							MeasureText(state->station.dialogue.speaker, 32) + 20,
							32, GRAY);
					DrawText(state->station.dialogue.speaker , 01, 600-32, 32, WHITE);
				}
				if(state->station.dialogue.readyToAdvance)
					DrawTexturePro(
							state->textures.button_e,
							{0,0,16,16},
							{WIDTH-64, 600-64, 64,64},
							{0,0},
							0,
							WHITE);
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
					if(PFLAG[4]) {
						INTERACTABLE(2, -46.5, 13.5, 35.5, 1'01'601); // PHONE (MANAGER)
					} else {
						INTERACTABLE(2, -46.5, 13.5, 35.5, 1'01'100); // PHONE (MANAGER)
					}
				}
				if(PFLAG[1]) {
					if(!PFLAG[2]) {
						INTERACTABLE(-45, -32, 10.5, 17.5, 1'01'200); // DIARY
						INTERACTABLE(-80.5, -44.5, 17.5, 43.5, 1'01'300); // TRANSITION TO MAIN HALLWAY
					} else {
						if(PFLAG[4]) {
							INTERACTABLE(-80.5, -44.5, 17.5, 43.5, 1'01'700); // TRANSITION TO MAIN HALLWAY
						} else {
							INTERACTABLE(-80.5, -44.5, 17.5, 43.5, 101); // TRANSITION TO MAIN HALLWAY
						}
					}
				}
				if(PFLAG[4] && PFLAG[1]) {
					INTERACTABLE(40, -45, 55, 16, 1'01'400);
				}
			} else {
				INTERACTABLE(-93, -44.5, 40.5, 43.5, 101); // TRANSITION TO MAIN HALLWAY
			}
			break;
        case MAIN_HALLWAY:
			if(PFLAG[4]) {
				INTERACTABLE(-92, -44.5, 39.5, 44, 201'1);
			} else {
				INTERACTABLE(-92, -44.5, 39.5, 44, 201);
			}
			INTERACTABLE(35, -45, 41, 45, 202);
			if(PDAY == 2) {
				INTERACTABLE(163, -45, 41, 45, 203);
			} else { 
				INTERACTABLE(163, -45, 41, 45, 1'02'500);
			}
			if(PFLAG[3]) {
				INTERACTABLE(291, -45, 41, 45, 204);
			} else  {
				INTERACTABLE(291, -45, 41, 45, 1'02'400);
			}

			INTERACTABLE(-44, -37, 17, 12, 1'02'001);
			INTERACTABLE(84, -37, 17, 12, 1'02'100);
			INTERACTABLE(212, -37, 17, 12, 1'02'200);
			INTERACTABLE(340, -37, 17, 12, 1'02'300);
			break;
        case SHIP_BOARDING:
			INTERACTABLE(-93, -45, 41, 44.5, 401);  // DOOR TO MAIN_HALLWAY
			INTERACTABLE(-9, -39, 19, 39, 1'03'000);
			INTERACTABLE(50, -45, 44, 29, 1'03'100);
			INTERACTABLE(99, -45, 41, 45, 1'03'200);
			break;
        case CAFETERIA:
			INTERACTABLE(-93, -45, 41, 44.5, 301);  // DOOR TO MAIN_HALLWAY
			if(!PFLAG[3]) {
				INTERACTABLE(67, -36, 29, 15, 1'04'000); // FOOD DISPENSER (SHITTY)
			} else {
				INTERACTABLE(67, -36, 29, 15, 1'04'006); // FOOD DISPENSER (SHITTY)
			}
			INTERACTABLE(116, -37, 17, 12, 1'04'100); // BACKROOM SIGN
			INTERACTABLE(147, -45, 41, 44.5, 1'04'200); // BACKROOM DOOR (LOCKED DIALOGUE)
			break;
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

void transitionToSleep(GameState *state) {
	state->station.stationState = STATION_SUBSTATE::SLEEPING;
	state->station.sleep.time = 10;
	for(int i = 0; i < 50; i++) {
		int tempSize = (rand() % 5) + 1;
		state->station.sleep.asteroids.push_back(ShipAsteroid {
			.pos = {(float)(rand() % 200) - 100,(float)(rand() % 200) - 100},
			.velocity = {0,0},
			.cosAsteroidRotation = {RAND_FLOAT, RAND_FLOAT, RAND_FLOAT},
			.size = tempSize,
			.durability = (float)tempSize + 0.9f,
		});
	}
	state->station.sleep.cam = {
		.position = {0,0,0},
		.target = {0,0,0},
		.up = {0,1,0},
		.fovy = 90.0f,
		.projection = CameraProjection::CAMERA_PERSPECTIVE,
	};
}

void transitionToStationLevel(GameState *state, int level, bool dialogAfter, int dialogNum) {
	state->transition.active = true;
	state->transition.maxTransitionTime = 1;
	state->transition.transitionTime = 1;
	if(state->gameState == STATE_STATION)
		state->transition.onPeak = STATION_LEVEL_TO_LEVEL;
	else if(state->gameState == STATE_SHIP)
		state->transition.onPeak = SHIP_TO_STATION;
	state->transition.toLevel = level;
	state->transition.dialogAfter = dialogAfter;
	state->transition.dialogNum = dialogNum;

	state->station.playerPosition = {0,0};
	ENDDIALOGUEP;
}
