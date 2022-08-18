#include "state.h"
#include <stdio.h>
#include <ncurses.h>

static void
reset(state_t * mem)
{
    clear();
    mem->row = 1;
    mem->col = 1;
}

static void
render(const state_t * mem)
{
    mvprintw(mem->row, 0, "              ");
    mvprintw(mem->row, mem->col, "you");
}

void
update(state_t * mem)
{
    int speed = 1;
    if (mem->dir == DIR_RIGHT)
        mem->col += speed;
    else
        mem->col -= speed;

    if (mem->col <= 0 || mem->col >= 10) {
        if (mem->dir == DIR_RIGHT)
            mem->dir = DIR_LEFT;
        else
            mem->dir = DIR_RIGHT;
    }

    render(mem);
}
