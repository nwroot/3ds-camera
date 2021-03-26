#include "ppm.h"

#include <stdio.h>

static const uint8_t header[] = {
    'P', '6', ' ', '6', '4', '0', ' ', '4', '8', '0', ' ', '2', '5', '5', ' '
};

void write_ppm_image(uint8_t *src, const char *filename) {
    FILE *fp = fopen(filename, "wb");
    
    fwrite(header, 1, sizeof(header), fp);
    fwrite(src, 1, 640 * 480 * 3, fp);
    
    fclose(fp);
}
