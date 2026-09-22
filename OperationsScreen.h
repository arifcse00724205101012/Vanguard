#ifndef OPERATIONS_SCREEN_H
#define OPERATIONS_SCREEN_H

struct GameState;

void initOperationsScreen();
void openOperationsScreen(GameState &state);
void renderOperationsScreen(const GameState &state);
void handleOperationsMouseMove(int mx, int my);
void handleOperationsMouseClick(GameState &state, int button, int mouseState, int mx, int my);

#endif // OPERATIONS_SCREEN_H
