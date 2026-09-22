#ifndef SECURITY_UNIT_H
#define SECURITY_UNIT_H

enum UnitState {
    UNIT_IDLE,
    UNIT_MOVING,
    UNIT_ENGAGING,
    UNIT_COMBAT,
    UNIT_RETREATING
};

// Security Unit (Module 9)
struct SecurityUnit {
    int id;
    char name[32];
    double posX, posY;
    double targetX, targetY;
    int currentZoneId;
    int targetZoneId;
    double strength;
    double maxStrength;
    bool isSelected;
    bool isMoving;
    bool isActive;
    bool inCombat;
    int combatEngagementId;
    double speed;
    UnitState state;
};

struct GameState;

bool deploySecurityUnit(GameState &state, int zoneId);
void selectSecurityUnit(GameState &state, int unitId);
bool orderSecurityUnitMoveToZone(GameState &state, int unitId, int targetZoneId);
void orderSecurityUnitMove(GameState &state, int unitId, double destX, double destY);
int findSecurityUnitAt(const GameState &state, int mouseX, int mouseY);
void updateSecurityUnitsMovement(GameState &state);

#endif // SECURITY_UNIT_H
