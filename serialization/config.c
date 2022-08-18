#if 1 // {
typedef enum {
    ZT_NONE = 0,
    ZT_CONFIG,
} zt_t;

void
config_serialize(const config_t * config, zone_t * z)
{
    zt_t * ztp;
    intptr_t * keyp;
    value_t * valuep;

    size_t config_len = shlenu(config);

    ztp = zone_alloc(z, sizeof(zt_t), sizeof(zt_t));
    PACK4_LE(ztp, ZT_NODE);

    keyp = zone_alloc(z, sizeof(intptr_t), sizeof(intptr_t));
    valuep = zone_alloc(z, sizeof(value_t), sizeof(void *));

    PACK8_LE(keyp, z->endptr);
    size_t l = strlen(config->key);
    char * cp = zone_alloc(z, sizeof(char)*(l+1), sizeof(char));
    strcpy(cp, config->key);

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

config_t *
config_deserialize(zone_t * z, size_t offset)
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

#endif // }
