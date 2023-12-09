#include <cstdlib>
#include <ctime>
#include <raylib.h>
#include "rayImGui/imgui.h"
#include "rayImGui/rlImGui.h"
#include "game.h"


#define DEBUG_MODE 1

int main() {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(1920, 1080, "Asteroid Miner");
	InitAudioDevice();

	srand(time(NULL));

	randomizeRotations(rotations);
#if defined(PLATFORM_WEB)
	SetExitKey(NULL);
#endif
	SetTargetFPS(60);
#if DEBUG_MODE
	rlImGuiSetup(true);
	bool drawTestTex = true;
	bool drawImGuiDemo = false;
	Texture testTex = LoadTexture("resources/web/manBase.png");
	Rectangle source, dest = {};
	Vector2 origin = {};
	float rotation = 0;
#endif
	GameState state;

	state.gameState = STATE_TITLE;
	state.station.playerPosition = {0,0};
	state.station.stationLevel = TEST_LEVEL;
	state.station.stationState = WALK;
	state.station.dialogue = {
		.timeToNextChar = 0,
	};
	state.transition = {};
	state.station.anim.currentCycle = 3;
#ifdef PLATFORM_WEB
	state.textures.buttons = LoadTexture("resources/web/e_button.png");
#else
	state.textures.buttons = LoadTexture("resources/tilemap_white.png");
#endif
	state.textures.basePlayer = LoadTexture("resources/web/manBase.png");

	Camera2D stationCam = {
		.offset = {(float)GetScreenWidth()/2, (float)GetScreenHeight()/2},
		.target = state.station.playerPosition,
		.rotation = 0.0f,
		.zoom = 10.0f,
	};

	state.models.size5Ast = LoadModel("resources/web/asteroids.obj");

	while(!WindowShouldClose()) {
		stationCam.target = {0, -10};
		BeginDrawing();
			if(state.gameState == GAME_STATE::STATE_TITLE) {
				DoTitle(&state);
			}
			if(state.gameState == GAME_STATE::STATE_STATION) {
				DoStation(&state, &stationCam);
			}
#if DEBUG_MODE
			if(drawTestTex) {
				BeginMode2D(stationCam);
					DrawTexturePro(testTex,  source, dest, origin, rotation, WHITE);
				EndMode2D();
			}
			rlImGuiBegin();
				ImGui::Begin("Debug");
					ImGui::Text("FPS: %d", GetFPS());
					ImGui::Text("State: %u", state.gameState);
					ImGui::Checkbox("Show ImGui Demo Window", &drawImGuiDemo);
					if(ImGui::TreeNode("Station")) {
						ImGui::Text("station player pos: %.02f %.02f", 
								state.station.playerPosition.x, state.station.playerPosition.y);
						ImGui::Text("station state: %d", state.station.stationState);
						ImGui::Checkbox("draw test tex",&drawTestTex);
						if(drawTestTex) {
							ImGui::DragFloat4("Source", &source.x);
							ImGui::DragFloat4("Dest", &dest.x);
							ImGui::DragFloat2("Origin", &origin.x);
						}
						if(ImGui::TreeNode("Dialogue")) {
							ImGui::Text("num: %d", state.station.dialogue.num);
							ImGui::Text("fullM: %s", state.station.dialogue.fullMessage);
							ImGui::Text("dispM: %s", state.station.dialogue.displayMessage);
							ImGui::Text("chars: %d", state.station.dialogue.chars);
							ImGui::Text("maxChars: %d", state.station.dialogue.maxChars);
							ImGui::Text("timeNextChar: %f", state.station.dialogue.timeToNextChar);
							ImGui::Text("nextDialog: %d", state.station.dialogue.nextDialog);
							ImGui::Text("ready: %d", state.station.dialogue.readyToAdvance);
							ImGui::Text("disengaged: %d", state.station.dialogue.alreadyDisengaged);
							ImGui::TreePop();
						}
						ImGui::TreePop();
					}
				ImGui::End();
				if(drawImGuiDemo) {
					ImGui::ShowDemoWindow();
				}
			rlImGuiEnd();
#endif
		EndDrawing();
	}

#if DEBUG_MODE
	rlImGuiShutdown();
	UnloadTexture(testTex);
#endif
	UnloadTexture(state.textures.buttons);
	CloseAudioDevice();
	CloseWindow();
}
