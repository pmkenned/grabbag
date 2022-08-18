#ifndef STATE_H
#define STATE_H

enum {
    DIR_RIGHT,
    DIR_LEFT
};

typedef struct {
    int col;
    int row;
    int dir;
} state_t;

#endif /* STATE_H */
