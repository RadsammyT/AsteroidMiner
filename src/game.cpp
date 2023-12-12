#include "game.h"
#include "dialog.h"
#include <cmath>
#include <cstdlib>
#include <cstring>

#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>
#include <rlgl.h>

Vector3 rotation = {};


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
				rotation,
				GetTime() * 10,
				{2.5, 2.5, 2.5},
				WHITE);
	EndMode3D();

	int len = MeasureText("Asteroid Miner", 48);
	int middle = len - (len/2);
	DrawText("Asteroid Miner", (WIDTH/2) - middle, 200, 48, {255, 0, 0, 255});

	len = MeasureText("by RadsammyT", 24);
	middle = len - (len/2);
	DrawText("by RadsammyT", (WIDTH/2) - middle, 250, 24, RED);
	
	
	len = MeasureText("Play", 48);
	middle = len - (len/2);
	if(CheckCollisionPointRec(GetMousePosition(), {(float)WIDTH/2 - middle, 900, (float)len, 48})) {
		DrawText("Play", (WIDTH/2) - middle, 900, 48, YELLOW);
		if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			state->gameState = STATE_STATION;
		}
	} else {
		DrawText("Play", (WIDTH/2) - middle, 900, 48, GRAY);
	}
	if(IsKeyPressed(KEY_ENTER)) {
		state->gameState = STATE_STATION;

	}
}



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

void ShipAsteroid::DrawAst(GameState* state, Frustum* frus) {
	Model* model;
	int divisor = 0;
#if DEBUG_MODE 
	//auto hb = state->debug.asteroidHitboxSizes;
#endif	
	float radius = 0;
	switch(size) {
		case 1:
			SETMODEL(model, &state->models.size1Ast);
			radius = .37;
			divisor = 5;
			break;
		case 2:
			SETMODEL(model, &state->models.size2Ast);
			radius = .62;
			divisor = 4;
			break;
		case 3:
			SETMODEL(model, &state->models.size3Ast);
			divisor = 3;
			radius = 1;
			break;
		case 4:
			SETMODEL(model, &state->models.size4Ast);
			divisor = 2;
			radius = 2;
			break;
		case 5:
			SETMODEL(model, &state->models.size5Ast);
			divisor = 1;
			radius = 4.75;
			break;
		default:
			model = &state->models.errorFallback;
			break;
	}
	if(SphereInFrustumV(frus, {pos.x, 0, pos.y}, radius))
		DrawModelEx(*model, {pos.x, 0, pos.y}, {cosAsteroidRotation.x, cosAsteroidRotation.y, cosAsteroidRotation.z}, GetTime() * 10, Vector3Scale(Vector3One(), 2.5f / divisor), WHITE);
#if DEBUG_MODE
	if(state->debug.drawHitboxes)
		DrawSphereWires({pos.x, 0, pos.y}, radius, 50, 50, RED);
#endif
	else {

	}
}

void DoShip(GameState *state, Camera3D *cam) {
	ClearBackground(BLACK);
	HandleState(state);
	float x = std::sin(state->ship.shipRotation) * RAD2DEG;
	float z = std::cos(state->ship.shipRotation) * RAD2DEG;
	cam->position = {state->ship.shipPosition.x, 0, state->ship.shipPosition.y};
	cam->target = {cam->position.x + x, 0, cam->position.z + z};
	Frustum frus;

	BeginMode3D(*cam);
	ExtractFrustum(&frus);
#if DEBUG_MODE
		DrawModel(state->models.errorFallback, {4,0,4}, 1, {255,255,255,255});
		DrawCubeWiresV({0,-5,0}, {200,0,200}, WHITE);
#endif
		for(auto i: state->ship.asteroids) {
			i.DrawAst(state, &frus);
		}
	EndMode3D();
#if DEBUG_MODE 
	for(auto i: state->ship.asteroids) {
		Vector2 screen = GetWorldToScreen({i.pos.x, 0, i.pos.y}, *cam);
		DrawText(TextFormat("size: %d", i.size), screen.x, screen.y, 12, WHITE);
	}
#endif
	// ship UI here 
	DrawRectangle(0, 0, 200, HEIGHT, BROWN);
	DrawRectangle(WIDTH-200, 0, 200, HEIGHT, BROWN);

	if(state->transition.active) {
		if(state->transition.transitionTime >= 0)
			DrawRectangle(0, 0, 2000,2000, BLACK);
		else if(state->transition.transitionTime > -state->transition.maxTransitionTime) {
			float mod = state->transition.maxTransitionTime + 
				(state->transition.transitionTime - (state->transition.maxTransitionTime*2));
			DrawRectangle(0, 0, 2000, 2000, {0,0,0,(unsigned char)((float)mod*255)});
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
			if(IsKeyPressed(KEY_R)) {
				state->station.dialogue.chars = state->station.dialogue.maxChars - 1;
			}
			if(state->station.dialogue.timeToNextChar > 0 
					&& !state->station.dialogue.readyToAdvance)
				state->station.dialogue.timeToNextChar -= GetFrameTime();
			else {
				state->station.dialogue.timeToNextChar = 0.03;
				if(SSDP.displayMessage[SSDP.chars] == '.') {
					state->station.dialogue.timeToNextChar = 0.5;
				}
				if(SSDP.displayMessage[SSDP.chars + 1] == '\n') {
					state->station.dialogue.timeToNextChar = 0;
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

	if(state->gameState == GAME_STATE::STATE_SHIP) {
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
		}  else {
			auto ship = &state->ship;
			if(IsKeyDown(KEY_D)) {
				ship->shipRotVelo -= 0.1 * GetFrameTime();
			}
			if(IsKeyDown(KEY_A)) {
				ship->shipRotVelo += 0.1 * GetFrameTime();
			}
			float x = std::sin(state->ship.shipRotation) * DEG2RAD;
			float z = std::cos(state->ship.shipRotation) * DEG2RAD;
			if(IsKeyDown(KEY_W)) {
				ship->shipVelocity.x += x * GetFrameTime() * 1.5; 
				ship->shipVelocity.y += z * GetFrameTime() * 1.5;
			}
			if(IsKeyDown(KEY_SPACE) && ship->airBreakCharge > 0) {
				ship->shipVelocity = Vector2Scale(ship->shipVelocity, 0.950);
				ship->shipRotVelo = ship->shipRotVelo * 0.950;
				ship->airBreakCharge -= 0.75 * GetFrameTime();
			} else {
				if(!(ship->airBreakCharge >= 1) && !IsKeyDown(KEY_SPACE))
					ship->airBreakCharge += 0.1 * GetFrameTime();
			}
			ship->shipRotation += ship->shipRotVelo;
			ship->shipPosition = Vector2Add(ship->shipPosition, ship->shipVelocity);
			
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

void transitionToShip(GameState *state, int asteroidCount) {
	state->transition.active = true;
	state->transition.maxTransitionTime = 1;
	state->transition.transitionTime = 1;
	state->transition.onPeak = STATION_TO_SHIP;

	for(int i = 0; i < asteroidCount; i++)
		state->ship.asteroids.push_back(ShipAsteroid {
			.pos = {(float)(rand() % 200) - 100,(float)(rand() % 200) - 100},
			.velocity = {0,0},
			.cosAsteroidRotation = {RAND_FLOAT, RAND_FLOAT, RAND_FLOAT},
			.size = (rand() % 5) + 1,
		});
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
			{state->station.playerPosition.x,state->station.playerPosition.y-26.0f, 64, 64},
			{32, 32}, 0.0f, WHITE);
}

