#include "game.h"
#include "frustum/Frustum.h"
#include <cstdio>
#include <cstring>

#include <raylib.h>
#include <raymath.h>

Vector3 rotation = {};

void DoTitle(GameState *state) {
  Camera3D cam = {.position = {10, 5, 0},
                  .target = {0, 0, 0},
                  .up = {0, 1, 0},
                  .fovy = 90.0f,
                  .projection = CameraProjection::CAMERA_PERSPECTIVE};

  // Title and Menu Buttons
  ClearBackground(BLACK);
  BeginMode3D(cam);
  DrawModelEx(state->models.size5AstBurrowed, {0, 0, 0}, rotation,
              GetTime() * 10, {2.5, 2.5, 2.5}, WHITE);
  EndMode3D();

  int len = MeasureText("Asteroid Miner", 48);
  int middle = len - (len / 2);
  DrawText("Asteroid Miner", (WIDTH / 2) - middle, 200, 48, {255, 0, 0, 255});

  len = MeasureText("by RadsammyT", 24);
  middle = len - (len / 2);
  DrawText("by RadsammyT", (WIDTH / 2) - middle, 250, 24, RED);

  len = MeasureText("Made for the raylib 2023-2024 SloJam", 12);
  middle = len - (len / 2);
  DrawText("Made for the raylib 2023-2024 SloJam", (WIDTH / 2) - middle, 0, 12,
           RAYWHITE);

  len = MeasureText("Press Enter to Play", 48);
  middle = len - (len / 2);
  DrawText("Press Enter to Play", (WIDTH / 2) - middle, HEIGHT - 70, 48, GRAY);

  DrawText(
      "Credts:\n\n"
      "Asteroid Models (original and modified) by Albert Buscio on Sketchfab\n"
      "Sci-Fi platform tiles by Eris on OpenGameArt.org\n"
      "Phone Ring SFX by timgormly on Freesound\n"
      "Footsteps SFX by EVRetro on Freesound\n"
      "Door SFX (modified) by MicktheMicGuy on FreeSound\n"
      "",
      0, 0, 12, RAYWHITE);

  DrawText("Instructions:\n"
           "(A/D) or (Left Arrow/Right Arrow) to walk\n"
           "(E) to interact with an object or when prompted\n"
           "(R) to 'fast-forward' dialogue\n"
           "Additional instructions will be given out in-game.",
           0, HEIGHT / 2, 12, RAYWHITE);

  if (IsKeyPressed(KEY_ENTER)) {
    state->gameState = STATE_STATION;
  }
}

void HandleState(GameState *state, Camera3D *cam) {
  if (state->gameState == GAME_STATE::STATE_STATION) {
    if (state->transition.active) {
      state->transition.transitionTime -= GetFrameTime();
      if (state->transition.transitionTime >= -0.1 &&
          state->transition.transitionTime <= 0.0 &&
          !state->transition.atPeak) {
        state->transition.atPeak = true;
        onTransitionPeak(state, cam);
      } else {
        state->transition.atPeak = false;
      }
      if (state->transition.transitionTime <
          -state->transition.maxTransitionTime) {
        state->transition.active = false;
      }
    } else if (state->station.stationState == STATION_SUBSTATE::WALK) {
      if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        state->station.playerPosition.x -= WALK_SPEED * GetFrameTime();
        state->station.anim.direction = 0;
        state->station.anim.timeToNextCycle -= GetFrameTime();
        if (state->station.anim.timeToNextCycle < 0) {
          state->station.anim.currentCycle++;
          walkSound(state);
          if (state->station.anim.currentCycle > 3) {
            state->station.anim.currentCycle = 0;
          }
          state->station.anim.timeToNextCycle = ANIM_SPEED;
        }
      } else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        state->station.playerPosition.x += WALK_SPEED * GetFrameTime();
        state->station.anim.direction = 1;
        state->station.anim.timeToNextCycle -= GetFrameTime();
        if (state->station.anim.timeToNextCycle < 0) {
          state->station.anim.currentCycle++;
          walkSound(state);
          if (state->station.anim.currentCycle > 3) {
            state->station.anim.currentCycle = 0;
          }
          state->station.anim.timeToNextCycle = ANIM_SPEED;
        }
      } else {
        state->station.anim.currentCycle = 4;
        state->station.anim.timeToNextCycle = ANIM_SPEED;
      }
    }
    if (state->station.stationState == STATION_SUBSTATE::DIALOG) {
      if (IsKeyPressed(KEY_R)) {
        state->station.dialogue.chars = state->station.dialogue.maxChars;
      }
      if (state->station.dialogue.timeToNextChar > 0 &&
          !state->station.dialogue.readyToAdvance)
        state->station.dialogue.timeToNextChar -= GetFrameTime();
      else {
        state->station.dialogue.timeToNextChar = 0.03;
        if (SSDP.displayMessage[SSDP.chars] == '.') {
          state->station.dialogue.timeToNextChar = 0.5;
        }
        if (SSDP.displayMessage[SSDP.chars + 1] == '\n') {
          state->station.dialogue.timeToNextChar = 0;
        }
        if (SSDP.displayMessage[SSDP.chars + 1] != ' ' &&
            SSDP.displayMessage[SSDP.chars + 1] != '\n' &&
            SSDP.chars < SSDP.maxChars) {
          switch (SSDP.dialogueSound) {
          case DIALOG_SOUND::NONE:
            break;
          case DIALOG_SOUND::AI:
            state->sounds.aiDialog.Play();
            break;
          case DIALOG_SOUND::PROTAG:
            state->sounds.protagDialog.Play();
            break;
          case DIALOG_SOUND::MANAGER:
            state->sounds.bossDialog.Play();
            break;
          case DIALOG_SOUND::PETER:
            state->sounds.peterDialog.Play();
            break;
          }
        }
        if (state->station.dialogue.chars < state->station.dialogue.maxChars)
          state->station.dialogue.chars++;
        writeToCharArr(state->station.dialogue.fullMessage,
                       state->station.dialogue.displayMessage,
                       state->station.dialogue.chars);
        if (state->station.dialogue.chars == state->station.dialogue.maxChars) {
          state->station.dialogue.readyToAdvance = true;
        }
        if (IsKeyPressed(KEY_E) && SSDP.readyToAdvance) {
          if (SSDP.nextDialog && !SSDP.alreadyDisengaged) {
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
    // for sleep state, see station.cpp:78
  }

  if (state->gameState == GAME_STATE::STATE_SHIP) {
    if (cam == NULL) {
      TraceLog(LOG_ERROR, "HandleState: Cam is NULL!\n");
      return;
    }
    if (state->transition.active) {
      state->transition.transitionTime -= GetFrameTime();
      if (state->transition.transitionTime >= -0.1 &&
          state->transition.transitionTime <= 0.0 &&
          !state->transition.atPeak) {
        state->transition.atPeak = true;
        onTransitionPeak(state, cam);
      } else {
        state->transition.atPeak = false;
      }
      if (state->transition.transitionTime <
          -state->transition.maxTransitionTime) {
        state->transition.active = false;
      }
    }
    auto ship = &state->ship;
    if (state->ship.state == SHIP_SUBSTATE::SST_ACTION) {
      if (IsKeyDown(KEY_D)) {
        ship->shipRotVelo -= 0.1 * GetFrameTime();
      }
      if (IsKeyDown(KEY_A)) {
        ship->shipRotVelo += 0.1 * GetFrameTime();
      }
      float x = std::sin(state->ship.shipRotation) * DEG2RAD;
      float z = std::cos(state->ship.shipRotation) * DEG2RAD;
      if (IsKeyDown(KEY_W)) {
        ship->shipVelocity.x += x * GetFrameTime() * 1.5;
        ship->shipVelocity.y += z * GetFrameTime() * 1.5;
      }
      if (IsKeyDown(KEY_SPACE) && ship->airBreakCharge > 0) {
        ship->shipVelocity = Vector2Scale(ship->shipVelocity, 0.950);
        ship->shipRotVelo = ship->shipRotVelo * 0.950;
        ship->airBreakCharge -= 0.75 * GetFrameTime();
      } else {
        if (!(ship->airBreakCharge >= 1) && !IsKeyDown(KEY_SPACE))
          ship->airBreakCharge += 0.1 * GetFrameTime();
      }
      if (IsKeyDown(KEY_E) && ship->laserCharge > 0) {
        ship->isLasering = true;
        ship->laserCharge -= 0.2 * GetFrameTime();
        for (int i = 0; i < ship->asteroids.size(); i++) {
          // TODO: rewrite collision with 2d in mind
          Vector2 laserEdge =
              Vector2Add(ship->shipPosition, Vector2Scale({x, z}, 100));
          for (float lerp = 0.0; lerp < 1.0; lerp += 0.01) {
            Vector2 lerpped = Vector2Lerp(ship->shipPosition, laserEdge, lerp);
#if DEBUG_MODE
            if (CheckCollisionPointCircle(lerpped, ship->asteroids[i].pos,
                                          ship->asteroids[i].getSphereRad() *
                                              state->debug.multiplier)) {
              state->debug.laserCollide = lerpped;
#else

            if (CheckCollisionPointCircle(lerpped, ship->asteroids[i].pos,
                                          ship->asteroids[i].getSphereRad())) {
#endif
              ship->asteroids[i].lasered = true;
              ship->asteroids[i].durability -= 1 * GetFrameTime();
              if (!ship->asteroids[i].burrowed) {
                if (ship->asteroids[i].size !=
                    (int)ship->asteroids[i].durability) {
                  PlaySound(state->sounds.mined);
                }
                ship->asteroids[i].size = (int)ship->asteroids[i].durability;
              }
              if (ship->asteroids[i].size == 0 ||
                  ship->asteroids[i].durability <= 0) {
                if (ship->asteroids[i].monster) {
                  ship->monster.active = true;
                  ship->monster.pos = ship->asteroids[i].pos;
                }
                for (int j = 0; j < 4; j++) {
                  Vector3 randvec = RAND_VEC3;
                  ship->particles.push_back(
                      Particle{.pos = Vector2to3XZ(ship->asteroids[i].pos),
                               .vel = Vector3Scale(randvec, 0.35),
                               .size = 1,
                               .lifetime = 5,
                               .originalLifetime = 5,
                               .tex = &state->textures.shipParticle});
                }
                PlaySound(state->sounds.explode);
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
        if (!(ship->laserCharge >= 1) && !IsKeyDown(KEY_E))
          ship->laserCharge += 0.1 * GetFrameTime();
        for (auto &i : ship->asteroids) {
          i.lasered = false;
        }
      }
      ship->shipRotation += ship->shipRotVelo;
      if (ship->shipRotation > 360 * DEG2RAD) {
        ship->shipRotation -= 360 * DEG2RAD;
      }
      if (ship->shipRotation < 0 * DEG2RAD) {
        ship->shipRotation += 360 * DEG2RAD;
      }
      ship->shipPosition = Vector2Add(ship->shipPosition, ship->shipVelocity);
      for (int i = 0; i < ship->particles.size(); i++) {
        ship->particles[i].pos =
            Vector3Add(ship->particles[i].pos,
                       Vector3Scale(ship->particles[i].vel, GetFrameTime()));
        ship->particles[i].lifetime -= GetFrameTime();
        if (ship->particles[i].lifetime <= 0) {
          ship->particles.erase(ship->particles.begin() + i);
        }
      }

      if (ship->asteroids.empty()) {
        if (CheckCollisionSpheres(Vector2to3XZ(ship->shipPosition), 1,
                                  {0, 0, -1}, 1)) {
          state->ship.state = SHIP_SUBSTATE::SST_ASCEND;
          state->ship.ascensionLerp = 0;
          state->ship.originalReturnPos = cam->position;
        }
      }
    } else if (ship->state == SHIP_SUBSTATE::SST_ASCEND) {
      if (state->ship.ascensionState == 0) {
        state->ship.ascensionLerp += 1 * GetFrameTime();
        Vector3 lerpped = Vector3Lerp(state->ship.originalReturnPos, {0, 0, 0},
                                      state->ship.ascensionLerp);
        cam->position = lerpped;
        state->ship.ascensionLerp += (1 / 3) * GetFrameTime();
        if (state->ship.ascensionLerp >= 1) {
          state->ship.ascensionState = 1;
        }
      }
      if (state->ship.ascensionState == 1) {
        cam->position.y += 1 * GetFrameTime();
        float x = std::sin(state->ship.shipRotation) * DEG2RAD;
        float z = std::cos(state->ship.shipRotation) * DEG2RAD;
        cam->target = {x, cam->position.y, z};
        if (!state->transition.active) {
          switch (PDAY) {
          case 1:
            PFLAG[4] = true;
            transitionToStationLevel(state, SHIP_BOARDING, 1, 1'03'300);
            state->transition.maxTransitionTime = 5;
            state->transition.transitionTime = 5;
            PSETPLAYERPOS(127.82);
            break;
          case 2:
            PFLAG[1] = true;
            transitionToStationLevel(state, SHIP_BOARDING, 1, 2'04'100);
            state->transition.maxTransitionTime = 5;
            state->transition.transitionTime = 5;
            PSETPLAYERPOS(127.82);
            break;
          case 3:
            break;
          }
        }
      }
    } else if (ship->state == SHIP_SUBSTATE::SST_DESCEND) {
      cam->position.y -= 1 * GetFrameTime();
      cam->target = {0, cam->position.y, 1};
      if (cam->position.y <= 0) {
        cam->position.y = 0;
        ship->state = SHIP_SUBSTATE::SST_ACTION;
      }
    }
  }
}
void writeToCharArr(const char *in, char *arr,
                    int limit) { // where limit defaults to -1
  if (limit == -1) {
    limit = strlen(in);
  }
  for (int i = 0; i <= limit + 1; i++) {
    if (i == limit)
      arr[i] = '\0';
    else
      arr[i] = in[i];
  }
}

void onTransitionPeak(GameState *state, Camera3D *cam) {
  switch (state->transition.onPeak) {
  case ON_TRANSITION_PEAK::SHIP_TO_STATION:
    state->transition.transitionTime = -state->transition.maxTransitionTime;
    DOORCLOSEP;
  case ON_TRANSITION_PEAK::STATION_LEVEL_TO_LEVEL:
    state->gameState = STATE_STATION;
    state->station.stationLevel = (STATION_LEVEL)state->transition.toLevel;
    if (state->transition.dialogAfter) {
      undertale(state, state->transition.dialogNum);
      state->transition.dialogAfter = false;
    }
    if (state->transition.closeSoundAfter) {
      DOORCLOSEP;
      state->transition.closeSoundAfter = false;
    }
    break;
  case ON_TRANSITION_PEAK::STATION_TO_SHIP:
    cam->position = {0, 5, 0};
    cam->target = {0, cam->position.y, 1};
    state->gameState = STATE_SHIP;
    state->ship.shipVelocity = Vector2Zero();
    state->ship.shipRotVelo = 0;
    state->ship.shipPosition = {0, 0};
    state->ship.shipRotation = 0;
    break;
  }
}

void SoundPack::Load(Sound in) {
  base = in;
  for (int i = 0; i < SOUNDALIAS_ARR_SIZE; i++) {
    alias[i] = LoadSoundAlias(base);
  }
}

void SoundPack::SetVolume(float vol) {
  SetSoundVolume(base, vol);
  for (auto i : alias) {
    SetSoundVolume(i, vol);
  }
}

void SoundPack::Play() {
  for (auto &i : alias) {
    if (IsSoundPlaying(i)) {
      continue;
    } else {
      PlaySound(i);
      break;
    }
  }
}

int SoundPack::soundsPlaying() {
  int res = 0;
  for (auto &i : alias) {
    if (IsSoundPlaying(i))
      res++;
  }
  return res;
}

Vector3 Vector2to3XZ(Vector2 in) { return {in.x, 0, in.y}; }
