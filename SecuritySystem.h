#ifndef SECURITY_SYSTEM_H
#define SECURITY_SYSTEM_H

// Continuous Combat Engagement (Rebel Inc Style)
struct CombatEngagement {
    int id;
    int securityUnitId;
    int enemyUnitId;
    int enemyCampId; // -1 if unit vs unit
    double posX, posY;
    int zoneId;
    double secStrength;
    double eneStrength;
    double secMaxStrength;
    double eneMaxStrength;
    bool isActive;
};

// Visual Combat Event Effect (Module 11)
struct CombatEvent {
    int id;
    double posX, posY;
    int duration;
    int maxDuration;
    char message[64];
    bool isSecurityVictorious;
    bool isActive;
};

struct GameState;

void triggerCombatEvent(GameState &state, double x, double y, const char *msg, bool isSecurityWin);
void startCombatEngagement(GameState &state, int secUnitId, int eneUnitId, int campId = -1);
void updateCombatEngagements(GameState &state);
void resolveCombat(GameState &state);
void updateUnitsAndMovement(GameState &state);
void renderUnitsAndCombatOnMap(const GameState &state);

#endif // SECURITY_SYSTEM_H
