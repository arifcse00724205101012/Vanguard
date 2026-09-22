#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "iGraphics/glut32.lib")
#pragma comment(lib, "iGraphics/glaux.lib")
#include "iGraphics.h"
#include "Game/GameState.h"
#include "Game/GameLogic.h"
#include "Map/Map.h"
#include "Units/SecurityUnit.h"
#include "Security/SecuritySystem.h"
#include "Operations/Operations.h"
#include "UI/UI.h"
#include "UI/MainMenu.h"
#include "UI/OperationsScreen.h"

// Central GameState instance
GameState g_gameState;

// Periodic game timer (1 tick per second)
void onGameTick() {
    if (g_gameState.currentScreen == SCREEN_GAMEPLAY || g_gameState.currentScreen == SCREEN_OPERATIONS) {
        updateGameTime(g_gameState);
    }
}

void iDraw() {
    renderGame(g_gameState);
}

void fixedUpdate() {
    // Continuous unit movement, animation, and combat updates (Module 9, 10, 11)
    if (g_gameState.currentScreen == SCREEN_GAMEPLAY || g_gameState.currentScreen == SCREEN_OPERATIONS) {
        updateUnitsAndMovement(g_gameState);
    }
}

void iPassiveMouseMove(int mx, int my) {
    if (g_gameState.currentScreen == SCREEN_MENU) {
        handleMainMenuMouseMove(mx, my);
        return;
    }
    if (g_gameState.currentScreen == SCREEN_OPERATIONS) {
        handleOperationsMouseMove(mx, my);
        return;
    }

    // If dragging a troop, keep drag target updated
    if (g_gameState.isDraggingTroop) {
        g_gameState.dragTargetX = (double)mx;
        g_gameState.dragTargetY = (double)my;
    }
}

void iMouseMove(int mx, int my) {
    if (g_gameState.currentScreen == SCREEN_MENU) {
        handleMainMenuMouseMove(mx, my);
        return;
    }
    if (g_gameState.currentScreen == SCREEN_OPERATIONS) {
        handleOperationsMouseMove(mx, my);
        return;
    }

    // Update live drag coordinates during active mouse drag
    if (g_gameState.isDraggingTroop) {
        g_gameState.dragTargetX = (double)mx;
        g_gameState.dragTargetY = (double)my;
    }
}

void iMouse(int button, int state, int mx, int my) {
    if (g_gameState.currentScreen == SCREEN_MENU) {
        if (state == GLUT_DOWN) {
            handleMainMenuMouseClick(g_gameState, button, mx, my);
        }
        return;
    }

    if (g_gameState.currentScreen == SCREEN_OPERATIONS) {
        handleOperationsMouseClick(g_gameState, button, state, mx, my);
        return;
    }

    if (state == GLUT_DOWN) {

        // 1. Map Area Interaction (x between 20 and 820, y between 20 and 700)
        if (mx <= 820) {
            if (button == GLUT_LEFT_BUTTON) {
                // Check if clicking on an active security unit first (Module 9 Dragging)
                int clickedUnit = findSecurityUnitAt(g_gameState, mx, my);
                if (clickedUnit != -1) {
                    selectSecurityUnit(g_gameState, clickedUnit);
                    g_gameState.isDraggingTroop = true;
                    g_gameState.dragTroopId = clickedUnit;
                    g_gameState.dragTargetX = (double)mx;
                    g_gameState.dragTargetY = (double)my;
                    return;
                }

                // Normal Map zone selection click — re-clicking a selected zone deselects it
                int clickedZone = findZoneAt(g_gameState, mx, my);
                if (clickedZone != -1) {
                    if (g_gameState.selectedZoneId == clickedZone) {
                        selectZone(g_gameState, -1); // Deselect → shows Regional Overview
                    } else {
                        selectZone(g_gameState, clickedZone);
                        selectSecurityUnit(g_gameState, -1); // Deselect units
                    }
                }
            } else if (button == GLUT_RIGHT_BUTTON) {
                // Right Click orders selected unit to move to adjacent zone
                if (g_gameState.selectedSecurityUnitId != -1) {
                    int destZone = findZoneAt(g_gameState, mx, my);
                    if (destZone != -1) {
                        orderSecurityUnitMoveToZone(g_gameState, g_gameState.selectedSecurityUnitId, destZone);
                    }
                }
            }
            return;
        }

        // 2. Side Panel Interactions (x >= 840) - Dedicated Zone Details
        if (button == GLUT_LEFT_BUTTON && mx >= 840 && g_gameState.selectedZoneId >= 0 && g_gameState.selectedZoneId < g_gameState.totalZones) {
            // "MANAGE OPERATIONS ->" button (x: 860-1160, y: 110-160)
            if (mx >= 860 && mx <= 1160 && my >= 110 && my <= 160) {
                openOperationsScreen(g_gameState);
                return;
            }

            // "+ DEPLOY TROOP ($40)" Quick Button (x: 860-1160, y: 55-105)
            if (mx >= 860 && mx <= 1160 && my >= 55 && my <= 105) {
                deploySecurityUnit(g_gameState, g_gameState.selectedZoneId);
                return;
            }
        }
    } else if (state == GLUT_UP) {
        if (button == GLUT_LEFT_BUTTON) {
            // Complete mouse drag troop movement (Module 9 Dragging - Region Based)
            if (g_gameState.isDraggingTroop && g_gameState.dragTroopId >= 0) {
                if (mx <= 820) {
                    int destZone = findZoneAt(g_gameState, mx, my);
                    if (destZone != -1) {
                        bool moved = orderSecurityUnitMoveToZone(g_gameState, g_gameState.dragTroopId, destZone);
                        if (moved) {
                            selectZone(g_gameState, destZone);
                        }
                    }
                }
                g_gameState.isDraggingTroop = false;
                g_gameState.dragTroopId = -1;
            }
        }
    }
}

void iKeyboard(unsigned char key) {
    if (g_gameState.currentScreen == SCREEN_MENU) {
        if (key == ' ' || key == 13) { // Space or Enter
            stopMenuMusic();
            changeGameScreen(g_gameState, SCREEN_GAMEPLAY);
        } else if (key == 27) { // ESC exits
            stopMenuMusic();
            exit(0);
        }
        return;
    }

    if (g_gameState.currentScreen == SCREEN_OPERATIONS) {
        if (key == 27 || key == 'b' || key == 'B') { // ESC or B returns to gameplay
            changeGameScreen(g_gameState, SCREEN_GAMEPLAY);
        }
        return;
    }

    if (key == 'r' || key == 'R') {
        initGameState(g_gameState);
    } else if (key == 'o' || key == 'O') {
        if (g_gameState.currentScreen == SCREEN_GAMEPLAY) {
            openOperationsScreen(g_gameState);
        } else if (g_gameState.currentScreen == SCREEN_OPERATIONS) {
            changeGameScreen(g_gameState, SCREEN_GAMEPLAY);
        }
    } else if (key == '1') {
        selectZone(g_gameState, 0);
    } else if (key == '2') {
        selectZone(g_gameState, 1);
    } else if (key == '3') {
        selectZone(g_gameState, 2);
    } else if (key == '4') {
        selectZone(g_gameState, 3);
    } else if (key == '5') {
        selectZone(g_gameState, 4);
    } else if (key == '6') {
        selectZone(g_gameState, 5);
    } else if (key == '7') {
        selectZone(g_gameState, 6);
    } else if (key == '8') {
        selectZone(g_gameState, 7);
    } else if (key == '9') {
        selectZone(g_gameState, 8);
    }
}

void iSpecialKeyboard(unsigned char key) {
    if (key == GLUT_KEY_RIGHT) {
        if (g_gameState.selectedZoneId < g_gameState.totalZones - 1) {
            selectZone(g_gameState, g_gameState.selectedZoneId + 1);
        }
    } else if (key == GLUT_KEY_LEFT) {
        if (g_gameState.selectedZoneId > 0) {
            selectZone(g_gameState, g_gameState.selectedZoneId - 1);
        }
    }
}

int main() {
    // 1. Initialize Game State and 9 Zones
    initGameState(g_gameState);

    // 2. Set timer callback (1 second per tick)
    iSetTimer(1000, onGameTick);

    // 3. Start iGraphics loop at 1200x780 resolution
    iInitialize(1200, 780, "Vanguard - Strategy Game (MPV)");

    return 0;
}
