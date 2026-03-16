#include <SFML/Audio.h>
#include <SFML/Graphics.h>

int main(void)
{
    sfVideoMode mode = { 500, 500, 32 };
    sfRenderWindow* window;
    sfEvent event;

    /* Create the main window */
    window = sfRenderWindow_create(mode, "Puissance 4", sfClose, NULL);
    if (!window)
    {
        return -1;
    }

    srand((unsigned int)time(NULL));

    /* Start the game loop */
    while (sfRenderWindow_isOpen(window))
    {
        /* Process events */
        while (sfRenderWindow_pollEvent(window, &event))
        {
            /* Close window : exit */
            if (event.type == sfEvtClosed)
            {
                sfRenderWindow_close(window);
            }

        }


        /* Clear the screen */
        sfRenderWindow_clear(window, sfBlack);

        /* Update the window */
        sfRenderWindow_display(window);
    }

    /* Cleanup resources */
    sfRenderWindow_destroy(window);

    return 1;
}
