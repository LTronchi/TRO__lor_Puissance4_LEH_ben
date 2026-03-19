#include <SFML/Audio.h>
#include <SFML/Graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 6
#define COLS 7
#define CELL 70

int grid[ROWS][COLS] = { 0 };

typedef enum {
    STATE_PLAY,
    STATE_ANIM_DROP,
    STATE_VICTORY
} GameState;

GameState state = STATE_PLAY;

// Animation variables
int anim_col = 0;
int anim_row = 0;
int anim_player = 1;
float anim_y = -60;
float anim_speed = 0;

// Winner
int winner = 0;

// ----------------------
// GRID
// ----------------------
void reset_grid()
{
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            grid[r][c] = 0;
}

int find_row(int col)
{
    for (int row = ROWS - 1; row >= 0; row--)
        if (grid[row][col] == 0)
            return row;
    return -1;
}

int check_line(int r, int c, int dr, int dc)
{
    int p = grid[r][c];
    if (p == 0) return 0;

    for (int i = 1; i < 4; i++)
    {
        int nr = r + dr * i;
        int nc = c + dc * i;

        if (nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS)
            return 0;

        if (grid[nr][nc] != p)
            return 0;
    }
    return p;
}

int check_win()
{
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            if (grid[r][c] == 0) continue;

            if (check_line(r, c, 0, 1)) return grid[r][c];
            if (check_line(r, c, 1, 0)) return grid[r][c];
            if (check_line(r, c, 1, 1)) return grid[r][c];
            if (check_line(r, c, 1, -1)) return grid[r][c];
        }
    }
    return 0;
}

// ----------------------
// ANIMATION
// ----------------------
int update_animation()
{
    anim_speed += 0.6f;
    anim_y += anim_speed;

    float y_final = anim_row * CELL + 5;

    if (anim_y >= y_final)
    {
        anim_y = y_final;
        return 1; // animation finished
    }
    return 0;
}

void draw_animation(sfRenderWindow* window)
{
    sfCircleShape* circle = sfCircleShape_create();
    sfCircleShape_setRadius(circle, 30);
    sfCircleShape_setFillColor(circle, anim_player == 1 ? sfRed : sfYellow);
    sfCircleShape_setPosition(circle, (sfVector2f) { anim_col* CELL + 5, anim_y });
    sfRenderWindow_drawCircleShape(window, circle, NULL);
    sfCircleShape_destroy(circle);
}

// ----------------------
// DRAW GRID
// ----------------------
void draw_grid(sfRenderWindow* window)
{
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            sfCircleShape* circle = sfCircleShape_create();
            sfCircleShape_setRadius(circle, 30);
            sfCircleShape_setPosition(circle, (sfVector2f) { c* CELL + 5, r* CELL + 5 });

            if (grid[r][c] == 1)
                sfCircleShape_setFillColor(circle, sfRed);
            else if (grid[r][c] == 2)
                sfCircleShape_setFillColor(circle, sfYellow);
            else
                sfCircleShape_setFillColor(circle, sfWhite);

            sfRenderWindow_drawCircleShape(window, circle, NULL);
            sfCircleShape_destroy(circle);
        }
    }
}

// ----------------------
// VICTORY SCREEN 
// ----------------------
void draw_victory(sfRenderWindow* window, int winner)
{
    // Fond selon le gagnant
    sfColor bg = (winner == 1 ? sfColor_fromRGB(255, 80, 80) : sfColor_fromRGB(255, 255, 120));
    sfRenderWindow_clear(window, bg);

    sfFont* font = sfFont_createFromFile("C:\\Windows\\Fonts\\arial.ttf");
    if (!font) return;

    sfText* title = sfText_create();
    sfText_setFont(title, font);
    sfText_setCharacterSize(title, 60);
    sfText_setFillColor(title, sfBlack);
    sfText_setString(title, "VICTOIRE !");
    sfText_setPosition(title, (sfVector2f) { 80, 120 });

    // Bouton rejouer
    sfRectangleShape* replayBtn = sfRectangleShape_create();
    sfRectangleShape_setSize(replayBtn, (sfVector2f) { 250, 60 });
    sfRectangleShape_setPosition(replayBtn, (sfVector2f) { 120, 250 });
    sfRectangleShape_setFillColor(replayBtn, sfColor_fromRGB(40, 200, 40));

    sfText* replayText = sfText_create();
    sfText_setFont(replayText, font);
    sfText_setCharacterSize(replayText, 30);
    sfText_setFillColor(replayText, sfBlack);
    sfText_setString(replayText, "REJOUER");
    sfText_setPosition(replayText, (sfVector2f) { 165, 260 });

    // Bouton quitter
    sfRectangleShape* quitBtn = sfRectangleShape_create();
    sfRectangleShape_setSize(quitBtn, (sfVector2f) { 250, 60 });
    sfRectangleShape_setPosition(quitBtn, (sfVector2f) { 120, 330 });
    sfRectangleShape_setFillColor(quitBtn, sfColor_fromRGB(200, 40, 40));

    sfText* quitText = sfText_create();
    sfText_setFont(quitText, font);
    sfText_setCharacterSize(quitText, 30);
    sfText_setFillColor(quitText, sfBlack);
    sfText_setString(quitText, "QUITTER");
    sfText_setPosition(quitText, (sfVector2f) { 175, 340 });

    // Dessin
    sfRenderWindow_drawText(window, title, NULL);
    sfRenderWindow_drawRectangleShape(window, replayBtn, NULL);
    sfRenderWindow_drawText(window, replayText, NULL);
    sfRenderWindow_drawRectangleShape(window, quitBtn, NULL);
    sfRenderWindow_drawText(window, quitText, NULL);

    // Nettoyage
    sfText_destroy(title);
    sfText_destroy(replayText);
    sfText_destroy(quitText);
    sfRectangleShape_destroy(replayBtn);
    sfRectangleShape_destroy(quitBtn);
    sfFont_destroy(font);
}

int check_victory_click(int mx, int my)
{
    if (mx >= 120 && mx <= 370 && my >= 250 && my <= 310)
        return 1; // replay

    if (mx >= 120 && mx <= 370 && my >= 330 && my <= 390)
        return 2; // quit

    return 0;
}

// ----------------------
// MAIN
// ----------------------
int main(void)
{
    sfVideoMode mode = { 500, 500, 32 };
    sfRenderWindow* window = sfRenderWindow_create(mode, "Puissance 4", sfClose, NULL);
    if (!window) return -1;

    sfRenderWindow_setFramerateLimit(window, 60);

    int current_player = 1;
    int hover_col = -1;

    while (sfRenderWindow_isOpen(window))
    {
        sfEvent event;
        while (sfRenderWindow_pollEvent(window, &event))
        {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);

            if (state == STATE_PLAY)
            {
                if (event.type == sfEvtMouseMoved)
                    hover_col = event.mouseMove.x / CELL;

                if (event.type == sfEvtMouseButtonPressed)
                {
                    int col = event.mouseButton.x / CELL;
                    int row = find_row(col);

                    if (row != -1)
                    {
                        anim_col = col;
                        anim_row = row;
                        anim_player = current_player;
                        anim_y = -60;
                        anim_speed = 0;

                        state = STATE_ANIM_DROP;
                    }
                }
            }
            else if (state == STATE_VICTORY)
            {
                if (event.type == sfEvtMouseButtonPressed)
                {
                    int r = check_victory_click(event.mouseButton.x, event.mouseButton.y);

                    if (r == 1)
                    {
                        reset_grid();
                        current_player = 1;
                        state = STATE_PLAY;
                    }
                    else if (r == 2)
                    {
                        sfRenderWindow_close(window);
                    }
                }
            }
        }

        // UPDATE
        if (state == STATE_ANIM_DROP)
        {
            if (update_animation())
            {
                grid[anim_row][anim_col] = anim_player;

                winner = check_win();
                if (winner != 0)
                    state = STATE_VICTORY;
                else
                {
                    current_player = (current_player == 1 ? 2 : 1);
                    state = STATE_PLAY;
                }
            }
        }

        // DRAW
        sfRenderWindow_clear(window, sfBlue);

        draw_grid(window);

        if (state == STATE_ANIM_DROP)
            draw_animation(window);

        if (state == STATE_VICTORY)
            draw_victory(window, winner);

        sfRenderWindow_display(window);
    }

    sfRenderWindow_destroy(window);
    return 0;
}