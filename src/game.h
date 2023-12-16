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

#define PFLAG state->story.flags
#define PDAY state->story.day

#define SSD state.station.dialogue
#define SSDP state->station.dialogue

#define SETMODEL(modelVar, model) modelVar = IsModelReady(*model) ? model : &state->models.errorFallback

#define PLAYERBOUNDS(MIN, MAX) \
if(state->station.playerPosition.x <= MIN)\
{state->station.playerPosition.x = MIN;}\
if(state->station.playerPosition.x >= MAX)\
{state->station.playerPosition.x = MAX;}

#define DIALOG(SPEAKER, MESSAGE, NEXTDIALOG)\
			writeToCharArr(MESSAGE, state->station.dialogue.fullMessage);\
			state->station.dialogue.maxChars = strlen(MESSAGE);\
			writeToCharArr(SPEAKER, state->station.dialogue.speaker);\
			state->station.dialogue.nextDialog = NEXTDIALOG;

#define PSETPLAYERPOS(x_pos) state->station.playerPosition.x = x_pos

#define INTERACTABLE(X, Y, W, H, DIALOGNUM) \
			if(CheckCollisionRecs({stationPlyPos.x - 23.0f, stationPlyPos.y - 32, 32, 3}, \
						{X,Y,W,H})) { \
				if(state->station.stationState != DIALOG) \
					DrawTexturePro(state->textures.buttons, KEY_E_SOURCE, \
							{stationPlyPos.x - 19, stationPlyPos.y - 50, 16, 16},  \
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

	INTRO, // room 00
	PROTAG_ROOM, // room 01
	MAIN_HALLWAY, // room 02, camera follows here. 
	SHIP_BOARDING, // room 03
	CAFETERIA, // room 04
	MANAGERS_OFFICE, // room 05
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
		float laserCharge; // also 1.00 to 0.00. about 5 seconds to use up, 10 seconds to recharge
		bool isLasering;
		RayCollision laserCollide;
		float airBreakCharge; // 1.00 to 0.00

		std::vector<ShipAsteroid> asteroids;
		int originalAsteroidsSize;
	} ship;
	struct {
		Texture buttons;
		Texture basePlayer;

		Texture testRoom;
		Texture protagRoom; 
		Texture mainHall;

		Texture shipUiArrow;
		Texture shipAirbreakGauge;
		Texture shipLaserGauge;
	} textures;
	struct {
		Model errorFallback; // Used when something went wrong when determining a model 

		Model size5Ast;
		Model size4Ast;
		Model size3Ast;
		Model size2Ast;
		Model size1Ast;

		Model station;
	} models;
	struct {
		Sound phoneRing;
		Sound doorOpen;
		Sound doorClose;
		Sound walkLeft;
		Sound walkRight;
	} sounds;
	struct {
		bool active;
		bool atPeak;
		float transitionTime;
		float maxTransitionTime;
		int onPeak;
		int toLevel;

		bool dialogAfter;
		int dialogNum;
	} transition;
	struct {
		int day = 1; // as in days 1-5
		bool flags[32]; // TODO: document flags
	} story;
#if DEBUG_MODE
	struct {
		float asteroidHitboxSizes[5];
		bool drawHitboxes = false;
		Vector2 laserPos;
		Vector2 laserEdge;
		Vector2 laserCollide;
		float multiplier = 1;
		float zoom2d = 1;
		Vector3 testCubePos;
		Vector3 testCubeSize;
		bool ship2dRep;

		bool drawTestRec = false;
		Rectangle testRec; 
	} debug;
#endif
};

struct ShipAsteroid {
	Vector2 pos;
	Vector2 velocity;
	Vector4 cosAsteroidRotation;
	int size; // see GameState.models 

	Model* model;
	float durability;
	bool lasered;
	 // Must be called during drawing in 3d mode
	void DrawAst(GameState*, Camera3D*);
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
void transitionToStationLevel(GameState*, int, bool = false, int = -1);
void onTransitionPeak(GameState*);

void writeToCharArr(const char*, char*, int = -1);
void DrawPlayerTex(GameState*);

void randomizeRotations(Vector4);

Vector3 Vector2to3XZ(Vector2);

