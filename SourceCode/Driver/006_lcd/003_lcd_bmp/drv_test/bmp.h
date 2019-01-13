#ifndef __BMP_H__
#define __BMP_H__


#include <stdio.h>

typedef struct bmp_file_head_t{
    unsigned char  type[2]; //file type, must be "BM"
    unsigned int  size;//bit map file size , Byte
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned int  off_bits;//bit mpa data positions, Byte
}__attribute__((packed)) bmp_file_head;//Prevents compiler optimization

typedef struct bmp_info_head_t{
    unsigned int size;//size of the structer
    int width;//bit map width, Pixel
    int height;//bit map height, Pixel
    unsigned short planes;//device level, must be 1
    unsigned short bit_count;// pixel digits,1->dobule colors 4->16 colors 8->256 colors 24->true color
    unsigned int compression;//compression type ,0->original 1->BI_ELE8 2->BI_RLE4
    unsigned int size_image;//image size, Byte
    int x_pels_per_meter;// horizontal resolution
    int y_pels_per_meter;//vertival resolution
    unsigned int clr_used;//Number of palette indexes actually used, 0->all
    unsigned int clr_important;//Important color number, 0->all
}__attribute__((packed)) bmp_info_head;

typedef struct rgb_quad_t{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
    unsigned char reserved;//luminance reserved
}__attribute__((packed)) rgb_quad;

#endif /* __BMP_H__ */
