

/*
 * file: pngLoad.h
 * author: Michael Norton
 *
 * Wrapper code for libpng to read a png file.
 * Code is based heavily on the examples found in 
 * the OReilly PNG book.
 * Found online at:
 * http://www.libpng.org/pub/png/book/chapter13.html#png.ch13.div.1
 *
 */
#ifndef PNGLOAD_H
#define PNGLOAD_H

#include <png.h>

/* exported function */
#ifdef __cplusplus
extern "C" {
#endif
int pngLoad(char *file, unsigned int *pwidth, 
            unsigned int *pheight, char **image_data_ptr);
#ifdef __cplusplus
}
#endif

#endif /* PNGLOAD_H */