#define _CRT_SECURE_NO_WARNINGS
#include "Units/EnemyUnit.h"
#include "Game/GameState.h"
#include "Map/Map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void initEnemiesAndCamps(GameState &state) {
    state.totalSecurityUnits = 0;
    state.selectedSecurityUnitId = -1;
    state.totalCombatEvents = 0;
    state.totalEngagements = 0;
    state.isInsurgencyUnlocked = false;
    state.insurgencyAlertTimer = 0;

    // 1. Initial Dormant Enemy Camps (Module 10)
    state.totalEnemyCamps = 3;

    // Camp 0: Shadowwood (Zone index 7 - Forest)
    state.enemyCamps[0].id = 1;
    strncpy(state.enemyCamps[0].name, "Shadowwood Cell", 31);
    state.enemyCamps[0].zoneId = 7;
    state.enemyCamps[0].posX = state.zones[7].centerX;
    state.enemyCamps[0].posY = state.zones[7].centerY;
    state.enemyCamps[0].strength = 45.0;
    state.enemyCamps[0].maxStrength = 45.0;
    state.enemyCamps[0].isDiscovered = false;
    state.enemyCamps[0].isDestroyed = false;
    state.enemyCamps[0].inCombat = false;
    state.enemyCamps[0].spawnTimer = 10;

    // Camp 1: Stonewall Range (Zone index 3 - Mountain)
    state.enemyCamps[1].id = 2;
    strncpy(state.enemyCamps[1].name, "Stonewall Stronghold", 31);
    state.enemyCamps[1].zoneId = 3;
    state.enemyCamps[1].posX = state.zones[3].centerX;
    state.enemyCamps[1].posY = state.zones[3].centerY;
    state.enemyCamps[1].strength = 50.0;
    state.enemyCamps[1].maxStrength = 50.0;
    state.enemyCamps[1].isDiscovered = false;
    state.enemyCamps[1].isDestroyed = false;
    state.enemyCamps[1].inCombat = false;
    state.enemyCamps[1].spawnTimer = 14;

    // Camp 2: Highpeak (Zone index 8 - Mountain)
    state.enemyCamps[2].id = 3;
    strncpy(state.enemyCamps[2].name, "Highpeak Outpost", 31);
    state.enemyCamps[2].zoneId = 8;
    state.enemyCamps[2].posX = state.zones[8].centerX;
    state.enemyCamps[2].posY = state.zones[8].centerY;
    state.enemyCamps[2].strength = 40.0;
    state.enemyCamps[2].maxStrength = 40.0;
    state.enemyCamps[2].isDiscovered = false;
    state.enemyCamps[2].isDestroyed = false;
    state.enemyCamps[2].inCombat = false;
    state.enemyCamps[2].spawnTimer = 12;

    // No roaming enemy units initially until Day 20
    state.totalEnemyUnits = 0;
}

void updateEnemyAI(GameState &state) {
    if (!state.isInsurgencyUnlocked) return;

    // 1. Enemy Camp Reinforcements (Module 10)
    for (int c = 0; c < state.totalEnemyCamps; ++c) {
        EnemyCamp &camp = state.enemyCamps[c];
        if (camp.isDestroyed || camp.inCombat) continue;

        camp.spawnTimer--;
        if (camp.spawnTimer <= 0) {
            if (state.totalEnemyUnits < GameState::MAX_ENEMY_UNITS) {
                int idx = state.totalEnemyUnits++;
                EnemyUnit &ene = state.enemyUnits[idx];

                ene.id = idx + 1;
                sprintf(ene.name, "Insurgent #%d", ene.id);
                ene.currentZoneId = camp.zoneId;
                ene.targetZoneId = camp.zoneId;
                ene.posX = camp.posX + (rand() % 20 - 10);
                ene.posY = camp.posY + (rand() % 20 - 10);
                ene.targetX = ene.posX;
                ene.targetY = ene.posY;
                ene.strength = 20.0 + (rand() % 15);
                ene.maxStrength = ene.strength;
                ene.isMoving = false;
                ene.isVisible = camp.isDiscovered;
                ene.isActive = true;
                ene.inCombat = false;
                ene.combatEngagementId = -1;
                ene.speed = 0.5;
                ene.moveTimer = 6 + (rand() % 8);
                ene.state = UNIT_IDLE;
            }
            camp.spawnTimer = 16 + (rand() % 10);
        }
    }

    // 2. Coordinated Group Movement & Expansion (Module 10 - Rebel Inc Style)
    // All enemies in the same region move together in ONE single direction.
    for (int z = 0; z < state.totalZones; ++z) {
        int enemiesInZone = 0;
        bool anyMovingOrCombat = false;
        int minTimer = 999;

        for (int e = 0; e < state.totalEnemyUnits; ++e) {
            const EnemyUnit &ene = state.enemyUnits[e];
            if (!ene.isActive || ene.currentZoneId != z) continue;

            enemiesInZone++;
            if (ene.inCombat || ene.isMoving) {
                anyMovingOrCombat = true;
                break;
            }
            if (ene.moveTimer < minTimer) {
                minTimer = ene.moveTimer;
            }
        }

        // If no idle group or group is currently in combat/already moving, skip
        if (enemiesInZone == 0 || anyMovingOrCombat) continue;

        // Decrement timer for the group in this zone
        for (int e = 0; e < state.totalEnemyUnits; ++e) {
            EnemyUnit &ene = state.enemyUnits[e];
            if (ene.isActive && !ene.inCombat && !ene.isMoving && ene.currentZoneId == z) {
                ene.moveTimer--;
            }
        }

        if (minTimer <= 1) {
            // Group evaluates adjacent regions and selects ONE single direction
            int targetZone = selectEnemyExpansionTarget(state, z);

            if (targetZone != -1 && targetZone != z) {
                // Move entire enemy group together to the selected adjacent zone
                int offsetIdx = 0;
                for (int e = 0; e < state.totalEnemyUnits; ++e) {
                    EnemyUnit &ene = state.enemyUnits[e];
                    if (ene.isActive && !ene.inCombat && !ene.isMoving && ene.currentZoneId == z) {
                        double offX = ((offsetIdx % 3) - 1) * 16.0;
                        double offY = ((offsetIdx / 3) - 1) * 16.0;
                        offsetIdx++;

                        ene.targetZoneId = targetZone;
                        ene.targetX = state.zones[targetZone].centerX + offX;
                        ene.targetY = state.zones[targetZone].centerY + offY;
                        ene.isMoving = true;
                        ene.state = UNIT_MOVING;
                        ene.moveTimer = 22 + (rand() % 10);
                    }
                }
            } else {
                // Stay in zone and reset timer
                for (int e = 0; e < state.totalEnemyUnits; ++e) {
                    EnemyUnit &ene = state.enemyUnits[e];
                    if (ene.isActive && ene.currentZoneId == z) {
                        ene.moveTimer = 16 + (rand() % 8);
                    }
                }
            }
        }
    }
}

int selectEnemyExpansionTarget(const GameState &state, int fromZoneId) {
    if (fromZoneId < 0 || fromZoneId >= state.totalZones) return -1;
    const Zone &curZone = state.zones[fromZoneId];

    int bestZone = -1;
    double bestScore = -99999.0;

    for (int i = 0; i < curZone.adjacentCount; ++i) {
        int adjId = curZone.adjacentZones[i];
        if (adjId < 0 || adjId >= state.totalZones) continue;
        const Zone &adjZone = state.zones[adjId];

        // Count security troops in adjacent zone
        int secCount = 0;
        for (int s = 0; s < state.totalSecurityUnits; ++s) {
            if (state.securityUnits[s].isActive && state.securityUnits[s].currentZoneId == adjId) {
                secCount++;
            }
        }

        // Scoring: lower security is easier to infiltrate, heavily avoid security garrisons
        double score = 100.0 - adjZone.security - (adjZone.stability * 0.4);
        if (secCount > 0) {
            score -= (secCount * 60.0);
        }
        if (adjZone.enemyPresence) {
            score += 20.0; // reinforce existing insurgency
        }
        // Forest & mountain terrain provide defensive cover
        if (adjZone.type == ZONE_FOREST || adjZone.type == ZONE_MOUNTAIN) {
            score += 15.0;
        }

        if (score > bestScore) {
            bestScore = score;
            bestZone = adjId;
        }
    }

    return bestZone;
}

int selectEnemyRetreatTarget(const GameState &state, int fromZoneId) {
    if (fromZoneId < 0 || fromZoneId >= state.totalZones) return -1;
    const Zone &curZone = state.zones[fromZoneId];

    int bestZone = -1;
    double bestScore = -99999.0;

    for (int i = 0; i < curZone.adjacentCount; ++i) {
        int adjId = curZone.adjacentZones[i];
        if (adjId < 0 || adjId >= state.totalZones) continue;
        const Zone &adjZone = state.zones[adjId];

        // Count security troops in adjacent zone
        int secCount = 0;
        for (int s = 0; s < state.totalSecurityUnits; ++s) {
            if (state.securityUnits[s].isActive && state.securityUnits[s].currentZoneId == adjId) {
                secCount++;
            }
        }

        // Cannot retreat into a region held by security units!
        if (secCount > 0) {
            continue;
        }

        // Priority 1: Adjacent region with active enemy camp
        bool hasCamp = false;
        for (int c = 0; c < state.totalEnemyCamps; ++c) {
            if (!state.enemyCamps[c].isDestroyed && state.enemyCamps[c].zoneId == adjId) {
                hasCamp = true;
                break;
            }
        }

        double score = 50.0;
        if (hasCamp) score += 60.0;
        if (adjZone.enemyPresence) score += 30.0;
        if (adjZone.type == ZONE_MOUNTAIN || adjZone.type == ZONE_FOREST) score += 25.0;
        score -= (adjZone.security * 0.4);

        if (score > bestScore) {
            bestScore = score;
            bestZone = adjId;
        }
    }

    return bestZone;
}

void updateEnemyUnitsMovement(GameState &state) {
    for (int i = 0; i < state.totalEnemyUnits; ++i) {
        EnemyUnit &e = state.enemyUnits[i];
        if (!e.isActive || e.inCombat) continue;

        if (e.isMoving) {
            double dx = e.targetX - e.posX;
            double dy = e.targetY - e.posY;
            double dist = sqrt(dx * dx + dy * dy);

            if (dist <= e.speed) {
                e.posX = e.targetX;
                e.posY = e.targetY;
                e.isMoving = false;
                e.state = UNIT_IDLE;
                e.currentZoneId = e.targetZoneId;
            } else {
                e.posX += (dx / dist) * e.speed;
                e.posY += (dy / dist) * e.speed;

                int z = findZoneAt(state, (int)e.posX, (int)e.posY);
                if (z != -1) {
                    e.currentZoneId = z;
                }
            }
        } else {
            e.state = UNIT_IDLE;
        }
    }
}
