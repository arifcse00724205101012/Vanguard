#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <windows.h>
#include "glut.h"
#include "UI/OperationsScreen.h"
#include "Game/GameState.h"
#include "Operations/Operations.h"

// iGraphics function declarations
void iClear();
void iSetColor(double r, double g, double b);
void iFilledRectangle(double left, double bottom, double dx, double dy);
void iRectangle(double left, double bottom, double dx, double dy);
void iLine(double x1, double y1, double x2, double y2);
void iPolygon(double x[], double y[], int n);
void iFilledPolygon(double x[], double y[], int n);
void iFilledCircle(double x, double y, double r, int slices = 100);
void iCircle(double x, double y, double r, int slices = 100);
void iText(double x, double y, char *str, void *font);
unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int texture);

// External mouse coordinates maintained by iGraphics
extern int iMouseX, iMouseY;

// Hover interaction targets
enum HoverTarget {
    HOVER_NONE = 0,
    HOVER_BACK_BTN,
    HOVER_ACTION_BTN,
    HOVER_TAB_0, HOVER_TAB_1, HOVER_TAB_2, HOVER_TAB_3,
    HOVER_OP_0, HOVER_OP_1, HOVER_OP_2, HOVER_OP_3,
    HOVER_LVL_1, HOVER_LVL_2, HOVER_LVL_3
};

static HoverTarget g_hoverTarget = HOVER_NONE;

// Texture handles for Operations Screen
static bool g_opsTexturesLoaded = false;

// Wallpaper
static unsigned int g_texOpsMainBg = 0;

// Frames
static unsigned int g_texFrameOpsLeft = 0;
static unsigned int g_texFrameOpsRight = 0;

// Header
static unsigned int g_texHeaderBar = 0;
static unsigned int g_texBtnBack = 0;
static unsigned int g_texBtnBackHover = 0;
static unsigned int g_texBtnBackPressed = 0;
static unsigned int g_texTitleOps = 0;
static unsigned int g_texTitleWingLeft = 0;
static unsigned int g_texTitleWingRight = 0;
static unsigned int g_texFundsBox = 0;
static unsigned int g_texFundsCoin = 0;

// Left Panel Assets
static unsigned int g_texBadgeAvailable = 0;
static unsigned int g_texBadgeLockedBlue = 0;
static unsigned int g_texBadgeLockedRed = 0;
static unsigned int g_texTitleBook = 0;

static unsigned int g_texSectionOverview = 0;
static unsigned int g_texSectionOutcomes = 0;
static unsigned int g_texSectionUnlock = 0;
static unsigned int g_texSectionLineA = 0;

static unsigned int g_texOutcomeUp1 = 0;
static unsigned int g_texOutcomeUp2 = 0;
static unsigned int g_texOutcomeUp3 = 0;
static unsigned int g_texOutcomeDown = 0;
static unsigned int g_texOutcomeClock = 0;

static unsigned int g_texRadioOn = 0;
static unsigned int g_texRadioOff = 0;

static unsigned int g_texBtnProceed = 0;
static unsigned int g_texBtnProceedHover = 0;
static unsigned int g_texBtnProceedPressed = 0;
static unsigned int g_texCoinBig = 0;

// Right Panel Assets
static unsigned int g_texTabActive = 0;
static unsigned int g_texTabInactive = 0;
static unsigned int g_texTabInactiveHover = 0;
static unsigned int g_texTabInactivePressed = 0;

static unsigned int g_texNodeEduActive = 0;
static unsigned int g_texNodeHealthIdle = 0;
static unsigned int g_texNodeFoodIdle = 0;
static unsigned int g_texNodeSanitationIdle = 0;

// Operation Category Icons (from Images/UI/assets/icons/ops/)
static unsigned int g_texInitEdu       = 0;  // cat_education.png
static unsigned int g_texInitHealth    = 0;  // cat_health.png
static unsigned int g_texInitFood      = 0;  // cat_food.png
static unsigned int g_texInitSanit     = 0;  // cat_sanitation.png
static unsigned int g_texInitIndustry  = 0;  // cat_industry.png
static unsigned int g_texInitOffice    = 0;  // cat_office.png
static unsigned int g_texInitAgri      = 0;  // cat_agriculture.png
static unsigned int g_texInitTrading   = 0;  // cat_trading.png
static unsigned int g_texInitElectric  = 0;  // cat_electricity.png
static unsigned int g_texInitInternet  = 0;  // cat_internet.png
static unsigned int g_texInitTelecom   = 0;  // cat_telecoms.png
static unsigned int g_texInitRoads     = 0;  // cat_roads.png
static unsigned int g_texInitTroops    = 0;  // cat_troops.png
static unsigned int g_texInitAirStrike = 0;  // cat_air_strike.png
static unsigned int g_texInitRadar     = 0;  // cat_radar.png

static unsigned int g_texBannerLevels = 0;

static unsigned int g_texRingGoldBook = 0;
static unsigned int g_texRingBlueLock = 0;
static unsigned int g_texRingRedLock = 0;

static unsigned int g_texLegendRingGreen = 0;
static unsigned int g_texLegendRingBlue = 0;
static unsigned int g_texLegendRingRed = 0;

// Robust Multi-Path Image Loader
static unsigned int safeLoadImage(const char *relPath) {
    if (!relPath) return 0;
    unsigned int tex = iLoadImage((char *)relPath);
    if (tex != 0) return tex;

    char alt[512];
    sprintf(alt, "Images/%s", relPath);
    tex = iLoadImage(alt);
    if (tex != 0) return tex;

    sprintf(alt, "../%s", relPath);
    tex = iLoadImage(alt);
    if (tex != 0) return tex;

    sprintf(alt, "E:/Vanguard Project/Vanguard Project/%s", relPath);
    tex = iLoadImage(alt);
    if (tex != 0) return tex;

    return 0;
}

// Helper to render texture with full Alpha Blending
static void renderSprite(int x, int y, int width, int height, unsigned int texture) {
    if (texture == 0) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    iShowImage(x, y, width, height, texture);
    glDisable(GL_BLEND);
}

// Rich Level Metadata Structure
struct OpLevelInfo {
    const char *levelTitle;
    const char *overviewLine1;
    const char *overviewLine2;
    const char *overviewLine3;
    const char *unlockReqDesc;
};

struct OpFullMetadata {
    int opId;
    OpLevelInfo levels[3];
};

static const OpFullMetadata g_opMetadata[15] = {
    // 0: Education
    { 1, {
        { "Basic Education", "Sets up primary literacy classes in the zone's community schools.", "Low cost, low risk entry point of the Education track.", "Always available to construct across all controlled zones.", "Always unlocked" },
        { "Vocational Training", "Establishes technical training centers for skilled trade craftsmanship.", "Enhances civic support and accelerates future commercial productivity.", "Builds upon local primary education foundations.", "Requires Level 1 & Support >= 50%" },
        { "University Program", "Funds advanced tertiary education and research institutions.", "Provides massive regional stability and high-tech economic synergy.", "Crown jewel of the civilian education initiative.", "Requires Level 2 & Support >= 65%" }
    }},
    // 1: Healthcare
    { 2, {
        { "Mobile Clinics", "Deploys rapid emergency medical units and essential medicines.", "Immediately treats acute civilian illness and builds local goodwill.", "Essential foundation for public health and regional trust.", "Always unlocked" },
        { "District Hospitals", "Constructs permanent regional clinics and surgical wards.", "Substantially reduces disease mortality and boosts population stability.", "Significantly cements government legitimacy in the zone.", "Requires Level 1 & Stability >= 50%" },
        { "Advanced Medical Center", "State-of-the-art regional medical research and trauma hospital.", "Provides comprehensive specialized healthcare and deep civic loyalty.", "Guarantees enduring support and high disease resilience.", "Requires Level 2 & Stability >= 65%" }
    }},
    // 2: Food & Water
    { 3, {
        { "Clean Water Wells", "Drills deep boreholes and installs community water filtration pumps.", "Eliminates waterborne disease vectors and secures basic survival.", "High-impact low-cost initiative with rapid positive response.", "Always unlocked" },
        { "Rationing & Distribution", "Establishes secure food distribution depots and logistics hubs.", "Shields vulnerable citizens from market supply shocks and famine.", "Strengthens regional food security and civilian confidence.", "Requires Level 1 & Support >= 45%" },
        { "Water Treatment Plant", "Builds automated purification facilities and municipal water piping.", "Ensures uninterrupted clean tap water and high quality of life.", "Permanently secures civic stability across the entire district.", "Requires Level 2 & Support >= 60%" }
    }},
    // 3: Sanitation
    { 4, {
        { "Waste Collection", "Organizes municipal trash collection and safe disposal zones.", "Cleans urban streets and reduces public health hazards rapidly.", "Visible demonstration of active government municipal order.", "Always unlocked" },
        { "Sewage Network", "Lays underground wastewater culverts and drainage channels.", "Prevents urban flooding and eradicates stagnant pest pools.", "Greatly elevates sanitation standards and living quality.", "Requires Level 1 & Stability >= 45%" },
        { "Modern Treatment Facility", "Eco-friendly recycling and bio-waste processing complex.", "Advanced ecological management eliminating industrial contamination.", "Maximizes civil contentment and zone environmental health.", "Requires Level 2 & Stability >= 60%" }
    }},
    // 4: Industry
    { 5, {
        { "Light Manufacturing", "Opens workshop spaces and consumer goods processing facilities.", "Provides immediate local employment and steady baseline revenues.", "Strong synergy with urban centers (+50% yield bonus).", "Always unlocked" },
        { "Heavy Industrial Park", "Develops heavy manufacturing, metalworks, and assembly factories.", "Generates substantial economic yield and daily tax contributions.", "Dramatically boosted when supported by paved Roads network.", "Requires Level 1 & Economy >= 45%" },
        { "Automated Complex", "Cutting-edge robotic manufacturing and export-ready logistics.", "Massive revenue generator driving regional economic dominance.", "Highest tier industrial engine for self-sustaining recovery.", "Requires Level 2 & Economy >= 65%" }
    }},
    // 5: Office
    { 6, {
        { "Commercial Offices", "Refurbishes administrative buildings and local business bureaus.", "Kickstarts corporate registration and professional service work.", "Urban affinity (+50%). Boosted by regional telecommunications.", "Always unlocked" },
        { "Corporate Center", "Constructs modern office high-rises and financial service suites.", "Attracts private enterprise investment and white-collar capital.", "Highly amplified when paired with high-speed Internet.", "Requires Level 1 & Economy >= 50%" },
        { "Financial Hub", "Headquarters for regional banking, equity, and market operations.", "Transforms zone into a premier economic powerhouse.", "Generates premier recurring revenue for stabilization budgets.", "Requires Level 2 & Economy >= 70%" }
    }},
    // 6: Agriculture
    { 7, {
        { "Community Farmlands", "Supplies fertilizer, seed reserves, and irrigation ditches to farmers.", "Stabilizes rural livelihoods and guarantees food abundance.", "Rural affinity (+50%). High impact across countryside zones.", "Always unlocked" },
        { "Mechanized Farming", "Introduces tractors, combine harvesters, and mechanized silos.", "Dramatically multiplies agricultural yields and regional trade goods.", "Greatly enhanced when connected by paved transport Roads.", "Requires Level 1 & Support >= 45%" },
        { "Agro-Industrial Silos", "Commercial processing plants, refrigeration vaults, and grain elevators.", "Dominates export markets and provides deep economic resilience.", "Maximizes tax revenues and rural population prosperity.", "Requires Level 2 & Support >= 60%" }
    }},
    // 7: Trading
    { 8, {
        { "Local Market Square", "Organizes municipal bazaar stalls and wholesale trading lanes.", "Facilitates fluid exchange of local goods and commodities.", "Benefits directly from Roads, Internet, and Telecom networks.", "Always unlocked" },
        { "Freight Logistics Depot", "Constructs dry-port container storage and fleet dispatch yards.", "Streamlines inter-zone freight flow and commercial cargo transit.", "Multiplies commerce revenue when infrastructure is modern.", "Requires Level 1 & Economy >= 50%" },
        { "Regional Trade Terminal", "International freight customs interchange and trade exchange.", "Major international hub drawing lucrative tariffs and commerce.", "Transforms district into a thriving cross-regional marketplace.", "Requires Level 2 & Economy >= 65%" }
    }},
    // 8: Electricity
    { 9, {
        { "Diesel Generators", "Installs decentralized diesel generator banks and backup power.", "Supplies critical electricity to municipal hospitals and pump stations.", "Prerequisite power grid foundation to unlock regional Internet.", "Always unlocked" },
        { "Substation Grid", "Lays high-voltage power lines and regional distribution transformers.", "Eliminates brownouts and delivers dependable 24/7 electricity.", "Significantly boosts industrial and office commercial capacity.", "Requires Level 1 & Stability >= 45%" },
        { "Regional Power Plant", "Builds modern clean power plant delivering abundant regional energy.", "Surplus electrical grid powering expansive industrial growth.", "Unlocks full potential of modern electronic infrastructure.", "Requires Level 2 & Stability >= 60%" }
    }},
    // 9: Internet
    { 10, {
        { "Satellite Broadband", "Deploys satellite ground downlinks and municipal wireless hotspots.", "Delivers initial internet connectivity to government and commerce.", "Strictly requires Electricity Lv 1+ to operate ground equipment.", "Requires Electricity Lv 1+" },
        { "Fiber Optic Backbone", "Lays underground fiber optic lines across primary thoroughfares.", "Provides blazing fast internet connectivity to businesses and schools.", "Strongly boosts Office and Trading financial contributions.", "Requires Level 1 & Electricity Lv 2+" },
        { "High-Speed Data Network", "High-capacity tier-1 datacenter and broadband infrastructure.", "Modernizes telecommunications, intelligence, and digital trade.", "Catapults local economy into modern global digital markets.", "Requires Level 2 & Electricity Lv 2+" }
    }},
    // 10: Telecoms
    { 11, {
        { "Radio Towers", "Erects regional cellular towers and emergency radio transceivers.", "Enables instant mobile communication and rapid police dispatch.", "Directly enhances early warning intelligence and trade security.", "Always unlocked" },
        { "Microwave Relay Grid", "Deploys multi-channel microwave repeaters across ridge towers.", "Extends crystal-clear voice and data coverage into remote terrain.", "Strengthens troop movement coordination and commercial trade.", "Requires Level 1 & Stability >= 45%" },
        { "Secure Defense Comms", "Encrypted military-grade telecommunications and command mesh.", "Provides battlefield tactical coordination and intelligence superiority.", "Fortifies civilian confidence and accelerates troop responsiveness.", "Requires Level 2 & Stability >= 60%" }
    }},
    // 11: Roads
    { 12, {
        { "Graded Dirt Roads", "Levels unpaved rural tracks and clears major mud bottlenecks.", "Allows reliable transit for cargo trucks and civilian transit.", "Boosts troop movement speed and local agriculture distribution.", "Always unlocked" },
        { "Paved Highway Network", "Lays durable asphalt highways connecting urban hubs and valleys.", "Speeds military reinforcement transit and heavily boosts Industry.", "Cuts transit delay across rugged terrain significantly.", "Requires Level 1 & Economy >= 45%" },
        { "Multi-Lane Expressways", "Engineered reinforced expressways with concrete bridges and overpasses.", "Maximum military mobility (+30% troop speed) and supreme trade.", "Transforms region into an unhindered transit powerhouse.", "Requires Level 2 & Economy >= 65%" }
    }},
    // 12: Troops
    { 13, {
        { "Militia Garrison", "Recruits and equips disciplined local defense volunteers.", "Enforces public order and suppresses low-level insurgent activity.", "Unlocks on Day 20 alongside military command mobilization.", "Unlocks on Day 20" },
        { "Rapid Reaction Force", "Elite motorized rapid-deployment units with heavy field weaponry.", "Rapidly intercepts hostile expansion and cleanses contested sectors.", "Superior combat firepower and high tactical mobility.", "Requires Level 1 & Security Day 20" },
        { "Armored Defense Brigade", "Mechanized armor division equipped with battle-hardened transports.", "Devastating suppressive firepower capable of wiping hostile insurgencies.", "Guarantees complete military dominance in secured territory.", "Requires Level 2 & Security Day 20" }
    }},
    // 13: Air Strike
    { 14, {
        { "Close Air Support", "Precision fighter jet sorties providing immediate tactical bombardments.", "Deals devastating kinetic damage (+35%) to hostile forces in combat.", "Tactical strike authorization available following Day 20 mobilization.", "Unlocks on Day 20 (Max Lv 1)" },
        { "Close Air Support", "Max level reached. Authorizes immediate airstrike sorties.", "Provides decisive tactical air supremacy over hostile insurgencies.", "Single-level doctrine weapon.", "Completed" },
        { "Close Air Support", "Max level reached. Authorizes immediate airstrike sorties.", "Provides decisive tactical air supremacy over hostile insurgencies.", "Single-level doctrine weapon.", "Completed" }
    }},
    // 14: Radar
    { 15, {
        { "Early Warning Radar", "Installs ground surveillance radar dish to detect insurgent movements.", "Scans adjacent sectors to reveal covert hostile insurgent camps.", "Unlocks on Day 20 with national military defense network.", "Unlocks on Day 20" },
        { "Air Surveillance Array", "High-frequency 3D phased array detecting low-altitude movements.", "Provides real-time threat telemetry and continuous tactical intel.", "Greatly aids friendly security unit positioning and strikes.", "Requires Level 1 & Security Day 20" },
        { "Deep Recon Station", "Strategic multi-spectrum intelligence station tracking entire valleys.", "Completely eliminates the fog of war across neighboring sectors.", "Peak tactical intelligence and early warning defense.", "Requires Level 2 & Security Day 20" }
    }}
};

// Map category tabs to operation list indices
static int getOperationsForCategory(OperationCategory cat, int outOps[4]) {
    switch (cat) {
        case OP_CAT_WELFARE:
            outOps[0] = 0; outOps[1] = 1; outOps[2] = 2; outOps[3] = 3;
            return 4;
        case OP_CAT_INFRASTRUCTURE:
            outOps[0] = 8; outOps[1] = 9; outOps[2] = 10; outOps[3] = 11;
            return 4;
        case OP_CAT_ECONOMIC:
            outOps[0] = 4; outOps[1] = 5; outOps[2] = 6; outOps[3] = 7;
            return 4;
        case OP_CAT_SECURITY:
            outOps[0] = 12; outOps[1] = 13; outOps[2] = 14;
            return 3;
        default:
            outOps[0] = 0; outOps[1] = 1; outOps[2] = 2; outOps[3] = 3;
            return 4;
    }
}

// Category tabs config
static const OperationCategory g_categories[4] = {
    OP_CAT_WELFARE,
    OP_CAT_INFRASTRUCTURE,
    OP_CAT_ECONOMIC,
    OP_CAT_SECURITY
};

static const char *g_catLabels[4] = {
    "CIVILIAN",
    "INFRASTRUCTURE",
    "ECONOMY",
    "SECURITY"
};

void initOperationsScreen() {
    if (g_opsTexturesLoaded) return;

    g_hoverTarget = HOVER_NONE;

    // Background
    g_texOpsMainBg = safeLoadImage("Images/UI/main_bg.jpg");

    // Frames
    g_texFrameOpsLeft = safeLoadImage("Images/UI/assets/frames/frame_ops_left.png");
    g_texFrameOpsRight = safeLoadImage("Images/UI/assets/frames/frame_ops_right.png");

    // Header
    g_texHeaderBar = safeLoadImage("Images/UI/assets/ui/header_bar.png");
    g_texBtnBack = safeLoadImage("Images/UI/assets/buttons/button_back.png");
    g_texBtnBackHover = safeLoadImage("Images/UI/assets/buttons/button_back_hover.png");
    g_texBtnBackPressed = safeLoadImage("Images/UI/assets/buttons/button_back_pressed.png");
    g_texTitleOps = safeLoadImage("Images/UI/assets/icons/ui_titles/title_operations.png");
    g_texTitleWingLeft = safeLoadImage("Images/UI/assets/icons/ops/title_wing_left.png");
    g_texTitleWingRight = safeLoadImage("Images/UI/assets/icons/ops/title_wing_right.png");
    g_texFundsBox = safeLoadImage("Images/UI/assets/ui/funds_box.png");
    g_texFundsCoin = safeLoadImage("Images/UI/assets/icons/ops/funds_coin.png");

    // Left Panel Assets
    g_texBadgeAvailable = safeLoadImage("Images/UI/assets/ui/badge_available.png");
    g_texBadgeLockedBlue = safeLoadImage("Images/UI/assets/ui/badge_locked_blue.png");
    g_texBadgeLockedRed = safeLoadImage("Images/UI/assets/ui/badge_locked_red.png");
    g_texTitleBook = safeLoadImage("Images/UI/assets/icons/ops/title_book.png");

    g_texSectionOverview = safeLoadImage("Images/UI/assets/icons/ops/section_overview.png");
    g_texSectionOutcomes = safeLoadImage("Images/UI/assets/icons/ops/section_outcomes.png");
    g_texSectionUnlock = safeLoadImage("Images/UI/assets/icons/ops/section_unlock.png");
    g_texSectionLineA = safeLoadImage("Images/UI/assets/icons/ops/section_line_a.png");

    g_texOutcomeUp1 = safeLoadImage("Images/UI/assets/icons/ops/outcome_up_1.png");
    g_texOutcomeUp2 = safeLoadImage("Images/UI/assets/icons/ops/outcome_up_2.png");
    g_texOutcomeUp3 = safeLoadImage("Images/UI/assets/icons/ops/outcome_up_3.png");
    g_texOutcomeDown = safeLoadImage("Images/UI/assets/icons/ops/outcome_down.png");
    g_texOutcomeClock = safeLoadImage("Images/UI/assets/icons/ops/outcome_clock.png");

    g_texRadioOn = safeLoadImage("Images/UI/assets/icons/ops/radio_on.png");
    g_texRadioOff = safeLoadImage("Images/UI/assets/icons/ops/radio_off.png");

    g_texBtnProceed = safeLoadImage("Images/UI/assets/buttons/button_proceed.png");
    g_texBtnProceedHover = safeLoadImage("Images/UI/assets/buttons/button_proceed_hover.png");
    g_texBtnProceedPressed = safeLoadImage("Images/UI/assets/buttons/button_proceed_pressed.png");
    g_texCoinBig = safeLoadImage("Images/UI/assets/icons/ops/coin_big.png");

    // Right Panel Assets
    g_texTabActive = safeLoadImage("Images/UI/assets/buttons/tab_active.png");
    g_texTabInactive = safeLoadImage("Images/UI/assets/buttons/tab_inactive.png");
    g_texTabInactiveHover = safeLoadImage("Images/UI/assets/buttons/tab_inactive_hover.png");
    g_texTabInactivePressed = safeLoadImage("Images/UI/assets/buttons/tab_inactive_pressed.png");

    g_texNodeEduActive = safeLoadImage("Images/UI/assets/nodes/node_education_active.png");
    g_texNodeHealthIdle = safeLoadImage("Images/UI/assets/nodes/node_healthcare_idle.png");
    g_texNodeFoodIdle = safeLoadImage("Images/UI/assets/nodes/node_food_water_idle.png");
    g_texNodeSanitationIdle = safeLoadImage("Images/UI/assets/nodes/node_sanitation_idle.png");

    // Operation category icons – all from Images/UI/assets/icons/ops/
    g_texInitEdu       = safeLoadImage("Images/UI/assets/icons/ops/cat_education.png");
    g_texInitHealth    = safeLoadImage("Images/UI/assets/icons/ops/cat_health.png");
    g_texInitFood      = safeLoadImage("Images/UI/assets/icons/ops/cat_food.png");
    g_texInitSanit     = safeLoadImage("Images/UI/assets/icons/ops/cat_sanitation.png");
    g_texInitIndustry  = safeLoadImage("Images/UI/assets/icons/ops/cat_industry.png");
    g_texInitOffice    = safeLoadImage("Images/UI/assets/icons/ops/cat_office.png");
    g_texInitAgri      = safeLoadImage("Images/UI/assets/icons/ops/cat_agriculture.png");
    g_texInitTrading   = safeLoadImage("Images/UI/assets/icons/ops/cat_trading.png");
    g_texInitElectric  = safeLoadImage("Images/UI/assets/icons/ops/cat_electricity.png");
    g_texInitInternet  = safeLoadImage("Images/UI/assets/icons/ops/cat_internet.png");
    g_texInitTelecom   = safeLoadImage("Images/UI/assets/icons/ops/cat_telecoms.png");
    g_texInitRoads     = safeLoadImage("Images/UI/assets/icons/ops/cat_roads.png");
    g_texInitTroops    = safeLoadImage("Images/UI/assets/icons/ops/cat_troops.png");
    g_texInitAirStrike = safeLoadImage("Images/UI/assets/icons/ops/cat_air_strike.png");
    g_texInitRadar     = safeLoadImage("Images/UI/assets/icons/ops/cat_radar.png");

    g_texBannerLevels = safeLoadImage("Images/UI/assets/ui/banner_levels.png");

    g_texRingGoldBook = safeLoadImage("Images/UI/assets/nodes/level_ring_gold_book.png");
    g_texRingBlueLock = safeLoadImage("Images/UI/assets/nodes/level_ring_blue_lock.png");
    g_texRingRedLock = safeLoadImage("Images/UI/assets/nodes/level_ring_red_lock.png");

    g_texLegendRingGreen = safeLoadImage("Images/UI/assets/icons/ops/legend_ring_green.png");
    g_texLegendRingBlue = safeLoadImage("Images/UI/assets/icons/ops/legend_ring_blue.png");
    g_texLegendRingRed = safeLoadImage("Images/UI/assets/icons/ops/legend_ring_red.png");

    g_opsTexturesLoaded = true;
    printf("[Vanguard Operations] Textures loaded successfully (Left: %u, Right: %u, Header: %u)\n", g_texFrameOpsLeft, g_texFrameOpsRight, g_texHeaderBar);
}

void openOperationsScreen(GameState &state) {
    state.currentScreen = SCREEN_OPERATIONS;
    state.selectedOpCategory = OP_CAT_WELFARE;
    state.selectedOpIndex = 0; // Default to Education

    if (state.selectedZoneId >= 0 && state.selectedZoneId < state.totalZones) {
        const Operation &op = state.zones[state.selectedZoneId].operations[state.selectedOpIndex];
        if (op.level < op.maxLevel) {
            state.selectedOpLevel = op.level + 1;
        } else {
            state.selectedOpLevel = op.maxLevel;
        }
    } else {
        state.selectedOpLevel = 1;
    }
}

void renderOperationsScreen(const GameState &state) {
    // Ensure textures are loaded upon first render
    if (!g_opsTexturesLoaded) {
        initOperationsScreen();
    }

    char buffer[256];

    int zoneId = state.selectedZoneId;
    if (zoneId < 0 || zoneId >= state.totalZones) zoneId = 0;
    const Zone &zone = state.zones[zoneId];

    int catOpIndices[4];
    int catOpCount = getOperationsForCategory(state.selectedOpCategory, catOpIndices);

    int opId = state.selectedOpIndex;
    if (opId < 0 || opId >= zone.totalOperations) opId = catOpIndices[0];
    const Operation &op = zone.operations[opId];

    int level = state.selectedOpLevel;
    if (level < 1) level = 1;
    if (level > op.maxLevel) level = op.maxLevel;
    int lvlIdx = level - 1;

    const OpLevelInfo &lvlInfo = g_opMetadata[opId].levels[lvlIdx];

    // 1. SCREEN BACKGROUND
    if (g_texOpsMainBg > 0) {
        renderSprite(0, 0, 1200, 780, g_texOpsMainBg);
    } else {
        iSetColor(12, 14, 18);
        iFilledRectangle(0, 0, 1200, 780);
    }

    // 2. TOP HEADER BAR (1200 x 70 at y: 710 - 780)
    renderSprite(0, 710, 1200, 70, g_texHeaderBar);

    // Back Button (100 x 34 at x: 20, y: 728)
    bool isBackHovered = (g_hoverTarget == HOVER_BACK_BTN);
    unsigned int backSprite = isBackHovered ? g_texBtnBackHover : g_texBtnBack;
    renderSprite(20, 728, 100, 34, backSprite);
    iSetColor(240, 245, 255);
    iText(46, 739, (char *)"<  BACK", GLUT_BITMAP_HELVETICA_12);

    // Center Title: Operations with Wings
    renderSprite(460, 742, 40, 20, g_texTitleWingLeft);
    if (g_texTitleOps > 0) {
        renderSprite(510, 740, 180, 24, g_texTitleOps);
    } else {
        iSetColor(241, 196, 15);
        iText(535, 745, (char *)"OPERATIONS", GLUT_BITMAP_TIMES_ROMAN_24);
    }
    renderSprite(700, 742, 40, 20, g_texTitleWingRight);

    // Subtitle / Breadcrumb -- centered below the title, clear of the wings (x:500-700)
    const char *catName = "Civilian";
    if (state.selectedOpCategory == OP_CAT_INFRASTRUCTURE) catName = "Infrastructure";
    else if (state.selectedOpCategory == OP_CAT_ECONOMIC) catName = "Economy";
    else if (state.selectedOpCategory == OP_CAT_SECURITY) catName = "Security";

    sprintf(buffer, "%s  >  %s  --  choose a level to review", catName, op.name);
    {
        // Center precisely under the OPERATIONS heading (center at x: 600, vertically centered between title and bar bottom)
        int textW = glutBitmapLength(GLUT_BITMAP_HELVETICA_10, (const unsigned char *)buffer);
        int centerX = 600 - textW / 2;
        if (centerX < 130) centerX = 130;        // clear of the Back button
        iSetColor(150, 158, 172);
        iText(centerX, 723, buffer, GLUT_BITMAP_HELVETICA_10);
    }

    // Right Funds Display Box (160 x 50 at x: 1015, y: 720)
    renderSprite(1015, 720, 160, 50, g_texFundsBox);
    renderSprite(1025, 730, 28, 28, g_texFundsCoin);
    iSetColor(150, 160, 175);
    iText(1065, 748, (char *)"FUNDS", GLUT_BITMAP_HELVETICA_10);
    sprintf(buffer, "$%.0f", state.funds);
    iSetColor(46, 204, 113);
    iText(1065, 728, buffer, GLUT_BITMAP_HELVETICA_18);

    // 3. LEFT PANEL: OPERATION DETAILS (420 x 660 at x: 20, y: 25)
    renderSprite(20, 25, 420, 660, g_texFrameOpsLeft);

    bool isLevelCompleted = (op.level >= level);
    bool isLevelConstructing = (op.state == OP_STATE_CONSTRUCTING && op.level + 1 == level);
    bool isNextLevel = (op.level + 1 == level);

    char reqReason[64] = "";
    bool prereqsMet = checkOperationPrerequisites(state, zoneId, opId, reqReason);

    // Top Status Badge (150 x 30 at x: 45, y: 635)
    // Badge texture: circular icon occupies left ~30px; text starts at x:76 to clear it
    if (isLevelCompleted) {
        renderSprite(45, 635, 150, 30, g_texBadgeAvailable);
        sprintf(buffer, "LEVEL %d - COMPLETED", level);
        iSetColor(46, 204, 113);
        iText(76, 644, buffer, GLUT_BITMAP_HELVETICA_10);
    } else if (isLevelConstructing) {
        renderSprite(45, 635, 150, 30, g_texBadgeAvailable);
        sprintf(buffer, "LEVEL %d - CONSTRUCTING", level);
        iSetColor(241, 196, 15);
        iText(76, 644, buffer, GLUT_BITMAP_HELVETICA_10);
    } else if (isNextLevel && prereqsMet) {
        renderSprite(45, 635, 150, 30, g_texBadgeAvailable);
        sprintf(buffer, "LEVEL %d - AVAILABLE", level);
        iSetColor(46, 204, 113);
        iText(76, 644, buffer, GLUT_BITMAP_HELVETICA_10);
    } else {
        renderSprite(45, 635, 150, 30, level == 3 ? g_texBadgeLockedRed : g_texBadgeLockedBlue);
        sprintf(buffer, "LEVEL %d - LOCKED", level);
        iSetColor(220, 100, 100);
        iText(76, 644, buffer, GLUT_BITMAP_HELVETICA_10);
    }

    // Operation Level Title
    renderSprite(45, 588, 32, 32, g_texTitleBook);
    iSetColor(250, 245, 235);
    iText(86, 597, (char *)lvlInfo.levelTitle, GLUT_BITMAP_HELVETICA_18);

    // Section 1: OVERVIEW
    renderSprite(45, 552, 22, 22, g_texSectionOverview);
    iSetColor(241, 196, 15);
    iText(74, 557, (char *)"OVERVIEW", GLUT_BITMAP_HELVETICA_12);
    renderSprite(45, 545, 360, 4, g_texSectionLineA);

    iSetColor(205, 215, 225);
    iText(45, 524, (char *)lvlInfo.overviewLine1, GLUT_BITMAP_HELVETICA_10);
    iText(45, 506, (char *)lvlInfo.overviewLine2, GLUT_BITMAP_HELVETICA_10);
    iText(45, 488, (char *)lvlInfo.overviewLine3, GLUT_BITMAP_HELVETICA_10);

    // Section 2: POSSIBLE OUTCOMES
    renderSprite(45, 456, 22, 22, g_texSectionOutcomes);
    iSetColor(241, 196, 15);
    iText(74, 461, (char *)"POSSIBLE OUTCOMES", GLUT_BITMAP_HELVETICA_12);
    renderSprite(45, 449, 360, 4, g_texSectionLineA);

    double outcomeY = 426;

    if (op.supportBonus != 0) {
        renderSprite(45, (int)outcomeY - 2, 20, 20, g_texOutcomeUp1);
        iSetColor(220, 230, 240);
        sprintf(buffer, "Support %s by +%.1f%% in %s", op.supportBonus >= 0 ? "rises slowly," : "decreases", fabs(op.supportBonus), zone.name);
        iText(72, outcomeY + 3, buffer, GLUT_BITMAP_HELVETICA_10);
        outcomeY -= 24;
    }

    if (op.stabilityBonus > 0) {
        renderSprite(45, (int)outcomeY - 2, 20, 20, g_texOutcomeUp2);
        iSetColor(220, 230, 240);
        sprintf(buffer, "Stability increases by +%.1f%%", op.stabilityBonus);
        iText(72, outcomeY + 3, buffer, GLUT_BITMAP_HELVETICA_10);
        outcomeY -= 24;
    }

    if (op.category == OP_CAT_ECONOMIC && op.revenuePerLevel > 0) {
        renderSprite(45, (int)outcomeY - 2, 20, 20, g_texOutcomeUp3);
        iSetColor(220, 230, 240);
        sprintf(buffer, "Economic revenue yield: +$%.1f/day", op.revenuePerLevel);
        iText(72, outcomeY + 3, buffer, GLUT_BITMAP_HELVETICA_10);
        outcomeY -= 24;
    } else if (op.economyBonus > 0) {
        renderSprite(45, (int)outcomeY - 2, 20, 20, g_texOutcomeUp3);
        iSetColor(220, 230, 240);
        sprintf(buffer, "Economic activity increases by +%.1f%%", op.economyBonus);
        iText(72, outcomeY + 3, buffer, GLUT_BITMAP_HELVETICA_10);
        outcomeY -= 24;
    }

    if (op.securityBonus > 0) {
        renderSprite(45, (int)outcomeY - 2, 20, 20, g_texOutcomeUp2);
        iSetColor(220, 230, 240);
        sprintf(buffer, "Security suppression bonus: +%.1f%%", op.securityBonus);
        iText(72, outcomeY + 3, buffer, GLUT_BITMAP_HELVETICA_10);
        outcomeY -= 24;
    }

    // Maintenance cost outcome
    renderSprite(45, (int)outcomeY - 2, 20, 20, g_texOutcomeDown);
    iSetColor(220, 230, 240);
    sprintf(buffer, "Zone maintenance cost: $%.1f/day", op.maintenanceCost);
    iText(72, outcomeY + 3, buffer, GLUT_BITMAP_HELVETICA_10);
    outcomeY -= 24;

    // Duration & Cost outcome
    renderSprite(45, (int)outcomeY - 2, 20, 20, g_texOutcomeClock);
    iSetColor(200, 220, 240);
    sprintf(buffer, "Duration: %d days   |   Cost: $%.0f", op.constructionDuration, op.cost);
    iText(72, outcomeY + 3, buffer, GLUT_BITMAP_HELVETICA_10);

    // Section 3: UNLOCK PATH
    renderSprite(45, 275, 22, 22, g_texSectionUnlock);
    iSetColor(241, 196, 15);
    iText(74, 280, (char *)"UNLOCK PATH", GLUT_BITMAP_HELVETICA_12);
    renderSprite(45, 268, 360, 4, g_texSectionLineA);

    // Radio item 1: Level 2
    renderSprite(50, 238, 18, 18, (op.level >= 2) ? g_texRadioOn : g_texRadioOff);
    iSetColor(241, 196, 15);
    sprintf(buffer, "Level 2  >  %s", g_opMetadata[opId].levels[1].levelTitle);
    iText(76, 243, buffer, GLUT_BITMAP_HELVETICA_10);
    iSetColor(140, 155, 175);
    sprintf(buffer, "%s", g_opMetadata[opId].levels[1].unlockReqDesc);
    iText(76, 228, buffer, GLUT_BITMAP_HELVETICA_10);

    // Radio item 2: Level 3
    renderSprite(50, 196, 18, 18, (op.level >= 3) ? g_texRadioOn : g_texRadioOff);
    iSetColor(241, 196, 15);
    sprintf(buffer, "Level 3  >  %s", g_opMetadata[opId].levels[2].levelTitle);
    iText(76, 201, buffer, GLUT_BITMAP_HELVETICA_10);
    iSetColor(140, 155, 175);
    sprintf(buffer, "%s", g_opMetadata[opId].levels[2].unlockReqDesc);
    iText(76, 186, buffer, GLUT_BITMAP_HELVETICA_10);

    // Bottom Action Button (380 x 56 at x: 40, y: 50)
    bool isActionHovered = (g_hoverTarget == HOVER_ACTION_BTN);
    unsigned int btnSprite = isActionHovered ? g_texBtnProceedHover : g_texBtnProceed;
    renderSprite(40, 50, 380, 56, btnSprite);

    if (isLevelCompleted) {
        iSetColor(140, 225, 160);
        sprintf(buffer, "LEVEL %d COMPLETED & ACTIVE", level);
        iText(130, 72, buffer, GLUT_BITMAP_HELVETICA_12);
    } else if (isLevelConstructing) {
        iSetColor(255, 235, 150);
        sprintf(buffer, "CONSTRUCTING: DAY %d / %d", op.constructionProgress, op.constructionDuration);
        iText(130, 72, buffer, GLUT_BITMAP_HELVETICA_12);
    } else if (isNextLevel) {
        bool canAfford = (state.funds >= op.cost && zone.playerControl && prereqsMet);
        if (canAfford) {
            renderSprite(105, 64, 28, 28, g_texCoinBig);
            iSetColor(255, 255, 255);
            sprintf(buffer, "PROCEED WITH LEVEL %d ($%.0f)", level, op.cost);
            iText(145, 72, buffer, GLUT_BITMAP_HELVETICA_12);
        } else if (!zone.playerControl) {
            iSetColor(230, 140, 140);
            iText(120, 72, (char *)"LOCKED -- ZONE CONTESTED", GLUT_BITMAP_HELVETICA_12);
        } else if (!prereqsMet) {
            iSetColor(230, 140, 140);
            sprintf(buffer, "LOCKED -- [%s]", reqReason);
            iText(115, 72, buffer, GLUT_BITMAP_HELVETICA_10);
        } else {
            iSetColor(230, 140, 140);
            sprintf(buffer, "INSUFFICIENT FUNDS ($%.0f NEEDED)", op.cost);
            iText(115, 72, buffer, GLUT_BITMAP_HELVETICA_10);
        }
    } else {
        iSetColor(160, 170, 185);
        sprintf(buffer, "LOCKED -- COMPLETE LEVEL %d FIRST", level - 1);
        iText(115, 72, buffer, GLUT_BITMAP_HELVETICA_10);
    }

    // 4. RIGHT PANEL: CATEGORY, OPERATIONS & LEVEL TREE (740 x 660 at x: 460, y: 25)
    renderSprite(460, 25, 740, 660, g_texFrameOpsRight);

    // Header Breadcrumb
    iSetColor(140, 150, 165);
    iText(485, 648, (char *)"OPERATIONS  >", GLUT_BITMAP_HELVETICA_10);
    iSetColor(241, 196, 15);
    sprintf(buffer, "%s  >", g_catLabels[state.selectedOpCategory]);
    iText(585, 648, buffer, GLUT_BITMAP_HELVETICA_10);
    iSetColor(255, 255, 255);
    sprintf(buffer, "%s", op.name);
    iText(685, 648, buffer, GLUT_BITMAP_HELVETICA_10);

    // 4 Category Tabs (150 x 40 each at y: 588)
    double startTabX = 480.0;
    double tabW = 150.0;
    double tabSpacing = 8.0;

    for (int t = 0; t < 4; ++t) {
        double tx = startTabX + t * (tabW + tabSpacing);
        bool isActiveCat = (state.selectedOpCategory == g_categories[t]);
        bool isTabHovered = (g_hoverTarget == (HOVER_TAB_0 + t));

        if (isActiveCat) {
            renderSprite((int)tx, 588, 150, 40, g_texTabActive);
            iSetColor(241, 196, 15);
        } else {
            unsigned int tabSprite = isTabHovered ? g_texTabInactiveHover : g_texTabInactive;
            renderSprite((int)tx, 588, 150, 40, tabSprite);
            iSetColor(isTabHovered ? 230 : 175, isTabHovered ? 235 : 185, isTabHovered ? 245 : 195);
        }

        int txtW = (int)strlen(g_catLabels[t]) * 6;
        iText(tx + 75 - txtW * 0.5, 603, (char *)g_catLabels[t], GLUT_BITMAP_HELVETICA_10);
    }

    // Operation Selector Nodes (y ≈ 500)
    // Horizontal connecting line behind nodes
    iSetColor(45, 55, 70);
    iLine(520, 520, 1140, 520);

    double opAreaW = 640.0;
    double opStep = opAreaW / (double)catOpCount;

    for (int o = 0; o < catOpCount; ++o) {
        int thisOpIdx = catOpIndices[o];
        const Operation &thisOp = zone.operations[thisOpIdx];
        double opCenterX = 490.0 + (o + 0.5) * opStep;
        double opCenterY = 520.0;

        bool isSelectedOp = (thisOpIdx == opId);
        bool isOpHovered = (g_hoverTarget == (HOVER_OP_0 + o));

        // Draw dedicated node graphics – one icon per operation slot
        unsigned int nodeTex = 0;
        if (state.selectedOpCategory == OP_CAT_WELFARE) {
            // Welfare: Education, Healthcare, Food&Water, Sanitation
            if      (o == 0) nodeTex = g_texInitEdu    ? g_texInitEdu    : g_texNodeEduActive;
            else if (o == 1) nodeTex = g_texInitHealth ? g_texInitHealth : g_texNodeHealthIdle;
            else if (o == 2) nodeTex = g_texInitFood   ? g_texInitFood   : g_texNodeFoodIdle;
            else if (o == 3) nodeTex = g_texInitSanit  ? g_texInitSanit  : g_texNodeSanitationIdle;
        } else if (state.selectedOpCategory == OP_CAT_ECONOMIC) {
            // Economy: Industry, Office, Agriculture, Trading
            if      (o == 0) nodeTex = g_texInitIndustry;
            else if (o == 1) nodeTex = g_texInitOffice;
            else if (o == 2) nodeTex = g_texInitAgri;
            else if (o == 3) nodeTex = g_texInitTrading;
        } else if (state.selectedOpCategory == OP_CAT_INFRASTRUCTURE) {
            // Infrastructure: Electricity, Internet, Telecoms, Roads
            if      (o == 0) nodeTex = g_texInitElectric;
            else if (o == 1) nodeTex = g_texInitInternet;
            else if (o == 2) nodeTex = g_texInitTelecom;
            else if (o == 3) nodeTex = g_texInitRoads;
        } else if (state.selectedOpCategory == OP_CAT_SECURITY) {
            // Security: Troops, Air Strike, Radar
            if      (o == 0) nodeTex = g_texInitTroops;
            else if (o == 1) nodeTex = g_texInitAirStrike;
            else if (o == 2) nodeTex = g_texInitRadar;
        }

        if (nodeTex > 0) {
            renderSprite((int)(opCenterX - 32), (int)(opCenterY - 32), 64, 64, nodeTex);
        } else {
            // High-polish vector node fallback
            iSetColor(isSelectedOp ? 45 : 25, isSelectedOp ? 55 : 32, isSelectedOp ? 70 : 42);
            iFilledCircle(opCenterX, opCenterY, 28);
            iSetColor(isSelectedOp ? 255 : 70, isSelectedOp ? 215 : 90, isSelectedOp ? 0 : 115);
            iCircle(opCenterX, opCenterY, 28);
            if (isSelectedOp) iCircle(opCenterX, opCenterY, 30);

            char initial[2] = { thisOp.name[0], '\0' };
            iSetColor(isSelectedOp ? 255 : 180, isSelectedOp ? 215 : 190, isSelectedOp ? 0 : 205);
            iText(opCenterX - 4, opCenterY - 5, initial, GLUT_BITMAP_HELVETICA_12);
        }

        if (isSelectedOp) {
            iSetColor(241, 196, 15);
            iCircle(opCenterX, opCenterY, 32);
            iCircle(opCenterX, opCenterY, 34);
        }

        // Operation Name below node
        int textW = (int)strlen(thisOp.name) * 6;
        iSetColor(isSelectedOp ? 241 : 180, isSelectedOp ? 196 : 190, isSelectedOp ? 15 : 205);
        iText(opCenterX - textW * 0.5, 474, (char *)thisOp.name, GLUT_BITMAP_HELVETICA_10);
    }

    // Banner: "<OPERATION> -- LEVELS" (400 x 36 at x: 630, y: 420)
    renderSprite(630, 420, 400, 36, g_texBannerLevels);
    sprintf(buffer, "%s  --  LEVELS", op.name);
    for (int c = 0; buffer[c]; ++c) {
        if (buffer[c] >= 'a' && buffer[c] <= 'z') buffer[c] -= 32;
    }
    int titleW = (int)strlen(buffer) * 7;
    iSetColor(241, 196, 15);
    iText(830 - titleW * 0.5, 432, buffer, GLUT_BITMAP_HELVETICA_12);

    // 3-LEVEL PROGRESSION TREE (y ≈ 250)
    // Connecting tracks
    iSetColor(50, 65, 80);
    iLine(585, 305, 1075, 305);
    iLine(585, 306, 1075, 306);

    if (op.level >= 1) {
        iSetColor(46, 204, 113);
        iLine(585, 305, 830, 305);
    }
    if (op.level >= 2) {
        iSetColor(46, 204, 113);
        iLine(830, 305, 1075, 305);
    }

    double ringX[3] = { 530.0, 775.0, 1020.0 };
    double ringCenter[3] = { 585.0, 830.0, 1075.0 };

    for (int l = 1; l <= 3; ++l) {
        int nIdx = l - 1;
        double rx = ringX[nIdx];
        double rcx = ringCenter[nIdx];
        bool isSelectedLevel = (level == l);
        bool isNodeHovered = (g_hoverTarget == (HOVER_LVL_1 + nIdx));
        bool isDone = (op.level >= l);
        bool isNext = (op.level + 1 == l);

        // Render Ring Asset (110 x 110 at y: 250)
        unsigned int ringSprite = 0;
        if (l == 1) ringSprite = g_texRingGoldBook;
        else if (l == 2) ringSprite = g_texRingBlueLock;
        else ringSprite = g_texRingRedLock;

        renderSprite((int)rx, 250, 110, 110, ringSprite);

        // Highlight ring on selection or hover
        if (isSelectedLevel || isNodeHovered) {
            iSetColor(241, 196, 15);
            iCircle(rcx, 305, 56);
            iCircle(rcx, 305, 58);
        }

        // Subtitles below Ring
        sprintf(buffer, "LEVEL %d", l);
        iSetColor(241, 196, 15);
        iText(rcx - 22, 228, buffer, GLUT_BITMAP_HELVETICA_10);

        const char *lTitle = g_opMetadata[opId].levels[nIdx].levelTitle;
        int subW = (int)strlen(lTitle) * 6;
        if (isDone) iSetColor(46, 204, 113);
        else if (isNext) iSetColor(241, 196, 15);
        else iSetColor(170, 180, 195);
        iText(rcx - subW * 0.5, 210, (char *)lTitle, GLUT_BITMAP_HELVETICA_10);

        const char *reqText = isDone ? "Completed" : g_opMetadata[opId].levels[nIdx].unlockReqDesc;
        int reqW = (int)strlen(reqText) * 5;
        iSetColor(130, 140, 155);
        iText(rcx - reqW * 0.5, 192, (char *)reqText, GLUT_BITMAP_HELVETICA_10);
    }

    // 5. BOTTOM LEGEND (y ≈ 78)
    renderSprite(515, 74, 16, 16, g_texLegendRingGreen);
    iSetColor(180, 195, 210);
    iText(538, 77, (char *)"Unlocked", GLUT_BITMAP_HELVETICA_10);

    renderSprite(665, 74, 16, 16, g_texLegendRingBlue);
    iSetColor(180, 195, 210);
    iText(688, 77, (char *)"Locked - Level 2 requirement", GLUT_BITMAP_HELVETICA_10);

    renderSprite(875, 74, 16, 16, g_texLegendRingRed);
    iSetColor(180, 195, 210);
    iText(898, 77, (char *)"Locked - Level 3 requirement", GLUT_BITMAP_HELVETICA_10);
}

void handleOperationsMouseMove(int mx, int my) {
    g_hoverTarget = HOVER_NONE;

    // Back Button (x: 20-120, y: 728-762)
    if (mx >= 20 && mx <= 120 && my >= 728 && my <= 762) {
        g_hoverTarget = HOVER_BACK_BTN;
        return;
    }

    // Action Proceed Button (x: 40-420, y: 50-106)
    if (mx >= 40 && mx <= 420 && my >= 50 && my <= 106) {
        g_hoverTarget = HOVER_ACTION_BTN;
        return;
    }

    // Category Tabs (y: 588-628)
    if (my >= 588 && my <= 628) {
        double startTabX = 480.0;
        double tabW = 150.0;
        double tabSpacing = 8.0;
        for (int t = 0; t < 4; ++t) {
            double tx = startTabX + t * (tabW + tabSpacing);
            if (mx >= tx && mx <= tx + tabW) {
                g_hoverTarget = (HoverTarget)(HOVER_TAB_0 + t);
                return;
            }
        }
    }

    // Operation Nodes (y: 488-552)
    if (my >= 488 && my <= 552) {
        double opAreaW = 640.0;
        double opStep = opAreaW / 4.0;
        for (int o = 0; o < 4; ++o) {
            double ox = 490.0 + (o + 0.5) * opStep;
            if (mx >= ox - 32 && mx <= ox + 32) {
                g_hoverTarget = (HoverTarget)(HOVER_OP_0 + o);
                return;
            }
        }
    }

    // Level Rings (y: 250-360)
    if (my >= 250 && my <= 360) {
        double ringCenter[3] = { 585.0, 830.0, 1075.0 };
        for (int l = 0; l < 3; ++l) {
            double distSq = (mx - ringCenter[l]) * (mx - ringCenter[l]) + (my - 305.0) * (my - 305.0);
            if (distSq <= 55.0 * 55.0) {
                g_hoverTarget = (HoverTarget)(HOVER_LVL_1 + l);
                return;
            }
        }
    }
}

void handleOperationsMouseClick(GameState &state, int button, int mouseState, int mx, int my) {
    if (button != GLUT_LEFT_BUTTON || mouseState != GLUT_DOWN) return;

    // 1. Back button -> Return to SCREEN_GAMEPLAY
    if (mx >= 20 && mx <= 120 && my >= 728 && my <= 762) {
        changeGameScreen(state, SCREEN_GAMEPLAY);
        return;
    }

    int zoneId = state.selectedZoneId;
    if (zoneId < 0 || zoneId >= state.totalZones) zoneId = 0;
    Zone &zone = state.zones[zoneId];

    int catOpIndices[4];
    int catOpCount = getOperationsForCategory(state.selectedOpCategory, catOpIndices);

    // 2. Category Tabs Click (y: 588-628)
    if (my >= 588 && my <= 628) {
        double startTabX = 480.0;
        double tabW = 150.0;
        double tabSpacing = 8.0;
        for (int t = 0; t < 4; ++t) {
            double tx = startTabX + t * (tabW + tabSpacing);
            if (mx >= tx && mx <= tx + tabW) {
                state.selectedOpCategory = g_categories[t];
                int newOps[4];
                getOperationsForCategory(state.selectedOpCategory, newOps);
                state.selectedOpIndex = newOps[0];

                const Operation &newOp = zone.operations[state.selectedOpIndex];
                if (newOp.level < newOp.maxLevel) {
                    state.selectedOpLevel = newOp.level + 1;
                } else {
                    state.selectedOpLevel = newOp.maxLevel;
                }
                return;
            }
        }
    }

    // 3. Operation Selector Click (y: 488-552)
    if (my >= 488 && my <= 552) {
        double opAreaW = 640.0;
        double opStep = opAreaW / (double)catOpCount;
        for (int o = 0; o < catOpCount; ++o) {
            double ox = 490.0 + (o + 0.5) * opStep;
            if (mx >= ox - 32 && mx <= ox + 32) {
                state.selectedOpIndex = catOpIndices[o];
                const Operation &selOp = zone.operations[state.selectedOpIndex];
                if (selOp.level < selOp.maxLevel) {
                    state.selectedOpLevel = selOp.level + 1;
                } else {
                    state.selectedOpLevel = selOp.maxLevel;
                }
                return;
            }
        }
    }

    // 4. Level Tree Node Click (y: 250-360)
    if (my >= 250 && my <= 360) {
        double ringCenter[3] = { 585.0, 830.0, 1075.0 };
        for (int l = 0; l < 3; ++l) {
            double distSq = (mx - ringCenter[l]) * (mx - ringCenter[l]) + (my - 305.0) * (my - 305.0);
            if (distSq <= 55.0 * 55.0) {
                const Operation &selOp = zone.operations[state.selectedOpIndex];
                int targetLvl = l + 1;
                if (targetLvl <= selOp.maxLevel) {
                    state.selectedOpLevel = targetLvl;
                }
                return;
            }
        }
    }

    // 5. Action Button Click (x: 40-420, y: 50-106)
    if (mx >= 40 && mx <= 420 && my >= 50 && my <= 106) {
        int opId = state.selectedOpIndex;
        if (opId >= 0 && opId < zone.totalOperations) {
            Operation &curOp = zone.operations[opId];
            int level = state.selectedOpLevel;

            if (curOp.level + 1 == level) {
                if (canPurchaseOperation(state, zoneId, opId)) {
                    purchaseOperation(state, zoneId, opId);
                }
            }
        }
        return;
    }
}
