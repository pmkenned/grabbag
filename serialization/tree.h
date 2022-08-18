#ifndef TREE_H
#define TREE_H

#include "zone.h"
#include <stddef.h>

struct node_t;
typedef struct node_t node_t;

struct node_t {
    const char * s;
    int x;
    struct node_t * lchild, * rchild;
};

node_t *    tree_create_pr(int p, int r);
node_t *    tree_create();
void        tree_destroy(node_t * np);

void        tree_print(const node_t * np, FILE * fp);

void        tree_serialize(const node_t * np, zone_t * z);
node_t *    tree_deserialize(zone_t * z, size_t offset);

#endif /* TREE_H */
