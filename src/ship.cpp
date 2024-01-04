#include "game.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <raylib.h>
#include <raymath.h>

void DoShip(GameState* state, Camera3D* cam) {
	if (cam == NULL) {
		TraceLog(LOG_ERROR, "DoShip: Cam is NULL!\n");
		return;
	}
	ClearBackground(BLACK);
	HandleState(state, cam);
	float x = std::sin(state->ship.shipRotation) * RAD2DEG;
	float z = std::cos(state->ship.shipRotation) * RAD2DEG;
	if (state->ship.state == SHIP_SUBSTATE::SST_ACTION) {
		cam->position = {state->ship.shipPosition.x, 0,
		                 state->ship.shipPosition.y};
		cam->target = {cam->position.x + x, 0, cam->position.z + z};
	}
	Frustum frus;

	if(state->ship.state == SST_FAILED) {
		DrawRectangle(0, 0, WIDTH, HEIGHT, {128,0,0,255});
		DrawText("YOU DIED", ((WIDTH/2) - ((MeasureText("YOU DIED", 60)/2))), HEIGHT/2, 60, WHITE);
		if(!state->ship.causeWritten) {
			switch(state->ship.deathCause) {
				case OUTOFBOUNDS:
					writeToCharArr(
							"The ship's internal bombs triggered as you tried to ditch your responsibilities.",
							state->ship.deathCauseMsg);
					break;
				case EXCESSIVESPIN:
					writeToCharArr(
							"You spinned the ship, with YOU in it, so fast that your head exploded.",
							state->ship.deathCauseMsg);
					break;
				case OBJECTCOLLISION:
					writeToCharArr(
							"Your ship hit something and it exploded. With you in it.",
							state->ship.deathCauseMsg);
					break;
			}
			state->ship.causeWritten = true;

		}
		DrawText(state->ship.deathCauseMsg, 
				((WIDTH/2) - ((MeasureText(state->ship.deathCauseMsg, 25)/2))),
				(HEIGHT/2)+(60+25), 25, WHITE);
		DrawText("Press R to restart", (WIDTH/2) - (MeasureText("Press R to restart", 25)/2),
				HEIGHT-30, 25, WHITE);
		return;
	}

	BeginMode3D(*cam);
	ExtractFrustum(&frus);
#if DEBUG_MODE
	DrawModel(state->models.errorFallback, {4, 0, 4}, 1, {255, 255, 255, 255});
	DrawCubeWiresV({0, -5, 0}, {200, 0, 200}, WHITE);
	DrawSphereWires(Vector2to3XZ(state->debug.laserPos), 0.05, 10, 10, BLUE);
	DrawSphereWires(Vector2to3XZ(state->debug.laserEdge), 0.1, 10, 10, GREEN);
	DrawSphereWires(Vector2to3XZ(state->debug.laserCollide), 0.1, 10, 10,
	                YELLOW);
	DrawCubeV(state->debug.testCubePos, state->debug.testCubeSize, RED);
#endif
	// dock cube: 0, 0, -1.030 | 2, 1, 1

	DrawModel(state->models.station, {0, -1, 0}, 1.0f, WHITE);
	for (auto i : state->ship.asteroids) {
		i.DrawAst(state, cam);
	}
	for (auto i : state->ship.particles) {
		DrawModel(state->models.size1Ast, i.pos, 0.15f, RED);
	}
	if(state->ship.monster.active) {
		DrawBillboardRec(*cam, state->textures.shipMonsterSprite, 
				{0 + (32 * (float)state->ship.monster.animIndex), 0, 32, 32},
				{state->ship.monster.pos.x, state->ship.monster.yCoord, state->ship.monster.pos.y},
				{5,5}, WHITE);
	}
	if (IsKeyDown(KEY_E) && state->ship.laserCharge > 0) {
		float x = std::sin(state->ship.shipRotation) * DEG2RAD;
		float z = std::cos(state->ship.shipRotation) * DEG2RAD;
		Vector3 dest = Vector2to3XZ(
			Vector2Add(state->ship.shipPosition, Vector2Scale({x, z}, 100)));
		dest.y = 0;
		DrawCylinderEx(
			{state->ship.shipPosition.x, -1, state->ship.shipPosition.y}, dest,
			0.025, 0.025, 40, RED);
	}
	EndMode3D();
#if DEBUG_MODE
	if (state->debug.ship2dRep) {
		BeginMode2D({
			.offset = {WIDTH / 2, HEIGHT / 2},
			.target = state->ship.shipPosition,
			.rotation = 0,
			.zoom = state->debug.zoom2d,
		});

		float x = std::sin(state->ship.shipRotation);
		float y = std::cos(state->ship.shipRotation);
		ClearBackground(BLACK);
		DrawCircleLinesV(state->ship.shipPosition, 1, WHITE);
		DrawLineV(
			state->ship.shipPosition,
			Vector2Add(state->ship.shipPosition, Vector2Scale({x, y}, 10)),
			WHITE);
		DrawLineV(state->ship.shipPosition,
		          Vector2Add(state->ship.shipPosition,
		                     Vector2Scale(state->ship.shipVelocity,
		                                  1 / GetFrameTime())),
		          WHITE);
		DrawCircleLinesV(state->debug.laserEdge, 1, GREEN);
		DrawCircleLinesV(state->debug.laserCollide, 1, YELLOW);
		for (auto i : state->ship.asteroids) {
			DrawCircleV(i.pos, i.getSphereRad() * state->debug.multiplier, RED);
		}

		EndMode2D();
	}
#endif

	// ship UI here
	DrawRectangle(0, 0, 200, HEIGHT, BROWN);
	DrawTexturePro(state->textures.shipUiLeft, {0,0,50,200}, {0,0,200,HEIGHT}, {0,0}, 0, WHITE);
	DrawRectangle(WIDTH - 200, 0, 200, HEIGHT, BROWN);
	DrawTexturePro(state->textures.shipUiRight, {0,0,50,200}, {WIDTH-200,0,200,HEIGHT}, {0,0}, 0, WHITE);

	DrawCircleLinesV({100, 100}, 80, ColorBrightness(BROWN, 0.5));
	DrawTexturePro(state->textures.shipUiArrow, {0, 0, 16, 16},
	               {100, 100, 64, 64}, {32, 32},
	               -state->ship.shipRotation * RAD2DEG, WHITE);
	for (auto i : state->ship.asteroids) {
		Vector2 pos = Vector2Subtract(i.pos, state->ship.shipPosition);
		float dist = sqrt((pos.x * pos.x) + (pos.y * pos.y));
		float angle = atan2(pos.x, pos.y);
		float x = sin(angle + (180 * DEG2RAD));
		float y = cos(angle + (180 * DEG2RAD));
		DrawCircle(100 + (x * 80), 100 + (y * 80), (float)i.size * 1.5,
		           dist <= 2.5 + i.getSphereRad() ? RED : GRAY);
	}
	if (state->ship.asteroids.empty()) {
		Vector2 pos = Vector2Subtract({0, 0}, state->ship.shipPosition);
		float angle = atan2(pos.x, pos.y);
		float x = sin(angle + (180 * DEG2RAD));
		float y = cos(angle + (180 * DEG2RAD));
		DrawCircle(100 + (x * 80), 100 + (y * 80), (float)3 * 1.5, GREEN);
	}

	DrawRectangle(1236, 25, 120, 207, GRAY);
	DrawRectangle(WIDTH - (200 - 40),
	              (200 - (200 * state->ship.airBreakCharge)) + 30, 113,
	              (200 * state->ship.airBreakCharge), BLUE);
	DrawTexturePro(state->textures.shipAirbreakGauge, {0, 0, 32, 64},
	               {WIDTH - (200 - 32), 0, 64 * 2, 128 * 2}, {0, 0}, 0, WHITE);

	DrawRectangle(1236, 524, 120, 208, GRAY);
	DrawRectangle(WIDTH - (200 - 40),
	              (200 - (200 * state->ship.laserCharge)) + 30 + (HEIGHT - 300),
	              113, (200 * state->ship.laserCharge), RED);
	DrawTexturePro(state->textures.shipLaserGauge, {0, 0, 32, 64},
	               {WIDTH - (200 - 32), HEIGHT - 300, 64 * 2, 128 * 2}, {0, 0},
	               0, WHITE);

	DrawLine(100, HEIGHT - 100, 100 + -(state->ship.shipVelocity.x * 200),
	         HEIGHT - 100, RED);
	DrawLine(100, HEIGHT - 100, 100,
	         (HEIGHT - 100) + -(state->ship.shipVelocity.y * 200), GREEN);

	if(!state->ship.asteroids.empty()) {
		DrawText(TextFormat("  %d\n___\n\n\n  %d", state->ship.asteroids.size(),
			state->ship.originalAsteroidsSize),
		 WIDTH - (200 - 50), (HEIGHT / 2) - 53, 50, GREEN);
	} else {
		DrawText("GO TO:", WIDTH-(200-32), (HEIGHT/2)-51, 40, GREEN);
		DrawCircle(1301, 407, 22, GREEN);
	}

#if DEBUG_MODE
	if (state->debug.drawTestRec) {
		DrawRectangleRec(state->debug.testRec, {255, 0, 0, 128});
	}
#endif
	if (state->transition.active) {
		if (state->transition.onPeak == STATION_TO_SHIP) {
			// fade-out at descend
			// the below commented declaration would make fade-out work on
			// desktop, but not for web, as we are going to 0..-255. Casting to
			// u8 (unsigned char) on desktop gets us 255..0 but on web, casting
			// to u8 gets us constant 0s, which is why web transitions bugs out.
			// float mod = state->transition.transitionTime /
			// state->transition.maxTransitionTime;
			float mod = (state->transition.transitionTime +
			             state->transition.maxTransitionTime) /
			            (state->transition.maxTransitionTime);
			DrawRectangle(
				0, 0, 2000, 2000,
				{0, 0, 0, static_cast<unsigned char>(((float)mod * 255))});
#if DEBUG_MODE
			DrawText(TextFormat("(%.01f | %d)", mod * 255,
			                    (unsigned char)((float)mod * 255)),
			         WIDTH / 2, 0, 50, RED);
#endif
		} else if (state->transition.onPeak == SHIP_TO_STATION) {
			float mod = abs(state->transition.transitionTime -
			                state->transition.maxTransitionTime) /
			            state->transition.maxTransitionTime;
			DrawRectangle(0, 0, 2000, 2000,
			              {0, 0, 0, (unsigned char)((float)mod * 255)});
		} else if (state->transition.transitionTime >= 0 &&
		           state->ship.asteroids.empty()) {
#if 1
			printf("FUCK\n");
#endif
		}
	}
}

void transitionToShip(GameState* state, int asteroidCount, int burrowedCount,
                      int monsterCount, bool reset) {
	state->transition.active = true;
	state->transition.maxTransitionTime = 1;
	state->transition.transitionTime = 1;
	state->transition.onPeak = STATION_TO_SHIP;
	state->ship.state = SHIP_SUBSTATE::SST_DESCEND;

	state->ship.originalAsteroidsSize = asteroidCount + burrowedCount + monsterCount;
	if(!reset) {
		state->ship.originalAsteroids[0] =  asteroidCount;
		state->ship.originalAsteroids[1] = burrowedCount;
		state->ship.originalAsteroids[2] = monsterCount;
	} 
	for (int i = 0; i < asteroidCount; i++) {
		int tempSize = (rand() % 5) + 1;
		state->ship.asteroids.push_back(ShipAsteroid{
			.pos = {(float)(rand() % 200) - 100, (float)(rand() % 200) - 100},
			.velocity = {0, 0},
			.cosAsteroidRotation = {RAND_FLOAT, RAND_FLOAT, RAND_FLOAT},
			.burrowed = false,
			.monster = false,
			.size = tempSize,
			.durability = (float)tempSize + 0.9f,
		});
	}
	for (int i = 0; i < burrowedCount; i++) {
		int tempSize = (rand() % 5) + 1;
		state->ship.asteroids.push_back(ShipAsteroid{
			.pos = {(float)(rand() % 200) - 100, (float)(rand() % 200) - 100},
			.velocity = {0, 0},
			.cosAsteroidRotation = {RAND_FLOAT, RAND_FLOAT, RAND_FLOAT},
			.burrowed = true,
			.monster = false,
			.size = tempSize,
			.durability = 1 + 0.9f,
		});
	}

	for (int i = 0; i < monsterCount; i++) {
		int tempSize = 5; // always a size 5, just to fit its sprite
		state->ship.asteroids.push_back(ShipAsteroid{
			.pos = {(float)(rand() % 200) - 100, (float)(rand() % 200) - 100},
			.velocity = {0, 0},
			.cosAsteroidRotation = {RAND_FLOAT, RAND_FLOAT, RAND_FLOAT},
			.burrowed = true,
			.monster = true,
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
	// auto hb = state->debug.asteroidHitboxSizes;
#endif
	float radius = getSphereRad();
	switch (size) {
	case 1:
		if (burrowed)
			SETMODEL(model, &state->models.size1AstBurrowed);
		else
			SETMODEL(model, &state->models.size1Ast);
		divisor = 5;
		break;
	case 2:
		if (burrowed)
			SETMODEL(model, &state->models.size2AstBurrowed);
		else
			SETMODEL(model, &state->models.size2Ast);
		divisor = 4;
		break;
	case 3:
		if (burrowed)
			SETMODEL(model, &state->models.size3AstBurrowed);
		else
			SETMODEL(model, &state->models.size3Ast);
		divisor = 3;
		break;
	case 4:
		if (burrowed)
			SETMODEL(model, &state->models.size4AstBurrowed);
		else
			SETMODEL(model, &state->models.size4Ast);
		divisor = 2;
		break;
	case 5:
		if (burrowed)
			SETMODEL(model, &state->models.size4AstBurrowed);
		else
			SETMODEL(model, &state->models.size4Ast);
		divisor = 1;
		break;
	default:
		model = &state->models.errorFallback;
		break;
	}
	if (SphereInFrustumV(&frus, {pos.x, 0, pos.y}, radius)) {
		Vector3 temp = {pos.x, 0, pos.y};
		if (lasered) {
			temp = Vector3Add(
				temp, {RAND_FLOAT / 10, RAND_FLOAT / 10, RAND_FLOAT / 10});
		}
		DrawModelEx(*model, temp,
		            {cosAsteroidRotation.x, cosAsteroidRotation.y,
		             cosAsteroidRotation.z},
		            GetTime() * 10, Vector3Scale(Vector3One(), 2.5f / divisor),
		            WHITE);
		if (monster) {
			DrawBillboardRec(*cam, state->textures.shipMonsterSprite,
			                 {0, 0, 32, 32}, {pos.x, 0, pos.y}, {5, 5},
			                 {128, 128, 128, 255});
		}
#if DEBUG_MODE
		if (state->debug.drawHitboxes)
			DrawSphereWires({pos.x, 0, pos.y}, radius, 50, 50, RED);
		EndMode3D();
		Vector2 screenPos = GetWorldToScreen({pos.x, 0, pos.y}, *cam);
		Vector2 distvec = Vector2Subtract(pos, state->ship.shipPosition);
		float dist =
			sqrt(abs(distvec.x * distvec.x) + abs(distvec.y + distvec.y));
		DrawText(TextFormat("size: %d, dura: %.01f, dist: %0.2f, b:%d, m:%d",
		                    size, durability, dist, burrowed, monster),
		         screenPos.x, screenPos.y, 12, WHITE);
		BeginMode3D(*cam);
#endif
	} else {
	}
}

float ShipAsteroid::getSphereRad() {
	switch (size) {
	case 1:
		return .37;
	case 2:
		return .62;
	case 3:
		return 1;
	case 4:
		return 2;
	case 5:
		return 4.75;
	default:
		return 0;
	}
}
