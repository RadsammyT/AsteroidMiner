#include "game.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <raylib.h>
#include <raymath.h>

void DrawStation(GameState* state) {
	switch (state->station.stationLevel) {
	case TEST_LEVEL:
		DrawTexture(state->textures.testRoom, -112, -112, WHITE);
		DrawRectangle(-25, -50, 25, 50, GREEN); // dialogue test
		DrawRectangle(50, -50, 25, 50, YELLOW); // transition to ship test
		break;
	case TEST_LEVEL_TRANSITION:
		DrawTexture(state->textures.testRoom, -112, -112, WHITE);
		DrawRectangle(-100, -50, 100, 50, BROWN); // dialogue test
		DrawRectangle(50, -50, 100, 50, GREEN);   //  transition to another room
		break;
	case PROTAG_ROOM:
		PLAYERBOUNDS(-80, 90);
		DrawTexture(state->textures.protagRoom, -112, -112, WHITE);
		break;
	case MAIN_HALLWAY:
		PLAYERBOUNDS(-80, 383);
		if (PDAY == 1)
			DrawTexture(state->textures.mainHall, -112, -112, WHITE);
		else if(PDAY == 2)
			DrawTexture(state->textures.mainHallDay2, -112, -112, WHITE);
		else if(PDAY == 3 && PFLAG[2])
			DrawTexture(state->textures.mainHallFinale, -112, -112, WHITE);
		else 
			DrawTexture(state->textures.mainHall, -112, -112, WHITE);
			
		break;
	case SHIP_BOARDING:
		if(PDAY == 3 && PFLAG[2])
			DrawTexture(state->textures.shipBoardingFinale, -112, -112, WHITE);
		else 
			DrawTexture(state->textures.shipBoarding, -112, -112, WHITE);
		break;
	case CAFETERIA:
		PLAYERBOUNDS(-80, 190);
		DrawTexture(state->textures.cafeteria, -112, -112, WHITE);
		break;
	case MANAGERS_OFFICE:
		DrawTexture(state->textures.managerRoom, -112, -112, WHITE);
		break;
	case INTRO:
		DrawRectangle(0, 0, 2000, 2000, BLACK);
		break;
	}
}

void DoStation(GameState* state, Camera2D* stationCam, Camera3D* shipCam) {
	if (PFLAG[0] && !PFLAG[1] && (PDAY == 1 || PDAY == 3)) {
		if (!IsSoundPlaying(state->sounds.phoneRing)) {
			PlaySound(state->sounds.phoneRing);
		}
	}

	if(PDAY == 3 && PFLAG[2] && state->station.stationLevel == MAIN_HALLWAY) {
		if(state->station.playerPosition.x <= 138 && !state->station.monster.active) {
			state->station.monster.active = true;
			state->station.monster.position = -90;
			PlaySound(state->sounds.jumpscare);
		}
	}
	if(PDAY == 3 && PFLAG[2] && state->station.stationLevel == SHIP_BOARDING) {
		if(!IsSoundPlaying(state->sounds.spaceAmbience)) {
			PlaySound(state->sounds.spaceAmbience);
		}
	} else {
		if(IsSoundPlaying(state->sounds.spaceAmbience) && state->station.stationState != SLEEPING) {
			StopSound(state->sounds.spaceAmbience);
		}
	}

	ClearBackground(BLACK);
	stationCam->target = {0, -30};
	if (state->station.stationLevel == TEST_LEVEL ||
	    state->station.stationLevel == MAIN_HALLWAY ||
	    state->station.stationLevel == CAFETERIA) {
		stationCam->target.x = state->station.playerPosition.x;
	}
	if (state->station.stationLevel == SHIP_BOARDING) {
		stationCam->target.x = 25;
	}
	if (state->station.stationLevel == MANAGERS_OFFICE) {
		stationCam->target.x = 25;
	}
	if (state->station.stationState != STATION_SUBSTATE::SLEEPING) {
		BeginMode2D(*stationCam);
		DrawStation(state);
#if DEBUG_MODE
		DrawRectangleV({(float)(state->station.playerPosition.x - 23),
		                state->station.playerPosition.y - 32},
		               {32, 3}, {255, 0, 0, 64});
		if (state->debug.drawTestRec) {
			DrawRectangleRec(state->debug.testRec, {255, 0, 0, 128});
		}
#endif
		DrawPlayerTex(state);
		if(state->station.monster.active) {
			auto& mon = state->station.monster;
			mon.position += 150 * GetFrameTime();
			if(mon.position + 100 >= state->station.playerPosition.x) {
				SetSoundVolume(state->sounds.explode, 5.5);
				PlaySound(state->sounds.explode);
				StopSound(state->sounds.jumpscare);
				state->gameState = GAME_STATE::STATE_AFTERSCARE;
				state->afterscare.timeUntil = 5;
			}
			Rectangle imgRec = {0, 0, (float)state->textures.stationMonsterSprite.width
				, (float)state->textures.stationMonsterSprite.height};
			DrawRectangle(-96, -96, mon.position + 98, 96, BLACK);
			DrawTexturePro(state->textures.stationMonsterSprite,
					imgRec,
					{mon.position, -96, 96, 96},
					{0,0}, 0, {127, 127, 127, 255});
		}
		if (!state->transition.active)
			CheckInteract(state);
		EndMode2D();
	} else {
		state->station.sleep.time -= GetFrameTime();
		if (state->station.sleep.time <= 0) {
			StopSound(state->sounds.spaceAmbience);
			state->station.stationState = STATION_SUBSTATE::WALK;
			for (int i = 0; i < 32; i++)
				state->story.flags[i] = 0;
			switch (state->story.day) {
			case 1:
			case 2:
				undertale(state, 2'01'001);
				break;
			case 3:
				undertale(state, 3'01'000);
				break;
			default:
				printf("DoStation(): DAY %d NOT IMPLEMENTED\n",
				       state->story.day);
				break;
			}
		}
		auto sleepCam = state->station.sleep.cam;
		float x = sin(GetTime());
		float y = cos(GetTime());
		state->station.sleep.cam.target =
			Vector3Add(sleepCam.position, {x, 0, y});
		BeginMode3D(sleepCam);
		for (auto i : state->station.sleep.asteroids) {
			i.DrawAst(state, &sleepCam);
		}
		EndMode3D();
		float meas = MeasureText("YOU ARE SLEEPING", 48);
		DrawRectangle((WIDTH / 2) - (meas / 2), (HEIGHT / 2) - 24, meas, 48,
		              BLACK);
		DrawText("YOU ARE SLEEPING", (WIDTH / 2) - (meas / 2),
		         (HEIGHT / 2) - 24, 48, {255, 0, 0, 255});
	}
	if (state->transition.active) {
		if (state->transition.transitionTime > 0)
			DrawRectangle(0, 0, 2000, 2000, BLACK);
	}

	HandleState(state, shipCam);
	if (state->station.stationState == DIALOG) {
		DrawRectangle(00, 600, WIDTH, 200, GRAY);
		DrawText(state->station.dialogue.displayMessage, 10, 610, 48, WHITE);
		if (strcmp(state->station.dialogue.speaker,
		           SPEAKER_NARRATOR)) { // if ssdp != SPEAKER_NARRATOR
			DrawRectangle(00, 600 - 32,
			              MeasureText(state->station.dialogue.speaker, 32) + 20,
			              32, GRAY);
			DrawText(state->station.dialogue.speaker, 01, 600 - 32, 32, WHITE);
		}
		if (state->station.dialogue.readyToAdvance)
			DrawTexturePro(state->textures.button_e, {0, 0, 16, 16},
			               {WIDTH - 64, 600 - 64, 64, 64}, {0, 0}, 0, WHITE);
	}
}

void CheckInteract(GameState* state) {
#define DIALOGCHECK IsKeyPressed(KEY_E) && !SSDP.alreadyDisengaged
	Vector2 stationPlyPos = state->station.playerPosition;
	if (SSDP.alreadyDisengaged || state->station.stationState == DIALOG) {
		SSDP.alreadyDisengaged = false;
		return;
	}
	switch (state->station.stationLevel) {
	case TEST_LEVEL:
		INTERACTABLE(-25, -50, 25, 50, 1);
		INTERACTABLE(50, -50, 25, 50, 8);
		break;
	case TEST_LEVEL_TRANSITION:
		INTERACTABLE(-100, -50, 100, 50, 5);
		INTERACTABLE(50, -50, 100, 50, 6);
		break;
	case PROTAG_ROOM:
		if (PDAY == 1) {
			if (PFLAG[0] && !PFLAG[1]) {
				if (PFLAG[4]) {
					INTERACTABLE(2, -46.5, 13.5, 35.5,
					             1'01'601); // PHONE (MANAGER)
				} else {
					INTERACTABLE(2, -46.5, 13.5, 35.5,
					             1'01'100); // PHONE (MANAGER)
				}
			}
			if (PFLAG[1]) {
				if (!PFLAG[2]) {
					INTERACTABLE(-45, -32, 10.5, 17.5, 1'01'200); // DIARY
					INTERACTABLE(-80.5, -44.5, 17.5, 43.5,
					             1'01'300); // TRANSITION TO MAIN HALLWAY
				} else {
					if (PFLAG[4]) {
						INTERACTABLE(-80.5, -44.5, 17.5, 43.5,
						             1'01'700); // TRANSITION TO MAIN HALLWAY
					} else {
						INTERACTABLE(-80.5, -44.5, 17.5, 43.5,
						             101); // TRANSITION TO MAIN HALLWAY
					}
				}
			}
			if (PFLAG[4] && PFLAG[1]) {
				INTERACTABLE(40, -45, 55, 16, 1'01'400);
			}
		} else if (PDAY == 2) {
			INTERACTABLE(-93, -44.5, 40.5, 43.5,
			             101); // TRANSITION TO MAIN HALLWAY
			INTERACTABLE(-45, -32, 10.5, 17.5, 2'01'200); // DIARY DAY 2
			if (PFLAG[1]) {
				INTERACTABLE(40, -45, 55, 16, 2'01'100);
			}
		} else if (PDAY == 3) {
			if (PFLAG[0] && !PFLAG[1]) {
				INTERACTABLE(2, -46.5, 13.5, 35.5, 3'01'100); // PHONE (MANAGER)
			}
			if (PFLAG[1]) {
				INTERACTABLE(-93, -44.5, 40.5, 43.5,
				             101); // TRANSITION TO MAIN HALLWAY
				INTERACTABLE(-45, -32, 10.5, 17.5, 3'01'200); // DIARY DAY 3
			}
		}
		break;
	case MAIN_HALLWAY:
		if (PDAY == 1) {
			if (PFLAG[4]) {
				INTERACTABLE(-92, -44.5, 39.5, 44, 201'1);
			} else {
				INTERACTABLE(-92, -44.5, 39.5, 44, 201);
			}
		} else {
			INTERACTABLE(-92, -44.5, 39.5, 44, 201);
		}
		INTERACTABLE(35, -45, 41, 45, 202);
		if (PDAY == 2) {
			INTERACTABLE(163, -45, 41, 45, 203);
		} else if (PDAY == 1) {
			INTERACTABLE(163, -45, 41, 45, 1'02'500);
		}
		if (PDAY == 1) {
			if (PFLAG[3]) {
				INTERACTABLE(291, -45, 41, 45, 204);
			} else {
				INTERACTABLE(291, -45, 41, 45, 1'02'400);
			}
		}
		if (PDAY == 2) {
			if (PFLAG[0]) {
				INTERACTABLE(291, -45, 41, 45, 204);
			} else {
				INTERACTABLE(291, -45, 41, 45, 2'02'000);
			}
		}
		if (PDAY == 3 && !PFLAG[2]) {
			INTERACTABLE(291, -45, 41, 45, 204);
			INTERACTABLE(163, -45, 41, 45, 3'02'000);
		}
		if(PDAY == 3 && PFLAG[2]) {
		} else {
			INTERACTABLE(-44, -37, 17, 12, 1'02'001);
			INTERACTABLE(84, -37, 17, 12, 1'02'100);
			INTERACTABLE(212, -37, 17, 12, 1'02'200);
			INTERACTABLE(340, -37, 17, 12, 1'02'300);
		}
		break;
	case SHIP_BOARDING:
		INTERACTABLE(-93, -45, 41, 44.5, 401); // DOOR TO MAIN_HALLWAY
		if(PDAY != 3 && !PFLAG[2]) {
			INTERACTABLE(-9, -39, 19, 39, 1'03'000);
			INTERACTABLE(50, -45, 44, 29, 1'03'100);
		}
		if (PDAY == 1 && !PFLAG[4]) {
			INTERACTABLE(99, -45, 41, 45, 1'03'200);
		} else if (PDAY == 2 && !PFLAG[1]) {
			INTERACTABLE(99, -45, 41, 45, 2'04'000);
		} else if (PDAY == 3 && !PFLAG[2]) {
			INTERACTABLE(99, -45, 41, 45, 3'04'000);
		}
		break;
	case CAFETERIA:
		INTERACTABLE(-93, -45, 41, 44.5, 301); // DOOR TO MAIN_HALLWAY
		if (!PFLAG[3]) {
			INTERACTABLE(67, -36, 29, 15, 1'04'000); // FOOD DISPENSER (SHITTY)
		} else {
			INTERACTABLE(67, -36, 29, 15, 1'04'006); // FOOD DISPENSER (SHITTY)
		}
		INTERACTABLE(116, -37, 17, 12, 1'04'100); // BACKROOM SIGN
		INTERACTABLE(147, -45, 41, 44.5,
		             1'04'200); // BACKROOM DOOR (LOCKED DIALOGUE)
		break;
	case MANAGERS_OFFICE:
		INTERACTABLE(16.5, -32, 38.5, 21, 2'03'100); // LAPTOP
		INTERACTABLE(68, -37, 17, 12, 2'03'000);     // DORM SIGN
		INTERACTABLE(99, -45, 41, 45, 2'03'200);     // DORM DOOR
		INTERACTABLE(-93, -45, 41, 45, 501);
		break;
	case INTRO:
		break;
	}
	if (SSDP.alreadyDisengaged) {
		SSDP.alreadyDisengaged = false;
	}
}

void DrawPlayerTex(GameState* state) {
	if (state->station.stationLevel == INTRO)
		return;
	auto multi = WALKING_CYCLE_ARR[state->station.anim.currentCycle];
	if (state->station.anim.direction) {
		multi.width *= -1;
	}
	if(PDAY == 3 && PFLAG[2]) {
		DrawTexturePro(state->textures.basePlayerFinale, multi,
					   {state->station.playerPosition.x,
						state->station.playerPosition.y - 14.0f, 52, 52},
					   {32, 32}, 0.0f, WHITE);
		return;
	}
	DrawTexturePro(state->textures.basePlayer, multi,
	               {state->station.playerPosition.x,
	                state->station.playerPosition.y - 14.0f, 52, 52},
	               {32, 32}, 0.0f, WHITE);
}

void transitionToSleep(GameState* state) {
	state->transition.active = true;
	state->transition.onPeak = -1;
	state->transition.dialogAfter = false;
	state->transition.transitionTime = 1;
	state->transition.maxTransitionTime = 1;
	state->station.stationState = STATION_SUBSTATE::SLEEPING;
	state->station.sleep.time = 10;
	SetSoundVolume(state->sounds.spaceAmbience, 1);
	PlaySound(state->sounds.spaceAmbience);
	for (int i = 0; i < 50; i++) {
		int tempSize = (rand() % 5) + 1;
		state->station.sleep.asteroids.push_back(ShipAsteroid{
			.pos = {(float)(rand() % 200) - 100, (float)(rand() % 200) - 100},
			.velocity = {0, 0},
			.cosAsteroidRotation = {RAND_FLOAT, RAND_FLOAT, RAND_FLOAT},
			.size = tempSize,
			.durability = (float)tempSize + 0.9f,
		});
	}
	state->station.sleep.cam = {
		.position = {0, 0, 0},
		.target = {0, 0, 0},
		.up = {0, 1, 0},
		.fovy = 90.0f,
		.projection = CameraProjection::CAMERA_PERSPECTIVE,
	};
}

void transitionToStationLevel(GameState* state, int level, bool dialogAfter,
                              int dialogNum, bool closeDoorSound) {
	state->transition.active = true;
	state->transition.maxTransitionTime = 1;
	state->transition.transitionTime = 1;
	if (state->gameState == STATE_STATION)
		state->transition.onPeak = STATION_LEVEL_TO_LEVEL;
	else if (state->gameState == STATE_SHIP)
		state->transition.onPeak = SHIP_TO_STATION;
	state->transition.toLevel = level;
	state->transition.dialogAfter = dialogAfter;
	state->transition.dialogNum = dialogNum;
	state->transition.closeSoundAfter = closeDoorSound;
	state->station.playerPosition = {0, 0};
	ENDDIALOGUEP;
}

void walkSound(GameState* state) {
	if (state->station.anim.currentCycle == 1) {
		if (!IsSoundPlaying(state->sounds.walkLeft)) {
			PlaySound(state->sounds.walkLeft);
		}
	}
	if (state->station.anim.currentCycle == 3) {
		if (!IsSoundPlaying(state->sounds.walkRight)) {
			PlaySound(state->sounds.walkRight);
		}
	}
}
