#define _CRT_SECURE_NO_WARNINGS
#include "Conditions/GameConditions.h"
#include "Game/GameState.h"
#include "Security/SecuritySystem.h"
#include <string.h>

void checkInsurgencyUnlock(GameState &state) {
    if (!state.isInsurgencyUnlocked && (state.gameDay >= 20 || state.gameMonth > 1 || state.gameYear > 1)) {
        state.isInsurgencyUnlocked = true;
        state.insurgencyAlertTimer = 180; // 3 seconds alert banner

        // Spawn initial outbreak insurgent cells on Day 20
        state.totalEnemyUnits = 2;

        // Insurgent 0 (in Shadowwood)
        state.enemyUnits[0].id = 1;
        strncpy(state.enemyUnits[0].name, "Shadow Militia", 31);
        state.enemyUnits[0].currentZoneId = 7;
        state.enemyUnits[0].targetZoneId = 7;
        state.enemyUnits[0].posX = 210;
        state.enemyUnits[0].posY = 120;
        state.enemyUnits[0].targetX = 210;
        state.enemyUnits[0].targetY = 120;
        state.enemyUnits[0].strength = 28.0;
        state.enemyUnits[0].maxStrength = 28.0;
        state.enemyUnits[0].isMoving = false;
        state.enemyUnits[0].isVisible = false;
        state.enemyUnits[0].isActive = true;
        state.enemyUnits[0].inCombat = false;
        state.enemyUnits[0].combatEngagementId = -1;
        state.enemyUnits[0].speed = 0.5;
        state.enemyUnits[0].moveTimer = 6;

        // Insurgent 1 (in Highpeak)
        state.enemyUnits[1].id = 2;
        strncpy(state.enemyUnits[1].name, "Highpeak Guerillas", 31);
        state.enemyUnits[1].currentZoneId = 8;
        state.enemyUnits[1].targetZoneId = 8;
        state.enemyUnits[1].posX = 570;
        state.enemyUnits[1].posY = 150;
        state.enemyUnits[1].targetX = 570;
        state.enemyUnits[1].targetY = 150;
        state.enemyUnits[1].strength = 24.0;
        state.enemyUnits[1].maxStrength = 24.0;
        state.enemyUnits[1].isMoving = false;
        state.enemyUnits[1].isVisible = false;
        state.enemyUnits[1].isActive = true;
        state.enemyUnits[1].inCombat = false;
        state.enemyUnits[1].combatEngagementId = -1;
        state.enemyUnits[1].speed = 0.5;
        state.enemyUnits[1].moveTimer = 8;

        triggerCombatEvent(state, state.zones[5].centerX, state.zones[5].centerY, "INSURGENCY OUTBREAK DETECTED!", false);
    }
}
