/* ============================================================================
 *  tp.h  -  터보 파스칼 Graph / Crt / Dos / System 호환 층  (DJGPP 용)
 * ----------------------------------------------------------------------------
 *  BONGBONG 원본은 터보 파스칼 16비트 리얼모드 프로그램이라 화면을 BGI(CGA.BGI)
 *  로 그렸다.  djgpp 는 32비트 보호모드(DPMI)라 BGI 드라이버를 쓸 수 없으므로,
 *  게임이 부르는 파스칼 런타임 함수만 골라 같은 이름·같은 의미로 다시 구현했다.
 *
 *      Graph  ->  VGA 모드 13h(320x200x256) 를 직접 찍는다.
 *                 CGA 4색(0..3)만 쓰므로 팔레트 0..3 번만 CGA 팔레트1로 맞춘다.
 *      Crt    ->  키보드는 int 16h, 소리는 8253 채널2 + 포트 61h.
 *      Dos    ->  GetTime/SetTime 은 진짜 시계를 건드리지 않고 uclock() 기반
 *                 가상 시계로 흉내낸다(원본은 매 초 SetTime(0,0,0,0) 을 한다).
 *      System ->  텍스트 파일 읽기(Assign/Reset/ReadChar/ReadInt/Readln/Close)와
 *                 GetMem/Halt.
 * ========================================================================== */
#ifndef TP_H
#define TP_H

#include <stdio.h>

typedef unsigned char byte;

/* ---- 터보 파스칼 Graph(BGI) 유닛 ---------------------------------------- */
void  InitGraph(int *driver, int *mode, const char *path);
void  CloseGraph(void);
void  ClearDevice(void);
void  SetColor(int color);
void  SetLineStyle(int style, unsigned pattern, int thickness);
void  SetFillStyle(int pattern, int color);
void  SetTextJustify(int horiz, int vert);
void  SetTextStyle(int font, int direction, int size);
void  Bar(int x1, int y1, int x2, int y2);
void  Line(int x1, int y1, int x2, int y2);
void  Rectangle(int x1, int y1, int x2, int y2);
void  PutPixel(int x, int y, int color);
void  OutTextXY(int x, int y, const char *s);
int   ImageSize(int x1, int y1, int x2, int y2);
void  GetImage(int x1, int y1, int x2, int y2, void *buf);
void  PutImage(int x, int y, void *buf, int mode);
#define CopyPut  0
#define XORPut   1
#define TransPut 2      /* 원본에 없던 것: 색 0 을 비침으로 처리 */

/* 원본에 없던 것: VGA 256색이므로 팔레트 한 칸을 마음대로 쓸 수 있다.
   r,g,b 는 0..63 (VGA DAC 값). */
void  SetPaletteEntry(int idx, int r, int g, int b);

/* 원본에 없던 것: 한글 글꼴.  HSMALL.FNT(조합형 8/4/4) + ESMALL.FNT(로마자).
   불러 두면 OutTextXY 가 UTF-8 문자열을 그대로 받아 한글을 찍는다.
   글자 한 줄 높이 12픽셀, 로마자 폭 6, 한글 폭 12. */
int   LoadTextFonts(const char *hangul_fnt, const char *latin_fnt);
int   TextWidth(const char *s);      /* 픽셀 폭 */
void  OutTextScaled(int x, int y, const char *s, int sc);  /* 정수배로 크게 */
#define TEXT_H 12

/* ---- 터보 파스칼 Crt 유닛 ------------------------------------------------ */
void  Sound(int hz);
/* 원본에 없던 것: 미끄러져 내려(올라)가는 소리.  떨어질 때 "삐이익~" 에 쓴다.
   게임을 멈추지 않는다 - FrameSync/Delay 가 이어서 울려 준다. */
void  SoundSlide(int hz_from, int hz_to, int ms);
void  SoundSlideStop(void);
int   SoundSlideBusy(void);

/* 원본에 없던 것: 애드립(OPL2) VGM 곡 재생.  게임을 멈추지 않는다.
   VgmPlay 는 애드립이 없거나 파일이 없으면 0 을 돌려준다. */
int   VgmPlay(const char *fname);
void  VgmStop(void);
int   VgmBusy(void);
int   VgmIsPlaying(const char *fname);

/* 원본에 없던 것: 끝낼 때 PC 스피커와 OPL 을 모두 끈다 */
void  SoundShutdown(void);

/* 원본에 없던 것: 소리 끄기 스위치 (S 키) */
void  SetSoundOn(int on);
int   GetSoundOn(void);

/* 원본에 없던 것: 게임을 멈추지 않고 짧은 가락을 연주한다.
   원본은 Sound + Delay 로 한 음씩 연주해서 그동안 키 입력이 막혔다
   (먹이를 먹을 때마다 0.3초쯤 멈칫하는 원인).  이 함수는 음을 예약만 하고
   곧바로 돌아오며, FrameSync/Delay 가 돌 때마다 다음 음으로 넘어간다.
   가락이 흐르는 동안 Sound/NoSound 는 무시된다(가락이 우선). */
void  SoundJingle(const int *hz, int n, int ms_each);
void  SoundJingleStop(void);
int   SoundJingleBusy(void);
void  NoSound(void);
void  Delay(int ms);
char  ReadKey(void);
int   KeyPressed(void);

/* 원본에는 없던 것.  게임 루프를 초당 fps 번으로 묶어 기계 속도와 무관하게
   같은 빠르기로 돌게 한다. */
void  FrameSync(int fps);
#define GAME_FPS 70

long  TicksMs(void);          /* 프로그램 시작 후 지난 밀리초 */

/* ---- 원본에는 없던 것: 뒤 버퍼 + 배경 페이지 ------------------------------
   부드럽게 움직이는 판(bongbong_smooth.c)에서만 쓴다.  GfxOpenPages() 를
   부르면 그 뒤로 모든 그리기가 램의 뒤 버퍼로 가고, GfxFlip() 이 그것을
   한 번에 화면으로 옮긴다(깜빡임 제거).
   "정적" 모드에서는 뒤 버퍼와 배경 페이지에 동시에 그린다.  움직이는 것만
   정적 모드를 끄고 그리면, 배경 페이지에는 "움직이는 것이 없는 화면"이
   늘 남아 있어서 GfxRestoreRect 로 스프라이트 자국을 지울 수 있다. */
int   GfxOpenPages(void);     /* 0 = 메모리 부족 (그러면 예전처럼 직접 그린다) */
void  GfxClosePages(void);
int   GfxPaged(void);
void  GfxStatic(int on);      /* 1 = 배경 페이지에도 같이 그린다 */
void  GfxRestoreRect(int x, int y, int w, int h);   /* 배경 -> 뒤 버퍼 */
void  GfxFlip(void);          /* 뒤 버퍼 -> 화면 */

/* ---- 터보 파스칼 System / Dos 유닛 --------------------------------------- */
typedef struct {                 /* 파스칼 text 파일 변수에 대응 */
    FILE *fp;
    char  name[80];
} FILE_T;

void  Assign(FILE_T *f, const char *name);
void  Reset(FILE_T *f);
int   ResetTry(FILE_T *f);   /* 원본에 없던 것: 없으면 멈추지 않고 0 을 준다 */
void  Close(FILE_T *f);
char  ReadChar(FILE_T *f);
char  PeekChar(FILE_T *f);   /* 원본에 없던 것: 다음 글자를 소비하지 않고 본다 */
int   ReadInt(FILE_T *f);
void  Readln(FILE_T *f);
void  GetMem(void **p, int size);
void  Str(long value, int width, char *result);   /* 파스칼 Str(value:width, result) */
void  Delete(char *s, int index, int count);      /* 파스칼 Delete(s, index, count) - index 는 1부터 */
void  GetTime(int *h, int *m, int *s, int *s100);
void  SetTime(int h, int m, int s, int s100);
void  Halt(int code);

#endif /* TP_H */
