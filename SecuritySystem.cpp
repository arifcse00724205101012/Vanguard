#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <windows.h>
#include "glut.h"
#include "Security/SecuritySystem.h"
#include "Game/GameState.h"
#include "Map/Map.h"
#include "Units/SecurityUnit.h"
#include "Units/EnemyUnit.h"
#include "UI/UI.h"

// iGraphics function declarations
void iSetColor(double r, double g, double b);
void iFilledRectangle(double left, double bottom, double dx, double dy);
void iRectangle(double left, double bottom, double dx, double dy);
void iLine(double x1, double y1, double x2, double y2);
void iPolygon(double x[], double y[], int n);
void iFilledCircle(double x, double y, double r, int slices = 100);
void iCircle(double x, double y, double r, int slices = 100);
void iText(double x, double y, char *str, void *font);
unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int texture);

// ---- Unit Sprite Textures ----
static unsigned int g_texSoldier      = 0;
static unsigned int g_texEnemy        = 0;
static unsigned int g_texEnemyFleeing = 0;
static unsigned int g_texCamp         = 0;
static bool         g_unitSpritesLoaded = false;

static unsigned int secSafeLoad(const char *rel) {
    if (!rel) return 0;
    unsigned int t = iLoadImage((char *)rel);
    if (t) return t;
    char alt[512];
    sprintf(alt, "Images/%s", rel);
    t = iLoadImage(alt); if (t) return t;
    sprintf(alt, "E:/Vanguard Project/Vanguard Project/%s", rel);
    t = iLoadImage(alt); if (t) return t;
    return 0;
}

static void ensureUnitSprites() {
    if (g_unitSpritesLoaded) return;
    g_texSoldier      = secSafeLoad("Images/Soldier.png");
    g_texEnemy        = secSafeLoad("Images/Enemy.png");
    g_texEnemyFleeing = secSafeLoad("Images/Enemy_fleeing.png");
    g_texCamp         = secSafeLoad("Images/camp.png");
    g_unitSpritesLoaded = true;
    printf("[Vanguard Units] Soldier=%u Enemy=%u Fleeing=%u Camp=%u\n",
           g_texSoldier, g_texEnemy, g_texEnemyFleeing, g_texCamp);
}

static void showSprite(int cx, int cy, int w, int h, unsigned int tex) {
    if (!tex) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    iShowImage(cx - w/2, cy - h/2, w, h, tex);
    glDisable(GL_BLEND);
}

void triggerCombatEvent(GameState &state, double x, double y, const char *msg, bool isSecurityWin) {
    int idx = -1;
    for (int i = 0; i < GameState::MAX_COMBAT_EVENTS; ++i) {
        if (!state.combatEvents[i].isActive) {
            idx = i;
            break;
        }
    }

    if (idx == -1) idx = 0; // overwrite oldest

    CombatEvent &ev = state.combatEvents[idx];
    ev.id = idx + 1;
    ev.posX = x;
    ev.posY = y;
    ev.duration = 45;
    ev.maxDuration = 45;
    ev.isSecurityVictorious = isSecurityWin;
    ev.isActive = true;
    strncpy(ev.message, msg, 63);

    if (idx >= state.totalCombatEvents) {
        state.totalCombatEvents = idx + 1;
    }
}

void startCombatEngagement(GameState &state, int secUnitId, int eneUnitId, int campId) {
    if (secUnitId < 0 || secUnitId >= state.totalSecurityUnits) return;
    SecurityUnit &sec = state.securityUnits[secUnitId];
    if (!sec.isActive || sec.inCombat) return;

    if (campId != -1) {
        if (campId < 0 || campId >= state.totalEnemyCamps) return;
        EnemyCamp &camp = state.enemyCamps[campId];
        if (camp.isDestroyed || camp.inCombat) return;
    } else {
        if (eneUnitId < 0 || eneUnitId >= state.totalEnemyUnits) return;
        EnemyUnit &ene = state.enemyUnits[eneUnitId];
        if (!ene.isActive || ene.inCombat) return;
    }

    int idx = -1;
    for (int i = 0; i < GameState::MAX_ENGAGEMENTS; ++i) {
        if (!state.engagements[i].isActive) {
            idx = i;
            break;
        }
    }
    if (idx == -1) return; // Max engagements reached

    CombatEngagement &eng = state.engagements[idx];
    eng.id = idx + 1;
    eng.securityUnitId = secUnitId;
    eng.enemyUnitId = eneUnitId;
    eng.enemyCampId = campId;
    eng.zoneId = sec.currentZoneId;
    eng.posX = sec.posX;
    eng.posY = sec.posY;
    eng.secStrength = sec.strength;
    eng.secMaxStrength = sec.maxStrength;

    if (campId != -1) {
        EnemyCamp &camp = state.enemyCamps[campId];
        eng.eneStrength = camp.strength;
        eng.eneMaxStrength = camp.maxStrength;
        camp.inCombat = true;
    } else {
        EnemyUnit &ene = state.enemyUnits[eneUnitId];
        eng.eneStrength = ene.strength;
        eng.eneMaxStrength = ene.maxStrength;
        ene.inCombat = true;
    }

    sec.inCombat = true;
    sec.isMoving = false;
    eng.isActive = true;

    if (idx >= state.totalEngagements) {
        state.totalEngagements = idx + 1;
    }
}

void updateCombatEngagements(GameState &state) {
    for (int i = 0; i < state.totalEngagements; ++i) {
        CombatEngagement &eng = state.engagements[i];
        if (!eng.isActive) continue;

        if (eng.securityUnitId < 0 || eng.securityUnitId >= state.totalSecurityUnits) {
            eng.isActive = false;
            continue;
        }

        SecurityUnit &sec = state.securityUnits[eng.securityUnitId];
        if (!sec.isActive) {
            eng.isActive = false;
            continue;
        }

        // Check if fighting camp or enemy unit
        bool fightingCamp = (eng.enemyCampId != -1);
        EnemyCamp *camp = fightingCamp ? &state.enemyCamps[eng.enemyCampId] : NULL;
        EnemyUnit *ene = !fightingCamp ? &state.enemyUnits[eng.enemyUnitId] : NULL;

        if (fightingCamp && (!camp || camp->isDestroyed)) {
            eng.isActive = false;
            sec.inCombat = false;
            continue;
        }
        if (!fightingCamp && (!ene || !ene->isActive)) {
            eng.isActive = false;
            sec.inCombat = false;
            continue;
        }

        // --- Continuous Multi-Tick Combat Calculation (Rebel Inc Style) ---
        double damageTick = 0.12; // Paced continuous damage per frame

        // Air Strike Bonus: +35% damage if Air Strike op is active in this zone
        double airBonus = 1.0;
        if (eng.zoneId >= 0 && eng.zoneId < state.totalZones) {
            if (state.zones[eng.zoneId].operations[13].state == OP_STATE_ACTIVE) {
                airBonus = 1.35;
            }
        }

        // Roads Synergy: +15% per Roads level
        double roadsBonus = 1.0;
        if (eng.zoneId >= 0 && eng.zoneId < state.totalZones) {
            int rLvl = (state.zones[eng.zoneId].operations[11].state == OP_STATE_ACTIVE) ? state.zones[eng.zoneId].operations[11].level : 0;
            roadsBonus += 0.15 * rLvl;
        }

        // Supporting Security Troops Bonus: +25% per additional security unit in this zone
        int supportingTroops = 0;
        for (int s = 0; s < state.totalSecurityUnits; ++s) {
            if (s != eng.securityUnitId && state.securityUnits[s].isActive && state.securityUnits[s].currentZoneId == eng.zoneId) {
                supportingTroops++;
            }
        }
        double supportMult = 1.0 + (supportingTroops * 0.25);

        // Security damage to Enemy
        double secDamage = (sec.strength / 100.0) * 0.85 * damageTick * airBonus * roadsBonus * supportMult;
        // Enemy damage to Security
        double enemyPwr = fightingCamp ? camp->strength : ene->strength;
        double eneDamage = (enemyPwr / 100.0) * 0.45 * damageTick;

        // Apply Damage
        sec.strength = fmax(0.0, sec.strength - eneDamage);
        eng.secStrength = sec.strength;

        if (fightingCamp) {
            camp->strength = fmax(0.0, camp->strength - secDamage);
            eng.eneStrength = camp->strength;
        } else {
            ene->strength = fmax(0.0, ene->strength - secDamage);
            eng.eneStrength = ene->strength;
        }

        // --- Check Battle Resolution ---

        // 1. Camp Cleared & Liberated
        if (fightingCamp && camp->strength <= 0.0) {
            camp->isDestroyed = true;
            camp->inCombat = false;
            sec.inCombat = false;
            eng.isActive = false;

            // Massive Rewards
            state.funds += 80.0;
            if (eng.zoneId >= 0 && eng.zoneId < state.totalZones) {
                Zone &z = state.zones[eng.zoneId];
                z.security = fmin(100.0, z.security + 25.0);
                z.stability = fmin(100.0, z.stability + 20.0);
                z.playerControl = true;
                z.enemyPresence = false;
            }
            triggerCombatEvent(state, eng.posX, eng.posY, "CAMP LIBERATED! (+$80)", true);
            continue;
        }

        // 2. Insurgents Defeated or Tactically Retreating (Rebel Inc style!)
        if (!fightingCamp && ene->strength <= 8.0) {
            ene->inCombat = false;
            sec.inCombat = false;
            sec.state = UNIT_IDLE;
            eng.isActive = false;

            // Check retreat options using region connectivity graph
            int fleeZone = selectEnemyRetreatTarget(state, eng.zoneId);

            if (fleeZone != -1 && ene->strength > 0.0 && supportingTroops < 2) {
                // Tactical Single-Direction Group Retreat to adjacent safe zone
                for (int e = 0; e < state.totalEnemyUnits; ++e) {
                    EnemyUnit &otherEne = state.enemyUnits[e];
                    if (otherEne.isActive && otherEne.currentZoneId == eng.zoneId) {
                        otherEne.inCombat = false;
                        otherEne.targetZoneId = fleeZone;
                        otherEne.targetX = state.zones[fleeZone].centerX + (rand() % 24 - 12);
                        otherEne.targetY = state.zones[fleeZone].centerY + (rand() % 24 - 12);
                        otherEne.isMoving = true;
                        otherEne.state = UNIT_RETREATING;
                        otherEne.speed = 0.85;
                        otherEne.moveTimer = 20;
                    }
                }

                // Region is liberated by security forces
                if (eng.zoneId >= 0 && eng.zoneId < state.totalZones) {
                    Zone &z = state.zones[eng.zoneId];
                    z.security = fmin(100.0, z.security + 10.0);
                    z.stability = fmin(100.0, z.stability + 6.0);
                    z.playerControl = true;
                    z.enemyPresence = false;
                }
                triggerCombatEvent(state, eng.posX, eng.posY, "INSURGENTS FORCED TO RETREAT!", true);
            } else {
                // Insurgents cornered / wiped out! No valid retreat region exists
                for (int e = 0; e < state.totalEnemyUnits; ++e) {
                    EnemyUnit &otherEne = state.enemyUnits[e];
                    if (otherEne.isActive && otherEne.currentZoneId == eng.zoneId) {
                        otherEne.isActive = false;
                        otherEne.inCombat = false;
                    }
                }

                if (eng.zoneId >= 0 && eng.zoneId < state.totalZones) {
                    Zone &z = state.zones[eng.zoneId];
                    z.security = fmin(100.0, z.security + 15.0);
                    z.stability = fmin(100.0, z.stability + 10.0);
                    z.playerControl = true;
                    z.enemyPresence = false;
                }
                triggerCombatEvent(state, eng.posX, eng.posY, "CORNERED INSURGENTS ELIMINATED!", true);
            }
            continue;
        }

        // 3. Security Unit Overwhelmed
        if (sec.strength <= 0.0) {
            sec.isActive = false;
            sec.inCombat = false;
            sec.isSelected = false;
            sec.state = UNIT_IDLE;
            if (fightingCamp) camp->inCombat = false;
            else ene->inCombat = false;
            eng.isActive = false;

            if (state.selectedSecurityUnitId == eng.securityUnitId) state.selectedSecurityUnitId = -1;

            if (eng.zoneId >= 0 && eng.zoneId < state.totalZones) {
                Zone &z = state.zones[eng.zoneId];
                z.stability = fmax(0.0, z.stability - 10.0);
                z.security = fmax(0.0, z.security - 12.0);
            }
            triggerCombatEvent(state, eng.posX, eng.posY, "SECURITY UNIT DESTROYED!", false);
            continue;
        }
    }
}

void resolveCombat(GameState &state) {
    if (!state.isInsurgencyUnlocked) return;

    // Detect proximity and initiate continuous engagements
    for (int s = 0; s < state.totalSecurityUnits; ++s) {
        SecurityUnit &sec = state.securityUnits[s];
        if (!sec.isActive || sec.inCombat) continue;

        // 1. Check against Enemy Units
        for (int e = 0; e < state.totalEnemyUnits; ++e) {
            EnemyUnit &ene = state.enemyUnits[e];
            if (!ene.isActive || ene.inCombat) continue;

            double dx = sec.posX - ene.posX;
            double dy = sec.posY - ene.posY;
            double dist = sqrt(dx * dx + dy * dy);

            // Automatic combat when forces meet in the same zone or close proximity
            if (dist <= 35.0 || (sec.currentZoneId == ene.currentZoneId && (dist <= 65.0 || (!sec.isMoving && !ene.isMoving)))) {
                sec.state = UNIT_COMBAT;
                ene.state = UNIT_COMBAT;
                startCombatEngagement(state, s, e, -1);
                break;
            }
        }

        // 2. Check against Enemy Camps
        for (int c = 0; c < state.totalEnemyCamps; ++c) {
            EnemyCamp &camp = state.enemyCamps[c];
            if (camp.isDestroyed || camp.inCombat) continue;

            double dx = sec.posX - camp.posX;
            double dy = sec.posY - camp.posY;
            double dist = sqrt(dx * dx + dy * dy);

            if (dist <= 40.0 || (sec.currentZoneId == camp.zoneId && !sec.isMoving)) {
                camp.isDiscovered = true;
                sec.state = UNIT_COMBAT;
                startCombatEngagement(state, s, -1, c);
                break;
            }
        }
    }
}

void updateUnitsAndMovement(GameState &state) {
    // 1. Move Security Units (Module 9)
    updateSecurityUnitsMovement(state);

    // 2. Move Enemy Units (Module 10)
    updateEnemyUnitsMovement(state);

    // 3. Visibility / Fog-of-War Discovery Check (Module 10)
    for (int e = 0; e < state.totalEnemyUnits; ++e) {
        EnemyUnit &ene = state.enemyUnits[e];
        if (!ene.isActive) continue;

        bool found = false;
        if (ene.currentZoneId >= 0 && ene.currentZoneId < state.totalZones) {
            if (state.zones[ene.currentZoneId].security >= 55.0) {
                found = true;
            }
        }

        for (int s = 0; s < state.totalSecurityUnits; ++s) {
            if (!state.securityUnits[s].isActive) continue;
            double dx = ene.posX - state.securityUnits[s].posX;
            double dy = ene.posY - state.securityUnits[s].posY;
            if (sqrt(dx * dx + dy * dy) <= 120.0) {
                found = true;
                break;
            }
        }

        ene.isVisible = found;
    }

    for (int c = 0; c < state.totalEnemyCamps; ++c) {
        EnemyCamp &camp = state.enemyCamps[c];
        if (camp.isDestroyed || camp.isDiscovered) continue;

        for (int s = 0; s < state.totalSecurityUnits; ++s) {
            if (!state.securityUnits[s].isActive) continue;
            if (state.securityUnits[s].currentZoneId == camp.zoneId) {
                camp.isDiscovered = true;
                break;
            }
            double dx = camp.posX - state.securityUnits[s].posX;
            double dy = camp.posY - state.securityUnits[s].posY;
            if (sqrt(dx * dx + dy * dy) <= 110.0) {
                camp.isDiscovered = true;
                break;
            }
        }
    }

    // 4. Dynamic Zone Enemy Presence Tracking (Module 10)
    for (int z = 0; z < state.totalZones; ++z) {
        int enemiesInZone = 0;
        for (int e = 0; e < state.totalEnemyUnits; ++e) {
            if (state.enemyUnits[e].isActive && state.enemyUnits[e].currentZoneId == z) {
                enemiesInZone++;
            }
        }
        for (int c = 0; c < state.totalEnemyCamps; ++c) {
            if (!state.enemyCamps[c].isDestroyed && state.enemyCamps[c].zoneId == z) {
                enemiesInZone++;
            }
        }

        state.zones[z].enemyPresence = (enemiesInZone > 0);
    }

    // 5. Initiate & Update Continuous Combat Engagements (Module 11)
    resolveCombat(state);
    updateCombatEngagements(state);

    // 6. Update Combat Event & Alert Durations
    if (state.insurgencyAlertTimer > 0) {
        state.insurgencyAlertTimer--;
    }

    for (int i = 0; i < state.totalCombatEvents; ++i) {
        if (state.combatEvents[i].isActive) {
            state.combatEvents[i].duration--;
            if (state.combatEvents[i].duration <= 0) {
                state.combatEvents[i].isActive = false;
            }
        }
    }
}

void renderUnitsAndCombatOnMap(const GameState &state) {
    char buf[64];

    // 1. Draw Destination Line for Moving Security Units (Module 9)
    for (int s = 0; s < state.totalSecurityUnits; ++s) {
        const SecurityUnit &sec = state.securityUnits[s];
        if (sec.isActive && sec.isMoving) {
            // Draw cyan destination trajectory line
            iSetColor(0, 220, 255);
            iLine(sec.posX, sec.posY, sec.targetX, sec.targetY);

            // Draw target waypoint circle at destination
            iSetColor(0, 255, 200);
            iCircle(sec.targetX, sec.targetY, 8);
            iCircle(sec.targetX, sec.targetY, 3);
        }
    }

    // 1b. Draw Rebel Inc-Style Region Dragging Feedback (Module 9 Dragging)
    if (state.isDraggingTroop && state.dragTroopId >= 0 && state.dragTroopId < state.totalSecurityUnits) {
        const SecurityUnit &dragUnit = state.securityUnits[state.dragTroopId];
        if (dragUnit.isActive) {
            int targetZone = findZoneAt(state, (int)state.dragTargetX, (int)state.dragTargetY);
            bool isAdjacent = (targetZone != -1 && areZonesConnected(dragUnit.currentZoneId, targetZone));

            // Highlight all valid adjacent deployment options in cool cyan
            if (dragUnit.currentZoneId >= 0 && dragUnit.currentZoneId < state.totalZones) {
                const Zone &curZ = state.zones[dragUnit.currentZoneId];
                for (int a = 0; a < curZ.adjacentCount; ++a) {
                    int adjId = curZ.adjacentZones[a];
                    if (adjId >= 0 && adjId < state.totalZones && adjId != targetZone) {
                        const Zone &az = state.zones[adjId];
                        iSetColor(52, 152, 219);
                        iPolygon((double *)az.polyX, (double *)az.polyY, az.polyCount);
                    }
                }
            }

            char dragBuf[64];

            if (targetZone != -1 && isAdjacent) {
                // VALID ADJACENT TARGET REGION (Vibrant Green)
                const Zone &tz = state.zones[targetZone];
                iSetColor(46, 204, 113);
                iPolygon((double *)tz.polyX, (double *)tz.polyY, tz.polyCount);

                // Double outline for glowing effect
                double tempX[Zone::MAX_POLY_POINTS], tempY[Zone::MAX_POLY_POINTS];
                for (int k = 0; k < tz.polyCount; ++k) {
                    double dirX = tz.polyX[k] - tz.centerX;
                    double dirY = tz.polyY[k] - tz.centerY;
                    tempX[k] = tz.polyX[k] + (dirX > 0 ? 2.0 : -2.0);
                    tempY[k] = tz.polyY[k] + (dirY > 0 ? 2.0 : -2.0);
                }
                iPolygon(tempX, tempY, tz.polyCount);

                // Movement Line from Unit to Target Region Center
                iSetColor(46, 204, 113);
                iLine(dragUnit.posX, dragUnit.posY, tz.centerX, tz.centerY);

                // Targeting Reticle at destination region center
                iCircle(tz.centerX, tz.centerY, 14);
                iCircle(tz.centerX, tz.centerY, 5);
                iLine(tz.centerX - 18, tz.centerY, tz.centerX + 18, tz.centerY);
                iLine(tz.centerX, tz.centerY - 18, tz.centerX, tz.centerY + 18);

                sprintf(dragBuf, "Deploy to: %s [VALID]", tz.name);
                drawBadge(state.dragTargetX - 70, state.dragTargetY + 20, 140, 20, dragBuf, 39, 174, 96);
            } else if (targetZone != -1 && targetZone == dragUnit.currentZoneId) {
                // CURRENT REGION (Amber)
                iSetColor(241, 196, 15);
                iLine(dragUnit.posX, dragUnit.posY, state.dragTargetX, state.dragTargetY);
                iCircle(state.dragTargetX, state.dragTargetY, 10);

                sprintf(dragBuf, "Current Region: %s", state.zones[targetZone].name);
                drawBadge(state.dragTargetX - 70, state.dragTargetY + 20, 140, 20, dragBuf, 241, 196, 15);
            } else if (targetZone != -1 && !isAdjacent) {
                // INVALID: NOT CONNECTED (Warning Red)
                const Zone &tz = state.zones[targetZone];
                iSetColor(231, 76, 60);
                iPolygon((double *)tz.polyX, (double *)tz.polyY, tz.polyCount);

                iLine(dragUnit.posX, dragUnit.posY, state.dragTargetX, state.dragTargetY);
                iCircle(state.dragTargetX, state.dragTargetY, 12);
                iLine(state.dragTargetX - 10, state.dragTargetY - 10, state.dragTargetX + 10, state.dragTargetY + 10);
                iLine(state.dragTargetX - 10, state.dragTargetY + 10, state.dragTargetX + 10, state.dragTargetY - 10);

                sprintf(dragBuf, "Cannot Reach: %s [NOT CONNECTED]", tz.name);
                drawBadge(state.dragTargetX - 95, state.dragTargetY + 20, 190, 20, dragBuf, 231, 76, 60);
            } else {
                // Outside Map (Muted)
                iSetColor(120, 130, 145);
                iLine(dragUnit.posX, dragUnit.posY, state.dragTargetX, state.dragTargetY);
                drawBadge(state.dragTargetX - 75, state.dragTargetY + 20, 150, 20, "[DRAG TO ADJACENT REGION]", 100, 110, 125);
            }
        }
    }

    // 2. Draw Enemy Camps (Module 10)
    ensureUnitSprites();
    for (int c = 0; c < state.totalEnemyCamps; ++c) {
        const EnemyCamp &camp = state.enemyCamps[c];
        if (camp.isDestroyed) continue;

        if (camp.isDiscovered) {
            if (g_texCamp) {
                showSprite(camp.posX, camp.posY, 36, 36, g_texCamp);
            } else {
                // Fallback
                iSetColor(180, 20, 20);
                iFilledRectangle(camp.posX - 14, camp.posY - 14, 28, 28);
                iSetColor(255, 80, 80);
                iRectangle(camp.posX - 14, camp.posY - 14, 28, 28);
                iSetColor(255, 255, 255);
                iText(camp.posX - 12, camp.posY - 4, (char *)"CAMP", GLUT_BITMAP_HELVETICA_10);
            }
            sprintf(buf, "Pwr: %.0f", camp.strength);
            drawBadge(camp.posX - 22, camp.posY + 20, 44, 14, buf, 231, 76, 60);
        }
    }

    // 3. Draw Enemy Insurgent Units (Module 10)
    for (int e = 0; e < state.totalEnemyUnits; ++e) {
        const EnemyUnit &ene = state.enemyUnits[e];
        if (!ene.isActive || !ene.isVisible) continue;

        // If retreating, draw retreat trajectory line
        if (ene.state == UNIT_RETREATING && ene.isMoving) {
            iSetColor(243, 156, 18);
            iLine(ene.posX, ene.posY, ene.targetX, ene.targetY);
            iCircle(ene.targetX, ene.targetY, 6);
        }

        // Choose sprite: fleeing or normal enemy
        unsigned int enemyTex = (ene.state == UNIT_RETREATING) ? g_texEnemyFleeing : g_texEnemy;
        if (enemyTex) {
            showSprite((int)ene.posX, (int)ene.posY, 28, 28, enemyTex);
        } else {
            // Fallback circles
            if (ene.state == UNIT_RETREATING) {
                iSetColor(180, 100, 30);
            } else {
                iSetColor(200, 35, 35);
            }
            iFilledCircle(ene.posX, ene.posY, 12);
            iSetColor(255, 100, 100);
            iCircle(ene.posX, ene.posY, 12);
            iSetColor(255, 255, 255);
            iText(ene.posX - 4, ene.posY - 4, (char *)"E", GLUT_BITMAP_HELVETICA_12);
        }

        // Health / Strength bar
        double barW = 28.0;
        double ratio = (ene.maxStrength > 0) ? (ene.strength / ene.maxStrength) : 1.0;
        if (ratio > 1.0) ratio = 1.0;
        if (ratio < 0.0) ratio = 0.0;

        iSetColor(40, 20, 20);
        iFilledRectangle(ene.posX - (barW / 2), ene.posY + 16, barW, 4);
        iSetColor(231, 76, 60);
        iFilledRectangle(ene.posX - (barW / 2), ene.posY + 16, barW * ratio, 4);

        if (ene.state == UNIT_RETREATING) {
            drawBadge(ene.posX - 30, ene.posY + 24, 60, 14, "FLEEING", 243, 156, 18);
        }
    }

    // 4. Draw Player Security Units (Module 9)
    for (int s = 0; s < state.totalSecurityUnits; ++s) {
        const SecurityUnit &sec = state.securityUnits[s];
        if (!sec.isActive) continue;

        // Golden selection halo if selected
        if (sec.isSelected) {
            iSetColor(255, 215, 0);
            iCircle(sec.posX, sec.posY, 19);
            iCircle(sec.posX, sec.posY, 20);
        }

        // Draw Soldier sprite (or fallback blue circle)
        if (g_texSoldier) {
            showSprite((int)sec.posX, (int)sec.posY, 28, 28, g_texSoldier);
        } else {
            iSetColor(30, 130, 230);
            iFilledCircle(sec.posX, sec.posY, 13);
            iSetColor(100, 210, 255);
            iCircle(sec.posX, sec.posY, 13);
            iSetColor(255, 255, 255);
            sprintf(buf, "%d", sec.id);
            iText(sec.posX - 4, sec.posY - 4, buf, GLUT_BITMAP_HELVETICA_12);
        }

        // Strength bar below unit sprite
        double barW = 28.0;
        double ratio = (sec.maxStrength > 0) ? (sec.strength / sec.maxStrength) : 1.0;
        if (ratio > 1.0) ratio = 1.0;
        if (ratio < 0.0) ratio = 0.0;

        iSetColor(20, 35, 50);
        iFilledRectangle(sec.posX - (barW / 2), sec.posY - 18, barW, 4);
        iSetColor(46, 204, 113);
        iFilledRectangle(sec.posX - (barW / 2), sec.posY - 18, barW * ratio, 4);
    }

    // 5. Draw Continuous Rebel Inc Style Combat Engagements (Module 11)
    for (int i = 0; i < state.totalEngagements; ++i) {
        const CombatEngagement &eng = state.engagements[i];
        if (!eng.isActive) continue;

        // Animated firefight sparks and clash circle
        static int animFrame = 0;
        animFrame++;
        iSetColor(255, 215, 0);
        iCircle(eng.posX, eng.posY, 20 + (animFrame % 6));
        iSetColor(255, 50, 50);
        iCircle(eng.posX, eng.posY, 14);

        // Crossed swords / crosshair battle indicator
        iLine(eng.posX - 14, eng.posY - 14, eng.posX + 14, eng.posY + 14);
        iLine(eng.posX - 14, eng.posY + 14, eng.posX + 14, eng.posY - 14);

        // Live Dual Battle Health Bar (Rebel Inc Style: Blue Security HP vs Red Insurgent HP)
        double totalBarW = 74.0;
        double barH = 7.0;
        double barX = eng.posX - (totalBarW / 2.0);
        double barY = eng.posY + 26.0;

        // Background container
        iSetColor(15, 20, 30);
        iFilledRectangle(barX - 2, barY - 2, totalBarW + 4, barH + 4);
        iSetColor(50, 70, 95);
        iRectangle(barX - 2, barY - 2, totalBarW + 4, barH + 4);

        // Blue Security Strength Bar (Left side)
        double secRatio = (eng.secMaxStrength > 0) ? (eng.secStrength / eng.secMaxStrength) : 0.0;
        if (secRatio > 1.0) secRatio = 1.0;
        if (secRatio < 0.0) secRatio = 0.0;
        iSetColor(41, 128, 185);
        iFilledRectangle(barX, barY, (totalBarW / 2.0) * secRatio, barH);

        // Red Insurgent Strength Bar (Right side)
        double eneRatio = (eng.eneMaxStrength > 0) ? (eng.eneStrength / eng.eneMaxStrength) : 0.0;
        if (eneRatio > 1.0) eneRatio = 1.0;
        if (eneRatio < 0.0) eneRatio = 0.0;
        iSetColor(231, 76, 60);
        iFilledRectangle(barX + (totalBarW / 2.0) + ((totalBarW / 2.0) * (1.0 - eneRatio)), barY, (totalBarW / 2.0) * eneRatio, barH);

        // Dividing center line
        iSetColor(255, 255, 255);
        iLine(barX + (totalBarW / 2.0), barY - 2, barX + (totalBarW / 2.0), barY + barH + 2);

        // Combat status label
        iSetColor(255, 220, 50);
        iText(barX - 8, barY + barH + 3, (char *)"[ENGAGED IN COMBAT]", GLUT_BITMAP_HELVETICA_10);
    }

    // 6. Draw Combat Events & Popups (Module 11)
    for (int i = 0; i < state.totalCombatEvents; ++i) {
        const CombatEvent &ev = state.combatEvents[i];
        if (!ev.isActive) continue;

        // Floating combat banner text
        drawBadge(ev.posX - 75, ev.posY + 32, 150, 22, ev.message,
                  ev.isSecurityVictorious ? 46 : 231,
                  ev.isSecurityVictorious ? 204 : 76,
                  ev.isSecurityVictorious ? 113 : 60);
    }
}

