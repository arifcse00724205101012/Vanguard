#define _CRT_SECURE_NO_WARNINGS
#include "Economy/Economy.h"
#include "Game/GameState.h"
#include "Operations/Operations.h"

// VANGUARD ECONOMY & REGIONAL METRICS (Module 8)

void calculateEconomy(GameState &state) {
    if (state.totalZones == 0) return;

    double baseRev = 15.0; // Base regional government tax
    double opRev = 0.0;
    double opMaint = 0.0;
    double secMaint = 0.0;

    for (int z = 0; z < state.totalZones; ++z) {
        Zone &zone = state.zones[z];
        double zoneRev = 0.0;
        double zoneMaint = 0.0;

        if (zone.playerControl) {
            baseRev += 2.5; // Additional tax revenue per secured zone
        }

        for (int i = 0; i < zone.totalOperations; ++i) {
            Operation &op = zone.operations[i];
            if (op.state == OP_STATE_ACTIVE && op.level > 0) {
                double maint = op.maintenanceCost * op.level;
                if (op.category == OP_CAT_SECURITY) {
                    secMaint += maint;
                } else {
                    opMaint += maint;
                }
                zoneMaint += maint;

                if (op.category == OP_CAT_ECONOMIC) {
                    double r = getOperationEffectiveRevenue(state, zone, op);
                    opRev += r;
                    zoneRev += r;
                }
            }
        }

        zone.zoneRevenue = zoneRev;
        zone.zoneMaintenance = zoneMaint;
        zone.zoneNetIncome = zoneRev - zoneMaint;
    }

    state.baseRevenue = baseRev;
    state.operationRevenue = opRev;
    state.totalRevenue = baseRev + opRev;
    state.operationMaintenance = opMaint;
    state.securityMaintenance = secMaint;
    state.totalMaintenance = opMaint + secMaint;
    state.netIncome = state.totalRevenue - state.totalMaintenance;
}

void calculateOverallMetrics(GameState &state) {
    if (state.totalZones == 0) return;
    
    double sumStability = 0.0;
    double sumSupport = 0.0;
    double sumEconomy = 0.0;
    
    for (int i = 0; i < state.totalZones; ++i) {
        sumStability += state.zones[i].stability;
        sumSupport += state.zones[i].support;
        sumEconomy += state.zones[i].economy;
    }
    
    state.overallStability = sumStability / state.totalZones;
    state.overallSupport = sumSupport / state.totalZones;
    state.overallEconomy = sumEconomy / state.totalZones;
}
