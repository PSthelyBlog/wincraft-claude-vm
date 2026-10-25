/*
 * xshot - write the X screen :0 to stdout as a binary PPM.
 *
 * For wcvm; not part of WinCraft OS's image. Older QEMU versions cannot
 * take a screenshot of a virtio GPU with 3D acceleration (screendump: "no
 * surface"), so wcvm builds this on the host, copies it into the guest
 * through its serial shell and reads the screen from the X server instead;
 * it runs on the image's own libX11. Taken from WinCraft OS's tools/xshot.c.
 *
 * SPDX-License-Identifier: MIT
 */
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

int main(void)
{
    Display *dpy = XOpenDisplay(":0");
    XWindowAttributes wa;
    XImage *img;
    unsigned char *row;
    int x, y;

    if (!dpy) {
        fprintf(stderr, "xshot: cannot open display :0\n");
        return 1;
    }
    XGetWindowAttributes(dpy, DefaultRootWindow(dpy), &wa);
    img = XGetImage(dpy, DefaultRootWindow(dpy), 0, 0, wa.width, wa.height, AllPlanes, ZPixmap);
    if (!img || !(row = malloc(wa.width * 3))) {
        fprintf(stderr, "xshot: cannot read the screen\n");
        return 1;
    }
    printf("P6\n%d %d\n255\n", wa.width, wa.height);
    for (y = 0; y < wa.height; y++) {
        for (x = 0; x < wa.width; x++) {
            unsigned long p = XGetPixel(img, x, y);

            row[x * 3] = (p & img->red_mask) >> 16;
            row[x * 3 + 1] = (p & img->green_mask) >> 8;
            row[x * 3 + 2] = p & img->blue_mask;
        }
        fwrite(row, 3, wa.width, stdout);
    }
    return 0;
}
