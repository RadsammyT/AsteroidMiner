#include "game.h"
#include "frustum/Frustum.h"
#include <cstring>

#include <raylib.h>

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
	
	len = MeasureText("Made for the raylib 2023-2024 SloJam", 12);
	middle = len - (len/2);
	DrawText("Made for the raylib 2023-2024 SloJam", (WIDTH/2) - middle, 0, 12, RAYWHITE);
	
	len = MeasureText("Press Enter to Play", 48);
	middle = len - (len/2);
	DrawText("Press Enter to Play", (WIDTH/2) - middle, HEIGHT - 70, 48, GRAY);

	DrawText(
			"Credts:\n\n"
			"Asteroid Models by Albert Buscio on Sketchfab\n"
			"Sci-Fi platform tiles by Eris on OpenGameArt.org\n"
			"Phone Ring SFX by timgormly on Freesound\n"
			"Footsteps SFX by EVRetro on Freesound\n"
			, 0, 0, 12, RAYWHITE);

	DrawText(
			"Instructions:\n"
			"(A/D) or (Left Arrow/Right Arrow) to walk\n"
			"(E) to interact with an object when prompted\n"
			"Additional instructions will be given out in-game."
			, 0, HEIGHT/2, 12, RAYWHITE);

	if(IsKeyPressed(KEY_ENTER)) {
		state->gameState = STATE_STATION;

	}
}

void HandleState(GameState * state) {
	if(state->gameState == GAME_STATE::STATE_STATION) {
		if(state->transition.active) {
			state->transition.transitionTime -= GetFrameTime();
			if(state->transition.transitionTime >= -0.1
					&& state->transition.transitionTime <= 0.0
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
			if(IsKeyDown(KEY_E) && ship->laserCharge > 0) {
				ship->isLasering = true;
				ship->laserCharge -= 0.2 * GetFrameTime();
				for(int i = 0; i < ship->asteroids.size(); i++) {
					// TODO: rewrite collision with 2d in mind
					Vector2 laserEdge = Vector2Add(ship->shipPosition, Vector2Scale({x,z}, 100));
					for(float lerp = 0.0; lerp < 1.0; lerp += 0.01) {
						Vector2 lerpped = Vector2Lerp(ship->shipPosition, laserEdge, lerp);
#if DEBUG_MODE
						if(CheckCollisionPointCircle(lerpped, ship->asteroids[i].pos, ship->asteroids[i].getSphereRad() * 
									state->debug.multiplier)) {
							state->debug.laserCollide = lerpped;
#else 
						
		if(CheckCollisionPointCircle(lerpped, ship->asteroids[i].pos, ship->asteroids[i].getSphereRad())) { 
#endif
							ship->asteroids[i].lasered = true;
							ship->asteroids[i].durability -= 1 * GetFrameTime();
							ship->asteroids[i].size = (int)ship->asteroids[i].durability;
							if(ship->asteroids[i].size == 0) {
								ship->asteroids.erase(ship->asteroids.begin() + i);
							}
							break;
						} else {
							ship->asteroids[i].lasered = false;
						}
					}
#if DEBUG_MODE
					state->debug.laserEdge = laserEdge;
					state->debug.laserPos = ship->shipPosition;
#endif
				}
			} else {
				ship->isLasering = false;
				if(!(ship->laserCharge >= 1) && !IsKeyDown(KEY_E))
					ship->laserCharge += 0.1 * GetFrameTime();
				for(auto& i: ship->asteroids) {
					i.lasered = false;
				}
			}
			ship->shipRotation += ship->shipRotVelo;
			ship->shipPosition = Vector2Add(ship->shipPosition, ship->shipVelocity);

			if(ship->asteroids.empty()) {
				if(CheckCollisionSpheres(Vector2to3XZ(ship->shipPosition), 1,
							{0,0,-1}, 1)) {
					if(!state->transition.active) {
						transitionToStationLevel(state, TEST_LEVEL);
					}
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

void onTransitionPeak(GameState *state) {
	switch(state->transition.onPeak) {
		case ON_TRANSITION_PEAK::STATION_LEVEL_TO_LEVEL:
			state->gameState = STATE_STATION;
			state->station.stationLevel = (STATION_LEVEL)state->transition.toLevel;
			if(state->transition.dialogAfter) {
				undertale(state, state->transition.dialogNum);
				state->transition.dialogAfter = false;
			}
			break;
		case ON_TRANSITION_PEAK::STATION_TO_SHIP:
			state->gameState = STATE_SHIP;
			break;
	}
}

Vector3 Vector2to3XZ(Vector2 in) {
	return {in.x, 0, in.y};
}
