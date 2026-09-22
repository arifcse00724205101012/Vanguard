#define _CRT_SECURE_NO_WARNINGS
#include "Units/SecurityUnit.h"
#include "Game/GameState.h"
#include "Map/Map.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

bool deploySecurityUnit(GameState &state, int zoneId) {
    if (zoneId < 0 || zoneId >= state.totalZones) return false;
    if (state.totalSecurityUnits >= GameState::MAX_SECURITY_UNITS) return false;
    
    // Locked before Day 20 (Rebel Inc style)
    if (!state.isInsurgencyUnlocked && state.gameDay < 20 && state.gameMonth == 1 && state.gameYear == 1) {
        return false;
    }

    // Check funds ($40 deployment cost)
    if (state.funds < 40.0) return false;
    
    // Zone must be secured under player control
    if (!state.zones[zoneId].playerControl) return false;

    // Deduct Funds
    state.funds -= 40.0;

    int idx = state.totalSecurityUnits++;
    SecurityUnit &unit = state.securityUnits[idx];

    unit.id = idx + 1;
    sprintf(unit.name, "Unit #%d", unit.id);

    // Calculate strength based on Troops operation level in this zone
    int troopsLevel = (state.zones[zoneId].operations[12].state == OP_STATE_ACTIVE) ? state.zones[zoneId].operations[12].level : 1;
    unit.strength = 35.0 + (troopsLevel * 15.0);
    unit.maxStrength = unit.strength;

    // Spawn at zone center with slight offset
    unit.posX = state.zones[zoneId].centerX + (rand() % 24 - 12);
    unit.posY = state.zones[zoneId].centerY + (rand() % 24 - 12);
    unit.targetX = unit.posX;
    unit.targetY = unit.posY;
    unit.currentZoneId = zoneId;
    unit.targetZoneId = zoneId;
    unit.isMoving = false;
    unit.isSelected = false;
    unit.isActive = true;
    unit.inCombat = false;
    unit.combatEngagementId = -1;
    unit.speed = 1.2;
    unit.state = UNIT_IDLE;

    // Immediate security boost in zone
    state.zones[zoneId].security = fmin(100.0, state.zones[zoneId].security + 5.0);

    // Select the deployed unit
    selectSecurityUnit(state, idx);

    return true;
}

void selectSecurityUnit(GameState &state, int unitId) {
    for (int i = 0; i < state.totalSecurityUnits; ++i) {
        state.securityUnits[i].isSelected = false;
    }

    if (unitId >= 0 && unitId < state.totalSecurityUnits && state.securityUnits[unitId].isActive) {
        state.securityUnits[unitId].isSelected = true;
        state.selectedSecurityUnitId = unitId;
    } else {
        state.selectedSecurityUnitId = -1;
    }
}

bool orderSecurityUnitMoveToZone(GameState &state, int unitId, int targetZoneId) {
    if (unitId < 0 || unitId >= state.totalSecurityUnits) return false;
    SecurityUnit &u = state.securityUnits[unitId];
    if (!u.isActive || u.inCombat) return false;
    if (targetZoneId < 0 || targetZoneId >= state.totalZones) return false;

    // Only allow movement to valid connected neighboring regions
    if (!areZonesConnected(u.currentZoneId, targetZoneId)) {
        return false;
    }

    u.targetZoneId = targetZoneId;
    u.targetX = state.zones[targetZoneId].centerX;
    u.targetY = state.zones[targetZoneId].centerY;
    u.isMoving = true;
    u.state = UNIT_MOVING;
    return true;
}

void orderSecurityUnitMove(GameState &state, int unitId, double destX, double destY) {
    int targetZone = findZoneAt(state, (int)destX, (int)destY);
    if (targetZone != -1) {
        orderSecurityUnitMoveToZone(state, unitId, targetZone);
    }
}

int findSecurityUnitAt(const GameState &state, int mouseX, int mouseY) {
    for (int i = 0; i < state.totalSecurityUnits; ++i) {
        const SecurityUnit &u = state.securityUnits[i];
        if (!u.isActive) continue;

        double dx = u.posX - mouseX;
        double dy = u.posY - mouseY;
        double dist = sqrt(dx * dx + dy * dy);

        if (dist <= 22.0) {
            return i;
        }
    }
    return -1;
}

void updateSecurityUnitsMovement(GameState &state) {
    for (int i = 0; i < state.totalSecurityUnits; ++i) {
        SecurityUnit &u = state.securityUnits[i];
        if (!u.isActive || u.inCombat) continue;

        if (u.isMoving) {
            u.state = UNIT_MOVING;
            double dx = u.targetX - u.posX;
            double dy = u.targetY - u.posY;
            double dist = sqrt(dx * dx + dy * dy);

            if (dist <= u.speed) {
                u.posX = u.targetX;
                u.posY = u.targetY;
                u.isMoving = false;
                u.state = UNIT_IDLE;
                u.currentZoneId = u.targetZoneId;
            } else {
                u.posX += (dx / dist) * u.speed;
                u.posY += (dy / dist) * u.speed;

                int z = findZoneAt(state, (int)u.posX, (int)u.posY);
                if (z != -1) {
                    u.currentZoneId = z;
                }
            }
        } else {
            u.state = UNIT_IDLE;
        }
    }
}
