#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

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

                if(r == in_diff[rin-2] &&
                   g == in_diff[rin-1] &&
                   b == in_diff[rin])
                        goto next;

                if(wbuf + 32 >= n_buf)
                        break;

                written = sprintf(buf+wbuf, "PX %d %d %02x%02x%02x\n",
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
