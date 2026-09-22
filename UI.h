#ifndef UI_H
#define UI_H

struct GameState;
struct Zone;

// Lifecycle
void initUI();
void renderGame(const GameState &state);

// Reusable UI Component Functions (Module 3)
void drawPanel(double x, double y, double width, double height, const char* title);
void drawProgressBar(double x, double y, double width, double height, double value, double maxValue, double r, double g, double b, const char* label, const char* valueFormat = "%.1f%%");
void drawBadge(double x, double y, double width, double height, const char* text, double r, double g, double b);

// Panel and Screen Renderers
void renderTopBar(const GameState &state);
void renderSidePanel(const GameState &state);
void renderZoneDetailsView(const GameState &state, const Zone &z);
void renderOperationsView(const GameState &state, const Zone &z);

#endif // UI_H
