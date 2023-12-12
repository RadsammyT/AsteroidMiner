#pragma once

#include <raylib.h>
#include <vector>
#include "frustum/Frustum.h"

#ifndef NO_DEBUG
#define DEBUG_MODE 1
#else
#define DEBUG_MODE 0
#endif

#define HEIGHT 800
#define WIDTH 1400

#define RAND_VEC3 {(float)(rand() % 100000) / 100000, \
					(float)(rand() % 100000) / 100000, \
					(float)(rand() % 100000) / 100000}

#define TITLE_MAX_ENTRIES 3

#define SPEAKER_NARRATOR "<NARR>"

#define MAN_SPRITE_STAND_SOURCE {0, 0, 32, 32}
#define MAN_SPRITE_MIDWALK_SOURCE {32, 0, 32, 32}
#define MAN_SPRITE_WALK1_SOURCE {0, 32, 32, 32}
#define MAN_SPRITE_WALK2_SOURCE {32, 32, 32, 32}

#ifdef PLATFORM_WEB
#define KEY_E_SOURCE {0, 0, 32, 32}
#else
#define KEY_E_SOURCE {900, 32, 16, 16}
#endif


#define SSD state.station.dialogue
#define SSDP state->station.dialogue

#define SETMODEL(modelVar, model) modelVar = IsModelReady(*model) ? model : &state->models.errorFallback

#define DIALOG(SPEAKER, MESSAGE, NEXTDIALOG) \
			writeToCharArr(MESSAGE, state->station.dialogue.fullMessage); \
			state->station.dialogue.maxChars = strlen(MESSAGE); \
			writeToCharArr(SPEAKER, state->station.dialogue.speaker); \
			state->station.dialogue.nextDialog = NEXTDIALOG;

#define PSETPLAYERPOS(x_pos) state->station.playerPosition.x = x_pos

#define INTERACTABLE(X, Y, W, H, DIALOGNUM) \
			if(CheckCollisionRecs({stationPlyPos.x - 16.0f, stationPlyPos.y - 32, 32, 3}, \
						{X,Y,W,H})) { \
				if(state->station.stationState != DIALOG) \
					DrawTexturePro(state->textures.buttons, KEY_E_SOURCE, \
							{stationPlyPos.x - 12, stationPlyPos.y - 64, 16, 16},  \
							{-5, 10}, 0, WHITE); \
				if(DIALOGCHECK) { \
					undertale(state, DIALOGNUM); \
				} \
			} \

#define ENDDIALOGUEP state->station.stationState = WALK;

#define RAND_FLOAT (float)(rand()%100000) / 100000

#define ANIM_SPEED 10.0f/60.0f
#define WALK_SPEED 50

const Rectangle WALKING_CYCLE_ARR[5] = {MAN_SPRITE_WALK1_SOURCE, MAN_SPRITE_MIDWALK_SOURCE,
							MAN_SPRITE_WALK2_SOURCE, MAN_SPRITE_MIDWALK_SOURCE, MAN_SPRITE_STAND_SOURCE};

extern Vector3 rotation;

enum GAME_STATE {
	STATE_TITLE,
	STATE_STATION, // 2d
	STATE_SHIP,
};

enum STATION_STATE {
	WALK,
	DIALOG,
};

enum STATION_LEVEL {
	TEST_LEVEL,
	TEST_LEVEL_TRANSITION,
};

enum ON_TRANSITION_PEAK {
	STATION_TO_SHIP,
	SHIP_TO_STATION,
	STATION_LEVEL_TO_LEVEL,
};

struct ShipAsteroid;
struct GameState {
	GAME_STATE gameState;
	struct { // station
		Vector2 playerPosition;
		STATION_STATE stationState;
		STATION_LEVEL stationLevel;
		struct {
			bool direction; // 0 = left, 1 = right
			int currentCycle; // see WALKING_CYCLE_ARR 
			float timeToNextCycle;
		} anim;
		struct { // dialogue
			int num; // tied to what dialogue to use, 0 = no dialogue
			char speaker[64];
			char fullMessage[512];
			char displayMessage[512];
			int chars;  // current characters of current dialogue to display. see undertale
			int maxChars; // maximum characters of current dialogue. also see undertale
			float timeToNextChar;
			bool nextDialog; // Is the next dialog possible for this one?
			bool readyToAdvance;
			bool alreadyDisengaged;
		} dialogue;
	} station;
	struct { // ship
		Vector2 shipPosition;
		Vector2 shipVelocity;
		float shipRotation;
		float shipRotVelo;
		float laserCharge;
		float airBreakCharge; // 1.00 to 0.00

		std::vector<ShipAsteroid> asteroids;
	} ship;
	struct {
		Texture buttons;
		Texture basePlayer;
		Texture testRoom;
		
	} textures;
	struct {
		Model errorFallback; // Used when something went wrong when determining a model 

		Model size5Ast;
		Model size4Ast;
		Model size3Ast;
		Model size2Ast;
		Model size1Ast;
	} models;
	struct {
		bool active;
		bool atPeak;
		float transitionTime;
		float maxTransitionTime;
		int onPeak;
		int toLevel;
	} transition;
#if DEBUG_MODE
	struct {
		float asteroidHitboxSizes[5];
		bool drawHitboxes = false;
	} debug;
#endif
};

struct ShipAsteroid {
	Vector2 pos;
	Vector2 velocity;
	Vector4 cosAsteroidRotation;
	int size; // see GameState.models 

	float durability;
	 // Must be called during drawing in 3d mode
	void DrawAst(GameState*, Frustum*);
	float getSphereRad();
}; 

void DoTitle(GameState*);

void DrawStation(GameState*);
void DoStation(GameState*, Camera2D*);

void DoShip(GameState*, Camera3D*);

void CheckInteract(GameState*);
void HandleState(GameState*);
void undertale(GameState*, int);
void transitionToShip(GameState*, int);
void transitionToStationLevel(GameState*, int);
void onTransitionPeak(GameState*);

void writeToCharArr(const char*, char*, int = -1);
void DrawPlayerTex(GameState*);

void randomizeRotations(Vector4);

