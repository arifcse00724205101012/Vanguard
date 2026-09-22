#define _CRT_SECURE_NO_WARNINGS
#include "Operations/Operations.h"
#include "Game/GameState.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

const char* getOperationCategoryName(OperationCategory cat) {
    switch (cat) {
        case OP_CAT_WELFARE: return "Welfare";
        case OP_CAT_ECONOMIC: return "Economic";
        case OP_CAT_INFRASTRUCTURE: return "Infrastructure";
        case OP_CAT_SECURITY: return "Security";
        default: return "Civilian";
    }
}

void initZoneOperations(Zone &zone) {
    zone.totalOperations = 15;

    // Structure for initializing operations
    struct OpTemplate {
        const char *name;
        OperationCategory category;
        double cost;
        double maint;
        int duration;
        int maxLvl;
        double stab;
        double supp;
        double econ;
        double sec;
        double revPerLvl;
        int prereqIdx;
        const char *prereqName;
        const char *synergy;
    };

    const OpTemplate templates[15] = {
        // --- WELFARE ---
        // 0. Education (Boosts Support & Long-term Economy)
        { "Education",      OP_CAT_WELFARE,        40.0, 2.0, 4, 3, 2.0, 5.0, 3.0, 0.0, 0.0,  -1, "", "Boosts Support & Economy" },
        // 1. Healthcare (Boosts Support & Stability)
        { "Healthcare",     OP_CAT_WELFARE,        45.0, 2.0, 4, 3, 6.0, 4.0, 0.0, 0.0, 0.0,  -1, "", "Boosts Support & Stability" },
        // 2. Food & Water (Boosts Support & Stability)
        { "Food & Water",   OP_CAT_WELFARE,        35.0, 1.0, 3, 3, 4.0, 5.0, 0.0, 0.0, 0.0,  -1, "", "Boosts Support & Stability" },
        // 3. Sanitation (Boosts Support & Stability)
        { "Sanitation",     OP_CAT_WELFARE,        30.0, 1.0, 3, 3, 4.0, 3.0, 0.0, 0.0, 0.0,  -1, "", "Boosts Support & Stability" },

        // --- ECONOMIC ---
        // 4. Industry (Urban Affinity +50%, Boosted by Roads)
        { "Industry",       OP_CAT_ECONOMIC,       60.0, 4.0, 6, 3, 0.0, 0.0, 8.0, 0.0, 14.0, -1, "", "Urban affinity (+50%). Boosted by Roads." },
        // 5. Office (Urban Affinity +50%, Boosted by Internet)
        { "Office",         OP_CAT_ECONOMIC,       50.0, 3.0, 5, 3, 0.0, 2.0, 6.0, 0.0, 11.0, -1, "", "Urban affinity (+50%). Boosted by Internet." },
        // 6. Agriculture (Rural Affinity +50%, Boosted by Roads)
        { "Agriculture",    OP_CAT_ECONOMIC,       40.0, 2.0, 4, 3, 2.0, 2.0, 5.0, 0.0,  9.0, -1, "", "Rural affinity (+50%). Boosted by Roads." },
        // 7. Trading (Boosted by Roads, Internet, Telecoms)
        { "Trading",        OP_CAT_ECONOMIC,       55.0, 3.0, 5, 3, 0.0, 0.0, 7.0, 0.0, 12.0, -1, "", "Boosted by Roads, Internet, Telecoms." },

        // --- INFRASTRUCTURE ---
        // 8. Electricity (Increases infra capacity; Prerequisite for Internet)
        { "Electricity",    OP_CAT_INFRASTRUCTURE, 50.0, 3.0, 5, 3, 4.0, 2.0, 4.0, 0.0, 0.0,  -1, "", "Required power grid for Internet." },
        // 9. Internet (Requires Electricity; Supports Office & Trading)
        { "Internet",       OP_CAT_INFRASTRUCTURE, 45.0, 2.0, 4, 3, 0.0, 3.0, 4.0, 0.0, 0.0,   8, "Electricity", "Requires Electricity. Boosts Office/Trade." },
        // 10. Telecoms (Supports Trading & Security Intel)
        { "Telecoms",       OP_CAT_INFRASTRUCTURE, 40.0, 2.0, 4, 3, 0.0, 3.0, 2.0, 3.0, 0.0,  -1, "", "Boosts Trading & Security Intel." },
        // 11. Roads (Supports Agriculture, Industry, Trading, Troop Movement)
        { "Roads",          OP_CAT_INFRASTRUCTURE, 35.0, 2.0, 3, 3, 0.0, 2.0, 3.0, 2.0, 0.0,  -1, "", "Boosts Agri, Industry, Trading, Troops." },

        // --- SECURITY ---
        // 12. Troops
        { "Troops",         OP_CAT_SECURITY,       70.0, 5.0, 3, 3, 4.0, -2.0, 0.0, 10.0, 0.0, -1, "", "Enforces security & suppresses threats." },
        // 13. Air Strike
        { "Air Strike",     OP_CAT_SECURITY,      100.0, 8.0, 1, 1, 2.0, -5.0, 0.0, 15.0, 0.0, -1, "", "Rapid tactical strike against hostile forces." },
        // 14. Radar
        { "Radar",          OP_CAT_SECURITY,       60.0, 4.0, 4, 3, 0.0,  0.0, 0.0,  8.0, 0.0, -1, "", "Provides regional intelligence coverage." }
    };

    for (int i = 0; i < 15; ++i) {
        zone.operations[i].id = i + 1;
        strncpy(zone.operations[i].name, templates[i].name, 63);
        zone.operations[i].category = templates[i].category;
        zone.operations[i].cost = templates[i].cost;
        zone.operations[i].maintenanceCost = templates[i].maint;
        zone.operations[i].level = 0;
        zone.operations[i].maxLevel = templates[i].maxLvl;
        zone.operations[i].constructionDuration = templates[i].duration;
        zone.operations[i].constructionProgress = 0;
        zone.operations[i].state = OP_STATE_AVAILABLE;
        zone.operations[i].stabilityBonus = templates[i].stab;
        zone.operations[i].supportBonus = templates[i].supp;
        zone.operations[i].economyBonus = templates[i].econ;
        zone.operations[i].securityBonus = templates[i].sec;
        zone.operations[i].revenuePerLevel = templates[i].revPerLvl;
        zone.operations[i].prerequisiteOpIndex = templates[i].prereqIdx;
        strncpy(zone.operations[i].prerequisiteName, templates[i].prereqName, 31);
        strncpy(zone.operations[i].synergyDesc, templates[i].synergy, 63);
    }
}

// GENERIC OPERATION SYSTEM & INFRASTRUCTURE SYNERGIES (Module 4 & 6)

bool checkOperationPrerequisites(const GameState &state, int zoneId, int opId, char *outReason) {
    if (zoneId < 0 || zoneId >= state.totalZones) return false;
    if (opId < 0 || opId >= state.zones[zoneId].totalOperations) return false;

    const Zone &zone = state.zones[zoneId];
    const Operation &op = zone.operations[opId];

    // Day 20 Lock for Security Operations (Rebel Inc style)
    if (op.category == OP_CAT_SECURITY && !state.isInsurgencyUnlocked && state.gameDay < 20 && state.gameMonth == 1 && state.gameYear == 1) {
        if (outReason) {
            sprintf(outReason, "Unlocks Day 20 (%d days)", 20 - state.gameDay);
        }
        return false;
    }

    if (op.prerequisiteOpIndex >= 0 && op.prerequisiteOpIndex < zone.totalOperations) {
        const Operation &reqOp = zone.operations[op.prerequisiteOpIndex];
        if (reqOp.state != OP_STATE_ACTIVE || reqOp.level < 1) {
            if (outReason) {
                sprintf(outReason, "Requires %s Lv 1+", reqOp.name);
            }
            return false;
        }
    }
    return true;
}

double getOperationEffectiveRevenue(const GameState &state, const Zone &zone, const Operation &op) {
    if (op.state != OP_STATE_ACTIVE || op.level <= 0) return 0.0;
    if (op.category != OP_CAT_ECONOMIC) return 0.0;

    double rev = op.revenuePerLevel * op.level;

    // 1. Zone Affinity Multiplier (Module 6)
    if (strcmp(op.name, "Industry") == 0 || strcmp(op.name, "Office") == 0) {
        if (zone.type == ZONE_URBAN) {
            rev *= 1.5; // +50% yield in Urban zones
        }
    } else if (strcmp(op.name, "Agriculture") == 0) {
        if (zone.type == ZONE_RURAL) {
            rev *= 1.5; // +50% yield in Rural zones
        }
    }

    // 2. Infrastructure Synergies (Module 6)
    int roadsLevel = (zone.operations[11].state == OP_STATE_ACTIVE) ? zone.operations[11].level : 0;
    int internetLevel = (zone.operations[9].state == OP_STATE_ACTIVE) ? zone.operations[9].level : 0;
    int telecomsLevel = (zone.operations[10].state == OP_STATE_ACTIVE) ? zone.operations[10].level : 0;

    double synergyBonus = 0.0;
    if (strcmp(op.name, "Agriculture") == 0) {
        synergyBonus += 0.20 * roadsLevel; // Roads support Agriculture
    } else if (strcmp(op.name, "Industry") == 0) {
        synergyBonus += 0.20 * roadsLevel; // Roads support Industry
    } else if (strcmp(op.name, "Office") == 0) {
        synergyBonus += 0.25 * internetLevel; // Internet supports Office
    } else if (strcmp(op.name, "Trading") == 0) {
        synergyBonus += (0.20 * roadsLevel + 0.20 * internetLevel + 0.20 * telecomsLevel); // Roads, Internet, Telecoms support Trading
    }

    rev *= (1.0 + synergyBonus);
    return rev;
}

double getOperationEffectiveEconomyBonus(const Zone &zone, const Operation &op) {
    double econ = op.economyBonus;
    if (strcmp(op.name, "Industry") == 0 || strcmp(op.name, "Office") == 0) {
        if (zone.type == ZONE_URBAN) econ *= 1.5;
    } else if (strcmp(op.name, "Agriculture") == 0) {
        if (zone.type == ZONE_RURAL) econ *= 1.5;
    }
    return econ;
}

bool canPurchaseOperation(const GameState &state, int zoneId, int opId) {
    if (zoneId < 0 || zoneId >= state.totalZones) return false;
    if (opId < 0 || opId >= state.zones[zoneId].totalOperations) return false;

    const Zone &zone = state.zones[zoneId];
    const Operation &op = zone.operations[opId];

    // Cannot build in contested zones without player control
    if (!zone.playerControl) return false;

    // Security operations locked before Day 20 (Rebel Inc style)
    if (op.category == OP_CAT_SECURITY && !state.isInsurgencyUnlocked && state.gameDay < 20 && state.gameMonth == 1 && state.gameYear == 1) {
        return false;
    }

    // Cannot exceed max level
    if (op.level >= op.maxLevel) return false;

    // Cannot purchase if already constructing
    if (op.state == OP_STATE_CONSTRUCTING || op.state == OP_STATE_PURCHASED) return false;

    // Check prerequisites (e.g. Internet requires Electricity)
    if (!checkOperationPrerequisites(state, zoneId, opId)) return false;

    // Check funds
    if (state.funds < op.cost) return false;

    return true;
}

bool purchaseOperation(GameState &state, int zoneId, int opId) {
    if (!canPurchaseOperation(state, zoneId, opId)) return false;

    Zone &zone = state.zones[zoneId];
    Operation &op = zone.operations[opId];

    // Deduct Funds
    state.funds -= op.cost;

    // Update State: AVAILABLE -> PURCHASED -> CONSTRUCTING
    op.state = OP_STATE_CONSTRUCTING;
    op.constructionProgress = 0;

    return true;
}

void updateOperationsProgress(GameState &state) {
    for (int z = 0; z < state.totalZones; ++z) {
        Zone &zone = state.zones[z];
        int activeCount = 0;

        for (int op = 0; op < zone.totalOperations; ++op) {
            Operation &operation = zone.operations[op];

            // Progress construction for constructing operations
            if (operation.state == OP_STATE_CONSTRUCTING) {
                operation.constructionProgress++;
                if (operation.constructionProgress >= operation.constructionDuration) {
                    operation.constructionProgress = operation.constructionDuration;
                    operation.level++;
                    operation.state = OP_STATE_ACTIVE;

                    // Apply immediate zone bonuses (with zone affinity)
                    double effEcon = getOperationEffectiveEconomyBonus(zone, operation);
                    zone.stability = fmin(100.0, zone.stability + operation.stabilityBonus);
                    zone.support = fmin(100.0, zone.support + operation.supportBonus);
                    zone.economy = fmin(100.0, zone.economy + effEcon);
                    zone.security = fmin(100.0, zone.security + operation.securityBonus);
                }
            }

            if (operation.state == OP_STATE_ACTIVE && operation.level > 0) {
                activeCount += operation.level;
            }
        }

        zone.activeOperationsCount = activeCount;
    }
}
