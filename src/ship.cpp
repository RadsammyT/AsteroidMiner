#include <cmath>
#include <cstdio>
#include <raylib.h>
#include <raymath.h>
#include "game.h"

void DoShip(GameState *state, Camera3D *cam) {
	if(cam == NULL) {
		TraceLog(LOG_ERROR, "DoShip: Cam is NULL!\n");
		return;
	}
	ClearBackground(BLACK);
	HandleState(state, cam);
	float x = std::sin(state->ship.shipRotation) * RAD2DEG;
	float z = std::cos(state->ship.shipRotation) * RAD2DEG;
	if(state->ship.state == SHIP_SUBSTATE::SST_ACTION) {
		cam->position = {state->ship.shipPosition.x, 0, state->ship.shipPosition.y};
		cam->target = {cam->position.x + x, 0, cam->position.z + z};
	}
	Frustum frus;

	BeginMode3D(*cam);
	ExtractFrustum(&frus);
#if DEBUG_MODE
		DrawModel(state->models.errorFallback, {4,0,4}, 1, {255,255,255,255});
		DrawCubeWiresV({0,-5,0}, {200,0,200}, WHITE);
		DrawSphereWires(Vector2to3XZ(state->debug.laserPos), 0.05, 10, 10, BLUE);
		DrawSphereWires(Vector2to3XZ(state->debug.laserEdge), 0.1, 10, 10, GREEN);
		DrawSphereWires(Vector2to3XZ(state->debug.laserCollide), 0.1, 10, 10, YELLOW);
		DrawCubeV(state->debug.testCubePos, state->debug.testCubeSize, RED);
#endif
		// dock cube: 0, 0, -1.030 | 2, 1, 1
	
		DrawModel(state->models.station, {0, -1, 0}, 1.0f, WHITE);
		for(auto i: state->ship.asteroids) {
			i.DrawAst(state, cam);
		}
		if(IsKeyDown(KEY_E) && state->ship.laserCharge > 0) {
			float x = std::sin(state->ship.shipRotation) * DEG2RAD;
			float z = std::cos(state->ship.shipRotation) * DEG2RAD;
			Vector3 dest = Vector2to3XZ(
									Vector2Add(state->ship.shipPosition, Vector2Scale({x,z}, 100))
								);
			dest.y = 0;
			DrawCylinderEx({state->ship.shipPosition.x, -1, state->ship.shipPosition.y},
					dest, 0.05, 0.05,  40, RED);
		}
	EndMode3D();
#if DEBUG_MODE 
	if(state->debug.ship2dRep) {
		BeginMode2D({
				.offset = {WIDTH/2, HEIGHT/2},
				.target = state->ship.shipPosition,
				.rotation = 0,
				.zoom = state->debug.zoom2d,
				});

			float x = std::sin(state->ship.shipRotation);
			float y = std::cos(state->ship.shipRotation);
			ClearBackground(BLACK);
			DrawCircleLinesV(state->ship.shipPosition, 1, WHITE);
			DrawLineV(state->ship.shipPosition,
					Vector2Add(state->ship.shipPosition, Vector2Scale({x,y}, 10)), WHITE);
			DrawLineV(state->ship.shipPosition,
					Vector2Add(state->ship.shipPosition,
						Vector2Scale(state->ship.shipVelocity, 1/GetFrameTime())), WHITE);
			DrawCircleLinesV(state->debug.laserEdge, 1, GREEN);
			DrawCircleLinesV(state->debug.laserCollide, 1, YELLOW);
			for(auto i: state->ship.asteroids) {
				DrawCircleV(i.pos, i.getSphereRad() * state->debug.multiplier, RED);
			}

		EndMode2D();
	}
#endif
	// ship UI here 
	DrawRectangle(0, 0, 200, HEIGHT, BROWN);
	DrawRectangle(WIDTH-200, 0, 200, HEIGHT, BROWN);

	DrawCircleLinesV({100, 100}, 80, ColorBrightness(BROWN, 0.5));
	DrawTexturePro(state->textures.shipUiArrow,
			{0, 0, 16, 16}, {100,100,64,64}, {32,32}, -state->ship.shipRotation * RAD2DEG, WHITE);
	for(auto i: state->ship.asteroids) {
		Vector2 pos = Vector2Subtract(i.pos, state->ship.shipPosition);
		float dist = sqrt((pos.x * pos.x) + (pos.y * pos.y));
		float angle = atan2(pos.x, pos.y);
		float x = sin(angle + (180 * DEG2RAD));
		float y = cos(angle + (180 * DEG2RAD));
		DrawCircle(100 + (x * 80), 100 + (y * 80), (float)i.size*1.5, dist <= 2.5 + i.getSphereRad() ? RED : BLACK);
	}

	DrawRectangle(WIDTH-(200-40),
			(200 - (200 * state->ship.airBreakCharge)) + 30,
			113,
			(200 * state->ship.airBreakCharge), BLUE);
	DrawTexturePro(state->textures.shipAirbreakGauge,
			{0,0,32,64}, {WIDTH-(200-32), 0, 64*2, 128*2}, {0,0}, 0, WHITE);

	DrawRectangle(WIDTH-(200-40),
			(200 - (200 * state->ship.laserCharge)) + 30 + (HEIGHT - 300),
			113,
			(200 * state->ship.laserCharge), RED);
	DrawTexturePro(state->textures.shipLaserGauge,
			{0,0,32,64}, {WIDTH-(200-32), HEIGHT - 300, 64*2, 128*2}, {0,0}, 0, WHITE);

	DrawCircleLinesV({100, HEIGHT - 100}, 80, ColorBrightness(BROWN, 0.5));
	DrawLine(100, HEIGHT-100 , 100 + -(state->ship.shipVelocity.x * 200), HEIGHT-100, RED);
	DrawLine(100, HEIGHT-100, 100, (HEIGHT - 100) + -(state->ship.shipVelocity.y * 200), GREEN);

	DrawText(
			TextFormat("%d\n__\n\n\n%d",
					state->ship.asteroids.size(),
					state->ship.originalAsteroidsSize),
			WIDTH - (200-32), (HEIGHT/2) - 50, 50, BLACK);
	
	if(state->transition.active) {
		if(state->transition.onPeak == STATION_TO_SHIP) {
			float mod = state->transition.transitionTime / state->transition.maxTransitionTime;
			DrawRectangle(0, 0, 2000,2000, {0,0,0,(unsigned char)(mod*255)});
		}
		else if(state->transition.onPeak == SHIP_TO_STATION) {
			float mod = abs(state->transition.transitionTime - state->transition.maxTransitionTime)
				/ state->transition.maxTransitionTime;
			DrawRectangle(0, 0, 2000, 2000, {0,0,0,(unsigned char)((float)mod*255)});
		} else if(state->transition.transitionTime >= 0 && state->ship.asteroids.empty()) {

		}
	}
}

void transitionToShip(GameState *state, int asteroidCount, int burrowedCount) {
	state->transition.active = true;
	state->transition.maxTransitionTime = 1;
	state->transition.transitionTime = 1;
	state->transition.onPeak = STATION_TO_SHIP;

	state->ship.originalAsteroidsSize = asteroidCount;
	for(int i = 0; i < asteroidCount; i++) {
		int tempSize = (rand() % 5) + 1;
		state->ship.asteroids.push_back(ShipAsteroid {
			.pos = {(float)(rand() % 200) - 100,(float)(rand() % 200) - 100},
			.velocity = {0,0},
			.cosAsteroidRotation = {RAND_FLOAT, RAND_FLOAT, RAND_FLOAT},
			.burrowed = false,
			.size = tempSize,
			.durability = (float)tempSize + 0.9f,
		});
	}
	for(int i = 0; i < burrowedCount; i++) {
		int tempSize = (rand() % 5) + 1;
		state->ship.asteroids.push_back(ShipAsteroid {
			.pos = {(float)(rand() % 200) - 100,(float)(rand() % 200) - 100},
			.velocity = {0,0},
			.cosAsteroidRotation = {RAND_FLOAT, RAND_FLOAT, RAND_FLOAT},
			.burrowed = true,
			.size = tempSize,
			.durability = 1 + 0.9f,
		});
	}
}

void ShipAsteroid::DrawAst(GameState* state, Camera3D* cam) {
	Frustum frus;
	ExtractFrustum(&frus);
	int divisor = 0;
#if DEBUG_MODE 
	//auto hb = state->debug.asteroidHitboxSizes;
#endif	
	float radius = getSphereRad();
	switch(size) {
		case 1:
			if(burrowed) SETMODEL(model, &state->models.size1AstBurrowed);
			else SETMODEL(model, &state->models.size1Ast);
			divisor = 5;
			break;
		case 2:
			if(burrowed) SETMODEL(model, &state->models.size2AstBurrowed);
			else SETMODEL(model, &state->models.size2Ast);
			divisor = 4;
			break;
		case 3:
			if(burrowed) SETMODEL(model, &state->models.size3AstBurrowed);
			else SETMODEL(model, &state->models.size3Ast);
			divisor = 3;
			break;
		case 4:
			if(burrowed) SETMODEL(model, &state->models.size4AstBurrowed);
			else SETMODEL(model, &state->models.size4Ast);
			divisor = 2;
			break;
		case 5:
			if(burrowed) SETMODEL(model, &state->models.size4AstBurrowed);
			else SETMODEL(model, &state->models.size4Ast);
			divisor = 1;
			break;
		default:
			model = &state->models.errorFallback;
			break;
	}
	if(SphereInFrustumV(&frus, {pos.x, 0, pos.y}, radius)) {
		Vector3 temp = {pos.x, 0, pos.y};
		if(lasered) {
			temp = Vector3Add(temp, {RAND_FLOAT/10, RAND_FLOAT/10, RAND_FLOAT/10});
		}
		DrawModelEx(*model, temp, {cosAsteroidRotation.x, cosAsteroidRotation.y, cosAsteroidRotation.z}, GetTime() * 10, Vector3Scale(Vector3One(), 2.5f / divisor), WHITE);
#if DEBUG_MODE
		if(state->debug.drawHitboxes)
			DrawSphereWires({pos.x, 0, pos.y}, radius, 50, 50, RED);
		EndMode3D();
		Vector2 screenPos = GetWorldToScreen({pos.x, 0, pos.y}, *cam);
		Vector2 distvec = Vector2Subtract(pos, state->ship.shipPosition);
		float dist = sqrt((distvec.x * distvec.x) + (distvec.y + distvec.y));
		DrawText(
				TextFormat("size: %d, dura: %.01f, dist: %0.2f", size, durability, dist),
				screenPos.x, screenPos.y, 12, WHITE
				);
		BeginMode3D(*cam);
#endif
	}
	else {
	}
}

float ShipAsteroid::getSphereRad() {
	switch(size) {
		case 1: return .37;
		case 2: return .62; 
		case 3: return 1;
		case 4: return 2;
		case 5: return 4.75;
		default: return 0;
	}
}
