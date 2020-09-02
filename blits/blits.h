#ifndef __rastercool_blits_h
#define __rastercool_blits_h

#include "../common.h"

typedef struct {
    int x;
    int y;
    int w;
    int h;
} RC_rect;

/*flip values:
  0 - no flip
  1 - vert. flip
  2 - horz. flip
  3 - both flip*/

struct get {
    s8 colorkey;
    s8 flip;
    s8 skippixels;
    s8 padding;
};

union RC_blitinfo {
    u32 packed;
    struct get;
};

typedef struct {
    int w;
    int h;
    u8* pixels;
};

void RC_change_palette_color(u8 idx, u8 r, u8 g, u8 b);
void RC_simple_blit(RC_surface* srcsurf, int x, int y);
void RC_blit(RC_surface* srcsurf, union RC_blitinfo info, int x, int y);
void RC_draw_small_text(const s8* txt, int x, int y, u8 color, int max);

void RC_blit(RC_surface* srcsurf, union RC_blitinfo info, int x, int y) {
    u8* dst = globals.framebuffer;
    u8* src = srcsurf->pixels;
    int w = srcsurf->w;
    int h = srcsurf->h;
    int sx = 0;
    int sy = 0;

    int dx = -x;
    if (dx > 0) {
        w -= dx;
        x = 0;
    }
    dx = x + w - globals.screenw;
    if (dx > 0) {
        w -= dx;
    }

    int dy = -y;
    if (dy > 0) {
        h -= dy;
        y += dy;
        sy += dy;
    }
    dy = y + h - globals.screenh;
    if (dy > 0) {
        h -= dy;
    }
    
    if (w < 1 && h < 1) {
        return;
    }

    u8* srcp = src + sy * srcsurf->w + x;
    int ss = srcsurf->w - w;
    u8* dstp = dst + y * globals.framepitch + x;
    int ds = globals.framepitch - w;

    if (info.get.colorkey) {
        switch (info.get.flip) {
            case 0:
                blit(srcp, dstp, h, w, ss, globals.framepitch);
                break;
            case 1:
                blit_vflip(srcp, dstp, h, w, ss, globals.framepitch);
                break;
            case 2:
                blit_hflip(srcp, dstp, h, 
        }
    }

}

static inline void blit(u8* src, u8* dst, int h1, int w1, int w2, int w3) {
    while (h1--) {
        memcpy(dst, src, w1);
        src += w2;
        dst += w3;
    }
}

static inline void blit_vflip(u8* src, u8* dst, int h1, int w1, int w2, int w3) {
    src += (w2*(h1-1));
    while (h1--) {
        memcpy(dst, src, w1);
        src -= w2;
        dst += w3;
    }
}

static inline void blit_hflip(u8* src, u8* dst, int h1, int w1, int w2, int ss, int sd) {
    src += w1;
    ss += (w2*2);
    while (h1--) {
        DUFFS_LOOP8({
            *dst++ = *src--;
        }, w1);
        src += ss;
        dst += ds;
    }
}

static inline void blit_vhflip(u8* src, u8* dst, int h1, int w1, int w2, int ss, int sd) {
    src += (w2*h1)-1;
    ss += (w*2);
    while (h1--) {
        DUFFS_LOOP8({
            *dst++ = *src--;
        }, w1);
        src -= w2;
        dst += w3;
    }
}

static inline void blit_key(u8* src, u8* dst, int h1, int w1, int ss, int sd) {
    while (h1--) {
        DUFFS_LOOP8({
            if (*src) {
                *dst = *src;
            }
            dst++;
            src++;
        }, w1);
        src += ss;
        dst += ds;
    }
}

static inline void stretch_row(u8* src, u8* dst, int sw, int dw) {
    u8 pixel;
    int pos = 0x10000;
    int inc = (sw << 16) / dw;
    for (int i = dw; i > 0; --i) {
        while (pos >= 0x10000) {
            pixel = *src++;
            pos -= 0x10000;
        }
        if (pixel) {
            *dst = pixel;
        }
        dst++;
        pos += inc;
    }
}

static inline void blit_stretch(u8* src, u8* dst, int ss, int ds, int sw, int sh, int dw, int dh) {
    int pos = 0x10000;
    int inc = (sh << 16) / dh;
    u8* dstp = dst;
    u8* srcp = src - ss;
    while (--dh) {
        while (pos >= 0x10000) {
            srcp += ss;
            pos -= 0x10000;
        }
        stretch_row(srcp, dstp, sw, dw);
        pos += inc;
        dstp += ds;
    }
}


#endif

