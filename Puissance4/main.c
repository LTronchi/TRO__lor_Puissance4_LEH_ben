#include <SFML/Graphics.h>
#include <SFML/Audio.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Plateau dynamique
int ROWS = 6;
int COLS = 7;
int** grid = NULL;

#define CELL 70

typedef enum {
    STATE_PLAY,
    STATE_ANIM_DROP,
    STATE_VICTORY
} GameState;

GameState state = STATE_PLAY;

// Animation
int anim_col = 0;
int anim_row = 0;
int anim_player = 1;
float anim_y = -60;
float anim_speed = 0;

int winner = 0;

// ----------------------
// ALLOCATION DU PLATEAU
// ----------------------
void create_grid(int rows, int cols)
{
    ROWS = rows;
    COLS = cols;

    grid = malloc(rows * sizeof(int*));
    for (int r = 0; r < rows; r++)
    {
        grid[r] = malloc(cols * sizeof(int));
        for (int c = 0; c < cols; c++)
            grid[r][c] = 0;
    }
}

void reset_grid()
{
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            grid[r][c] = 0;
}

// ----------------------
// MENU DE CHOIX DE TAILLE
// ----------------------
int choose_board_size(sfRenderWindow* window)
{
    sfFont* font = sfFont_createFromFile("C:\\Windows\\Fonts\\arial.ttf");

    while (sfRenderWindow_isOpen(window))
    {
        sfEvent event;
        while (sfRenderWindow_pollEvent(window, &event))
        {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);

            if (event.type == sfEvtMouseButtonPressed)
            {
                int x = event.mouseButton.x;
                int y = event.mouseButton.y;

                if (x >= 100 && x <= 400)
                {
                    if (y >= 150 && y <= 210) return 1; // Petit
                    if (y >= 230 && y <= 290) return 2; // Moyen
                    if (y >= 310 && y <= 370) return 3; // Grand
                }
            }
        }

        sfRenderWindow_clear(window, sfColor_fromRGB(20, 20, 60));

        sfText* title = sfText_create();
        sfText_setFont(title, font);
        sfText_setCharacterSize(title, 40);
        sfText_setString(title, "Choisir la taille");
        sfText_setPosition(title, (sfVector2f) { 90, 50 });
        sfRenderWindow_drawText(window, title, NULL);
        sfText_destroy(title);

        sfRectangleShape* btn = sfRectangleShape_create();
        sfRectangleShape_setSize(btn, (sfVector2f) { 300, 60 });

        // Petit
        sfRectangleShape_setPosition(btn, (sfVector2f) { 100, 150 });
        sfRectangleShape_setFillColor(btn, sfColor_fromRGB(80, 80, 200));
        sfRenderWindow_drawRectangleShape(window, btn, NULL);

        sfText* t = sfText_create();
        sfText_setFont(t, font);
        sfText_setCharacterSize(t, 30);
        sfText_setString(t, "Petit (6x7)");
        sfText_setPosition(t, (sfVector2f) { 150, 160 });
        sfRenderWindow_drawText(window, t, NULL);
        sfText_destroy(t);

        // Moyen
        sfRectangleShape_setPosition(btn, (sfVector2f) { 100, 230 });
        sfRenderWindow_drawRectangleShape(window, btn, NULL);

        t = sfText_create();
        sfText_setFont(t, font);
        sfText_setCharacterSize(t, 30);
        sfText_setString(t, "Moyen (8x9)");
        sfText_setPosition(t, (sfVector2f) { 150, 240 });
        sfRenderWindow_drawText(window, t, NULL);
        sfText_destroy(t);

        // Grand
        sfRectangleShape_setPosition(btn, (sfVector2f) { 100, 310 });
        sfRenderWindow_drawRectangleShape(window, btn, NULL);

        t = sfText_create();
        sfText_setFont(t, font);
        sfText_setCharacterSize(t, 30);
        sfText_setString(t, "Grand (10x12)");
        sfText_setPosition(t, (sfVector2f) { 150, 320 });
        sfRenderWindow_drawText(window, t, NULL);
        sfText_destroy(t);

        sfRectangleShape_destroy(btn);

        sfRenderWindow_display(window);
    }

    return 1;
}

// ----------------------
// LOGIQUE DU JEU
// ----------------------
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
        for (int c = 0; c < COLS; c++)
            if (grid[r][c] != 0)
                if (check_line(r, c, 0, 1) ||
                    check_line(r, c, 1, 0) ||
                    check_line(r, c, 1, 1) ||
                    check_line(r, c, 1, -1))
                    return grid[r][c];
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
        return 1;
    }
    return 0;
}

void draw_animation(sfRenderWindow* window)
{
    sfCircleShape* c = sfCircleShape_create();
    sfCircleShape_setRadius(c, 30);
    sfCircleShape_setFillColor(c, anim_player == 1 ? sfRed : sfYellow);
    sfCircleShape_setPosition(c, (sfVector2f) { anim_col* CELL + 5, anim_y });
    sfRenderWindow_drawCircleShape(window, c, NULL);
    sfCircleShape_destroy(c);
}

// ----------------------
// DESSIN DU PLATEAU
// ----------------------
void draw_grid(sfRenderWindow* window)
{
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
        {
            sfCircleShape* s = sfCircleShape_create();
            sfCircleShape_setRadius(s, 30);
            sfCircleShape_setPosition(s, (sfVector2f) { c* CELL + 5, r* CELL + 5 });

            if (grid[r][c] == 1) sfCircleShape_setFillColor(s, sfRed);
            else if (grid[r][c] == 2) sfCircleShape_setFillColor(s, sfYellow);
            else sfCircleShape_setFillColor(s, sfWhite);

            sfRenderWindow_drawCircleShape(window, s, NULL);
            sfCircleShape_destroy(s);
        }
}

// ----------------------
// ECRAN DE VICTOIRE
// ----------------------
void draw_victory(sfRenderWindow* window, int winner)
{
    sfColor bg = (winner == 1 ? sfColor_fromRGB(255, 80, 80)
        : sfColor_fromRGB(255, 255, 120));
    sfRenderWindow_clear(window, bg);

    sfFont* font = sfFont_createFromFile("C:\\Windows\\Fonts\\arial.ttf");

    sfText* title = sfText_create();
    sfText_setFont(title, font);
    sfText_setCharacterSize(title, 60);
    sfText_setFillColor(title, sfBlack);
    sfText_setString(title, "VICTOIRE !");
    sfText_setPosition(title, (sfVector2f) { 80, 120 });
    sfRenderWindow_drawText(window, title, NULL);
    sfText_destroy(title);

    sfRectangleShape* replay = sfRectangleShape_create();
    sfRectangleShape_setSize(replay, (sfVector2f) { 250, 60 });
    sfRectangleShape_setPosition(replay, (sfVector2f) { 120, 250 });
    sfRectangleShape_setFillColor(replay, sfColor_fromRGB(40, 200, 40));
    sfRenderWindow_drawRectangleShape(window, replay, NULL);

    sfText* t = sfText_create();
    sfText_setFont(t, font);
    sfText_setCharacterSize(t, 30);
    sfText_setFillColor(t, sfBlack);
    sfText_setString(t, "REJOUER");
    sfText_setPosition(t, (sfVector2f) { 165, 260 });
    sfRenderWindow_drawText(window, t, NULL);
    sfText_destroy(t);

    sfRectangleShape* quit = sfRectangleShape_create();
    sfRectangleShape_setSize(quit, (sfVector2f) { 250, 60 });
    sfRectangleShape_setPosition(quit, (sfVector2f) { 120, 330 });
    sfRectangleShape_setFillColor(quit, sfColor_fromRGB(200, 40, 40));
    sfRenderWindow_drawRectangleShape(window, quit, NULL);

    t = sfText_create();
    sfText_setFont(t, font);
    sfText_setCharacterSize(t, 30);
    sfText_setFillColor(t, sfBlack);
    sfText_setString(t, "QUITTER");
    sfText_setPosition(t, (sfVector2f) { 175, 340 });
    sfRenderWindow_drawText(window, t, NULL);
    sfText_destroy(t);

    sfRectangleShape_destroy(replay);
    sfRectangleShape_destroy(quit);
    sfFont_destroy(font);
}

int check_victory_click(int mx, int my)
{
    if (mx >= 120 && mx <= 370 && my >= 250 && my <= 310) return 1;
    if (mx >= 120 && mx <= 370 && my >= 330 && my <= 390) return 2;
    return 0;
}

// ----------------------
// MAIN
// ----------------------
int main()
{
    sfVideoMode mode = { 500, 500, 32 };
    sfRenderWindow* window = sfRenderWindow_create(mode, "Puissance 4", sfClose, NULL);

    // MENU DE TAILLE
    int choice = choose_board_size(window);

    if (choice == 1) create_grid(6, 7);
    if (choice == 2) create_grid(8, 9);
    if (choice == 3) create_grid(10, 12);

    // AGRANDISSEMENT AUTOMATIQUE DE LA FENÊTRE
    int win_w = COLS * CELL;
    int win_h = ROWS * CELL;

    sfVideoMode newMode = { win_w, win_h, 32 };
    sfRenderWindow_destroy(window);
    window = sfRenderWindow_create(newMode, "Puissance 4", sfClose, NULL);
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