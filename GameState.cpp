#define _CRT_SECURE_NO_WARNINGS
#include "Game/GameState.h"
#include "Map/Map.h"
#include "Units/EnemyUnit.h"
#include "Economy/Economy.h"
#include "UI/MainMenu.h"

void initGameState(GameState &state) {
    state.currentScreen = SCREEN_MENU;
    state.sidePanelView = VIEW_ZONE_DETAILS;
    state.selectedOpCategory = OP_CAT_WELFARE;
    state.selectedOpIndex = 0;
    state.selectedOpLevel = 1;
    
    state.gameDay = 1;
    state.gameMonth = 1;
    state.gameYear = 1;
    state.gameSpeed = 1;
    
    state.funds = 350.0;
    state.baseRevenue = 15.0;
    state.operationRevenue = 0.0;
    state.totalRevenue = 15.0;
    state.operationMaintenance = 0.0;
    state.securityMaintenance = 0.0;
    state.totalMaintenance = 0.0;
    state.netIncome = 15.0;
    
    state.selectedZoneId = 5; // Default select Central City (Zone 6)
    state.selectedSecurityUnitId = -1;
    state.isDraggingTroop = false;
    state.dragTroopId = -1;
    state.dragTargetX = 0;
    state.dragTargetY = 0;
    state.isWon = false;
    state.isLost = false;
    
    initZones(state);
    initEnemiesAndCamps(state);
    calculateEconomy(state);
    calculateOverallMetrics(state);
}

void changeGameScreen(GameState &state, GameScreen newScreen) {
    if (state.currentScreen == SCREEN_MENU && newScreen != SCREEN_MENU) {
        stopMenuMusic();
    }
    state.currentScreen = newScreen;
}
