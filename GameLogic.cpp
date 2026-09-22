#define _CRT_SECURE_NO_WARNINGS
#include "Game/GameLogic.h"
#include "Game/GameState.h"
#include "Conditions/GameConditions.h"
#include "Operations/Operations.h"
#include "Units/EnemyUnit.h"
#include "Economy/Economy.h"

void updateGameTime(GameState &state) {
    if (state.gameSpeed <= 0) return;
    
    state.gameDay += state.gameSpeed;
    if (state.gameDay > 30) {
        state.gameDay = 1;
        state.gameMonth++;
        if (state.gameMonth > 12) {
            state.gameMonth = 1;
            state.gameYear++;
        }
    }

    // Check Day 20 Insurgency Outbreak (Rebel Inc style)
    checkInsurgencyUnlock(state);

    // Advance operations construction
    updateOperationsProgress(state);

    // Update Enemy AI & Spawns (Module 10)
    updateEnemyAI(state);

    // Calculate economy and metrics
    calculateEconomy(state);
    calculateOverallMetrics(state);

    // Daily Economy Income Update (Module 8)
    state.funds += state.netIncome;
    if (state.funds < 0.0) {
        state.funds = 0.0; // Prevent runaway negative treasury
    }
}
