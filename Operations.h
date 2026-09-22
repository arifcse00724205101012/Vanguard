#ifndef OPERATIONS_H
#define OPERATIONS_H

// Operation Categories (Module 4 & 5)
enum OperationCategory {
    OP_CAT_WELFARE,
    OP_CAT_ECONOMIC,
    OP_CAT_INFRASTRUCTURE,
    OP_CAT_SECURITY
};

// Operation Lifecycle States
enum OperationState {
    OP_STATE_AVAILABLE,
    OP_STATE_PURCHASED,
    OP_STATE_CONSTRUCTING,
    OP_STATE_COMPLETED,
    OP_STATE_ACTIVE,
    OP_STATE_LOCKED
};

// Generic Operation Structure (Module 4 & 6)
struct Operation {
    int id;
    char name[64];
    OperationCategory category;
    double cost;
    double maintenanceCost;
    int level;
    int maxLevel;
    int constructionDuration; // In days
    int constructionProgress; // In days completed
    OperationState state;
    
    // Performance and Zone Effects
    double stabilityBonus;
    double supportBonus;
    double economyBonus;
    double securityBonus;

    // Economic Revenue Generation (Module 8)
    double revenuePerLevel;

    // Prerequisites & Synergies (Module 6)
    int prerequisiteOpIndex; // -1 if none, or index of prerequisite op (e.g. Electricity for Internet)
    char prerequisiteName[32];
    char synergyDesc[64];
};

struct GameState;
struct Zone;

const char* getOperationCategoryName(OperationCategory cat);
void initZoneOperations(Zone &zone);
bool checkOperationPrerequisites(const GameState &state, int zoneId, int opId, char *outReason = 0);
double getOperationEffectiveRevenue(const GameState &state, const Zone &zone, const Operation &op);
double getOperationEffectiveEconomyBonus(const Zone &zone, const Operation &op);
bool canPurchaseOperation(const GameState &state, int zoneId, int opId);
bool purchaseOperation(GameState &state, int zoneId, int opId);
void updateOperationsProgress(GameState &state);

#endif // OPERATIONS_H
