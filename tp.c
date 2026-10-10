/* ============================================================================
 *  tp.c  -  터보 파스칼 Graph / Crt / Dos / System 호환 층  (DJGPP 구현)
 * ----------------------------------------------------------------------------
 *  djgpp 는 32비트 보호모드(DPMI)라 16비트 BGI 드라이버(CGA.BGI)를 부를 수 없다.
 *  그래서 BGI 대신 VGA 모드 13h 프레임버퍼(0xA0000)를 직접 찍는다.
 *  원본이 쓰는 색은 CGA 4색(0..3)뿐이므로 DAC 0..3 번만 CGA 팔레트 1
 *  (검정 / 밝은 청록 / 밝은 자홍 / 흰색) 로 맞춰 두면 화면이 원본과 같다.
 *
 *  화면 해상도만 320x200 으로 같고 색 깊이는 8비트다.  원본 CGA 는 한 픽셀
 *  2비트지만, 게임이 색 번호 0..3 만 쓰기 때문에 보이는 결과는 동일하다.
 * ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>        /* pow - 미끄러지는 소리 */

#include <time.h>         /* djgpp: uclock(), uclock_t, UCLOCKS_PER_SEC */
#include <dos.h>
#include <dpmi.h>         /* __dpmi_int, __dpmi_regs, __dpmi_yield */
#include <go32.h>         /* _dos_ds                   */
#include <sys/farptr.h>   /* _farpeekb, _farpokeb      */
#include <pc.h>           /* inportb, outportb         */
#include <sys/movedata.h> /* dosmemput                 */

#include "tp.h"

#ifndef UCLOCKS_PER_SEC   /* uclock 이 없는 환경 대비 (정밀도만 떨어진다) */
   typedef clock_t uclock_t;
#  define uclock()        clock()
#  define UCLOCKS_PER_SEC CLOCKS_PER_SEC
#endif

#define SCR_W     320
#define SCR_H     200
#define VGA_BASE  0xA0000UL

/* ---------------------------------------------------------------- 상태 --- */
static int  gfx_on     = 0;
static int  cur_color  = 3;    /* SetColor      - Line/Rectangle/OutTextXY */
static int  fill_color = 0;    /* SetFillStyle  - Bar                      */
static int  just_h     = 0;    /* 0 왼쪽 1 가운데 2 오른쪽 (BGI 기본 0)    */
static int  just_v     = 2;    /* 0 아래 1 가운데 2 위     (BGI 기본 2)    */
static int  text_size  = 1;    /* SetTextStyle 배율 (게임은 1 만 쓴다)     */
static unsigned long font8x8 = 0;   /* ROM 8x8 글꼴의 선형 주소 */

/* 뒤 버퍼 / 배경 페이지 (GfxOpenPages 를 부른 경우에만 쓰인다) */
static unsigned char *page = NULL;   /* 그리기가 실제로 들어가는 곳 */
static unsigned char *bgpg = NULL;   /* 움직이는 것이 없는 화면 */
static int static_mode = 1;          /* 1 이면 배경 페이지에도 같이 그린다 */
#define PAGE_BYTES ((unsigned long)SCR_W * SCR_H)

/* ------------------------------------------------------------ 저수준 --- */
static void bios_video(unsigned ax)
{
    __dpmi_regs r;
    memset(&r, 0, sizeof r);
    r.x.ax = ax;
    __dpmi_int(0x10, &r);
}

/* VGA DAC 한 칸 설정 (r,g,b 는 0..63) */
static void set_dac(int idx, int r, int g, int b)
{
    outportb(0x3C8, (unsigned char)idx);
    outportb(0x3C9, (unsigned char)r);
    outportb(0x3C9, (unsigned char)g);
    outportb(0x3C9, (unsigned char)b);
}

static void px_put(int x, int y, int c)
{
    unsigned long o;
    if ((unsigned)x >= SCR_W || (unsigned)y >= SCR_H) return;
    o = (unsigned long)y * SCR_W + x;
    if (page) {
        page[o] = (unsigned char)c;
        if (static_mode) bgpg[o] = (unsigned char)c;
    } else {
        _farpokeb(_dos_ds, VGA_BASE + o, (unsigned char)c);
    }
}

static int px_get(int x, int y)
{
    unsigned long o;
    if ((unsigned)x >= SCR_W || (unsigned)y >= SCR_H) return 0;
    o = (unsigned long)y * SCR_W + x;
    return page ? page[o] : _farpeekb(_dos_ds, VGA_BASE + o);
}

/* ROM 8x8 글꼴 주소를 BIOS 에게 물어본다 (int 10h AX=1130h BH=03) */
static void font_init(void)
{
    __dpmi_regs r;
    memset(&r, 0, sizeof r);
    r.x.ax = 0x1130;
    r.h.bh = 0x03;
    __dpmi_int(0x10, &r);
    font8x8 = ((unsigned long)r.x.es << 4) + r.x.bp;
}

/* ============================== Graph ==================================== */

void InitGraph(int *driver, int *mode, const char *path)
{
    (void)path;                  /* BGI 드라이버 경로 - 쓸 일이 없다 */

    bios_video(0x0013);          /* VGA 320x200x256 */
    gfx_on = 1;

    /* CGA 팔레트 1 (고휘도) 을 DAC 0..3 에 얹는다 */
    set_dac(0,  0,  0,  0);      /* 검정        */
    set_dac(1, 21, 63, 63);      /* 밝은 청록   */
    set_dac(2, 63, 21, 63);      /* 밝은 자홍   */
    set_dac(3, 63, 63, 63);      /* 흰색        */

    font_init();

    cur_color  = 3;              /* BGI 기본: 최대 색 번호 */
    fill_color = 3;
    just_h     = 0;              /* LeftText */
    just_v     = 2;              /* TopText  */

    if (driver) *driver = 1;     /* CGA  */
    if (mode)   *mode   = 1;     /* CGAC1 */

    /* [원본에 없던 것] 어떤 길로 끝나든(Halt, exit, 오류) 소리는 끄고 나간다 */
    atexit(SoundShutdown);
}

void CloseGraph(void)
{
    SoundShutdown();             /* [원본에 없던 것] 남은 소리를 모두 끈다 */
    GfxClosePages();
    if (gfx_on) {
        bios_video(0x0003);      /* 80x25 텍스트 모드로 복귀 */
        gfx_on = 0;
    }
}

void ClearDevice(void)
{
    unsigned long i;
    if (page) {
        memset(page, 0, (size_t)PAGE_BYTES);
        if (static_mode) memset(bgpg, 0, (size_t)PAGE_BYTES);
        return;
    }
    for (i = 0; i < PAGE_BYTES; i++)
        _farpokeb(_dos_ds, VGA_BASE + i, 0);
}

/* ---------------------- 뒤 버퍼 / 배경 페이지 --------------------------- */

int GfxOpenPages(void)
{
    if (page) return 1;
    page = (unsigned char *)malloc((size_t)PAGE_BYTES);
    bgpg = (unsigned char *)malloc((size_t)PAGE_BYTES);
    if (!page || !bgpg) {                 /* 메모리가 없으면 예전처럼 직접 그린다 */
        free(page); free(bgpg);
        page = bgpg = NULL;
        return 0;
    }
    memset(page, 0, (size_t)PAGE_BYTES);
    memset(bgpg, 0, (size_t)PAGE_BYTES);
    static_mode = 1;
    return 1;
}

void GfxClosePages(void)
{
    free(page); free(bgpg);
    page = bgpg = NULL;
}

int  GfxPaged(void)        { return page != NULL; }
void GfxStatic(int on)     { static_mode = on ? 1 : 0; }

/* 배경 페이지의 한 조각을 뒤 버퍼로 되살린다 (스프라이트 자국 지우기) */
void GfxRestoreRect(int x, int y, int w, int h)
{
    int yy;
    if (!page) return;
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > SCR_W) w = SCR_W - x;
    if (y + h > SCR_H) h = SCR_H - y;
    if (w <= 0 || h <= 0) return;
    for (yy = 0; yy < h; yy++) {
        unsigned long o = (unsigned long)(y + yy) * SCR_W + x;
        memcpy(page + o, bgpg + o, (size_t)w);
    }
}

/* 뒤 버퍼를 화면으로.  찢김을 줄이려고 수직 귀선 시작을 잠깐 기다린다. */
void GfxFlip(void)
{
    uclock_t limit;
    if (!page) return;
    limit = uclock() + UCLOCKS_PER_SEC / 50;          /* 최대 20ms 만 기다린다 */
    while ( (inportb(0x3DA) & 0x08) && uclock() < limit) ;   /* 귀선 중이면 끝나길 */
    while (!(inportb(0x3DA) & 0x08) && uclock() < limit) ;   /* 귀선 시작까지 */
    dosmemput(page, (size_t)PAGE_BYTES, VGA_BASE);
}

void SetColor(int color)                        { cur_color = color; }
void SetFillStyle(int pattern, int color)       { (void)pattern; fill_color = color; }
void SetLineStyle(int s, unsigned p, int t)     { (void)s; (void)p; (void)t; }  /* 실선만 쓴다 */
void SetTextJustify(int horiz, int vert)        { just_h = horiz; just_v = vert; }

void SetTextStyle(int font, int direction, int size)
{
    (void)font;                                 /* 기본 8x8 비트맵 글꼴만 있다 */
    (void)direction;                            /* 가로쓰기만 쓴다 */
    text_size = (size < 1) ? 1 : size;
}

void PutPixel(int x, int y, int color)          { px_put(x, y, color); }

/* VGA 팔레트 한 칸 (원본 CGA 에는 없던 것 - 음영용 색을 여기에 만든다) */
void SetPaletteEntry(int idx, int r, int g, int b) { set_dac(idx, r, g, b); }

void Bar(int x1, int y1, int x2, int y2)
{
    int x, y, t;
    if (x1 > x2) { t = x1; x1 = x2; x2 = t; }
    if (y1 > y2) { t = y1; y1 = y2; y2 = t; }
    for (y = y1; y <= y2; y++)
        for (x = x1; x <= x2; x++)
            px_put(x, y, fill_color);
}

void Line(int x1, int y1, int x2, int y2)
{
    int dx =  abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;

    for (;;) {
        px_put(x1, y1, cur_color);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

void Rectangle(int x1, int y1, int x2, int y2)
{
    Line(x1, y1, x2, y1);
    Line(x2, y1, x2, y2);
    Line(x2, y2, x1, y2);
    Line(x1, y2, x1, y1);
}

/* ============================================================================
   [원본에 없던 것]  한글 글자 찍기
   ----------------------------------------------------------------------------
   HSMALL.FNT  조합형 8/4/4 벌식 한글 글꼴.  글리프 360장 x 32바이트
               (초성 8벌x20 + 중성 4벌x22 + 종성 4벌x28), 16x16 상자 안에
               12x12 로 그려져 있다.
   ESMALL.FNT  로마자 글꼴 256자 x 16바이트 (8x16 상자, 실제 잉크는 5x11).

   소스의 한글 문자열은 UTF-8 그대로 두고, 여기서 UTF-8 -> 유니코드 ->
   초성/중성/종성으로 풀어 조합형 글꼴에서 조각 세 개를 꺼내 겹쳐 찍는다.
   그래서 소스에 조합형/완성형 코드를 박아 넣을 필요가 없다.

   글자 한 줄 높이 12픽셀, 로마자 폭 6, 한글 폭 12.
   ========================================================================== */

#define TXT_H      12
#define TXT_ASC_W   6
#define TXT_HAN_W  12

static unsigned char *hfnt = NULL;     /* HSMALL.FNT 11520 바이트 */
static unsigned char *efnt = NULL;     /* ESMALL.FNT  4096 바이트 */

/* 조합형 벌 고르기 표 (조합형 5비트 코드 -> 벌 안의 자리 / 벌 번호) */
static const unsigned char T_JUNG[32] =
    {0,0,0,1,2,3,4,5,0,0,6,7,8,9,10,11,0,0,12,13,14,15,16,17,0,0,18,19,20,21,0,0};
static const unsigned char T_JONG[32] =
    {0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,0,17,18,19,20,21,22,23,24,25,26,27,0,0};
static const unsigned char T_CHO_NOBAT[22] =
    {0,0,0,0,0,0,0,0,0,1,3,3,3,1,2,4,4,4,2,1,3,0};
static const unsigned char T_CHO_BAT[22] =
    {0,5,5,5,5,5,5,5,5,6,7,7,7,6,6,7,7,7,6,6,7,5};
static const unsigned char T_JONG_BEOL[22] =
    {0,0,2,0,2,1,2,1,2,3,0,2,1,3,3,1,2,1,3,3,1,1};
/* 유니코드 순서 -> 조합형 5비트 코드 */
static const unsigned char T_UJUNG[21] =
    {3,4,5,6,7,10,11,12,13,14,15,18,19,20,21,22,23,26,27,28,29};
static const unsigned char T_UJONG[28] =
    {0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,19,20,21,22,23,24,25,26,27,28,29};

static unsigned char *load_file(const char *path, long want)
{
    FILE *f = fopen(path, "rb");
    unsigned char *b;
    if (!f) return NULL;
    b = (unsigned char *)malloc((size_t)want);
    if (!b) { fclose(f); return NULL; }
    if (fread(b, 1, (size_t)want, f) != (size_t)want) { free(b); fclose(f); return NULL; }
    fclose(f);
    return b;
}

int LoadTextFonts(const char *hangul, const char *latin)
{
    if (!hfnt) hfnt = load_file(hangul, 11520L);
    if (!efnt) efnt = load_file(latin,   4096L);
    return (hfnt && efnt) ? 1 : 0;
}

static unsigned hglyph_row(int idx, int r)
{
    const unsigned char *g = hfnt + (long)idx * 32;
    return ((unsigned)g[r * 2] << 8) | g[r * 2 + 1];
}

/* 유니코드 한글 음절 하나 -> 12줄짜리 비트무늬 */
static void han_rows(int uni, unsigned *out)
{
    int c    = uni - 0xAC00;
    int cho  = c / 588 + 1;                 /* 1..19 */
    int jung = (c % 588) / 28;              /* 0..20 */
    int jong = c % 28;                      /* 0..27 */
    int vj   = T_JUNG[T_UJUNG[jung]];
    int vg   = jong ? T_JONG[T_UJONG[jong]] : 0;
    int ci   = (vg ? T_CHO_BAT : T_CHO_NOBAT)[vj];
    int jb   = (cho == 1 || cho == 16) ? 0 : 1;
    int r;

    if (vg) jb += 2;
    for (r = 0; r < TXT_H; r++) {
        unsigned v = hglyph_row(ci * 20 + cho, r) | hglyph_row(160 + jb * 22 + vj, r);
        if (vg) v |= hglyph_row(248 + T_JONG_BEOL[vj] * 28 + vg, r);
        out[r] = v;
    }
}

/* UTF-8 한 글자 읽기.  끝이면 -1 */
static long utf8_next(const unsigned char **pp)
{
    const unsigned char *p = *pp;
    unsigned c = *p;
    if (!c) return -1;
    if (c < 0x80)          { *pp = p + 1; return (long)c; }
    if ((c & 0xE0) == 0xC0 && p[1]) {
        *pp = p + 2;
        return (long)(((c & 0x1F) << 6) | (p[1] & 0x3F));
    }
    if ((c & 0xF0) == 0xE0 && p[1] && p[2]) {
        *pp = p + 3;
        return (long)(((c & 0x0F) << 12) | ((p[1] & 0x3F) << 6) | (p[2] & 0x3F));
    }
    *pp = p + 1;                            /* 알 수 없는 바이트는 건너뛴다 */
    return 0;
}

static int cp_width(long cp)
{
    if (cp >= 0xAC00 && cp <= 0xD7A3) return TXT_HAN_W;
    return TXT_ASC_W;
}

/* [원본에 없던 것] 글자를 정수배로 크게 찍는다 (첫 화면 제목용).
   같은 글꼴을 픽셀 단위로 sc 배 늘려 찍는다.  가로/세로 맞춤도 따른다. */
void OutTextScaled(int x, int y, const char *s, int sc)
{
    const unsigned char *p = (const unsigned char *)s;
    long cp;
    int  row, bit, sx, sy;

    if (sc < 1) sc = 1;
    if (!hfnt || !efnt) { OutTextXY(x, y, s); return; }

    if (just_h == 1)      x -= TextWidth(s) * sc / 2;
    else if (just_h == 2) x -= TextWidth(s) * sc;
    if (just_v == 0)      y -= TXT_H * sc;
    else if (just_v == 1) y -= TXT_H * sc / 2;

    while ((cp = utf8_next(&p)) >= 0) {
        if (cp >= 0xAC00 && cp <= 0xD7A3) {         /* 한글 음절 */
            unsigned g[TXT_H];
            han_rows((int)cp, g);
            for (row = 0; row < TXT_H; row++)
                for (bit = 0; bit < TXT_HAN_W; bit++)
                    if (g[row] & (0x8000u >> bit))
                        for (sy = 0; sy < sc; sy++)
                            for (sx = 0; sx < sc; sx++)
                                px_put(x + bit * sc + sx, y + row * sc + sy, cur_color);
            x += TXT_HAN_W * sc;
        } else {                                     /* 로마자·숫자·기호 */
            const unsigned char *g = efnt + (cp & 0xFF) * 16;
            for (row = 0; row < TXT_H; row++)
                for (bit = 0; bit < 8; bit++)
                    if (g[row] & (0x80 >> bit))
                        for (sy = 0; sy < sc; sy++)
                            for (sx = 0; sx < sc; sx++)
                                px_put(x + bit * sc + sx, y + row * sc + sy, cur_color);
            x += TXT_ASC_W * sc;
        }
    }
}

int TextWidth(const char *s)
{
    const unsigned char *p = (const unsigned char *)s;
    long cp;
    int w = 0;
    if (!hfnt || !efnt) return (int)strlen(s) * 8;
    while ((cp = utf8_next(&p)) >= 0)
        w += cp_width(cp);
    return w;
}

/* BGI 텍스트는 배경을 지우지 않고 켜진 점만 찍는다 */
void OutTextXY(int x, int y, const char *s)
{
    const unsigned char *p = (const unsigned char *)s;
    long cp;
    int  row, bit, sx, sy;

    if (!hfnt || !efnt) {                   /* 글꼴 파일이 없으면 예전 ROM 글꼴 */
        int n  = (int)strlen(s);
        int cw = 8 * text_size, chh = 8 * text_size;
        int i;
        if (just_h == 1) x -= n * cw / 2; else if (just_h == 2) x -= n * cw;
        if (just_v == 0) y -= chh;         else if (just_v == 1) y -= chh / 2;
        for (i = 0; i < n; i++) {
            unsigned ch = (unsigned char)s[i];
            for (row = 0; row < 8; row++) {
                unsigned bits = _farpeekb(_dos_ds, font8x8 + ch * 8 + row);
                for (bit = 0; bit < 8; bit++) {
                    if (!(bits & (0x80 >> bit))) continue;
                    for (sy = 0; sy < text_size; sy++)
                        for (sx = 0; sx < text_size; sx++)
                            px_put(x + i * cw + bit * text_size + sx,
                                   y + row * text_size + sy, cur_color);
                }
            }
        }
        return;
    }

    if (just_h == 1)      x -= TextWidth(s) / 2;
    else if (just_h == 2) x -= TextWidth(s);
    if (just_v == 0)      y -= TXT_H;
    else if (just_v == 1) y -= TXT_H / 2;

    while ((cp = utf8_next(&p)) >= 0) {
        if (cp >= 0xAC00 && cp <= 0xD7A3) {         /* 한글 음절 */
            unsigned g[TXT_H];
            han_rows((int)cp, g);
            for (row = 0; row < TXT_H; row++)
                for (bit = 0; bit < TXT_HAN_W; bit++)
                    if (g[row] & (0x8000u >> bit))
                        px_put(x + bit, y + row, cur_color);
            x += TXT_HAN_W;
        } else {                                     /* 로마자·숫자·기호 */
            const unsigned char *g = efnt + (cp & 0xFF) * 16;
            for (row = 0; row < TXT_H; row++)
                for (bit = 0; bit < 8; bit++)
                    if (g[row] & (0x80 >> bit))
                        px_put(x + bit, y + row, cur_color);
            x += TXT_ASC_W;
        }
    }
}

/* ---- 이미지 버퍼 --------------------------------------------------------
   BGI 와 같은 모양으로 담는다:  워드 (폭-1), 워드 (높이-1), 그 뒤 픽셀.
   원본 CGA BGI 는 픽셀을 2비트로 눌러 담지만, 여기서는 게임이 버퍼 속을
   들여다보지 않으므로 한 픽셀 1바이트로 둔다(크기만 ImageSize 와 맞으면 된다). */

static void put_w(unsigned char *p, int i, unsigned v)
{
    p[i]     = (unsigned char)(v & 0xFF);
    p[i + 1] = (unsigned char)(v >> 8);
}

static unsigned get_w(const unsigned char *p, int i)
{
    return (unsigned)p[i] | ((unsigned)p[i + 1] << 8);
}

int ImageSize(int x1, int y1, int x2, int y2)
{
    return 4 + (x2 - x1 + 1) * (y2 - y1 + 1);
}

void GetImage(int x1, int y1, int x2, int y2, void *buf)
{
    unsigned char *p = (unsigned char *)buf;
    int w = x2 - x1 + 1, h = y2 - y1 + 1;
    int x, y, i = 4;

    put_w(p, 0, (unsigned)(w - 1));
    put_w(p, 2, (unsigned)(h - 1));
    for (y = 0; y < h; y++)
        for (x = 0; x < w; x++)
            p[i++] = (unsigned char)px_get(x1 + x, y1 + y);
}

void PutImage(int x, int y, void *buf, int mode)
{
    const unsigned char *p = (const unsigned char *)buf;
    int w = (int)get_w(p, 0) + 1;
    int h = (int)get_w(p, 2) + 1;
    int i = 4, dx, dy;

    for (dy = 0; dy < h; dy++) {
        for (dx = 0; dx < w; dx++) {
            int c = p[i++];
            if (mode == 2) {                     /* TransPut: 색 0 은 비침 */
                if (c) px_put(x + dx, y + dy, c);
                continue;
            }
            if (mode == 1)                       /* XORPut */
                c ^= px_get(x + dx, y + dy);
            px_put(x + dx, y + dy, c);
        }
    }
}

/* ================================ Crt ==================================== */

/* ---- 소리 ---------------------------------------------------------------
   원본은 PC 스피커에 사각파를 그냥 켰다 껐다 한다.  음과 음 사이에 틈이 없어
   한 덩어리로 뭉쳐 들리고, 시작과 끝이 뚝 끊겨 딸깍거린다.  그래서 딱딱하다.

   PC 스피커는 켜짐/꺼짐 두 단계뿐이지만, 게이트(포트 61h 의 1번 비트)를
   귀에 안 들릴 만큼 빠르게 여닫으면 "켜져 있는 시간의 비율"로 음량을
   흉내낼 수 있다.  이걸 써서 다음 네 가지를 넣었다.

     - 음이 바뀔 때 아주 짧은 쉼(4ms) : 음이 또렷하게 끊긴다
     - 음 시작에 4ms 짧은 글라이드    : 딸깍임이 사라진다
     - 기다리는 동안 음량을 서서히 줄임(감쇠) : 사각파의 날카로움이 죽는다
     - 긴 음에는 아주 옅은 비브라토   : 밋밋함이 줄어든다

   지금은 SOFT_SOUND 0 - 원본과 똑같이 사각파를 그냥 켰다 끈다.
   부드러운 소리를 다시 쓰고 싶으면 1 로 바꾸면 된다. */

#define SOFT_SOUND 0
#define PWM_TICKS  (UCLOCKS_PER_SEC / 3000)     /* 약 3kHz 로 음량 조절 */

static int      spk_on = 0;                     /* 지금 소리가 나는 중인가 */
static int      spk_hz = 0;
static uclock_t spk_t0 = 0;

static void pit_freq(int hz)
{
    unsigned div;
    if (hz < 19) hz = 19;
    div = (unsigned)(1193180L / hz);
    outportb(0x43, 0xB6);                       /* 채널2, 모드3, 16비트 */
    outportb(0x42, (unsigned char)(div & 0xFF));
    outportb(0x42, (unsigned char)(div >> 8));
}

static void spk_enable(int on)                  /* 타이머 게이트 + 소리 */
{
    unsigned char v = inportb(0x61);
    if (on) v |= 0x03; else v = (unsigned char)(v & ~0x03);
    outportb(0x61, v);
}

#if SOFT_SOUND
static void spk_gate(int on)                    /* 타이머는 두고 소리만 여닫기 */
{
    unsigned char v = inportb(0x61);
    if (on) v |= 0x02; else v = (unsigned char)(v & ~0x02);
    outportb(0x61, v);
}

static void spin_us(long us)                    /* 아주 짧은 대기 */
{
    uclock_t until = uclock() + (uclock_t)us * (UCLOCKS_PER_SEC / 1000) / 1000;
    while (uclock() < until)
        ;
}

/* 음이 시작되고 ms 밀리초 지났을 때의 음량 (0..64) */
static int env_duty(long ms)
{
    if (ms < 3)   return 20 + (int)(ms * 15);   /* 3ms 페이드인 */
    if (ms < 45)  return 64;                    /* 유지 */
    if (ms < 260) return 64 - (int)((ms - 45) * 30 / 215);   /* 감쇠 */
    return 34;
}
#endif  /* SOFT_SOUND */

/* [원본에 없던 것] 소리 끄기 스위치.  꺼 두면 Sound() 가 아무 일도 안 한다. */
static int snd_on = 1;
void VgmStop(void);
void SetSoundOn(int on) { snd_on = on ? 1 : 0;
                         if (!snd_on) { SoundJingleStop(); VgmStop(); NoSound(); } }
int  GetSoundOn(void)   { return snd_on; }

static void snd_raw_off(void);

static void snd_raw_on(int hz)
{
    if (hz <= 18) { snd_raw_off(); return; }

#if SOFT_SOUND
    if (spk_on) {                               /* 앞 음과 붙지 않게 짧은 쉼 */
        spk_gate(0);
        spin_us(4000);
    }
    pit_freq(hz - hz / 40);                     /* 2.5% 아래에서 시작해서 */
    spk_enable(1);
    spin_us(2000);
    pit_freq(hz - hz / 100);                    /* 미끄러져 올라온다 */
    spin_us(2000);
#endif

    pit_freq(hz);
    spk_enable(1);
    spk_hz = hz;
    spk_on = 1;
    spk_t0 = uclock();
}

static void snd_raw_off(void)
{
#if SOFT_SOUND
    if (spk_on) {                               /* 끝을 뚝 끊지 않고 닫는다 */
        int i;
        for (i = 7; i >= 0; i--) {
            spk_gate(1); spin_us(50L * i);
            spk_gate(0); spin_us(50L * (8 - i));
        }
    }
#endif
    spk_enable(0);
    spk_on = 0;
}

/* [원본에 없던 것] 미끄러져 내려가는 소리 - 너구리가 떨어질 때 "삐이익~"
   PC 스피커의 음높이를 한 프레임마다 조금씩 낮춰 이어 붙인다.  가락(jingle)
   처럼 게임을 멈추지 않고, FrameSync 가 부를 때마다 이어 간다.
   사람 귀는 음높이를 로그로 느끼므로 주파수를 지수로 내려야 고르게 들린다. */
static int      sl_on = 0;
static int      sl_hz0, sl_hz1, sl_last;
static uclock_t sl_t0, sl_t1;

static void slide_poll(void)
{
    uclock_t now;
    double   r;
    if (!sl_on) return;
    now = uclock();
    if (now >= sl_t1) { sl_on = 0; snd_raw_off(); return; }
    r = (double)(now - sl_t0) / (double)(sl_t1 - sl_t0);
    {
        int hz = (int)(sl_hz0 * pow((double)sl_hz1 / (double)sl_hz0, r));
        /* 1/50 이상 달라졌을 때만 다시 맞춘다.  귀로는 이어지는 소리로
           들리면서, 타이머를 쓸데없이 몇 천 번씩 다시 쓰지 않는다. */
        if (sl_last <= 0 || hz * 50 < sl_last * 49 || hz * 49 > sl_last * 50) {
            sl_last = hz;
            snd_raw_on(hz);
        }
    }
}

void SoundSlide(int hz_from, int hz_to, int ms)
{
    if (!snd_on || ms <= 0 || hz_from <= 0 || hz_to <= 0) return;
    SoundJingleStop();
    sl_hz0 = hz_from; sl_hz1 = hz_to;
    sl_t0  = uclock();
    sl_t1  = sl_t0 + (uclock_t)ms * (UCLOCKS_PER_SEC / 1000);
    sl_on  = 1;
    sl_last = hz_from;
    snd_raw_on(hz_from);
}

void SoundSlideStop(void) { if (sl_on) { sl_on = 0; snd_raw_off(); } }
int  SoundSlideBusy(void) { return sl_on; }

/* [원본에 없던 것] 게임을 멈추지 않는 짧은 가락 -------------------------- */
#define JINGLE_MAX 16
static int      jg_hz[JINGLE_MAX];
static int      jg_n = 0, jg_i = -1, jg_step = 0;
static uclock_t jg_next;

static void jingle_poll(void)
{
    if (jg_i < 0) return;
    while (uclock() >= jg_next) {
        jg_i++;
        if (jg_i >= jg_n) { jg_i = -1; snd_raw_off(); return; }
        snd_raw_on(jg_hz[jg_i]);
        jg_next += (uclock_t)jg_step * (UCLOCKS_PER_SEC / 1000);
    }
}

void SoundJingle(const int *hz, int n, int ms_each)
{
    int i;
    if (!snd_on || n <= 0) return;
    SoundSlideStop();
    if (n > JINGLE_MAX) n = JINGLE_MAX;
    for (i = 0; i < n; i++) jg_hz[i] = hz[i];
    jg_n = n; jg_i = 0; jg_step = ms_each;
    jg_next = uclock() + (uclock_t)ms_each * (UCLOCKS_PER_SEC / 1000);
    snd_raw_on(jg_hz[0]);
}

void SoundJingleStop(void)
{
    if (jg_i < 0) return;
    jg_i = -1;
    snd_raw_off();
}

int SoundJingleBusy(void) { return jg_i >= 0; }

/* 가락이 흐르는 동안에는 걸음 소리 같은 것이 끼어들지 않게 한다 */
void Sound(int hz)
{
    if (!snd_on || jg_i >= 0 || sl_on) return;
    snd_raw_on(hz);
}

void NoSound(void)
{
    if (jg_i >= 0 || sl_on) return;
    snd_raw_off();
}

/* [원본에 없던 것] 애드립(OPL2 / YM3812) VGM 재생 -------------------------
   BB00.VGM 같은 VGM 파일은 "몇 번째 샘플에 어느 레지스터에 무엇을 써라" 를
   적어 둔 기록이다.  OPL2 는 포트 0x388(주소) / 0x389(자료) 두 칸이라
   그대로 써 주기만 하면 애드립·사운드블래스터·도스박스에서 소리가 난다.
   게임을 멈추지 않는다 - 짧은 가락(jingle)과 같이 FrameSync 가 부를 때마다
   때가 된 명령만 처리한다. */
#define OPL_ADDR 0x388
#define OPL_DATA 0x389

/* ---- SOUND.DAT : 곡을 한 파일에 묶어 둔 것 --------------------------------
   낱개 BB0n.VGM 파일 일곱 개를 들고 다니는 대신, 앞에 목록(이름/위치/길이)을
   붙여 한 덩어리로 이어 붙인 것이다.  VGM 자체는 손대지 않고 그대로 들어
   있으므로, 나중에 곡 하나만 갈아 끼우고 다시 묶으면 된다.
       0..7    "BBSND10"           (표시, 마지막 바이트는 0)
       8..9    곡 수                (2바이트, 낮은 자리 먼저)
       10..    곡 하나마다 20바이트  이름 12 / 시작 위치 4 / 길이 4
       그 뒤   VGM 자료들
   tools/mksnd.py 가 만든다. */
#define SND_DAT   "SOUND.DAT"
#define SND_MAGIC "BBSND10"
#define SND_MAX   24

typedef struct { char name[13]; unsigned long off, len; } snd_ent;
static snd_ent snd_toc[SND_MAX];
static int     snd_toc_n = -1;           /* -1 아직 안 읽음 / 0 없다 */

static int snd_ieq(const char *a, const char *b)
{
    while (*a && *b) {
        int x = *a, y = *b;
        if (x >= 'a' && x <= 'z') x -= 32;
        if (y >= 'a' && y <= 'z') y -= 32;
        if (x != y) return 0;
        a++; b++;
    }
    return *a == *b;
}

static unsigned long snd_u32(const unsigned char *p)
{
    return (unsigned long)p[0] | ((unsigned long)p[1] << 8)
         | ((unsigned long)p[2] << 16) | ((unsigned long)p[3] << 24);
}

static void snd_toc_load(void)
{
    FILE *f;
    unsigned char e[20];
    int n, i;

    snd_toc_n = 0;
    f = fopen(SND_DAT, "rb");
    if (!f) return;
    if (fread(e, 1, 8, f) != 8 || memcmp(e, SND_MAGIC, 8) != 0) { fclose(f); return; }
    if (fread(e, 1, 2, f) != 2) { fclose(f); return; }
    n = (int)(e[0] | (e[1] << 8));
    if (n > SND_MAX) n = SND_MAX;
    for (i = 0; i < n; i++) {
        if (fread(e, 1, 20, f) != 20) { fclose(f); snd_toc_n = 0; return; }
        memcpy(snd_toc[i].name, e, 12);
        snd_toc[i].name[12] = 0;
        snd_toc[i].off = snd_u32(e + 12);
        snd_toc[i].len = snd_u32(e + 16);
    }
    fclose(f);
    snd_toc_n = n;
}

/* 묶음에서 곡 하나를 통째로 읽어 온다.  못 찾으면 0 */
static unsigned char *snd_load(const char *name, long *len)
{
    FILE *f;
    unsigned char *p;
    int i;

    if (snd_toc_n < 0) snd_toc_load();
    for (i = 0; i < snd_toc_n; i++)
        if (snd_ieq(snd_toc[i].name, name)) break;
    if (i >= snd_toc_n) return 0;
    if (snd_toc[i].len < 0x40 || snd_toc[i].len > 1024L * 1024L) return 0;

    f = fopen(SND_DAT, "rb");
    if (!f) return 0;
    if (fseek(f, (long)snd_toc[i].off, SEEK_SET) != 0) { fclose(f); return 0; }
    p = (unsigned char *)malloc((size_t)snd_toc[i].len);
    if (!p) { fclose(f); return 0; }
    *len = (long)fread(p, 1, (size_t)snd_toc[i].len, f);
    fclose(f);
    if (*len < 0x40 || memcmp(p, "Vgm ", 4) != 0) { free(p); return 0; }
    return p;
}

static unsigned char *vgm_buf = 0;
static long      vgm_len = 0, vgm_pos = 0, vgm_loop = 0;
static char      vgm_name[16] = "";      /* 지금 올려 둔 곡 */
static uclock_t  vgm_next = 0;
static int       vgm_on = 0;
static int       opl_found = -1;         /* -1 아직 안 봄 / 0 없다 / 1 있다 */

/* OPL2 는 레지스터를 쓴 뒤 조금 쉬어야 한다 (주소 3.3us / 자료 23us).
   상태 포트를 몇 번 읽는 것이 예부터 쓰던 방법이다 - 기계가 빨라도 맞는다. */
static void opl_wait(int n) { int i; for (i = 0; i < n; i++) (void)inportb(OPL_ADDR); }

static void opl_write(int reg, int val)
{
    outportb(OPL_ADDR, (unsigned char)reg);
    opl_wait(6);
    outportb(OPL_DATA, (unsigned char)val);
    opl_wait(35);
}

/* 애드립이 있나 - 타이머1 을 돌려 상태 비트가 바뀌는지 본다 */
static int opl_detect(void)
{
    unsigned char s1, s2;
    int i;
    opl_write(4, 0x60);                  /* 타이머 둘 다 리셋 */
    opl_write(4, 0x80);                  /* IRQ 리셋 */
    s1 = inportb(OPL_ADDR);
    opl_write(2, 0xFF);                  /* 타이머1 에 0xFF */
    opl_write(4, 0x21);                  /* 타이머1 시작 */
    for (i = 0; i < 400; i++) (void)inportb(OPL_ADDR);   /* 80us 넘게 */
    s2 = inportb(OPL_ADDR);
    opl_write(4, 0x60);
    opl_write(4, 0x80);
    return ((s1 & 0xE0) == 0x00 && (s2 & 0xE0) == 0xC0);
}

static void opl_silence(void)
{
    int i;
    for (i = 0xB0; i <= 0xB8; i++) opl_write(i, 0x00);   /* 키 오프 */
    for (i = 0x40; i <= 0x55; i++) opl_write(i, 0x3F);   /* 음량 최소 */
    opl_write(0xBD, 0x00);
}

/* 칩을 처음 켠 상태로 되돌린다.  키 오프만 해서는 소리의 꼬리(릴리스)가
   남고, 프로그램이 끝난 뒤에도 그 음이 계속 울린다.  레지스터를 전부 0 으로
   써 주면 포락선이 통째로 꺼져 완전히 조용해진다. */
static void opl_reset(void)
{
    int i;
    for (i = 0xB0; i <= 0xB8; i++) opl_write(i, 0x00);   /* 먼저 키 오프 */
    opl_write(0xBD, 0x00);                               /* 리듬부 끄기 */
    for (i = 0x60; i <= 0x75; i++) opl_write(i, 0xFF);   /* 감쇠를 가장 빠르게 */
    for (i = 0x80; i <= 0x95; i++) opl_write(i, 0xFF);   /* 릴리스도 가장 빠르게 */
    for (i = 0x40; i <= 0x55; i++) opl_write(i, 0x3F);   /* 음량 최소 */
    for (i = 0x01; i <= 0xF5; i++) opl_write(i, 0x00);   /* 나머지 전부 0 */
}

/* 프로그램이 끝날 때 - PC 스피커와 OPL 을 모두 조용하게 */
void SoundShutdown(void)
{
    jg_i = -1;
    sl_on = 0;
    vgm_on = 0;
    snd_raw_off();
    if (opl_found == 1) opl_reset();
}

static uclock_t vgm_smp(long n)          /* 44100Hz 샘플 수 -> uclock */
{
    return (uclock_t)((double)n * (double)UCLOCKS_PER_SEC / 44100.0);
}

void VgmStop(void)
{
    if (!vgm_on) return;
    vgm_on = 0;
    opl_silence();
    opl_write(0xBD, 0x00);
}

int VgmBusy(void) { return vgm_on; }

/* 지금 울리고 있는 곡이 이 파일인가 */
int VgmIsPlaying(const char *fname)
{
    return vgm_on && strncmp(vgm_name, fname, sizeof vgm_name - 1) == 0;
}

int VgmPlay(const char *fname)
{
    unsigned long off;
    int   i;

    VgmStop();
    if (!snd_on) return 0;
    if (opl_found < 0) opl_found = opl_detect();
    if (!opl_found) return 0;                       /* 애드립이 없다 */

    /* 다른 곡이면 다시 읽는다 (첫 화면 BB00 <-> 게임 BB01) */
    if (vgm_buf && strncmp(vgm_name, fname, sizeof vgm_name - 1) != 0) {
        free(vgm_buf); vgm_buf = 0;
    }
    if (!vgm_buf) {
        /* [원본과 다름] 곡은 SOUND.DAT 묶음 안에 들어 있다 */
        vgm_buf = snd_load(fname, &vgm_len);
        if (!vgm_buf) return 0;
        strncpy(vgm_name, fname, sizeof vgm_name - 1);
        vgm_name[sizeof vgm_name - 1] = 0;
    }

    /* 0x1C 에 적힌 되풀이 시작 위치 (0 이면 한 번만 울리고 끝난다) */
    {
        unsigned long lp = (unsigned long)vgm_buf[0x1C]
                         | ((unsigned long)vgm_buf[0x1D] << 8)
                         | ((unsigned long)vgm_buf[0x1E] << 16)
                         | ((unsigned long)vgm_buf[0x1F] << 24);
        vgm_loop = lp ? (long)(0x1C + lp) : 0;
        if (vgm_loop >= vgm_len) vgm_loop = 0;
    }

    /* 1.50 판부터는 0x34 에 자료 시작 위치가 상대값으로 적혀 있다 */
    off = (unsigned long)vgm_buf[0x34] | ((unsigned long)vgm_buf[0x35] << 8)
        | ((unsigned long)vgm_buf[0x36] << 16) | ((unsigned long)vgm_buf[0x37] << 24);
    vgm_pos = off ? (long)(0x34 + off) : 0x40;
    if (vgm_pos >= vgm_len) return 0;

    /* 칩을 깨끗이 해 두고 시작한다 */
    for (i = 0x20; i <= 0xF5; i++) opl_write(i, 0x00);
    opl_write(1, 0x20);                             /* 웨이브폼 쓰기 허용 */
    opl_silence();

    vgm_next = uclock();
    vgm_on   = 1;
    return 1;
}

static void vgm_poll(void)
{
    uclock_t now;
    if (!vgm_on) return;

    now = uclock();
    /* 한참 밀렸으면(다른 화면에 갔다 왔다든지) 지금 시각으로 맞춘다 */
    if (now > vgm_next + UCLOCKS_PER_SEC / 4) vgm_next = now;

    while (vgm_on && now >= vgm_next) {
        unsigned char c;
        if (vgm_pos >= vgm_len) {
            if (!vgm_loop) { VgmStop(); return; }
            vgm_pos = vgm_loop;
        }
        c = vgm_buf[vgm_pos];
        if (c == 0x5A) {                            /* OPL2 포트0 레지스터 쓰기 */
            opl_write(vgm_buf[vgm_pos + 1], vgm_buf[vgm_pos + 2]);
            vgm_pos += 3;
        } else if (c == 0x61) {                     /* n 샘플 쉬기 */
            long w = (long)vgm_buf[vgm_pos + 1] | ((long)vgm_buf[vgm_pos + 2] << 8);
            vgm_pos += 3;
            vgm_next += vgm_smp(w);
        } else if (c == 0x62) { vgm_pos++; vgm_next += vgm_smp(735); }   /* 1/60초 */
        else if (c == 0x63) { vgm_pos++; vgm_next += vgm_smp(882); }     /* 1/50초 */
        else if (c >= 0x70 && c <= 0x7F) { vgm_pos++; vgm_next += vgm_smp((c & 15) + 1); }
        else if (c >= 0x80 && c <= 0x8F) { vgm_pos++; vgm_next += vgm_smp(c & 15); }
        else if (c == 0x66) {                       /* 끝 - 되풀이할 데가 있으면 돌아간다 */
            if (!vgm_loop) { VgmStop(); return; }
            vgm_pos = vgm_loop;
        }
        else if (c == 0x4F || c == 0x50) vgm_pos += 2;          /* 다른 칩 - 건너뛴다 */
        else if (c >= 0x51 && c <= 0x5F) vgm_pos += 3;
        else if (c >= 0x30 && c <= 0x3F) vgm_pos += 2;
        else if (c >= 0xA0 && c <= 0xBF) vgm_pos += 3;
        else if (c >= 0xC0 && c <= 0xDF) vgm_pos += 4;
        else if (c >= 0xE0) vgm_pos += 5;
        else vgm_pos++;                                          /* 모르는 것 */
        now = uclock();
    }
}

/* ------------------------------------------------------- 가상 시계 ------ */
static uclock_t clk_base = 0;

static long vclock_cs(void)      /* 1/100 초 단위 경과 시각 */
{
    return (long)((uclock() - clk_base) / (UCLOCKS_PER_SEC / 100));
}

/* 시각이 until 이 될 때까지 기다린다.  1ms 넘게 남았으면 DPMI 호스트에게
   시간을 양보해 CPU 를 100% 물고 있지 않도록 한다. */
static void wait_until(uclock_t until)
{
    uclock_t now;

#if SOFT_SOUND
    if (spk_on) {
        /* 소리가 나는 동안에는 게이트를 빠르게 여닫아 음량 곡선을 만든다.
           (여기서는 CPU 를 양보하지 않는다 - 양보하면 곡선이 끊긴다) */
        long lastvib = -1;
        while ((now = uclock()) < until) {
            long ms   = (long)((now - spk_t0) / (UCLOCKS_PER_SEC / 1000));
            long ph   = (long)((now - spk_t0) % PWM_TICKS);
            int  duty = env_duty(ms);

            spk_gate(ph * 64 < (long)duty * PWM_TICKS);

            if (ms > 90) {                      /* 긴 음에만 옅은 비브라토 */
                static const int vib[4] = { 0, 1, 0, -1 };
                long v = (ms / 28) & 3;         /* 약 9Hz */
                if (v != lastvib) {
                    pit_freq(spk_hz + spk_hz * vib[v] / 200);
                    lastvib = v;
                }
            }
        }
        spk_gate(1);
        return;
    }
#endif

    while ((now = uclock()) < until) {
        jingle_poll();                  /* [원본에 없던 것] 가락을 이어 간다 */
        slide_poll();                   /* [원본에 없던 것] 미끄러지는 소리 */
        vgm_poll();                     /* [원본에 없던 것] 애드립 곡을 이어 간다 */
        if (until - now > UCLOCKS_PER_SEC / 1000)
            __dpmi_yield();
    }
}

/* 원본(터보 파스칼)의 Delay 는 프로그램 시작 때 잰 빈 루프 횟수로 기다렸다.
   그래서 빠른 기계에서는 사실상 0 이 되어 게임이 눈 깜짝할 사이에 지나간다.
   여기서는 PIT 를 읽는 uclock() 으로 실제 시간을 재므로 기계가 아무리 빨라도
   똑같은 시간을 기다린다. */
void Delay(int ms)
{
    uclock_t until;
    if (ms <= 0) return;
    until = uclock() + (uclock_t)ms * (UCLOCKS_PER_SEC / 1000);
    wait_until(until);
}

/* 한 프레임의 최소 길이를 보장한다.  게임 루프가 이걸 부르면 기계 속도와
   상관없이 초당 fps 번으로 돌아간다(원본은 그냥 최대 속도로 돌았다). */
void FrameSync(int fps)
{
    static uclock_t next = 0;
    uclock_t step = UCLOCKS_PER_SEC / (fps > 0 ? fps : 70);
    uclock_t now;

    jingle_poll();                      /* [원본에 없던 것] 가락을 이어 간다 */
    slide_poll();                       /* [원본에 없던 것] 미끄러지는 소리 */
    vgm_poll();                         /* [원본에 없던 것] 애드립 곡을 이어 간다 */
    now = uclock();

    if (next == 0 || now > next + step * 4)   /* 처음이거나 너무 밀렸으면 다시 맞춘다 */
        next = now;

    wait_until(next);
    next += step;
}

/* 프로그램 시작 후 지난 밀리초 */
long TicksMs(void)
{
    return (long)(uclock() / (UCLOCKS_PER_SEC / 1000));
}

/* Turbo Pascal ReadKey: 확장키는 0 을 먼저 주고, 다음 호출에서 스캔코드를 준다 */
static int pending_scan = -1;

/* 원본은 int 16h 의 옛 기능(AH=00/01)을 썼다.  그 기능은 101키 자판에만
   있는 코드(Alt+화살표 같은)를 걸러 버리므로, 여기서는 향상된 기능
   (AH=10/11)을 쓴다.  대신 회색 화살표키는 AL 에 0 대신 0xE0 이 오므로
   둘 다 확장키로 본다. */
int KeyPressed(void)
{
    __dpmi_regs r;
    if (pending_scan >= 0) return 1;
    memset(&r, 0, sizeof r);
    r.h.ah = 0x11;                          /* 향상된 자판: 눌린 키가 있나 */
    __dpmi_int(0x16, &r);
    return (r.x.flags & 0x40) ? 0 : 1;      /* ZF=1 이면 대기 중인 키 없음 */
}

char ReadKey(void)
{
    __dpmi_regs r;

    if (pending_scan >= 0) {
        char c = (char)pending_scan;
        pending_scan = -1;
        return c;
    }
    memset(&r, 0, sizeof r);
    r.h.ah = 0x10;                          /* 향상된 자판: 키 하나 읽기 */
    __dpmi_int(0x16, &r);

    if (r.h.al == 0 || r.h.al == 0xE0) {    /* 확장키 */
        pending_scan = r.h.ah;              /* 0x48 위 0x50 아래 0x4B 왼 0x4D 오
                                               0x9B Alt+왼 0x9D Alt+오른 */
        return 0;
    }
    return (char)r.h.al;
}

/* ================================ Dos ====================================
   원본은 GetTime 으로 초/100분의1초를 읽고 매 초 SetTime(0,0,0,0) 으로 시계를
   되돌린다.  진짜 시스템 시계를 건드리면 곤란하므로 가상 시계로 흉내낸다. */

void GetTime(int *h, int *m, int *s, int *s100)
{
    long t = vclock_cs();
    if (s100) *s100 = (int)(t % 100);
    t /= 100;
    if (s) *s = (int)(t % 60);
    t /= 60;
    if (m) *m = (int)(t % 60);
    t /= 60;
    if (h) *h = (int)(t % 24);
}

void SetTime(int h, int m, int s, int s100)
{
    long t = ((long)h * 3600L + (long)m * 60L + (long)s) * 100L + s100;
    clk_base = uclock() - (uclock_t)t * (UCLOCKS_PER_SEC / 100);
}

/* ============================== System =================================== */

static void die(const char *msg)
{
    CloseGraph();
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

void Assign(FILE_T *f, const char *name)
{
    f->fp = NULL;
    strncpy(f->name, name, sizeof f->name - 1);
    f->name[sizeof f->name - 1] = '\0';
}

void Reset(FILE_T *f)
{
    f->fp = fopen(f->name, "rb");
    if (!f->fp) {
        char buf[128];
        sprintf(buf, "%s 를 열 수 없습니다.", f->name);
        die(buf);
    }
}

/* [원본에 없던 것] 파일이 없어도 프로그램을 멈추지 않는다.
   있으면 1, 없으면 0. */
int ResetTry(FILE_T *f)
{
    f->fp = fopen(f->name, "rb");
    return f->fp ? 1 : 0;
}

void Close(FILE_T *f)
{
    if (f->fp) { fclose(f->fp); f->fp = NULL; }
}

/* 파스칼 read(f, ch) : 줄 끝에서는 CR(#13) 을 돌려주고 줄을 넘어가지 않는다 */
char ReadChar(FILE_T *f)
{
    int c = fgetc(f->fp);
    if (c == EOF) return 26;                 /* ^Z */
    if (c == '\r' || c == '\n') { ungetc(c, f->fp); return '\r'; }
    return (char)c;
}

/* 다음 글자를 소비하지 않고 들여다본다 (줄 끝이면 CR, 파일 끝이면 ^Z) */
char PeekChar(FILE_T *f)
{
    int c = fgetc(f->fp);
    if (c == EOF) return 26;
    ungetc(c, f->fp);
    if (c == '\r' || c == '\n') return '\r';
    return (char)c;
}

/* 파스칼 read(f, i) : 앞의 공백과 줄바꿈을 건너뛰고 정수 하나 */
int ReadInt(FILE_T *f)
{
    int v = 0;
    if (fscanf(f->fp, " %d", &v) != 1) v = 0;
    return v;
}

/* 파스칼 readln(f) : 줄 끝까지 버리고 다음 줄 처음으로 */
void Readln(FILE_T *f)
{
    int c;
    while ((c = fgetc(f->fp)) != EOF && c != '\n')
        ;
}

/* 파스칼 Str(value:width, result) - width 0 이면 최소 길이 */
void Str(long value, int width, char *result)
{
    if (width > 0) sprintf(result, "%*ld", width, value);
    else           sprintf(result, "%ld", value);
}

/* 파스칼 Delete(s, index, count) - index 는 1부터 */
void Delete(char *s, int index, int count)
{
    int n = (int)strlen(s);
    if (index < 1 || index > n || count <= 0) return;
    if (index - 1 + count > n) count = n - (index - 1);
    memmove(s + index - 1, s + index - 1 + count,
            (size_t)(n - (index - 1 + count) + 1));
}

void GetMem(void **p, int size)
{
    *p = malloc((size_t)size);
    if (!*p) die("메모리가 모자랍니다.");
}

void Halt(int code)
{
    NoSound();
    CloseGraph();
    exit(code);
}
