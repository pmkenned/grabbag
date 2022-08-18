#define _XOPEN_SOURCE 500
#include "tree.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static const char *
strings[] = {
    "hello",
    "fish",
    "dog",
    "cat",
    "goodbye",
    "tomorrow",
};

node_t *
tree_create_pr(int p, int r)
{
    assert((p >= 0) && (p <= 100));
    assert((r >= 0) && (r <= 100));
    node_t * n = malloc(sizeof(*n));
    n->s = strings[rand() % NELEMS(strings)];
    n->x = rand() % 10;
    if (rand() % 100 < p) {
        n->lchild = tree_create_pr(p*r/100, r);
    } else {
        n->lchild = NULL;
    }
    if (rand() % 100 < p) {
        n->rchild = tree_create_pr(p*r/100, r);
    } else {
        n->rchild = NULL;
    }
    return n;
}

node_t *
tree_create()
{
    return tree_create_pr(90, 80);
}

static void
tree_print_depth(const node_t * np, FILE * fp, int depth)
{
    for (size_t i = 0; i < depth; i++) {
        fprintf(fp, "  ");
    }
    fprintf(fp, "%s, %x\n", np->s, np->x);
    if (!np->lchild && !np->rchild)
        return;
    if (np->lchild)
        tree_print_depth(np->lchild, fp, depth+1);
    else {
        for (size_t i = 0; i < depth+1; i++) {
            fprintf(fp, "  ");
        }
        fprintf(fp, "-\n");
    }
    if (np->rchild)
        tree_print_depth(np->rchild, fp, depth+1);
    else {
        for (size_t i = 0; i < depth+1; i++) {
            fprintf(fp, "  ");
        }
        fprintf(fp, "-\n");
    }
}

void
tree_print(const node_t * np, FILE * fp)
{
    tree_print_depth(np, fp, 0);
}

typedef enum {
    ZT_NONE = 0,
    ZT_NODE,
} zt_t;

#define PACK4_LE(DP, S)                 \
    do {                                \
        uint8_t * p = (uint8_t *) DP;   \
        *p++ = S & 0xff;                \
        *p++ = (S >> 8) & 0xff;         \
        *p++ = (S >> 16) & 0xff;        \
        *p++ = (S >> 24) & 0xff;        \
    } while (0)

#define PACK8_LE(DP, S)                 \
    do {                                \
        PACK4_LE(DP, S);                \
        uint8_t * p = (uint8_t *) DP;   \
        *p++ = (S >> 32) & 0xff;        \
        *p++ = (S >> 40) & 0xff;        \
        *p++ = (S >> 48) & 0xff;        \
        *p++ = (S >> 56) & 0xff;        \
    } while (0)

#define UNPACK4_LE(X) ((u64)X[3] << 24) | ((u64)X[2] << 16) | ((u64)X[1] << 8) | ((u64)X[0])
#define UNPACK8_LE(X) ((u64)X[7] << 56) | ((u64)X[6] << 48) | ((u64)X[5] << 40) | ((u64)X[4] << 32) | UNPACK4_LE(X)

void
tree_serialize(const node_t * np, zone_t * z)
{
    zt_t * ztp;
    intptr_t *sp, *rchildp, *lchildp;
    int * xp;

    ztp = zone_alloc(z, sizeof(zt_t), sizeof(zt_t));
    *ztp = ZT_NODE;

    sp = zone_alloc(z, sizeof(intptr_t), sizeof(intptr_t));
    xp = zone_alloc(z, sizeof(int), sizeof(int));
    lchildp = zone_alloc(z, sizeof(intptr_t), sizeof(intptr_t));
    rchildp = zone_alloc(z, sizeof(intptr_t), sizeof(intptr_t));

    *sp = z->endptr + to_align(z->endptr, sizeof(char));
    size_t l = strlen(np->s);
    char * cp = zone_alloc(z, sizeof(char)*(l+1), sizeof(char));
    strcpy(cp, np->s);

    //*xp = np->x;
    PACK4_LE(xp, np->x);

    if (np->lchild) {
        PACK8_LE(lchildp, z->endptr + to_align(z->endptr, sizeof(zt_t)));
        tree_serialize(np->lchild, z);
    } else {
        *lchildp = 0;
    }

    if (np->rchild) {
        PACK8_LE(rchildp, z->endptr + to_align(z->endptr, sizeof(zt_t)));
        tree_serialize(np->rchild, z);
    } else {
        *rchildp = 0;
    }
}


node_t *
tree_deserialize(zone_t * z, size_t offset)
{
    u8 * bp = z->buffer + offset;

    node_t * np = malloc(sizeof(*np));
    np->s = "";
    np->x = 0;
    np->lchild = NULL;
    np->rchild = NULL;

    zt_t zt = UNPACK4_LE(bp);
    assert(zt == ZT_NODE);

    bp += sizeof(zt_t);
    bp += to_align((intptr_t)bp, sizeof(intptr_t));

    intptr_t p = UNPACK8_LE(bp);
    np->s = strdup(z->buffer + p);

    bp += sizeof(char *);
    bp += to_align((intptr_t)bp, sizeof(int));
    np->x = UNPACK4_LE(bp);

    bp += sizeof(int);
    bp += to_align((intptr_t)bp, sizeof(intptr_t));
    p = UNPACK8_LE(bp); 
    if (p != 0) {
        np->lchild = tree_deserialize(z, p);
    }

    bp += sizeof(intptr_t);
    bp += to_align((intptr_t)bp, sizeof(intptr_t));
    p = UNPACK8_LE(bp);
    if (p != 0) {
        np->rchild = tree_deserialize(z, p);
    }

    return np;
}

void
tree_destroy(node_t * np)
{
    if (!np)
        return;
    tree_destroy(np->lchild);
    tree_destroy(np->rchild);
    free(np);
}
