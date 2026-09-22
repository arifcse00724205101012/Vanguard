#ifndef ENEMY_UNIT_H
#define ENEMY_UNIT_H

#include "Units/SecurityUnit.h"

// Enemy Insurgent Unit (Module 10)
struct EnemyUnit {
    int id;
    char name[32];
    double posX, posY;
    double targetX, targetY;
    int currentZoneId;
    int targetZoneId;
    double strength;
    double maxStrength;
    bool isMoving;
    bool isVisible;
    bool isActive;
    bool inCombat;
    int combatEngagementId;
    double speed;
    int moveTimer;
    UnitState state;
};

// Enemy Camp / Stronghold (Module 10)
struct EnemyCamp {
    int id;
    char name[32];
    double posX, posY;
    int zoneId;
    double strength;
    double maxStrength;
    bool isDiscovered;
    bool isDestroyed;
    bool inCombat;
    int spawnTimer;
};

struct GameState;

void initEnemiesAndCamps(GameState &state);
void updateEnemyAI(GameState &state);
void updateEnemyUnitsMovement(GameState &state);
int selectEnemyExpansionTarget(const GameState &state, int fromZoneId);
int selectEnemyRetreatTarget(const GameState &state, int fromZoneId);

#endif // ENEMY_UNIT_H
