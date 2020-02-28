#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>


#ifndef THRESH
#define THRESH 2
#endif

#define CMP(x,y) (((x-y) <= THRESH) && ((y-x) <= THRESH))

static char nibbles[16] = "0123456789ABCDEF";

static inline size_t format(char* buf, uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b)
{
        int d = 0;
        int dt;
        size_t idx = 3;

        buf[0] = 'P';
        buf[1] = 'X';
        buf[2] = ' ';

        /* x coordinate */
        d=1;
        if(x>=10) d=2;
        if(x>=100) d=3;
        if(x>=1000) d=4;
        if(x>=10000) d=5;
        dt = d;
        for(; d; d--, x/=10) buf[idx+(d-1)] = nibbles[x % 10];
        idx += dt;

        buf[idx++] = ' ';

        /* y coordinate */
        d=1;
        if(y>=10) d=2;
        if(y>=100) d=3;
        if(y>=1000) d=4;
        if(y>=10000) d=5;
        dt = d;
        for(; d; d--, y/=10) buf[idx+(d-1)] = nibbles[y % 10];
        idx += dt;

        buf[idx++] = ' ';

        /* color value as hex */
        buf[idx++] = nibbles[r >> 4];
        buf[idx++] = nibbles[r & 0xF];
        buf[idx++] = nibbles[g >> 4];
        buf[idx++] = nibbles[g & 0xF];
        buf[idx++] = nibbles[b >> 4];
        buf[idx++] = nibbles[b & 0xF];

        buf[idx++] = '\n';

        return idx;
}


int write_pixbuf(int fd, uint16_t x, uint16_t y,
                 uint16_t in_width, uint16_t in_height, uint8_t *raw_in,
                 uint8_t *in_diff, size_t n_in, char *buf, size_t n_buf)
{
        uint16_t wx = x, wy = y;        // Write x,y
        off_t wbuf = 0;                 // buf write pos
        off_t rin = 2;                  // raw_in read pos
        uint8_t r,g,b;
        size_t written;

        while(rin < n_in) {
                r = raw_in[rin-2];
                g = raw_in[rin-1];
                b = raw_in[rin];

                if(CMP(r, in_diff[rin-2]) &&
                   CMP(g, in_diff[rin-1]) &&
                   CMP(b, in_diff[rin]))
                        goto next;

                if(wbuf + 32 >= n_buf)
                        break;

                written = format(buf+wbuf,
                                  wx, wy, r, g, b);
                wbuf += written;

        next:;
                /* Step to next pixel */
                if(++wx >= in_width+x) {
                        wy = ((wy+1-y)%in_height) + y;
                        wx = x;
                }

                rin += 3;
        }

        write(fd, buf, wbuf);

        return wbuf;
}

int main(int argc, char **argv)
{
        uint8_t *rbuf;
        uint8_t *rdiff;
        char *wbuf;
        uint16_t x,y;                   // draw offset
        uint16_t w,h;                   // dimensions
        size_t len_rbuf, len_wbuf;
        size_t rn;                      // read bytes

        if(argc < 5)
                return 1;

        x = strtol(argv[1], NULL, 10);
        y = strtol(argv[2], NULL, 10);
        w = strtol(argv[3], NULL, 10);
        h = strtol(argv[4], NULL, 10);

        if(w <= 0 || w > 0xFFFF ||
           h <= 0 || h > 0xFFFF ||
           x < 0 || x+w > 0xFFFF ||
           y < 0 || y+h > 0xFFFF)
                return 2;

        len_rbuf = sizeof(uint8_t)*3*w*h;  // RGB
        len_wbuf = sizeof(uint8_t)*32*w*h; // 32 chars per pixel

        rbuf  = malloc( len_rbuf );
        rdiff = malloc( len_rbuf );
        wbuf  = malloc( len_wbuf );

        for(;;) {
                rn = 0;
                while(rn < len_rbuf)
                        rn += read(STDIN_FILENO, rbuf+rn, len_rbuf-rn);
                write_pixbuf(STDOUT_FILENO, x, y, w, h, rbuf, rdiff, len_rbuf, wbuf, len_wbuf);
                memcpy(rdiff, rbuf, len_rbuf);
        }

        return 0;
}
