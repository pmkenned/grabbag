#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

int main(int argc, char * argv[])
{
    static char buffer[1 << 10];
    if (argc < 2) {
        fprintf(stderr, "usage: %s FILE\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    FILE * fp = fopen(argv[1], "r");
    if (!fp) {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }
    if (!fgets(buffer, sizeof(buffer), fp)) {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    } 
    size_t len = strlen(buffer);
    printf("%zu %s", len, buffer);
    if (len > 0) {
        if (buffer[0] == 'a') {
            printf("first char was a\n");
            memset(buffer, '\0', sizeof(buffer) + 10);
        }
    }
    fclose(fp);
    return 0;
}
