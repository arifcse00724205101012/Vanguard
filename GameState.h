#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "Map/Zone.h"
#include "Units/SecurityUnit.h"
#include "Units/EnemyUnit.h"
#include "Security/SecuritySystem.h"
#include "Operations/Operations.h"

// Screen states
enum GameScreen {
    SCREEN_MENU,
    SCREEN_GAMEPLAY,
    SCREEN_OPERATIONS,
    SCREEN_VICTORY,
    SCREEN_DEFEAT
};

// Side Panel Views
enum SidePanelView {
    VIEW_ZONE_DETAILS,
    VIEW_OPERATIONS
};

// Central GameState structure
struct GameState {
    GameScreen currentScreen;
    SidePanelView sidePanelView;
    OperationCategory selectedOpCategory;
    int selectedOpIndex;  // Currently selected operation index (0-14)
    int selectedOpLevel;  // Currently selected level to review (1-3)

    
    // Game time tracking
    int gameDay;
    int gameMonth;
    int gameYear;
    int gameSpeed; // 0 = Paused, 1 = Normal, 2 = Fast
    
    // Core government resources & economic breakdown (Module 8)
    double funds;
    double baseRevenue;
    double operationRevenue;
    double totalRevenue;
    double operationMaintenance;
    double securityMaintenance;
    double totalMaintenance;
    double netIncome;
    
    // Overall regional metrics (0.0 to 100.0)
    double overallStability;
    double overallSupport;
    double overallEconomy;
    
    // Selection & Drag Movement State
    int selectedZoneId; // -1 if no zone is selected
    int selectedSecurityUnitId; // -1 if no unit selected
    bool isDraggingTroop;
    int dragTroopId;
    double dragTargetX, dragTargetY;
    
    // Insurgency & Pacing Phase (Day 20 Unlock - Rebel Inc Style)
    bool isInsurgencyUnlocked;
    int insurgencyAlertTimer; // Frames to display outbreak alert banner
    
    // Victory & Defeat state
    bool isWon;
    bool isLost;
    
    // 9 Zones
    static const int MAX_ZONES = 9;
    Zone zones[MAX_ZONES];
    int totalZones;

    // Security & Enemy Units (Module 9, 10, 11)
    static const int MAX_SECURITY_UNITS = 16;
    SecurityUnit securityUnits[MAX_SECURITY_UNITS];
    int totalSecurityUnits;

    static const int MAX_ENEMY_UNITS = 16;
    EnemyUnit enemyUnits[MAX_ENEMY_UNITS];
    int totalEnemyUnits;

    static const int MAX_ENEMY_CAMPS = 6;
    EnemyCamp enemyCamps[MAX_ENEMY_CAMPS];
    int totalEnemyCamps;

    static const int MAX_ENGAGEMENTS = 8;
    CombatEngagement engagements[MAX_ENGAGEMENTS];
    int totalEngagements;

    static const int MAX_COMBAT_EVENTS = 8;
    CombatEvent combatEvents[MAX_COMBAT_EVENTS];
    int totalCombatEvents;
};

void initGameState(GameState &state);
void changeGameScreen(GameState &state, GameScreen newScreen);

#endif // GAME_STATE_H
