#define _XOPEN_SOURCE 500
#include "tree.h"
#include "zone.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static zone_t *
zone_create_from_file(const char * fn)
{
    FILE * fp = fopen(fn, "rb");

    struct stat sb;
    fstat(fileno(fp),  &sb);
    zone_t * z = zone_create(sb.st_size+1);

    z->endptr = fread(z->buffer, 1, z->cap, fp);
    fclose(fp);

    return z;
}

static void
zone_write_to_file(const zone_t * z, const char * fn)
{
    FILE * ofp = fopen(fn, "wb");
    fwrite(z->buffer, 1, z->endptr, ofp);
    fclose(ofp);
}

int main(int argc, char * argv[])
{
    long seed;
    if (argc > 1) {
        seed = atoi(argv[1]);
    } else {
        seed = time(NULL);
    }

    fprintf(stderr, "seed: %ld\n", seed);
    srand(seed);

    // serialize

    node_t * my_tree = tree_create_pr(95, 95);
    tree_print(my_tree, stdout);

    zone_t * z = zone_create(32*1024);
    tree_serialize(my_tree, z);
    //zone_print(z, stdout);
    zone_write_to_file(z, "my.data");

    zone_destroy(z);
    tree_destroy(my_tree);

    // deserialize

    z = zone_create_from_file("my.data");
    //zone_print(z, stdout);
    node_t * new_tree = tree_deserialize(z, 0);
    //tree_print(new_tree, stdout);

    tree_destroy(new_tree);
    zone_destroy(z);

    return 0;
}
