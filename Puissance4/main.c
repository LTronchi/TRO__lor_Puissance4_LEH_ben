#include <SFML/Audio.h>
#include <SFML/Graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 6
#define COLS 7

int grid[ROWS][COLS] = { 0 };

int drop_piece(int col, int player)
{
    for (int row = ROWS - 1; row >= 0; row--)
    {
        if (grid[row][col] == 0)
        {
            grid[row][col] = player;
            return 1;
        }
    }
    return 0;
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

int main(void)
{
    sfVideoMode mode = { 500, 500, 32 };
    sfRenderWindow* window;
    sfEvent event;

    window = sfRenderWindow_create(mode, "Puissance 4", sfClose, NULL);
    if (!window)
        return -1;

    srand((unsigned int)time(NULL));

    int current_player = 1;

    while (sfRenderWindow_isOpen(window))
    {
        while (sfRenderWindow_pollEvent(window, &event))
        {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);

            if (event.type == sfEvtMouseButtonPressed)
            {
                int col = event.mouseButton.x / (500 / COLS);

                if (col >= 0 && col < COLS)
                {
                    if (drop_piece(col, current_player))
                    {
                        int winner = check_win();
                        if (winner != 0)
                        {
                            printf("Le joueur %d gagne !\n", winner);
                            sfRenderWindow_close(window);
                        }

                        current_player = (current_player == 1 ? 2 : 1);
                    }
                }
            }
        }

        sfRenderWindow_clear(window, sfBlue);

        for (int r = 0; r < ROWS; r++)
        {
            for (int c = 0; c < COLS; c++)
            {
                sfCircleShape* circle = sfCircleShape_create();
                sfCircleShape_setRadius(circle, 30);
                sfCircleShape_setPosition(circle, (sfVector2f) { c * 70.f + 20, r * 70.f + 20 });

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

        sfRenderWindow_display(window);
    }

    sfRenderWindow_destroy(window);
    return 1;
}
