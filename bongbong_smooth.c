/* ============================================================================
 *  BONG BONG  -  [Bong Bong] (1989)  Programed By Lee T.K. / Yonsei COMSCI
 * ----------------------------------------------------------------------------
 *  BONGBONG.EXE (32,064 bytes) 를 역어셈블해서 C 로 옮긴 것.
 *
 *  원본은 볼랜드 **터보 파스칼** 로 만든 16비트 리얼모드 DOS 프로그램이다
 *  (문자열이 전부 길이 접두 파스칼 문자열이고, Graph(BGI)/Crt/System 유닛을
 *   far call 로 부른다).  화면은 BGI CGA 드라이버, 320x200.
 *
 *  실행 파일 구조 (MZ 헤더 뒤 이미지 기준)
 *      0x0000..0x268F  주 프로그램 : 게임 코드 + 파스칼 문자열 상수  <- 이 파일
 *      0x2690 (seg 0269)  Dos 유닛   (GetTime/SetTime)
 *      0x2710 (seg 0271)  Crt 유닛   (Sound/NoSound/Delay/ReadKey/KeyPressed)
 *      0x2D30 (seg 02D3)  Graph 유닛 (BGI)
 *      0x60D0 (seg 060D)  System 유닛(런타임)
 *  진입점 CS:IP = 0000:2602 = 아래 main().
 *
 *  원본에는 심볼이 없다.  함수 이름은 하는 일에 맞게 새로 붙였고, 각 함수 위에
 *  원래 오프셋을 적어 두었다(아래 [함수 목록] 표로도 대조할 수 있다).
 *  전역변수도 하는 일에 맞게 이름을 붙였고(player_col, stage, score ...),
 *  선언마다 원래 DS 오프셋을 주석에 남겨 두었다.
 *  컴파일러가 넣은 스택검사(@StackCheck)와 I/O 결과검사(@IOCheck)는 뺐다.
 *
 *  터보 파스칼 Graph(BGI)/Crt/Dos/System 함수는 tp.h / tp.c 에 djgpp 용으로
 *  다시 구현해 두었다(djgpp 는 보호모드라 BGI 드라이버를 못 쓴다).
 *
 *  ※ 이 파일은 bongbong.c 의 **부드럽게 움직이는 판**이다.
 *    게임 로직(칸 좌표, 충돌 판정, 시간)은 원본 그대로 두고, 화면에 그리는
 *    방식만 바꿨다 - 뒤 버퍼에 그려 한 번에 뿌리고(깜빡임 제거), 움직이는
 *    것은 목표 칸까지 픽셀 단위로 미끄러진다(뚝뚝 끊김 제거).
 *    원본에 충실한 판은 bongbong.c 에 그대로 남아 있다.
 *      make -f bongbong.mak smooth   - BONGS.EXE 를 만든다
 * ========================================================================== */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tp.h"

/* ============================ 전역 변수 ====================================
   (원본에 심볼이 없어 DS 오프셋으로만 알 수 있었다. 오프셋을 주석에 남긴다) */

FILE_T  file_sprites;        /* 0x028C  16X16.DAT 파일 변수 */
FILE_T  file_stages;         /* 0x038C  B_ST.DAT 파일 변수 */

void  * sprite[128];         /* 0x0488  16x16 그림 42장의 버퍼 포인터 [1..42]
                                (원본은 43칸.  43번부터는 꼬리 흔들기·몸 꿈틀
                                 거리기 변형을 만들어 넣으려고 늘린 자리다.
                                 43..52 는 11~20판에 쓰는 새 먹이 그림이다) */

/* [원본에 없던 것] 11~20판에 쓰는 새 먹이 그림 10장이 들어가는 자리.
   16X16B.DAT 을 읽어 채운다.  파일이 없으면 예전처럼 원본 10가지를 돌려 쓴다. */
#define TITLE_VGM  "BB00.VGM"     /* [원본에 없던 것] 첫 화면 애드립 곡 */
#define STAGE_VGM  "BB01.VGM"     /* [원본에 없던 것] 게임 중 애드립 곡 (되풀이) */
#define OVER_VGM   "BB03.VGM"     /* [원본에 없던 것] 게임 끝 애드립 곡 */
#define CLEAR_VGM  "BB02.VGM"     /* [원본에 없던 것] 판 깼을 때 팡파르 */
#define END_VGM    "BB04.VGM"     /* [원본에 없던 것] 20판을 다 깼을 때 끝맺음 곡 */
#define FALL_VGM   "BB05.VGM"     /* [원본에 없던 것] 떨어질 때 "삐이익~" */
#define HISCORE_VGM "BB06.VGM"    /* [원본에 없던 것] 최고 기록 화면 곡 (되풀이) */
#define CONT_VGM   "BB07.VGM"     /* [원본에 없던 것] 이어하기 화면 곡 (되풀이) */
#define NEWFOOD_BASE  0x2b       /* 43 */
#define NEWFOOD_N     10
int     new_food_ok;             /* 새 먹이 그림 10장을 다 읽었는가 */
int     sprite_size;         /* 0x0534  그림 한 장의 바이트 크기 */
#define SPR_FLOOR  sprite[4]    /* 0x0498  바닥 타일     */
#define SPR_LIFE   sprite[14]   /* 0x04C0  목숨 아이콘   */

/* 원본은 스테이지가 10개라 [2700] 이었다.  20개로 늘리면서 자리도 늘렸다.
   (필요한 최대 첨자 = 20*240 + 20*12 + 12 = 5052) */
int     map_cur[5100];       /* 0x033C  현재 타일맵  [stage*240 + col*12 + row] */
int     map_orig[5100];      /* 0x15FC  원본 타일맵(스테이지 재시작용) */

int     graph_driver;        /* 0x0286  InitGraph driver */
int     graph_mode;          /* 0x0288  InitGraph mode */
byte    last_key;            /* 0x028A  마지막으로 읽은 키 */

int     stage;               /* 0x2AE4  현재 스테이지 1..10 */
int     round_no;            /* 0x2AE6  화면 오른쪽에 나오는 판 번호.
                                원본은 판이 끝없이 돌아서 통산 횟수였지만,
                                20판이 끝인 한글판에서는 stage 와 같다 */
int     lives;               /* 0x2AE8  남은 목숨(3에서 시작, 0이면 게임 끝) */
long    score;               /* 0x2B94  점수 (longint) */
long    hi_score;            /* 0x2B90  하이스코어 (longint, 초기 30000) */
int     time_left;           /* 0x2B8E  남은 시간(100에서 감소) */

int     food_tile;           /* 0x2AEA  이번 판 먹이 타일 번호(0x12..0x1B) */
int     food_left[22];       /* 0x2AB4  스테이지별 남은 먹이 수  (원본 [12]) */
int     food_total[22];      /* 0x2AC8  스테이지별 먹이 총수(원본) (원본 [12]) */
int     stage_param[88];     /* 배열 밑번지 0x2AE2, 자료는 0x2AEC 부터. 스테이지별
                                적 속도 [stage*4+i]  (원본 [48]) */

int     player_col;          /* 0x2B82  플레이어 칸(col) 1..20 */
int     player_row;          /* 0x2B84  플레이어 줄(row) 1..12 */
int     player_sprite;       /* 0x2AE0  플레이어 자세(그림 번호) */
int     move_dir;            /* 0x2B8C  플레이어 이동 방향 -1 왼쪽 / 0 정지 / +1 오른쪽 */

/* 적 4마리 (인덱스 1..4) */
/* 원본은 적이 4마리(1..4, 줄 11/8/5/2 에 한 마리씩)다.  11판부터는 한 줄에
   두 마리씩 나오도록 5..8 번을 더 쓴다 - 5번은 1번과 같은 줄, 6번은 2번과
   같은 줄... 이런 식이고 속도도 짝이 되는 적과 같다.  대신 반대쪽에서
   반대 방향으로 출발한다. */
#define ENEMY_MAX   8
#define ENEMY_ROW2_COL 14        /* 둘째 무리가 출발하는 칸.
                                    10칸(한가운데)에서 출발시키면 두 마리가
                                    거울처럼 움직여 늘 가운데에서 만났다 헤어진다.
                                    14칸에서 시작하면 만나는 자리가 한쪽으로
                                    치우쳐 줄 전체를 고루 훑는다. */

int     enemy_col[ENEMY_MAX + 1];        /* 0x2B4A  적의 칸(col) */
int     enemy_row[ENEMY_MAX + 1];        /* 0x2B52  적의 줄(row) - 11/8/5/2 */
int     enemy_dir[ENEMY_MAX + 1];        /* 0x2B3A  적의 진행 방향 (+1/-1) */
int     enemy_sprite[ENEMY_MAX + 1];     /* 0x2B5A  적의 그림 번호 */
double  enemy_last_move[ENEMY_MAX + 1];  /* 0x2B5E  마지막으로 움직인 시각
                                            (원본 6바이트 real) */

/* 움직이는 발판 */
int     platform_col;        /* 0x2B86  움직이는 발판의 칸 */
int     platform_row;        /* 0x2B88  움직이는 발판의 줄 */
int     platform_dir;        /* 0x2B8A  발판의 진행 방향 */
int     platform_sprite;     /* 0x2AE2  움직이는 발판의 현재 그림 번호 */
byte    platform_on;         /* 0x2BA0  움직이는 발판이 살아 있는가 */
byte    platform_phase;      /* 0x2BA1  발판 애니메이션 위상 */
double  platform_last_move;  /* 0x2B7C  발판이 마지막으로 움직인 시각(6바이트 real) */

int     now_sec;             /* 0x2B9A  현재 시각 - 초    (GetTime) */
int     now_csec;            /* 0x2B9C  현재 시각 - 1/100초(GetTime) */
byte    stage_cleared;       /* 0x2B9E  스테이지 클리어 플래그 */
byte    player_dead;         /* 0x2B9F  죽음 플래그 */
byte    quit_flag;           /* 0x2BA6  프로그램 종료 플래그 */

/* ===== [함수 목록]  이름 <-> 원본 오프셋 대조표 =========================
     LoadSprites       0x000A   16X16.DAT 스프라이트 42장 적재
     LoadStageData     0x01DC   B_ST.DAT 스테이지 10개 적재
     GridToPixel       0x03CC   격자->픽셀 ((n-1)*16)
     DrawTile          0x03EF   격자 칸에 스프라이트 그리기
     DrawTitleRow      0x0430   타이틀 캐릭터 줄
     TitleScreen       0x0500   타이틀 화면 + 멜로디
     DrawStage         0x07DB   스테이지 복원 후 전체 그리기
     DrawStatusBar     0x09D2   하단 상태표시줄 틀
     FormatNumber      0x0A2F   longint -> 6자리 문자열
     UpdateActors      0x0B12   매 프레임: 적4 + 발판 이동, 시간 감소
     DelayAnimated     0x0EAC   적 굴리며 대기
     EatFood           0x0F06   먹이 획득 (+100, 클리어 판정)
     EatBonus          0x1033   보너스 획득 (+500/+1000)
     SpawnPlatform     0x11D1   움직이는 발판 생성
     FallAndDie        0x1289   추락 4프레임 후 사망
     CheckCurrentCell  0x1358   선 칸 판정 -> 알맞은 처리로
     JumpMove          0x14BF   점프 / 대각선 위 이동
     LandLeft          0x1927   왼쪽 아래로 착지
     LandRight         0x1A87   오른쪽 아래로 착지
     ClimbUp           0x1BE7   사다리 위로
     WalkLeft          0x1C84   왼쪽으로 걷기
     WalkRight         0x1D18   오른쪽으로 걷기
     ClimbDown         0x1DBD   사다리 아래로
     HandleKey         0x1E5B   키 입력 처리
     StartNewGame      0x1ECB   새 게임 초기화 (목숨3/점수0/시간100)
     DeathAnim         0x1FDC   죽는 연출 + 목숨 -1
     RestartStage      0x20BC   스테이지 처음 상태로 재시작
     HandleDeath       0x22CB   죽음 처리
     StageClear        0x2309   클리어 -> 다음 판 + 시간 보너스
     GameLoop          0x24D8   한 게임의 메인 루프
     ResetGame         0x2540   게임 상태 리셋
     GameOverScreen    0x2579   GAME OVER 화면
     main              0x2602   프로그램 본체
   ====================================================================== */


/* ============================================================================
   [원본에 없던 것]  움직이는 것을 픽셀 단위로 옮기는 층
   ----------------------------------------------------------------------------
   원본은 칸(16픽셀) 단위로 "원래 타일로 지웠다가 새 칸에 다시 찍기" 를 했다.
   그래서 이동이 16픽셀씩 뚝뚝 끊기고, 지우는 순간과 찍는 순간 사이에 깜빡였다.

   여기서는
     - 게임 로직(칸 좌표 player_col/enemy_col..., 충돌, 시간)은 손대지 않고,
     - 화면에 나오는 위치만 목표 칸을 향해 시간에 비례해 미끄러지게 하고,
     - 배경(타일/글자)은 배경 페이지에 남겨 두었다가 매 프레임 스프라이트
       자국만 되살린다(GfxRestoreRect).  그래서 지우개 자국이 안 보인다.

   AcMove(id, col, row, 그림, ms)  목표 칸을 바꾸고 ms 안에 도착하게 한다
   AcPut (id, col, row, 그림)      보간 없이 그 칸에 바로 세운다
   AcSprite / AcHide / AcReset
   ========================================================================== */


/* ============================================================================
   [원본에 없던 것]  스프라이트에 입체감 주기
   ----------------------------------------------------------------------------
   원본은 CGA 4색(0 검정 / 1 청록 / 2 자홍 / 3 흰색)뿐이다.  화면은 이미
   VGA 모드 13h(256색)이므로, 색 하나마다 어두움~밝음 4단계를 팔레트에
   더 만들어 두고, 스프라이트를 읽어 들인 뒤 테두리에 음영을 넣는다.

       왼쪽 위 모서리 -> 제일 밝게      (빛이 왼쪽 위에서 온다고 본다)
       위/왼쪽 테두리 -> 밝게
       아래/오른쪽    -> 어둡게
       속             -> 원래 색

   상자 밖은 "속이 찬 것" 으로 친다.  그래야 발판처럼 옆으로 이어 붙이는
   타일에 16픽셀마다 세로 줄이 생기지 않는다.
   ========================================================================== */

/* ---- 스프라이트마다 다른 색 ------------------------------------------------
   원본은 온 화면이 청록/자홍 두 색뿐이라 열 가지 먹이가 전부 같은 자홍색이다.
   VGA 256색이니 스프라이트 종류마다 "무엇을 무슨 색으로 칠할지"(테마)를 정해
   두고, 테마마다 색 3개 x 음영 4단계 = 12칸씩 팔레트를 잡는다.
   발판·사다리·글자는 원본 색 그대로 두고, 먹이·항아리처럼 단조롭던 것만
   제 색을 준다. */

#define TH_N     19
#define TH_BASE  16                         /* 팔레트 16번부터 19*12=228칸 */
#define SH(t, c, k)  (TH_BASE + (t) * 12 + ((c) - 1) * 4 + (k))

/* 테마마다 색1 / 색2 / 색3 의 RGB (VGA DAC 0..63) */
static const unsigned char theme_rgb[TH_N][9] = {
/*  0 원본 그대로 (발판·사다리) */ { 21,63,63,  63,21,63,  58,58,58 },
/*  1 너구리 (주황)            */ { 44, 6, 0,  60,28,10,  60,56,44 },
                       /* 세 색이 각각 귀·코 / 몸 / 배다.
                          원본 CGA 는 몸이 자홍, 귀와 코가 하늘색, 배가 흰색
                          이었다 - 색이 넷뿐이라 그렇게 그린 것이다.
                          여기서는 짙은 빨강 귀·코 + 주황 몸 + 크림색 배로,
                          진짜 너구리에 가깝게 칠했다.  적(보랏빛)·발판(청록)
                          과도 잘 갈린다.  음영 4단계는 ShadeRamp 가 이 세
                          색에서 자동으로 만든다. */
/*  2 적 (보랏빛)              */ { 26,52,63,  48,18,63,  56,56,56 },
/*  3 체리                     */ { 22,52,20,  58,10,20,  58,58,58 },
/*  4 당근                     */ { 24,54,22,  63,32, 6,  58,58,58 },
/*  5 사과                     */ { 24,52,20,  60,12,14,  58,58,58 },
/*  6 바나나                   */ { 30,50,18,  56,42, 8,  61,50, 8 },
/*  7 포도                     */ { 28,52,22,  36,16,58,  58,58,58 },
/*  8 아이스크림               */ { 52,36,16,  63,36,48,  60,52,34 },
/*  9 핫도그                   */ { 58,20,14,  56,38,18,  60,56,44 },
/* 10 수박                     */ { 18,50,20,  58,14,26,  58,58,58 },
/* 11 케이크                   */ { 26,52,24,  62,24,40,  62,60,50 },
/* 12 파인애플                 */ { 26,52,20,  58,40, 8,  62,52,16 },
/* 13 보너스 항아리 (금빛)     */ { 30,60,60,  60,42,10,  62,60,46 },
/* 14 +500 / +1000 표시        */ { 63,58,24,  63,44,12,  63,63,50 },
/* 15 움직이는 발판            */ { 24,58,63,  52,30,63,  58,58,58 },
/* 16 즉사 기둥 (상아색)       */ { 30,60,63,  63,30,30,  60,56,48 }
,
/* 17 밤·버섯 (갈색)         */ { 30,18, 8,  50,32,14,  62,56,42 },
/* 18 가지 (보라)            */ { 24,52,20,  30,10,48,  52,34,63 }
};

/* 스프라이트 번호 -> 테마 번호  (0번 자리는 안 쓴다) */
static const unsigned char spr_theme[53] = {
     0,
/*  1 빈칸        */ 0,
/*  2..5 발판     */ 0, 0, 0, 0,
/*  6 사다리      */ 0,
/*  7 즉사 기둥   */ 16,
/*  8,9 적        */ 2, 2,
/* 10..17 플레이어*/ 1, 1, 1, 1, 1, 1, 1, 1,
/* 18 체리        */ 3,
/* 19 당근        */ 4,
/* 20 사과        */ 5,
/* 21 바나나      */ 6,
/* 22 포도        */ 7,
/* 23 아이스크림  */ 8,
/* 24 핫도그      */ 9,
/* 25 수박        */ 10,
/* 26 케이크      */ 11,
/* 27 파인애플    */ 12,
/* 28,29,30 항아리*/ 13, 13, 13,
/* 31..34 점수표시*/ 14, 14, 14, 14,
/* 35..38 발판    */ 15, 15, 15, 15,
/* 39..42 추락    */ 1, 1, 1, 1
,
/* --- 11~20판에 쓰는 새 먹이 10가지 (16X16B.DAT) --- */
/* 43 딸기        */ 3,
/* 44 오렌지      */ 4,
/* 45 레몬        */ 12,
/* 46 배          */ 6,
/* 47 복숭아      */ 11,
/* 48 키위        */ 10,
/* 49 라임        */ 5,
/* 50 망고스틴    */ 18,
/* 51 사포딜라    */ 17,
/* 52 파파야      */ 9
};

static void ShadeRamp(int t, int c, int r, int g, int b)
{
    /* k0 어두움 / k1 원래 색 / k2 밝게 / k3 아주 밝게 */
    SetPaletteEntry(SH(t, c, 0), r * 40 / 100, g * 40 / 100, b * 40 / 100);
    SetPaletteEntry(SH(t, c, 1), r, g, b);
    SetPaletteEntry(SH(t, c, 2), r + (63 - r) * 30 / 100,
                                 g + (63 - g) * 30 / 100,
                                 b + (63 - b) * 30 / 100);
    SetPaletteEntry(SH(t, c, 3), r + (63 - r) * 62 / 100,
                                 g + (63 - g) * 62 / 100,
                                 b + (63 - b) * 62 / 100);
}

/* (x,y) 가 비어 있는가.  좌우로 상자를 벗어나면 "속이 찬 것" 으로 친다
   (발판처럼 옆으로 이어 붙이는 타일에 16픽셀마다 세로 줄이 생기지 않게).
   위아래로 벗어나면 "빈 것" 으로 친다 (칸 위쪽에 딱 붙은 발판도 윗면이
   빛나야 하므로). */
static int sh_empty(const unsigned char *src, int x, int y)
{
    if (x < 0 || x > 15) return 0;              /* 좌우 바깥 = 찬 것 */
    if (y < 0 || y > 15) return 1;              /* 위아래 바깥 = 빈 것 */
    return src[y * 16 + x] == 0;
}

#define SPR_BLACK 249            /* 스프라이트 안쪽(눈동자) 검정 - 비치지 않는다 */

/* [원본에 없던 것] 그림 "안쪽" 에 있는 빈 칸을 진짜 검정으로 채운다.
   원본은 CGA 4색이라 눈동자·입을 색 0(검정)으로 찍었는데, 여기서는 색 0 을
   "비침"(TransPut)으로 쓰기 때문에 그 자리로 배경이 비쳐 보인다 - 눈이
   뚫린 것처럼 보이고, 발판 위에 서면 눈에 발판이 비친다.
   테두리에서 번져 닿는 색 0 만 진짜 바깥(비침)이고, 닿지 않는 색 0 은
   그림 안쪽이므로 검정으로 칠한다. */
static void FillHoles(unsigned char *buf)
{
    unsigned char *px = buf + 4;
    unsigned char seen[16 * 16];
    int st[16 * 16], sp = 0, i, k;

    memset(seen, 0, sizeof seen);
    for (i = 0; i < 16; i++) {                   /* 네 변에서 시작한다 */
        int e[4];
        e[0] = i;  e[1] = 15 * 16 + i;  e[2] = i * 16;  e[3] = i * 16 + 15;
        for (k = 0; k < 4; k++)
            if (!px[e[k]] && !seen[e[k]]) { seen[e[k]] = 1; st[sp++] = e[k]; }
    }
    while (sp > 0) {
        int p = st[--sp], x = p & 15, y = p >> 4;
        if (x >  0 && !px[p - 1]  && !seen[p - 1])  { seen[p - 1]  = 1; st[sp++] = p - 1;  }
        if (x < 15 && !px[p + 1]  && !seen[p + 1])  { seen[p + 1]  = 1; st[sp++] = p + 1;  }
        if (y >  0 && !px[p - 16] && !seen[p - 16]) { seen[p - 16] = 1; st[sp++] = p - 16; }
        if (y < 15 && !px[p + 16] && !seen[p + 16]) { seen[p + 16] = 1; st[sp++] = p + 16; }
    }
    for (i = 0; i < 16 * 16; i++)
        if (!px[i] && !seen[i]) px[i] = SPR_BLACK;
}

/* 비침으로 그려지는 것(너구리·오리)만 안쪽을 메운다.  발판·사다리·먹이는
   배경에 통째로 찍히므로 그럴 까닭이 없다. */
static int SprIsActor(int i)
{
    return (i == 8 || i == 9)                    /* 오리 */
        || (i >= 0x0a && i <= 0x11)              /* 너구리 여덟 자세 */
        || (i >= 0x1f && i <= 0x22)              /* 먹는 자세 */
        || (i >= 0x27 && i <= 0x2a);             /* 추락 */
}

static void ShadeOne(unsigned char *buf, int t)
{
    unsigned char src[16 * 16];
    int x, y;

    memcpy(src, buf + 4, sizeof src);           /* 앞 4바이트는 폭/높이 */

    for (y = 0; y < 16; y++) {
        for (x = 0; x < 16; x++) {
            int c = src[y * 16 + x];
            int up, lf, dn, rt, thick, k;

            if (c == 0 || c > 3) continue;      /* 빈 칸/이미 칠한 것은 그대로 */

            up = sh_empty(src, x, y - 1);
            lf = sh_empty(src, x - 1, y);
            dn = sh_empty(src, x, y + 1);
            rt = sh_empty(src, x + 1, y);

            /* 1~2픽셀짜리 가는 선(양쪽이 다 비었음)에는 음영을 넣지 않는다.
               넣으면 원래 그림이 뭉개져 알아보기 어려워진다. */
            thick = (!dn && !rt);

            if (thick && (up || lf))
                k = 3;                          /* 두꺼운 덩어리의 위/왼쪽 가장자리 */
            else if (thick && (sh_empty(src, x, y - 2) || sh_empty(src, x - 2, y)))
                k = 2;                          /* 그 안쪽 한 줄 */
            else if (!up && !lf && (dn || rt))
                k = 0;                          /* 아래/오른쪽 가장자리 */
            else
                k = 1;                          /* 속 / 가는 선 - 원래 색 */

            buf[4 + y * 16 + x] = (unsigned char)SH(t, c, k);
        }
    }
}

/* 먼지 색 (테마가 쓰는 16..219 뒤의 빈 칸을 쓴다) */
#define DUST_C1 244              /* 먼지 - 옅은 회색 */
#define DUST_C2 245              /* 먼지 - 짙은 회색 */
#define SCORE_C1 247             /* 떠오르는 점수 - 밝은 노랑 */
#define SCORE_C2 248             /* 떠오르는 점수 - 사라질 때 */
#define SPARK_C 246              /* 아이템 반짝임 - 흰색 */

static void ShadeSprites(void)
{
    int t, i;

    SetPaletteEntry(DUST_C1, 48, 48, 48);        /* 밝은 회색 */
    SetPaletteEntry(DUST_C2, 26, 26, 26);        /* 짙은 회색 */
    SetPaletteEntry(SCORE_C1, 63, 60, 24);       /* 떠오르는 점수 - 노랑 */
    SetPaletteEntry(SCORE_C2, 40, 34,  8);       /* 그 점수가 사그라들 때 */
    SetPaletteEntry(SPARK_C, 63, 63, 63);        /* 반짝임 (흰색) */
    SetPaletteEntry(SPR_BLACK, 0, 0, 0);         /* 눈동자 - 진짜 검정 */

    for (t = 0; t < TH_N; t++)
        for (i = 0; i < 3; i++)
            ShadeRamp(t, i + 1, theme_rgb[t][i*3], theme_rgb[t][i*3+1], theme_rgb[t][i*3+2]);

    for (i = 1; i <= NEWFOOD_BASE + NEWFOOD_N - 1; i++)
        if (sprite[i]) {
            if (SprIsActor(i)) FillHoles((unsigned char *)sprite[i]);
            ShadeOne((unsigned char *)sprite[i], spr_theme[i]);
        }
}

/* [원본에 없던 것] 이 판에 나오는 적 마리 수와, 적 i 의 속도.
   11판부터는 한 층에 두 마리씩 나온다. */
#define ENEMY_N()   ((stage > 10) ? ENEMY_MAX : 4)
#define ENEMY_SLOW_NUM 2         /* 두 마리일 때 속도를 2/3 로 늦춘다 */
#define ENEMY_LOSS     1         /* 원본이 1초마다 빠뜨리던 걸음 한 칸 */
#define ENEMY_SLOW_DEN 3

/* 적 i 의 속도(초당 몇 칸).  짝이 되는 두 마리는 B_ST.DAT 의 같은 값을 쓰되,
   한 층을 둘이 훑으면 너무 빡빡하므로 조금 느리게 한다. */
static int ENEMY_SPEED(int i)
{
    int v = stage_param[stage * 4 + ((i - 1) & 3) + 1];
    if (v == 0) return 0;                    /* 그 층엔 적이 없다 */
    if (stage > 10) {
        v = v * ENEMY_SLOW_NUM / ENEMY_SLOW_DEN;
        if (v < 1) v = 1;
    }
    /* [원본 빠르기에 맞추기] 원본은 1초마다 걸음 하나를 빠뜨려서, 적힌 속도
       보다 대략 한 칸 느리게 걸었다(속도 2 는 초당 1칸, 속도 5 는 초당 4칸...).
       걸음을 빠뜨리던 버릇은 고쳤으므로 - 안 고치면 제자리에서 어물거린다 -
       속도 쪽에서 한 칸을 빼서 눈에 보이던 빠르기를 그대로 지킨다.
       ENEMY_LOSS 를 0 으로 하면 B_ST.DAT 에 적힌 속도 그대로 달린다. */
    v -= ENEMY_LOSS;
    if (v < 1) v = 1;
    return v;
}

/* 플레이어가 한 칸 옮기는 데 걸리는 시간(ms).  원본이 그 동작에 쓰는
   시간(DelayAnimated 두 번 = 약 74ms 등)에 맞춰 두었다.  이 시간 내내
   픽셀 단위로 미끄러지므로 멈췄다 튀는 느낌이 없다. */
/* [원본에 없던 것] 상태표시줄 자리.  한글 라벨이 12픽셀이라 새로 잡았다.
   바닥 타일은 한 칸 중 위 8줄만 쓰므로 y 184 아래는 비어 있다. */
#define STAGE_MAX  20        /* B_ST.DAT 에 든 스테이지 수 (원본은 10) */

#define HUD_Y       186
/* 화면 가운데에 오도록 잡은 자리.  한글 한 글자 12, 숫자 한 글자 6 픽셀이라
   전체 폭이 292 이고 양쪽 여백이 14 씩이다. */
#define HUD_HI_X     14      /* "최고"           14..38   */
#define HUD_HIN_X    44      /* 하이스코어 6자리 44..80   */
#define HUD_SC_X     94      /* "점수"           94..118  */
#define HUD_SCN_X   124      /* 점수 6자리      124..160  */
#define HUD_TM_X    174      /* "시간"          174..198  */
#define HUD_TMN_X   204      /* 남은 시간 3자리 204..222  */
#define HUD_ST_X    236      /* "판"            236..248  */
#define HUD_STN_X   254      /* 판 번호 2자리   254..266  */
#define HUD_LIFE_X  280      /* 목숨 아이콘     280..296  */
#define HUD_LV_X    300      /* 목숨 수         300..306  */
#define NUM_W         6      /* 숫자 한 글자 폭 */

#define WALK_MS   88          /* 걷기 : DelayAnimated(0x19) x 2 */
/* 사다리 한 칸.  원본은 걷기와 똑같다(DelayAnimated(0x19) x 2).
   느리게 해 보고 싶으면 이 숫자 둘을 같은 비율로 키우면 된다.  다만 느려질수록
   시범 플레이(D)의 자동 조종이 불리해진다 - 안전 판단이 "오리가 오기까지"와
   "사다리로 피하기까지"를 견주는 것이라 그렇다.  두 배(0x32/176)까지는
   DEMO_STUCK_MS 45초 + 길찾기 사다리 값 176 으로 맞추면 20판을 다 깬다. */
#define CLIMB_DELAY 0x19
#define CLIMB_MS   88
#define DIAG_MS   78          /* 대각선 : DelayAnimated(0x25) */
#define HOP_MS   110          /* 제자리 점프 : DelayAnimated(0x32) */

#define A_PLAYER    0
#define A_PLATFORM  (ENEMY_MAX + 1)
#define A_N         (ENEMY_MAX + 2)   /* 0 플레이어, 1..8 적, 9 발판 */
#define FP          256L           /* 화면 좌표는 1/256 픽셀 고정소수점 */

static struct {
    int  on;                       /* 보이는가 */
    int  spr;                      /* 그림 번호 */
    long x, y;                     /* 지금 그려지는 자리 (x256) */
    long tx, ty;                   /* 목표 자리 (x256) */
    long vx, vy;                   /* 초당 픽셀 (x256), 항상 0 이상 */
    int  ox, oy, drawn;            /* 지난 프레임에 그린 자리 */
} act[A_N];

static long render_last_ms = 0;

static void DustClear(void);
static void SparkClear(void);
static void ScoreClear(void);

static void AcReset(void)
{
    int i;
    for (i = 0; i < A_N; i++) { act[i].on = 0; act[i].drawn = 0; }
    DustClear();
    SparkClear();
    ScoreClear();
}

static void AcMove(int id, int col, int row, int spr, int ms)
{
    long tx = (long)(col - 1) * 16 * FP;
    long ty = (long)(row - 1) * 16 * FP;
    long dx, dy;

    act[id].spr = spr;
    if (!act[id].on) {                       /* 처음 나타나면 제자리에서 시작 */
        act[id].on = 1;
        act[id].x = tx; act[id].y = ty;
    }
    act[id].tx = tx; act[id].ty = ty;
    if (ms < 1) ms = 1;
    dx = tx - act[id].x; if (dx < 0) dx = -dx;
    dy = ty - act[id].y; if (dy < 0) dy = -dy;
    act[id].vx = dx * 1000L / ms;
    act[id].vy = dy * 1000L / ms;
}

static void AcPut(int id, int col, int row, int spr)
{
    act[id].on  = 1;
    act[id].spr = spr;
    act[id].x = act[id].tx = (long)(col - 1) * 16 * FP;
    act[id].y = act[id].ty = (long)(row - 1) * 16 * FP;
    act[id].vx = act[id].vy = 0;
}

static void AcSprite(int id, int spr) { act[id].spr = spr; }
static void AcHide(int id)            { act[id].on = 0; }

static void ac_step(long *cur, long tgt, long vel, int dt)
{
    long d = tgt - *cur;
    long s;
    if (d == 0) return;
    s = vel * dt / 1000L;
    if (s <= 0) { *cur = tgt; return; }      /* 속도 0 이면 그냥 도착시킨다 */
    if (d > 0) { *cur += s; if (*cur > tgt) *cur = tgt; }
    else       { *cur -= s; if (*cur < tgt) *cur = tgt; }
}

/* ---- 걷는 모습 + 꼬리 흔들기 ---------------------------------------------
   원본에는 "걷는 자세"(0x0b / 0x0f - 몸을 낮춘 그림)가 그려져 있는데도
   대각선 점프 때만 잠깐 쓰고 걸을 때는 쓰지 않는다.  그래서 그림이 그대로
   미끄러지기만 한다.

     1) 움직이는 동안 서 있는 자세 <-> 걷는 자세를 90ms 마다 번갈아 쓴다.
        원래 있던 그림이라 한눈에 걷는 게 보인다.
     2) 그 위에 꼬리를 살짝 흔든다.  꼬리는 몸 뒤에 붙은 세로로 긴 덩어리라,
        뿌리(아래)는 그대로 두고 위로 갈수록 좌우로 많이 밀어(shear) 옮긴
        변형 두 장을 만들어 두고 왼쪽-가운데-오른쪽-가운데로 돌린다
        (WAG_MS 마다 한 단계 - 초당 두 번쯤 살랑거린다).
        16x16 상자 안이라 한 픽셀 넘게 밀면 꼬리가 잘리거나 몸에서 떨어져
        보이므로 폭은 한 픽셀로 둔다.
        서 있을 때도 흔든다 - 걸을 때만 흔들면 한 걸음이 짧아 눈에 안 띈다.
   몸통은 한 픽셀도 옮기지 않는다. */

#define TAIL_PH   4                              /* 만들어 둘 변형 장수 */
#define WAG_MS  120      /* 꼬리 한 단계 (여덟 단계가 한 번 왕복) */
#define POSE_MS  90      /* 걷는 자세가 바뀌는 간격 (한 걸음에 한 번쯤) */

/* 옆모습 꼬리는 뿌리를 축으로 "돌린다".  기울이거나(shear) 밀면 꼬리가
   몸 앞뒤로 미끄러지는 것처럼 보이는데, 뿌리를 고정하고 통째로 돌리면
   꼬리 끝이 호를 그려서 좌우로 쓸리는 것처럼 보인다.
   각도는 1/256 고정소수점 sin/cos 로 준다 (약 -22, -11, +11, +22도). */
static const short TAIL_COS[TAIL_PH] = { 237, 251, 251, 237 };
static const short TAIL_SIN[TAIL_PH] = { -96, -49,  49,  96 };
/* 사다리(뒤모습)는 그냥 좌우로 민다 */
static const signed char TAIL_DH[TAIL_PH] = { -1, -1, 1, 1 };
/* 보여 줄 순서.  -1 은 원본 그대로 */
static const signed char WAG_SEQ[8] = { -1, 1, 0, 1, -1, 2, 3, 2 };

/* base : 원본 그림 번호
   c0..c1, r0..r1 : 꼬리가 들어 있는 네모
   tip_right : 꼬리가 오른쪽에 있으면 1 (뿌리 위치와 미는 방향이 뒤집힌다)
   rot       : 1 이면 뿌리를 축으로 돌린다(옆모습), 0 이면 좌우로 민다(사다리)

   사다리 자세는 뒤에서 본 모습이라 화면 좌우가 실제 좌우와 같고, 꼬리도
   두 다리 사이로 아래로 늘어져 있다.  그래서 그때만 좌우로 민다. */
#define TAILSRC_N 5
static const struct {
    unsigned char base, c0, c1, r0, r1, tip_right, rot;
} TAILSRC[TAILSRC_N] = {
    { 0x0a, 11, 15,  6, 15, 1, 1 },   /* 왼쪽 보고 서기  - 꼬리는 오른쪽 */
    { 0x0b, 11, 15,  7, 15, 1, 1 },   /* 왼쪽 걷기        */
    { 0x0e,  0,  4,  6, 15, 0, 1 },   /* 오른쪽 보고 서기 - 꼬리는 왼쪽  */
    { 0x0f,  0,  4,  7, 15, 0, 1 },   /* 오른쪽 걷기      */
    { 0x0d,  6,  9,  8, 15, 1, 0 }    /* 사다리 (뒤에서 본 모습) */
};

static unsigned char tail_var[43][TAIL_PH];      /* 원본 번호 -> 변형 번호 */
static int extra_next = NEWFOOD_BASE + NEWFOOD_N;/* 만든 그림이 들어가는 자리 */

/* 반올림 나눗셈 (음수도 대칭으로) */
static int div_round(int num, int den)
{
    return (num >= 0) ? (num + den / 2) / den : -((-num + den / 2) / den);
}

static void MakeTailFrames(void)
{
    int t, k, x, y;

    for (t = 0; t < TAILSRC_N; t++) {
        int b    = TAILSRC[t].base;
        int c0   = TAILSRC[t].c0, c1 = TAILSRC[t].c1;
        int r0   = TAILSRC[t].r0, r1 = TAILSRC[t].r1;
        int span = r1 - r0 + 1;
        int sgn  = TAILSRC[t].tip_right ? 1 : -1;
        int rot  = TAILSRC[t].rot;
        int px   = TAILSRC[t].tip_right ? c0 : c1;   /* 꼬리 뿌리(회전축) */
        int py   = r1 - 1;
        unsigned char *src = (unsigned char *)sprite[b];

        if (!src) continue;

        for (k = 0; k < TAIL_PH; k++) {
            unsigned char *out = (unsigned char *)malloc((size_t)sprite_size);
            if (!out) return;
            memcpy(out, src, (size_t)sprite_size);

            /* 1) 꼬리가 있던 자리를 비운다 */
            for (y = r0; y <= r1; y++)
                for (x = c0; x <= c1; x++)
                    out[4 + y * 16 + x] = 0;

            if (rot) {
                /* 2a) 뿌리를 축으로 돌린다.  결과 픽셀마다 원래 자리를 거꾸로
                       찾아오므로 구멍이 생기지 않는다. */
                int co = TAIL_COS[k], si = TAIL_SIN[k] * sgn;
                for (y = r0 - 3; y <= r1; y++) {
                    for (x = c0 - 3; x <= c1 + 2; x++) {
                        int u, v, sx, sy, c;
                        if (x < 0 || x > 15 || y < 0 || y > 15) continue;
                        u = x - px; v = y - py;
                        sx = px + div_round( co * u - si * v, 256);
                        sy = py + div_round( si * u + co * v, 256);
                        if (sx < c0 || sx > c1 || sy < r0 || sy > r1) continue;
                        c = src[4 + sy * 16 + sx];
                        if (c) out[4 + y * 16 + x] = (unsigned char)c;
                    }
                }
            } else {
                /* 2b) 뿌리는 그대로, 끝(아래)으로 갈수록 많이 좌우로 민다 */
                for (y = r0; y <= r1; y++) {
                    int dx = div_round(sgn * TAIL_DH[k] * (y - r0 + 1), span);
                    for (x = c0; x <= c1; x++) {
                        int c  = src[4 + y * 16 + x];
                        int nx = x + dx;
                        if (c && nx >= 0 && nx < 16)
                            out[4 + y * 16 + nx] = (unsigned char)c;
                    }
                }
            }

            sprite[extra_next] = out;
            tail_var[b][k] = (unsigned char)extra_next;
            extra_next++;
        }
    }
}

/* ---- 눈 깜빡임 -----------------------------------------------------------
   너구리 눈은 흰자(색 3) 안에 검은 눈동자가 들어 있는 모양이다.  색칠하기
   전의 그림에서 위쪽 절반에 있는 흰자 픽셀을 찾아 눈 자리를 알아내고,
   깜빡일 때 그 자리를 살색으로 덮은 뒤 가운데에 어두운 선 하나를 긋는다.
   그림을 따로 만들지 않고 찍은 위에 덧그리므로 (걷는 자세 x 꼬리 단계)
   조합마다 그림을 만들 필요가 없다. */

#define BLINK_PERIOD 3400        /* 이 간격마다 한 번 */
#define BLINK_LEN     110        /* 감고 있는 시간(ms) */

static struct { unsigned char on, x0, y0, x1, y1; } eyebox[43];

static void FindEyes(void)       /* ShadeSprites 보다 먼저 불러야 한다 */
{
    static const unsigned char list[6] = { 0x0a, 0x0b, 0x0c, 0x0e, 0x0f, 0x10 };
    int i, x, y;

    for (i = 0; i < 6; i++) {
        int b = list[i];
        unsigned char *p = (unsigned char *)sprite[b];
        int x0 = 99, y0 = 99, x1 = -1, y1 = -1;

        if (!p) continue;
        for (y = 0; y < 8; y++) {             /* 얼굴은 위쪽 절반에 있다 */
            for (x = 0; x < 16; x++) {
                if (p[4 + y * 16 + x] != 3) continue;
                if (x < x0) x0 = x;
                if (x > x1) x1 = x;
                if (y < y0) y0 = y;
                if (y > y1) y1 = y;
            }
        }
        if (x1 < 0) continue;                 /* 사다리 자세엔 눈이 안 보인다 */
        eyebox[b].on = 1;
        eyebox[b].x0 = (unsigned char)x0; eyebox[b].y0 = (unsigned char)y0;
        eyebox[b].x1 = (unsigned char)x1; eyebox[b].y1 = (unsigned char)y1;
    }
}

static int EyesShut(long ms)
{
    return (ms % BLINK_PERIOD) < BLINK_LEN;
}

static void DrawClosedEye(int pose, int sx, int sy)
{
    int x, y, mid;
    if (pose < 0 || pose > 42 || !eyebox[pose].on) return;
    mid = (eyebox[pose].y0 + eyebox[pose].y1 + 1) / 2;
    for (y = eyebox[pose].y0; y <= eyebox[pose].y1; y++)
        for (x = eyebox[pose].x0; x <= eyebox[pose].x1; x++)
            PutPixel(sx + x, sy + y, (y == mid) ? SH(1, 2, 0) : SH(1, 2, 1));
}

/* 움직이는 동안: 서 있는 자세 <-> 걷는 자세 (원본에 있는 그림) */
static int WalkPose(int spr, long ms)
{
    if (!(((ms / POSE_MS) & 1))) return spr;
    if (spr == 0x0a) return 0x0b;
    if (spr == 0x0b) return 0x0a;
    if (spr == 0x0e) return 0x0f;
    if (spr == 0x0f) return 0x0e;
    return spr;
}

/* 그 위에 꼬리 한 픽셀 흔들기 */
static int TailWag(int spr, long ms)
{
    int k;
    if (spr < 0 || spr > 42 || !tail_var[spr][0]) return spr;
    k = WAG_SEQ[(int)((ms / WAG_MS) & 7)];
    return (k < 0) ? spr : tail_var[spr][k];
}

/* ---- 적(벌레) 몸 꿈틀거리기 --------------------------------------------
   적도 그림이 두 장(왼쪽·오른쪽)뿐이라 그냥 미끄러지기만 한다.
   머리(위 5줄)는 그대로 두고 몸통 줄들을 좌우로 한 픽셀씩 어긋나게 밀어
   물결이 아래로 흘러가는 변형 네 장을 만들어 두고 번갈아 보여 준다. */

#define WAVE_PH  4
static const signed char WAVE_TAB[8] = { 0, 1, 1, 0, 0, -1, -1, 0 };
static unsigned char wave_var[43][WAVE_PH];

#define WAVE_R0  6                                /* 몸통 시작 줄 */
#define WAVE_R1 14

static void MakeWaveFrames(void)
{
    static const unsigned char base[2] = { 8, 9 };   /* 적 그림 두 장 */
    int t, ph, x, y;

    for (t = 0; t < 2; t++) {
        int b = base[t];
        if (!sprite[b]) continue;
        for (ph = 0; ph < WAVE_PH; ph++) {
            unsigned char *src = (unsigned char *)sprite[b];
            unsigned char *out = (unsigned char *)malloc((size_t)sprite_size);
            if (!out) return;
            memcpy(out, src, (size_t)sprite_size);
            for (y = WAVE_R0; y <= WAVE_R1; y++) {
                int dx = WAVE_TAB[((y - WAVE_R0) + ph * 2) & 7];
                for (x = 0; x < 16; x++) {
                    int sx = x - dx;
                    out[4 + y * 16 + x] =
                        (sx >= 0 && sx < 16) ? src[4 + sx + y * 16] : 0;
                }
            }
            sprite[extra_next] = out;
            wave_var[b][ph] = (unsigned char)extra_next;
            extra_next++;
        }
    }
}

/* ---- 사다리를 오르내릴 때 손발 엇갈리기 ----------------------------------
   원본은 사다리 자세(0x0d)가 한 장뿐이라, 오르내려도 그림이 그대로 미끄러져
   올라갈 뿐이었다.  한쪽 손과 그 반대쪽 발을 한 픽셀 들어 올린 그림 두 장을
   만들어 한 칸 오르내릴 때마다 번갈아 쓴다.  왼손이 올라가면 오른발이,
   오른손이 올라가면 왼발이 올라가므로 실제로 사다리를 타는 것처럼 보인다.

   뒤에서 본 자세라 가운데 세로줄(6~9칸)은 꼬리다 - 거기는 건드리지 않는다.
       팔  왼쪽 x1..4  / 오른쪽 x11..14   (y9..11)
       발  왼쪽 x2..5  / 오른쪽 x10..13   (y14..15)
   꼬리 흔들림(TailWag)이 만든 그림에도 똑같이 만들어 두어야 사다리에서도
   꼬리가 살랑거린다. */
#define CLIMB_SPR 0x0d

static unsigned char climb_var[128][2];
static int climb_phase;                      /* 한 칸 갈 때마다 뒤집는다 */

/* 그 네모 안을 dy 픽셀만큼 위로 민다 (원래 자리는 비운다) */
static void climb_lift(unsigned char *out, const unsigned char *src,
                       int x0, int x1, int y0, int y1, int dy)
{
    int x, y;
    for (y = y0; y <= y1; y++)
        for (x = x0; x <= x1; x++)
            out[4 + y * 16 + x] = 0;
    for (y = y0; y <= y1; y++)
        for (x = x0; x <= x1; x++) {
            int c = src[4 + y * 16 + x];
            if (c && y - dy >= 0) out[4 + (y - dy) * 16 + x] = (unsigned char)c;
        }
}

/* 팔은 두 픽셀, 발은 한 픽셀.  16픽셀짜리 그림이라 한 픽셀만 움직이면
   눈에 잘 안 띄는데, 발은 두 픽셀 올리면 몸에서 떨어져 보인다. */
#define CLIMB_ARM_DY 2
#define CLIMB_LEG_DY 1

static void climb_make(int b)
{
    const unsigned char *src = (const unsigned char *)sprite[b];
    int k;
    if (!src || b < 0 || b >= 128) return;
    for (k = 0; k < 2; k++) {
        unsigned char *out = (unsigned char *)malloc((size_t)sprite_size);
        if (!out) return;
        memcpy(out, src, (size_t)sprite_size);
        if (k == 0) {
            climb_lift(out, src,  1,  4,  9, 11, CLIMB_ARM_DY);  /* 왼손 올리고 */
            climb_lift(out, src, 10, 13, 14, 15, CLIMB_LEG_DY);  /* 오른발 올린다 */
        } else {
            climb_lift(out, src, 11, 14,  9, 11, CLIMB_ARM_DY);  /* 오른손 올리고 */
            climb_lift(out, src,  2,  5, 14, 15, CLIMB_LEG_DY);  /* 왼발 올린다 */
        }
        sprite[extra_next] = out;
        climb_var[b][k] = (unsigned char)extra_next;
        extra_next++;
    }
}

/* MakeTailFrames 다음에 불러야 한다 (꼬리 변형까지 같이 만든다) */
static void MakeClimbFrames(void)
{
    int k;
    climb_make(CLIMB_SPR);
    for (k = 0; k < TAIL_PH; k++)
        if (tail_var[CLIMB_SPR][k]) climb_make(tail_var[CLIMB_SPR][k]);
}

static int ClimbPose(int spr)
{
    if (spr < 0 || spr >= 128 || !climb_var[spr][0]) return spr;
    return climb_var[spr][climb_phase & 1];
}

static int WaveFrame(int spr, long ms)
{
    if (spr < 0 || spr > 42 || !wave_var[spr][0]) return spr;
    return wave_var[spr][(int)((ms / 90) & 3)];
}

/* ---- 오른쪽 "지나온 판" 아이콘 --------------------------------------------
   원본은 16x16 그림을 세로로 한 줄 늘어놓아서 11개밖에 못 올린다.
   판이 20개가 되었으니 그림을 절반(8x8)으로 줄여 두 줄로 늘어놓는다.
   2 x 10 = 20개가 y 2..92 안에 들어간다. */

/* ============================================================================
   [원본에 없던 것]  층마다 다른 발판 색 (256색)
   ----------------------------------------------------------------------------
   원본 발판은 온통 한 가지 청록이라 두께도 층도 느껴지지 않는다.  256색이니
   위에서 아래로 12단계 색띠를 하나 만들어 두고, 발판 한 칸(위 8줄)을 그 띠로
   칠한다 - 윗면은 빛을 받아 희끗하고 아래로 갈수록 짙어져 두툼해 보인다.
   층마다 띠에서 시작하는 자리를 한 칸씩 내려서, 위층은 밝고 아래층은 묵직하게
   깔린다.  맨 아래 바닥은 가장 짙어 땅처럼 보인다.
   왼쪽/위 가장자리는 한 단계 밝게, 오른쪽/아래는 한 단계 어둡게 해서 빛이
   왼쪽 위에서 오는 것으로 맞춘다 (스프라이트 음영과 같은 규칙).
   칸 밖(좌우)은 "이어져 있는 것" 으로 치므로 16픽셀마다 세로 줄이 생기지 않는다.
   ========================================================================== */

#define PLAT_N    18             /* 색띠 단계 수 */
#define PLAT_H     8             /* 발판 두께 (한 칸 16줄 중 위 8줄) */
#define PLAT_STEP  2             /* 한 줄 내려갈 때마다 색띠에서 두 칸씩 */
#define PLAT_LV    4             /* 층 0(위) .. 3(맨 아래 바닥) */
#define PLAT_T0    2             /* 발판 스프라이트 번호 2..5 */
#define PLAT_T1    5
#define PLAT_PH    2             /* 빛줄기 무늬의 마디 = 두 칸(32픽셀) */

/* 색띠가 들어앉을 팔레트 칸.  4..15 와 250..255 는 다른 데서 안 쓴다. */
static const unsigned char plat_pal[PLAT_N] = {
    4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 250, 251, 252, 253, 254, 255
};

/* 원본 청록(21,63,63)을 가운데에 두고 위는 희끗하게 아래는 짙은 청록으로.
   색 자체는 예전과 같은 청록 계열이라 화면 인상이 바뀌지 않는다. */
static const unsigned char plat_rgb[PLAT_N][3] = {
    {58,63,63}, {47,63,63}, {36,63,63},
    {28,63,63}, {21,63,63}, {20,60,60},
    {19,57,58}, {17,54,55}, {16,51,53},
    {15,48,50}, {14,45,47}, {13,42,44},
    {12,39,41}, {11,36,38}, {10,33,35},
    { 9,30,32}, { 8,27,29}, { 7,24,26}
};
/* 층마다 색띠에서 시작하는 자리 (아래층일수록 짙게).
   발판은 3 / 6 / 9 행과 맨 아래 바닥에 놓이므로 네 단이면 넉넉하다. */
static const unsigned char plat_off[PLAT_LV] = { 0, 1, 2, 3 };

static void *plat_spr[PLAT_LV][PLAT_T1 + 1][PLAT_PH];

/* 행 번호 -> 층 번호.  발판은 3 / 6 / 9 행에 놓이고 맨 아래가 바닥이다. */
static int PlatLevel(int row)
{
    return (row <= 3) ? 0 : (row <= 6) ? 1 : (row <= 9) ? 2 : 3;
}

static void MakePlatFrames(void)
{
    int lv, t, x, y, ph;

    for (t = 0; t < PLAT_N; t++)
        SetPaletteEntry(plat_pal[t], plat_rgb[t][0], plat_rgb[t][1], plat_rgb[t][2]);

    for (t = PLAT_T0; t <= PLAT_T1; t++) {
        unsigned char *src = (unsigned char *)sprite[t];
        if (!src) continue;

        for (lv = 0; lv < PLAT_LV; lv++)
        for (ph = 0; ph < PLAT_PH; ph++) {
            unsigned char *out = (unsigned char *)malloc((size_t)sprite_size);
            if (!out) return;
            memcpy(out, src, (size_t)sprite_size);

            for (y = 0; y < 16; y++) {
                for (x = 0; x < 16; x++) {
                    int k, sh, up, dn, lf, rt;
                    if (!src[4 + y * 16 + x]) continue;      /* 빈 칸은 그대로 */

                    up = (y > 0)  && !src[4 + (y - 1) * 16 + x];
                    dn = (y < 15) && !src[4 + (y + 1) * 16 + x];
                    lf = (x > 0)  && !src[4 + y * 16 + (x - 1)];   /* 칸 밖은 이어짐 */
                    rt = (x < 15) && !src[4 + y * 16 + (x + 1)];

                    /* 줄마다 두 단계씩 - 위는 밝고 아래로 갈수록 짙다 */
                    k = ((y < PLAT_H) ? y : PLAT_H - 1) * PLAT_STEP;

                    /* 사선으로 스치는 빛 (유리에 비친 것처럼).
                       화면에서의 가로 자리는 col*16 + x 이고 32 로 나눈 나머지만
                       쓰므로, 칸 번호의 홀짝(ph)만 알면 된다.  그래서 빛줄기가
                       칸을 건너서도 끊기지 않고 비스듬히 길게 이어진다.
                       넓은 줄기 하나와 가는 줄기 하나가 32픽셀마다 짝을 이룬다. */
                    sh = (x + y + ph * 16) & 31;
                    if (sh == 0 || sh == 1) k -= 3;          /* 넓은 빛줄기 */
                    else if (sh == 2 || sh == 31) k -= 1;    /* 그 가장자리 */
                    else if (sh == 6) k -= 2;                /* 가는 빛줄기 */
                    else if (sh == 7) k -= 1;

                    if (up || lf) k--;                        /* 위/왼쪽 - 밝게 */
                    if (dn || rt) k++;                        /* 아래/오른쪽 - 어둡게 */
                    k += plat_off[lv];
                    if (k < 0) k = 0;
                    if (k > PLAT_N - 1) k = PLAT_N - 1;

                    out[4 + y * 16 + x] = plat_pal[k];
                }
            }
            plat_spr[lv][t][ph] = out;
        }
    }
}

/* 그 칸에 쓸 발판 그림.  칸 번호의 홀짝으로 빛줄기 무늬가 이어진다.
   발판이 아니거나 아직 안 만들었으면 원래 것 */
static void *PlatImage(int tile, int lv, int col)
{
    if (tile >= PLAT_T0 && tile <= PLAT_T1 && lv >= 0 && lv < PLAT_LV
        && plat_spr[lv][tile][col & 1])
        return plat_spr[lv][tile][col & 1];
    return sprite[tile];
}

#define ICON_W    8
#define ICON_STEP 9              /* 세로 간격 (그림 8 + 틈 1) */

static void *icon_small[20];     /* 먹이 그림 20가지의 8x8 판 */

static void MakeSmallIcons(void)
{
    int n, x, y;

    for (n = 0; n < 20; n++) {
        int spr = (n < 10) ? (0x12 + n)
                : (new_food_ok ? (NEWFOOD_BASE + n - 10) : (0x12 + n - 10));
        unsigned char *src = (unsigned char *)sprite[spr];
        unsigned char *out;
        if (!src) continue;
        out = (unsigned char *)malloc(4 + ICON_W * ICON_W);
        if (!out) return;
        out[0] = ICON_W - 1; out[1] = 0;          /* 폭-1 */
        out[2] = ICON_W - 1; out[3] = 0;          /* 높이-1 */
        for (y = 0; y < ICON_W; y++) {
            for (x = 0; x < ICON_W; x++) {
                /* 2x2 칸에서 가장 자주 나오는 색을 고른다 (빈 칸은 빼고) */
                int cnt[256], i, best = 0, bn = 0;
                for (i = 0; i < 256; i++) cnt[i] = 0;
                for (i = 0; i < 4; i++) {
                    int c = src[4 + (y * 2 + (i >> 1)) * 16 + (x * 2 + (i & 1))];
                    if (c) cnt[c]++;
                }
                for (i = 1; i < 256; i++)
                    if (cnt[i] > bn) { bn = cnt[i]; best = i; }
                out[4 + y * ICON_W + x] = (unsigned char)best;
            }
        }
        icon_small[n] = out;
    }
}

/* ---- 점프할 때 발밑 먼지 --------------------------------------------------
   원본에는 없다.  뛰어오를 때와 내려앉을 때 발밑에서 작은 점 몇 개가
   옆으로 퍼지며 살짝 올라갔다 떨어지고, 곧 어두워지다 사라진다.
   눈에 거슬리지 않게 점 3개 / 0.19초로 옅게 잡았다 (DUST_PUFF / DUST_LIFE).
   배경 페이지에는 안 남기고 뒤 버퍼에만 찍으므로 다음 프레임에 저절로 지워진다. */

#define DUST_N     12
#define DUST_PUFF   5            /* 한 번에 이는 점 개수 */
#define DUST_LIFE 280            /* 한 점이 살아 있는 시간(ms) */

static struct {
    int  on;
    long born;
    long x, y, vx, vy;           /* 1/256 픽셀, 초당 1/256 픽셀 */
    long gy;                     /* 바닥 높이 - 이보다 아래로는 안 내려간다 */
    int  ox, oy, drawn;
} dust[DUST_N];

static unsigned long dust_seed = 12345UL;

static int dust_rand(int lo, int hi)
{
    dust_seed = dust_seed * 1103515245UL + 12345UL;
    return lo + (int)((dust_seed >> 16) % (unsigned long)(hi - lo + 1));
}

/* 발밑에서 먼지를 낸다.  화면 위치가 아니라 칸 좌표로 자리를 잡으므로
   아직 미끄러지는 중이어도 먼지는 정확히 발판 위에서 인다. */
static void SpawnDustN(int col, int row, int puff, int spread)
{
    long bx = ((long)(col - 1) * 16 + 8) * FP;   /* 발 가운데 */
    long by = ((long)(row - 1) * 16 + 15) * FP;  /* 발판 바로 위 */
    int  i, n = 0;

    for (i = 0; i < DUST_N && n < puff; i++) {
        if (dust[i].on) continue;
        dust[i].on    = 1;
        dust[i].born  = TicksMs();
        dust[i].x     = bx + (long)dust_rand(-3, 3) * FP;
        dust[i].y     = by;
        dust[i].gy    = by;                      /* 이 높이 아래로는 안 간다 */
        dust[i].vx    = (long)dust_rand(-spread, spread) * FP;
        dust[i].vy    = (long)dust_rand(-30, -10) * FP;
        dust[i].drawn = 0;
        n++;
    }
}

static void SpawnDust(int col, int row)          /* 내려앉을 때 */
{
    SpawnDustN(col, row, DUST_PUFF, 52);
}

/* [원본에 없던 것] 뛰어오를 때 발밑에서 차이는 먼지.  내려앉을 때보다
   점 수를 줄이고 옆으로 더 세게 퍼뜨려, "밟고 찼다" 는 느낌만 남긴다.
   자리는 화면 좌표가 아니라 뛰기 시작한 칸으로 잡으므로, 몸이 올라간 뒤에도
   먼지는 발판 위에 그대로 남는다. */
static void SpawnDustKick(int col, int row)
{
    SpawnDustN(col, row, 3, 74);
}

static void DustClear(void)
{
    int i;
    for (i = 0; i < DUST_N; i++) { dust[i].on = 0; dust[i].drawn = 0; }
}

static void DustStep(long now, int dt)
{
    int i;
    for (i = 0; i < DUST_N; i++) {
        long age;
        int  c, x, y;
        if (!dust[i].on) continue;
        age = now - dust[i].born;
        if (age > DUST_LIFE) { dust[i].on = 0; continue; }

        dust[i].vy += 200L * FP * dt / 1000;         /* 중력 */
        dust[i].x  += dust[i].vx * dt / 1000;
        dust[i].y  += dust[i].vy * dt / 1000;
        if (dust[i].y > dust[i].gy) {                /* 바닥에 내려앉으면 멈춘다 */
            dust[i].y  = dust[i].gy;
            dust[i].vy = 0;
        }

        c = (age < DUST_LIFE / 2) ? DUST_C1          /* 밝은 회색으로 시작해 */
                                  : DUST_C2;         /* 곧 짙어지며 사라진다 */
        x = (int)(dust[i].x / FP);
        y = (int)(dust[i].y / FP);
        dust[i].ox = x; dust[i].oy = y; dust[i].drawn = 1;
        PutPixel(x, y, c);
        if (age < DUST_LIFE / 2) {                    /* 처음엔 2x2 로 굵게 */
            PutPixel(x + 1, y,     c);
            PutPixel(x,     y - 1, c);
            PutPixel(x + 1, y - 1, c);
        }
    }
}

/* ---- 아이템 반짝임 --------------------------------------------------------
   먹이·보너스 항아리·발판 타일 위에 가끔 흰 점이 반짝 하고 커졌다 사라진다.
   배경은 그대로 두고 뒤 버퍼에만 찍으므로 다음 프레임에 저절로 지워진다. */

#define SPARK_N      2           /* 동시에 반짝이는 개수 */
#define SPARK_LIFE 260           /* 한 번 반짝이는 시간(ms) */
#define SPARK_GAP  380           /* 다음 반짝임까지 쉬는 시간(ms) */
/* 색 SPARK_C 는 위 팔레트 칸에 잡아 두었다 */

static struct {
    int  on;
    long born;
    int  x, y;                   /* 화면 좌표 */
    int  drawn;
} spark[SPARK_N];
static long spark_next[SPARK_N];

/* 지금 화면에 있는 아이템 칸 하나를 고른다 (한 번 훑으며 고르는 저수지 뽑기) */
static int PickItemCell(int *px, int *py)
{
    int c, r, n = 0, cc = 0, rr = 0;
    for (r = 1; r <= 11; r++) {
        for (c = 1; c <= 20; c++) {
            int v = map_cur[stage * 240 + c * 12 + r];
            if (!((v >= 0x12 && v <= 0x1e) ||
                  (v >= NEWFOOD_BASE && v < NEWFOOD_BASE + NEWFOOD_N)))
                continue;
            n++;
            if (dust_rand(1, n) == 1) { cc = c; rr = r; }
        }
    }
    if (!n) return 0;
    *px = (cc - 1) * 16 + dust_rand(4, 11);
    *py = (rr - 1) * 16 + dust_rand(4, 11);
    return 1;
}

static void SparkStep(long now)
{
    int i;
    for (i = 0; i < SPARK_N; i++) {
        long age;
        if (!spark[i].on) {
            if (now < spark_next[i]) continue;
            if (!PickItemCell(&spark[i].x, &spark[i].y)) {
                spark_next[i] = now + SPARK_GAP;
                continue;
            }
            spark[i].on   = 1;
            spark[i].born = now;
        }
        age = now - spark[i].born;
        if (age > SPARK_LIFE) {
            spark[i].on = 0;
            spark_next[i] = now + SPARK_GAP + dust_rand(0, 700);
            continue;
        }
        PutPixel(spark[i].x, spark[i].y, SPARK_C);
        if (age > SPARK_LIFE / 4 && age < SPARK_LIFE * 3 / 4) {   /* 가운데쯤 커진다 */
            PutPixel(spark[i].x - 1, spark[i].y,     SPARK_C);
            PutPixel(spark[i].x + 1, spark[i].y,     SPARK_C);
            PutPixel(spark[i].x,     spark[i].y - 1, SPARK_C);
            PutPixel(spark[i].x,     spark[i].y + 1, SPARK_C);
        }
        spark[i].drawn = 1;
    }
}

static void SparkClear(void)
{
    int i;
    for (i = 0; i < SPARK_N; i++) {
        spark[i].on = 0; spark[i].drawn = 0; spark_next[i] = 0;
    }
}

/* ---- 먹으면 점수가 떠오른다 ----------------------------------------------
   원본에는 없다.  먹은 자리에서 "+100" 이 위로 떠오르며 옅어지다 사라진다.
   화면 아래 점수판만 바뀌면 무엇이 몇 점인지 눈에 잘 안 들어오는데, 먹은
   자리에 점수가 뜨면 보너스 항아리(+500/+1000)가 왜 좋은지 바로 보인다.
   먼지·반짝임과 마찬가지로 배경에는 안 남기고 뒤 버퍼에만 찍는다. */

#define SCORE_N      4           /* 동시에 떠 있을 수 있는 개수 */
#define SCORE_LIFE 800           /* 떠 있는 시간(ms) */
#define SCORE_RISE  18           /* 그동안 올라가는 높이(픽셀) */

static struct {
    int  on;
    long born;
    int  x, y;                   /* 뜨기 시작하는 자리 (글자 가운데 위) */
    int  ox, oy, ow, drawn;      /* 지난 프레임에 그린 자리 */
    char s[8];
} spop[SCORE_N];

static void ScoreClear(void)
{
    int i;
    for (i = 0; i < SCORE_N; i++) { spop[i].on = 0; spop[i].drawn = 0; }
}

/* 그 칸에서 점수를 띄운다 */
static void SpawnScore(int col, int row, int value)
{
    int i, pick = 0;
    long old = 0;
    for (i = 0; i < SCORE_N; i++) {
        if (!spop[i].on) { pick = i; break; }
        if (!old || spop[i].born < old) { old = spop[i].born; pick = i; }
    }
    spop[pick].on   = 1;
    spop[pick].born = TicksMs();
    spop[pick].x    = (col - 1) * 16 + 8;        /* 칸 가운데 */
    spop[pick].y    = (row - 1) * 16 + 2;
    sprintf(spop[pick].s, "+%d", value);
    spop[pick].drawn = 0;
}

static void ScoreStep(long now)
{
    int i;
    for (i = 0; i < SCORE_N; i++) {
        long age;
        int  y, w;
        if (!spop[i].on) continue;
        age = now - spop[i].born;
        if (age > SCORE_LIFE) { spop[i].on = 0; continue; }

        y = spop[i].y - (int)(age * SCORE_RISE / SCORE_LIFE);
        if (y < 0) y = 0;
        w = TextWidth(spop[i].s);
        /* 화면 밖으로 잘리지 않게 한다 (맨 왼쪽/오른쪽 칸에서 먹었을 때) */
        if (spop[i].x - w / 2 < 1)        spop[i].x = w / 2 + 1;
        if (spop[i].x + w / 2 > 318)      spop[i].x = 318 - w / 2;

        /* 마지막 1/3 동안은 어두워지며 사라진다 */
        SetColor(age * 3 < SCORE_LIFE * 2 ? SCORE_C1 : SCORE_C2);
        SetTextJustify(1, 2);                    /* 가운데 맞춤 */
        OutTextXY(spop[i].x, y, spop[i].s);
        SetTextJustify(0, 2);
        SetColor(3);

        spop[i].ox = spop[i].x - w / 2 - 1;
        spop[i].oy = y - 1;
        spop[i].ow = w + 2;
        spop[i].drawn = 1;
    }
}

/* 한 프레임: 지난 자국 지우기 -> 조금 이동 -> 다시 찍기 -> 화면으로 */
static void RenderFrame(void)
{
    long now = TicksMs();
    int  dt  = (int)(now - render_last_ms);
    int  i;

    if (dt < 0 || dt > 200) dt = 16;         /* 멈췄다 다시 돌기 시작한 경우 */
    render_last_ms = now;

    for (i = 0; i < A_N; i++)
        if (act[i].drawn) {
            GfxRestoreRect(act[i].ox, act[i].oy, 16, 16);
            act[i].drawn = 0;
        }
    for (i = 0; i < DUST_N; i++)
        if (dust[i].drawn) {
            GfxRestoreRect(dust[i].ox, dust[i].oy - 1, 2, 2);
            dust[i].drawn = 0;
        }
    for (i = 0; i < SPARK_N; i++)
        if (spark[i].drawn) {
            GfxRestoreRect(spark[i].x - 1, spark[i].y - 1, 3, 3);
            spark[i].drawn = 0;
        }
    for (i = 0; i < SCORE_N; i++)
        if (spop[i].drawn) {
            GfxRestoreRect(spop[i].ox, spop[i].oy, spop[i].ow, TEXT_H + 2);
            spop[i].drawn = 0;
        }

    for (i = 0; i < A_N; i++) {
        if (!act[i].on) continue;
        ac_step(&act[i].x, act[i].tx, act[i].vx, dt);
        ac_step(&act[i].y, act[i].ty, act[i].vy, dt);
    }

    GfxStatic(0);                            /* 움직이는 것은 배경에 안 남긴다 */
    for (i = A_N - 1; i >= 0; i--) {         /* 플레이어(0)를 맨 나중에 = 맨 위 */
        int spr, pose;
        if (!act[i].on) continue;
        spr = pose = act[i].spr;
        /* 너구리: 움직이면 걷는 자세를 섞고, 꼬리는 늘 살랑거린다 */
        if (i == A_PLAYER) {
            if (act[i].x != act[i].tx || act[i].y != act[i].ty)
                spr = WalkPose(spr, now);
            pose = spr;                          /* 눈 자리는 자세로 찾는다 */
            spr  = TailWag(spr, now);
            if (pose == CLIMB_SPR) spr = ClimbPose(spr);   /* 사다리 손발 */
        }
        else if (i >= 1 && i <= ENEMY_MAX)       /* 적은 늘 몸을 꿈틀거린다 */
            spr = WaveFrame(spr, now + i * 53);
        act[i].ox = (int)(act[i].x / FP);
        act[i].oy = (int)(act[i].y / FP);
        PutImage(act[i].ox, act[i].oy, sprite[spr], TransPut);
        if (i == A_PLAYER && EyesShut(now))      /* 가끔 눈을 깜빡인다 */
            DrawClosedEye(pose, act[i].ox, act[i].oy);
        act[i].drawn = 1;
    }
    DustStep(now, dt);                       /* 발밑 먼지 */
    SparkStep(now);                          /* 아이템 반짝임 */
    ScoreStep(now);                          /* 떠오르는 점수 */
    GfxStatic(1);

    GfxFlip();
}

/* 검증용 훅 (테스트 하네스에서 부를 수 있게) */
void SmoothRenderHook(void) { RenderFrame(); }
void SmoothScoreHook(int col, int row, int v) { SpawnScore(col, row, v); }
int  SmoothClimbHook(int k) { climb_phase = k; return ClimbPose(CLIMB_SPR); }
void SmoothEyeHook(int pose, int x, int y) { DrawClosedEye(pose, x, y); }
void SmoothShadeHook(void)  { FindEyes(); ShadeSprites(); MakeTailFrames();
                              MakeWaveFrames(); MakeClimbFrames();
                              MakeSmallIcons(); MakePlatFrames(); }


/* Delay 하는 동안에도 화면을 계속 갱신한다 */
static void DelayFrames(int ms)
{
    long end = TicksMs() + ms;
    do {
        FrameSync(120);                      /* 수직귀선이 안 잡히는 경우의 상한 */
        RenderFrame();
    } while (TicksMs() < end);
}


void LoadSprites(void);
void LoadExtraSprites(void);      /* [원본에 없던 것] 새 먹이 그림 */
int  FoodTile(int st);            /* [원본에 없던 것] 판별 먹이 타일 번호 */
void LoadStageData(void);
int  GridToPixel(int idx);
void DrawTile(int col, int row, int img);
void DrawTitleRow(int img);
int  TitleScreen(void);
void DrawStage(int stage);
void DrawStatusBar(void);
void FormatNumber(long value, char pad, char *result);
void UpdateActors(void);
void DelayAnimated(int ms);
void EatFood(void);
void EatBonus(void);
void SpawnPlatform(void);
void FallAndDie(void);
void CheckCurrentCell(void);
void JumpMove(void);
void LandLeft(void);
void LandRight(void);
void ClimbUp(void);
void WalkLeft(void);
void WalkRight(void);
void ClimbDown(void);
void HandleKey(void);
void StartNewGame(void);
void DeathAnim(void);
void RestartStage(void);
void HandleDeath(void);
void StageClear(void);
void GameLoop(void);
/* [원본에 없던 것] */
static void UpdateHiScore(void);   /* 점수가 최고 기록을 넘으면 위 칸도 바꾼다 */
static void PauseScreen(void);
static void SoundToggle(void);
static int  ContinueScreen(void);
extern byte continue_flag;
void LoadHiScores(void);
void SaveHiScores(void);
void RankingScreen(int mark);
int  DemoRun(void);
int  DemoAll(void);
void ResetGame(void);
void GameOverScreen(void);

/* ===========================================================================
   아래는 원본 어셈블리에서 옮긴 함수 본문이다
   Turbo Pascal 16bit real mode -> C
   ---------------------------------------------------------------------------
   런타임 헬퍼 식별 (seg 0x60d):
     0xcc8 = @Assign(f, name)      0xd38 = @Reset(f)       0xd91 = @Close(f)
     0x1059= @ReadChar  (파일포인터를 스택에 남긴 채 문자 1개 반환, AL)
     0x111a= @ReadInt   (정수 1개 반환, AX)
     0x1033= @ReadEnd   (Read 문 종료 - 스택의 파일포인터 정리)
     0xfeb = @ReadLnEnd (Readln 문 종료 - 줄 끝까지 건너뛰고 정리)
     0x329 = @GetMem(p, size)
   BGI (seg 0x2d3):
     0xb28 ClearDevice  0xbf3 SetLineStyle?(3인자)  0xc2f Rectangle/Bar(4인자)
     0xe00 ImageSize    0xe34 PutImage  0x1756 OutTextXY
     0x1828 PutPixel    0x183f GetImage
   @StackCheck(0x244) / @IOCheck(0x20e) 는 컴파일러 삽입 코드이므로 생략.
   =========================================================================== */

/* [원본 버그 수정]  16X16.DAT 의 그림 사이 구분줄은 앞쪽에서는 빈 줄이지만
   뒤쪽에서는 "[ 16 * 16 ] : b1.dat" 같은 이름줄이고, 그 위치도 그림 뒤에서
   그림 앞으로 바뀐다.  원본은 무조건 한 줄만 건너뛰어서 23번째 그림부터
   한 줄씩 밀렸고, 그 결과 항아리·떨어지는 너구리 같은 그림 위에 엉뚱한
   줄이 한 줄 붙고 맨 아랫줄이 잘렸다.
   여기서는 자료 줄(첫 글자가 . 1 2 3)이 나올 때까지 건너뛴다. */
static void SkipToDotData(FILE_T *f)
{
    for (;;) {
        char c = PeekChar(f);
        if (c == '.' || (c >= '1' && c <= '3')) return;
        if (c == 26) return;                 /* 파일 끝 */
        Readln(f);
    }
}


/* LoadSprites  (원래 오프셋 0x000A)
   역할: "16X16.DAT" 텍스트 파일에서 16x16 도트 그림 42장을 읽어 화면에 찍고
         GetImage 로 스프라이트 버퍼 sprite[1..42] 에 캡처한다. */
void LoadSprites(void)
{
    int  i, j, k;          /* [bp-0xe], [bp-8], [bp-6] */
    int  color = 0;        /* [bp-4]  : 매칭되는 문자가 없으면 이전 값 유지 */
    int  px = 0, py = 0;   /* [bp-0xa], [bp-0xc] */
    char ch;               /* [bp-1] */
    char sep;              /* [bp-2] : 문자 사이의 구분자(공백) - 읽고 버린다 */

    Assign(&file_sprites, "16X16.DAT");
    Reset(&file_sprites);
    Readln(&file_sprites);                       /* 첫 줄(헤더/공백) 건너뜀 */

    sprite_size = ImageSize(1, 1, 16, 16);      /* 16x16 이미지 1장 크기 */

    for (i = 1; ; i++) {                   /* 그림 42장 */

        SkipToDotData(&file_sprites);      /* [원본 버그 수정] 구분줄/이름줄 건너뛰기 */

        for (j = 1; ; j++) {               /* 세로 16줄 */
            for (k = 1; ; k++) {           /* 가로 16점 */

                if (k == 16) {
                    /* 줄의 마지막 점: 읽고 줄바꿈까지 소비 */
                    ch = ReadChar(&file_sprites);
                    Readln(&file_sprites);
                } else {
                    /* 점 하나 + 뒤따르는 구분자(공백) */
                    ch = ReadChar(&file_sprites);
                    sep = ReadChar(&file_sprites);
                    (void)sep;
                }

                /* 문자 -> CGA 색상번호 */
                if (ch == '.')      color = 0;
                else if (ch == '1') color = 1;
                else if (ch == '2') color = 2;
                else if (ch == '3') color = 3;

                /* 42장을 화면에 3줄로 배치: 1~17번은 y=0, 18~34번은 y+20,
                   35~42번은 y+40 위치에 16픽셀 간격으로 그린다. */
                if (i >= 0x12) {                   /* 18 이상 */
                    px = ((i - 0x12) << 4) + k;
                    py = j + 0x14;                 /* +20 */
                } else {
                    px = ((i - 1) << 4) + k;
                    py = j;
                }
                if (i >= 0x23) {                   /* 35 이상 (윗줄 덮어씀) */
                    px = ((i - 0x23) << 4) + k;
                    py = j + 0x28;                 /* +40 */
                }

                PutPixel(px, py, color);

                if (k == 16) break;
            }
            if (j == 16) break;
        }

        /* (원본은 여기서 Readln 으로 구분줄 한 줄을 건너뛰었다.
            위의 SkipToDotData 가 그 일을 대신한다) */

        /* 방금 그린 16x16 영역을 스프라이트로 저장 */
        GetMem(&sprite[i], sprite_size);
        GetImage(px - 15, py - 15, px, py, sprite[i]);

        if (i == 0x2a) break;              /* 42장 완료 */
    }

    Close(&file_sprites);
}


/* [원본에 없던 것] LoadExtraSprites
   역할: "16X16B.DAT" 에서 11~20판에 쓰는 새 먹이 그림 10장을 읽어
         sprite[43..52] 에 넣는다.  16X16.DAT 과 같은 글자 그림 형식이다.
         파일이 없으면 아무 일도 하지 않고, 게임은 예전처럼 원본 먹이
         10가지를 11판부터 다시 돌려 쓴다. */
void LoadExtraSprites(void)
{
    FILE_T f;
    int  i, j, k, color;
    char ch;
    unsigned char *buf;

    new_food_ok = 0;

    Assign(&f, "16X16B.DAT");
    if (!ResetTry(&f))
        return;                                 /* 없으면 조용히 넘어간다 */

    for (i = 0; i < NEWFOOD_N; i++) {

        SkipToDotData(&f);                      /* 이름줄/빈줄 건너뛰기 */

        GetMem((void **)&buf, sprite_size);
        buf[0] = 15; buf[1] = 0;                /* 폭 - 1  (16비트) */
        buf[2] = 15; buf[3] = 0;                /* 높이 - 1 */

        for (j = 0; j < 16; j++) {
            for (k = 0; k < 16; k++) {
                ch = ReadChar(&f);
                if (k < 15) ReadChar(&f);       /* 점 사이의 빈칸 */
                else        Readln(&f);
                if      (ch == '1') color = 1;
                else if (ch == '2') color = 2;
                else if (ch == '3') color = 3;
                else                color = 0;
                buf[4 + j * 16 + k] = (unsigned char)color;
            }
        }
        sprite[NEWFOOD_BASE + i] = buf;
    }

    Close(&f);
    new_food_ok = 1;
}


/* [원본에 없던 것] 그 판의 먹이 그림 번호.
   원본은 그림이 10가지뿐이라 11판부터 다시 돌려 썼다.  16X16B.DAT 이 있으면
   11~20판은 새 그림 10가지를 쓴다. */
int FoodTile(int st)
{
    if (new_food_ok && st >= 11 && st <= STAGE_MAX)
        return NEWFOOD_BASE + (st - 11);
    return 0x12 + (st - 1) % 10;
}


/* LoadStageData  (원래 오프셋 0x01DC)
   역할: "B_ST.DAT" 에서 스테이지 1~10 의 맵(11행 x 20열 타일)과 파라미터를 읽어들인다. */
void LoadStageData(void)
{
    int  row;      /* [bp-4]  : 행(row) 1..11 - 파일의 한 줄에 대응 */
    int  col;      /* [bp-2]  : 열(col) 1..20 - 한 줄 안의 글자 위치 */
    int  m;        /* [bp-6]  : 파라미터 인덱스 1..4 */
    int  v;        /* [bp-0xa]: 타일 코드 */
    char ch;       /* [bp-7]  */

    Assign(&file_stages, "B_ST.DAT");
    Reset(&file_stages);

    for (stage = 1; ; stage++) {          /* 스테이지 1..10 */

        Readln(&file_stages);                    /* 스테이지 구분 줄 건너뜀 */

        for (row = 1; ; row++) {            /* 행 1..11 (파일의 11줄) */
            for (col = 1; ; col++) {        /* 열 1..20 (한 줄에 20글자) */

                if (col == 0x14) {
                    ch = ReadChar(&file_stages); /* 줄의 마지막 글자 + 줄바꿈 */
                    Readln(&file_stages);
                } else {
                    ch = ReadChar(&file_stages);
                }

                /* 문자 -> 타일(스프라이트 번호) 변환 */
                if (ch == '.')
                    v = 1;                              /* 빈칸 */
                else
                    v = (unsigned char)ch - '0';        /* '0'~'9' */

                if (ch == '#')
                    v = FoodTile(stage);           /* 그 판의 먹이 그림
                                                      (원본: food_tile+stage-1) */

                if (ch == 'a')      v = 0x1c;           /* 28 */
                else if (ch == 'b') v = 0x1d;           /* 29 */
                else if (ch == 'c') v = 0x1e;           /* 30 */

                /* 현재 맵과 원본 맵 양쪽에 기록 */
                map_cur[stage * 240 + col * 12 + row] = v;
                map_orig[stage * 240 + col * 12 + row] = v;

                if (col == 0x14) break;
            }
            if (row == 0xb) break;
        }

        /* 스테이지 파라미터 4개 (정수) */
        for (m = 1; ; m++) {
            stage_param[stage * 4 + m] = ReadInt(&file_stages);
            if (m == 4) break;
        }

        /* 다섯 번째 정수 + 줄바꿈, 그리고 그 사본 */
        food_left[stage] = ReadInt(&file_stages);
        Readln(&file_stages);
        food_total[stage] = food_left[stage];

        /* 모든 열의 12번째 행(맵 맨 아래)은 항상 3번 타일(바닥)로 채움 */
        for (col = 1; ; col++) {
            map_cur[stage * 240 + col * 12 + 12] = 3;
            if (col == 0x14) break;
        }

        if (stage == STAGE_MAX) break;
    }

    Close(&file_stages);
}


/* GridToPixel  (원래 오프셋 0x03CC)
   역할: 타일 격자 좌표(1부터 시작)를 픽셀 좌표로 변환한다. ((idx-1)*16) */
int GridToPixel(int idx)          /* [bp+4] = 유일한 인자 (near call, ret 2) */
{
    int px;                     /* [bp-2] */

    px = (idx - 1) << 4;
    return px;
}


/* DrawTile  (원래 오프셋 0x03EF)
   역할: 격자 좌표 (col,row) 위치에 img 번 스프라이트를 그린다. */
void DrawTile(int col, int row, int img)   /* [bp+8], [bp+6], [bp+4] (ret 6) */
{
    int x;      /* [bp-2] */
    int y;      /* [bp-4] */

    x = GridToPixel(col);
    y = GridToPixel(row);

    PutImage(x, y, sprite[img], 0);         /* 0 = CopyPut */
}


/* DrawTitleRow  (원래 오프셋 0x0430)
   역할: 화면 아래쪽(y=130)에 img 번 스프라이트를 32픽셀 간격으로 10개 나란히 그린다.
         (타이틀 화면의 캐릭터 행렬 / 하단 표시줄) */
void DrawTitleRow(int img)         /* [bp+4] = 유일한 인자 (ret 2) */
{
    int y = 0x92;               /* [bp-4] : 146 (원본은 0x82=130 - 한글판에서
                                   위에 문구를 넣으려고 내렸다) */
    int i;                      /* [bp-2] */
    int x;

    for (i = 0; ; i++) {
        x = ((i << 4) << 1) + 8;            /* i*32 + 8 */
        PutImage(x, y, sprite[img], 0);
        if (i == 9) break;                  /* 10개 */
    }
}


/* [한글판] 첫 화면 제목 양옆에서 춤추는 너구리 둘.
   예전의 열 마리 줄(DrawTitleRow)을 대신한다.  제목이 세 배로 커졌으니
   너구리도 두 배로 키워 찍는다 (빈 칸은 검정으로 덮어 앞 자세를 지운다). */
#define TT_MASC_Y  20
static void PutImage2x(int x, int y, int img)
{
    const unsigned char *p = (const unsigned char *)sprite[img];
    int sx, sy;
    if (!p) return;
    p += 4;                                   /* 앞 4바이트는 폭/높이 */
    for (sy = 0; sy < 16; sy++)
        for (sx = 0; sx < 16; sx++) {
            int c = p[sy * 16 + sx];
            PutPixel(x + sx * 2,     y + sy * 2,     c);
            PutPixel(x + sx * 2 + 1, y + sy * 2,     c);
            PutPixel(x + sx * 2,     y + sy * 2 + 1, c);
            PutPixel(x + sx * 2 + 1, y + sy * 2 + 1, c);
        }
}

static void DrawTitleMascots(int img)
{
    static const int mx[2] = { 60, 228 };
    int i;
    for (i = 0; i < 2; i++)
        PutImage2x(mx[i], TT_MASC_Y, img);
}


/* TitleScreen  (원래 오프셋 0x0500)
   역할: 타이틀/인트로 화면 - 제목·제작자·조작키를 출력하고 캐릭터를 좌우로
         움직이며 멜로디를 연주한 뒤 키 입력을 기다린다(Esc 면 종료 플래그). */
int TitleScreen(void)
{
    /* 연주할 멜로디: (주파수, 지속시간ms) */
    static const struct { int hz; int ms; } melody[11] = {
        { 0x20b, 400 },   /* 523Hz  C  */
        { 0x293, 400 },   /* 659Hz  E  */
        { 0x310, 200 },   /* 784Hz  G  */
        { 0x293, 200 },   /* 659Hz  E  */
        { 0x20b, 400 },   /* 523Hz  C  */
        { 0x370, 200 },   /* 880Hz  A  */
        { 0x310, 200 },   /* 784Hz  G  */
        { 0x370, 200 },   /* 880Hz  A  */
        { 0x3dc, 200 },   /* 988Hz  B  */
        { 0x417, 400 },   /* 1047Hz C  */
        { 0x20b, 200 }    /* 523Hz  C  */
    };
    int t;      /* [bp-2] : 현재 표시중인 스프라이트 번호(10 <-> 14 를 오감) */
    int n;
    int vgm_title;              /* [원본에 없던 것] 애드립 곡이 울리고 있나 */

    AcReset();
    ClearDevice();

    /* [한글판 - 새로 꾸민 화면]
       예전에는 글이 여덟 줄로 빽빽하고 너구리가 열 마리 줄지어 있어 어수선했다.
       큰 제목 / 얇은 가로줄 / 두 칸으로 나눈 조작법 / 작게 내린 만든이 순으로
       층을 나누고, 너구리는 제목 양옆에 둘만 남겼다. */
    SetLineStyle(0, 3, 1);
    SetColor(SH(0, 1, 0));                          /* 은은한 청록 */
    Rectangle(4, 4, 315, 195);

    SetTextJustify(1, 2);

    /* 제목 - 너구리 빛깔로 크게, 뒤에 그림자 한 겹 */
    SetColor(SH(1, 1, 0));
    OutTextScaled(163, 21, "봉 봉", 3);
    SetColor(SH(1, 2, 3));
    OutTextScaled(160, 18, "봉 봉", 3);

    SetColor(SH(1, 3, 2));                          /* 크림색 */
    OutTextXY(160, 60, "VGA 한글판");

    SetColor(SH(0, 1, 0));
    Line(70, 80, 250, 80);
    Line(70, 144, 250, 144);

    /* 조작법 - 키는 오른쪽 맞춤, 뜻은 왼쪽 맞춤으로 두 칸을 만든다 */
    for (n = 0; n < 3; n++) {
        static const char *kl[3] = { "화살표", "P", "D" };
        static const char *ll[3] = { "움직이기", "멈춤", "시범 보기" };
        static const char *kr[3] = { "스페이스", "S", "Esc" };
        static const char *lr[3] = { "뛰기", "소리 끄기", "끝내기" };
        int y = 92 + n * 16;

        SetTextJustify(2, 2);
        SetColor(3);
        OutTextXY(104, y, kl[n]);
        OutTextXY(222, y, kr[n]);
        SetTextJustify(0, 2);
        SetColor(1);
        OutTextXY(112, y, ll[n]);
        OutTextXY(230, y, lr[n]);
    }

    /* 만든이 - 작고 어둑하게 맨 아래 (발판 색띠 가운데쯤 - 읽히되 앞에 안 나선다) */
    SetTextJustify(1, 2);
    SetColor(plat_pal[7]);
    OutTextXY(160, 150, "최초 만든이  Lee T.K.  1989");
    OutTextXY(160, 162, "고침  하늘소 (도스박물관)  2026");

    /* 제목 양옆에서 춤추는 너구리 둘 */
    t = 10;
    DrawTitleMascots(t);
    NoSound();
    GfxFlip();

    /* [원본에 없던 것] 애드립(OPL2)이 있으면 BB00.VGM 을 튼다.
       없으면(또는 파일이 없으면) 예전처럼 PC 스피커 멜로디를 울린다. */
    vgm_title = VgmPlay(TITLE_VGM);

    for (n = 0; n < 11; n++) {
        if (n > 0) {
            t = 0x18 - t;                   /* 10 <-> 14 */
            DrawTitleMascots(t);
            GfxFlip();
            Delay(0x1e);                    /* 30ms */
        }
        if (!vgm_title) Sound(melody[n].hz);
        Delay(melody[n].ms);
        if (!vgm_title) NoSound();
    }

    SetTextJustify(0, 2);                           /* 원래대로 */
    GfxFlip();

    /* [원본에 없던 것] 한참(12초) 아무 키도 안 누르면 시범 플레이로 넘어간다.
       D 를 누르면 1판부터 끝까지 보여 주는 "전체 시범" 이다.
       (5초는 화면을 읽고 D 를 누르기에 너무 짧았다) */
    {
        long end = TicksMs() + 12000L;
        int  blink = -1;
        /* 곡이 아직 흐르고 있으면 끝까지 들려준다 */
        while (TicksMs() < end || VgmBusy()) {
            /* [한글판] "아무 키나" 는 0.45초마다 깜빡인다 */
            int want = ((TicksMs() / 450L) & 1L) ? 0 : 1;
            if (want != blink) {
                blink = want;
                SetFillStyle(1, 0);
                Bar(80, 178, 240, 194);
                if (blink) {
                    SetColor(3);
                    SetTextJustify(1, 2);
                    OutTextXY(160, 181, "아무 키나 누르세요 !");
                    SetTextJustify(0, 2);
                }
                GfxFlip();
            }
            if (KeyPressed()) {
                VgmStop();
                last_key = ReadKey();
                if (last_key == 0) { last_key = ReadKey(); return 1; }
                if (last_key == 0x1b) { quit_flag = 1; return 1; }   /* Esc */
                if (last_key == 'd' || last_key == 'D') return 2;    /* 전체 시범 */
                return 1;
            }
            FrameSync(60);
        }
    }
    VgmStop();
    return 0;                                  /* 아무도 안 누름 -> 시범 플레이 */
}
/* ============================================================================
   --
   ----------------------------------------------------------------------------
   [배열 표기 규약]
   접어 넣는다. 아래 이름 뒤의 _a 는 "그 오프셋을 베이스로 하는 배열"이라는 뜻이며
   실제 첫 원소는 주석에 적어 둔 주소다.

     map_cur [stage*240 + col*12 + row]  : 현재 타일맵   (col 1..20, row 1..11)
     map_orig [stage*240 + col*12 + row]  : 원본(무손상) 타일맵
     sprite [tile]                      : 16x16 스프라이트 far 포인터 배열
     stage_param[stage*4 + i]              : 스테이지별 적 i(1..4) 이동속도, 베이스 0x2ae2
     food_left[stage]                    : 스테이지별 남은 먹이 개수
     enemy_dir[i]                        : 적 i 의 진행방향(+1/-1)      실주소 0x2b3c..
     enemy_col[i]                        : 적 i 의 열(col)             실주소 0x2b4c..
     enemy_row[i]                        : 적 i 의 행(row)             실주소 0x2b54..
     enemy_sprite[i]                        : 적 i 의 스프라이트 번호      실주소 0x2b5c..
     enemy_last_move[i]                        : 적 i 가 마지막으로 움직인 시각(6바이트 real)
                                          실주소 0x2b64.. (i 당 6바이트)
     platform_last_move                             : 움직이는 발판이 마지막으로 움직인 시각(real,
                                          워드 3개 0x2B7C/7E/80 을 차지)
   ============================================================================ */


/* 파스칼 문자열 대입 (dst := src;  string[max] 로 잘림)  @StrCopy */
static void strcopy(char *dst, const char *src, int max)
{
    int n = (int)strlen(src);
    if (n > max) n = max;
    memmove(dst, src, (size_t)n);
    dst[n] = '\0';
}


/* 파스칼 문자열 앞에 한 글자 덧붙이기 (s := c + s;  최대 max 글자) */
static void s_prepend(char *s, char c, int max)
{
    int n = (int)strlen(s);
    if (n > max - 1) n = max - 1;
    memmove(s + 1, s, (size_t)n);
    s[0] = c;
    s[n + 1] = '\0';
}


/* DrawStage  (원래 오프셋 0x07DB)
   역할: 한 스테이지 화면을 통째로 새로 그린다 — 원본 맵을 작업 맵으로 복사한 뒤
         타일 전체 / 맨 아래 바닥줄 / 우측 스테이지 아이콘 / 세로 구분선 / 수치를 출력. */
void DrawStage(int stage)
{
    int row;        /* [bp-4]   세로 1..11 */
    int col;        /* [bp-2]   가로 1..20 */
    int last;       /* [bp-0xe] 그릴 아이콘 개수 */
    char s[7];      /* [bp-0xc] string[6] */

    ClearDevice();

    /* 1) 원본 맵 -> 작업 맵 복원 */
    for (row = 1; row <= 11; row++)
        for (col = 1; col <= 20; col++)
            map_cur[stage * 240 + col * 12 + row] =
                map_orig[stage * 240 + col * 12 + row];

    /* 2) 현재 스테이지(stage)의 타일을 전부 찍는다 (0 == 빈 칸이라 건너뜀) */
    for (row = 1; row <= 11; row++) {
        for (col = 1; col <= 20; col++) {
            int tile = map_cur[stage * 240 + col * 12 + row];
            if (tile != 0)
                PutImage((col - 1) * 16, (row - 1) * 16,
                         PlatImage(tile, PlatLevel(row), col - 1), 0 /*CopyPut*/);
        }
    }

    /* 3) 맨 아래(y=176) 바닥 타일 한 줄 */
    for (col = 1; col <= 20; col++)
        PutImage((col - 1) * 16, 0xb0,
                 PlatImage(4, PLAT_LV - 1, col - 1),
                 0 /*CopyPut*/);      /* 맨 아래 바닥은 가장 짙게 */

    /* 4) 우측 20번 열에 "지나온 스테이지" 아이콘.  원본은 16x16 한 줄이라
          11개까지밖에 못 올렸다.  8x8 로 줄여 두 줄로 늘어놓아 20개를 담는다. */
    col = 20;
    SetColor(3);
    row = 0;
    if (stage != 1) {
        int i;
        last = stage - 1;
        if (last > 20) last = 20;
        for (i = 0; i < last; i++) {
            void *ic = icon_small[i];
            if (ic)
                PutImage(0x130 + (i & 1) * ICON_W, (i / 2) * ICON_STEP + 2,
                         ic, TransPut);
        }
        row = (last + 1) / 2;                    /* 아이콘이 차지한 줄 수 */
    }

    /* 5) 놀이판과 우측 정보란을 가르는 세로선 */
    Line(0x130, 0, 0x130, 0xa0);

    /* 6) 아이콘 바로 아래에 수치(round_no) 출력 */
    Str((long)round_no, 0, s /*string[6]*/);          /* @Str */
    OutTextXY(0x130, row * ICON_STEP + 6, s);
}


/* DrawStatusBar  (원래 오프셋 0x09D2)
   역할: 화면 하단 상태표시줄(HI-SCORE / SCORE / TIME / 목숨)의 바탕 틀을 그린다. */
void DrawStatusBar(void)
{
    char sn[8];
    char tmp[0x100];

    SetTextJustify(0, 2);           /* LeftText, TopText */
    SetTextStyle(0, 0, 1);          /* 기본 폰트, 가로, 1배 */

    SetFillStyle(1, 0);             /* SolidFill, 검정 */
    Bar(0, HUD_Y - 2, 0x13f, 0xc7); /* 상태표시줄 자리를 비운다 */

    SetColor(3);
    OutTextXY(HUD_HI_X, HUD_Y, "최고");
    OutTextXY(HUD_SC_X, HUD_Y, "점수");
    OutTextXY(HUD_TM_X, HUD_Y, "시간");
    OutTextXY(HUD_ST_X, HUD_Y, "판");

    FormatNumber(hi_score, '0', tmp);
    OutTextXY(HUD_HIN_X, HUD_Y, tmp);

    FormatNumber(score, '0', tmp);
    OutTextXY(HUD_SCN_X, HUD_Y, tmp);

    FormatNumber((long)time_left, '0', tmp);
    strcopy(sn, tmp, 6);
    Delete(sn, 1, 3);               /* 뒤 3자리만 */
    OutTextXY(HUD_TMN_X, HUD_Y, sn);

    Str((long)stage, 0, sn);                   /* [원본에 없던 것] 판 번호 */
    OutTextXY(HUD_STN_X, HUD_Y, sn);

    PutImage(HUD_LIFE_X, HUD_Y - 2, SPR_LIFE, TransPut);   /* 목숨 아이콘 */
    Str((long)lives, 0, sn);
    OutTextXY(HUD_LV_X, HUD_Y, sn);

    SetFillStyle(1, 0);
}


/* FormatNumber  (원래 오프셋 0x0A2F)
   역할: longint 값을 6자리 문자열로 만든다(모자란 앞자리는 pad 문자로 채움).
         파스칼 string 함수이므로 결과 버퍼 포인터가 숨은 인자([bp+0xa])로 넘어온다. */
void FormatNumber(long value, char pad, char *result /* string[6] */)
{
    char s[7];      /* [bp-0xe]  만들어지는 문자열 */
    int cnt;        /* [bp-4]    만들어진 자릿수 */
    int digit;      /* [bp-2] */
    int nfill;      /* [bp-0x10] */
    int i;          /* [bp-6] */

    s[0] = '\0';
    cnt  = 0;

    /* 아래 자리부터 하나씩 앞에 붙인다 */
    do {
        digit = (int)(value % 10);          /* @LongDiv 의 나머지 */
        value = value / 10;                 /* @LongDiv 의 몫 */
        s_prepend(s, (char)('0' + digit), 6);
        cnt++;
    } while (value != 0);

    /* 6자리가 안 되면 앞을 pad 로 채운다 */
    if (cnt != 6) {
        nfill = 6 - cnt;
        for (i = 1; i <= nfill; i++)
            s_prepend(s, pad, 6);
    }

    strcopy(result, s, 6);                  /* @StrCopy : result := s */
}


/* UpdateActors  (원래 오프셋 0x0B12)
   역할: 매 프레임 호출되는 "적 4마리 + 움직이는 발판" 이동/충돌/시간감소 처리기.
         시스템 시계(1/100초)를 기준으로 각 객체의 다음 이동 시각을 판정한다. */
void UpdateActors(void)
{
    int  hour;      /* [bp-0xe]  GetTime 결과(사용 안 함) */
    int  minute;    /* [bp-0x10] GetTime 결과(사용 안 함) */
    int  i;         /* [bp-2]    적 번호 1..4 */
    char buf[6+1];  /* [bp-0xc]  시간 표시용 string[6] */
    char tmp[0x100];/* [bp-0x110] FormatNumber 결과 임시 버퍼 */

    /* 초(now_sec) / 1/100초(now_csec) 를 전역에 직접 받는다 */
    GetTime(&hour, &minute, &now_sec, &now_csec);

    /* ---------------- 적 (원본 4마리, 11판부터 8마리) ---------------- */
    for (i = 1; i <= ENEMY_N(); i++) {

        /* 이 스테이지에서 속도가 0 인 적은 존재하지 않는 것 */
        if (ENEMY_SPEED(i) == 0)
            continue;

        /* 속도 * (현재시각 - 마지막이동시각) / 100 >= 1 이 되면 한 칸 이동 */
        if ((double)ENEMY_SPEED(i)
              * ((double)now_csec - enemy_last_move[i]) / 100.0 < 1.0)
            continue;

        enemy_last_move[i] = (double)now_csec;       /* 마지막 이동 시각 갱신 */

        /* 좌우로 한 칸 이동.  화면에서는 다음 이동 때까지 걸리는 시간
           (1000/속도 ms) 에 맞춰 그 칸까지 미끄러진다. */
        enemy_col[i] += enemy_dir[i];
        AcMove(i, enemy_col[i], enemy_row[i], enemy_sprite[i],
               1000 / ENEMY_SPEED(i));

        /* 플레이어와 같은 칸이면 죽음 */
        if (enemy_col[i] == player_col && enemy_row[i] == player_row)
            player_dead = 1;

        /* 좌우 끝(1 또는 19)에 닿으면 방향과 좌/우 스프라이트를 뒤집는다 */
        if (enemy_col[i] == 0x13 || enemy_col[i] == 1) {
            enemy_dir[i] = -enemy_dir[i];
            enemy_sprite[i] = 0x11 - enemy_sprite[i];   /* 짝을 이루는 두 스프라이트(합 17) */
            AcSprite(i, enemy_sprite[i]);
        }
    }

    /* ---------------- 움직이는 발판 ---------------- */
    if (platform_on != 0) {

        /* 2 * (현재시각 - 마지막이동시각) / 100 >= 0.8 => 0.4초마다 한 박자 */
        if (2.0 * ((double)now_csec - platform_last_move) / 100.0 >= 0.8) {

            platform_last_move = (double)now_csec;

            /* 두 박자에 한 칸씩 움직이며 스프라이트를 2프레임으로 번갈아 쓴다.
               실제로 칸을 옮기는 박자에서만 다음 칸까지(두 박자 = 0.8초)
               미끄러지게 한다. */
            if (platform_phase != 0) {
                platform_sprite -= platform_dir;           /* 애니메이션 프레임 되돌리기 */
                platform_phase  = 0;
                AcSprite(A_PLATFORM, platform_sprite);
            } else {
                platform_sprite += platform_dir;           /* 애니메이션 프레임 진행 */
                platform_col += platform_dir;           /* 실제로 한 칸 이동 */
                platform_phase  = 1;
                AcMove(A_PLATFORM, platform_col, platform_row, platform_sprite, 800);
            }

            /* 플레이어와 같은 칸이면 죽음 */
            if (platform_col == player_col && platform_row == player_row)
                player_dead = 1;

            /* 끝에 닿으면 방향/스프라이트 반전 */
            if ((platform_col == 0x13 || platform_col == 1) && platform_phase == 0) {
                platform_dir = -platform_dir;
                platform_sprite = 0x49 - platform_sprite;     /* 짝을 이루는 두 스프라이트(합 73) */
            }
        }
    }

    /* ---------------- 1초가 지났으면 남은 시간 1 감소 ---------------- */
    if (now_sec >= 1) {

        SetTime(0, 0, 0, 0);                /* 시계를 0 으로 되돌린다 */

        /* [원본 버릇을 고친 것] 원본은 시계를 0 으로 되돌리면서 "마지막 이동
           시각" 도 0 으로 밀어 버렸다.  그러면 1초가 넘어가는 자리에서 한 걸음이
           통째로 사라진다.  속도 2칸/초인 적은 초당 한 칸밖에 못 가서, 화면에서
           보면 제자리에서 어물거리는 것처럼 보인다.
           시계를 되돌린 만큼(100 = 1초) 같이 빼 주면 걸음 간격이 이어진다. */
        for (i = 1; i <= ENEMY_MAX; i++) {
            enemy_last_move[i] -= 100.0;
            if (enemy_last_move[i] < -100.0) enemy_last_move[i] = -100.0;
        }
        platform_last_move -= 100.0;
        if (platform_last_move < -100.0) platform_last_move = -100.0;

        time_left--;                           /* 남은 시간 */

        /* 시간 표시 영역을 지우고 다시 찍는다 */
        Bar(HUD_TMN_X, HUD_Y, HUD_TMN_X + 3 * NUM_W, 0xc7);
        FormatNumber((long)time_left, '0', tmp);
        strcopy(buf, tmp, 6);               /* @StrCopy */
        Delete(buf, 1, 3);                  /* 앞 3자리 버리고 3자리만 표시 */
        OutTextXY(HUD_TMN_X, HUD_Y, buf);

        if (time_left == 0)
            player_dead = 1;                     /* 시간 초과 => 죽음 */
    }
}


/* DelayAnimated  (원래 오프셋 0x0EAC)
   역할: ms 만큼 "대기"하되 그동안에도 UpdateActors(적 이동)을 계속 돌려 준다. */
void DelayAnimated(int ms)
{
    int n;      /* [bp-2], [bp-6]  반복 횟수 */
    int k;      /* [bp-4] */

    n = ms / 12;
    for (k = 1; k <= n; k++) {
        DelayFrames(ms / k);    /* 원본은 Delay - 여기서는 그동안 화면도 갱신 */
        UpdateActors();
    }
}


/* [원본에 없던 것] 먹을 때 나는 도-레-미-파-솔-라-시-도.
   원본은 Sound + Delay 로 한 음씩 연주해서 다 울릴 때까지(약 0.3초) 키를
   받지 않았다.  판마다 먹이가 열몇 개라 먹을 때마다 멈칫하는 느낌이 났다.
   여기서는 가락을 예약만 하고 곧바로 돌아오므로 먹으면서 계속 움직인다. */
static const int EAT_TUNE[8] = {
    0x20b, 0x24b, 0x293, 0x2ba, 0x310, 0x370, 0x3dc, 0x417
};
#define EAT_MS  37               /* 한 음 길이 (원본 DelayAnimated(0x19) 와 같다) */

/* 보너스를 먹는 자세를 언제 되돌릴지 (0 이면 없음) */
long eat_pose_end;

static void EatPosePoll(void)
{
    if (!eat_pose_end) return;
    if (TicksMs() < eat_pose_end) return;
    eat_pose_end = 0;
    AcSprite(A_PLAYER, player_sprite);
}


/* EatFood  (원래 오프셋 0x0F06)
   역할: 먹이를 하나 먹었을 때 — 도레미 상승음 연주, 점수 +100 표시,
         해당 칸을 빈 칸으로 바꾸고 남은 먹이가 0 이면 스테이지 클리어 플래그를 세운다. */
void EatFood(void)
{
    char tmp[0x100];    /* [bp-0x100] FormatNumber 결과 버퍼 */

    /* C-D-E-F-G-A-B-C 상승 음계 - 게임을 멈추지 않고 예약만 한다 */
    SoundJingle(EAT_TUNE, 8, EAT_MS);

    /* 점수 +100 (원본은 longint : 0x2B94 하위 / 0x2B96 상위) */
    score += 100;
    SpawnScore(player_col, player_row, 100);    /* [원본에 없던 것] 떠오르는 점수 */
    UpdateHiScore();

    /* 점수 표시 갱신 */
    Bar(HUD_SCN_X, HUD_Y, HUD_SCN_X + 6 * NUM_W, 0xc7);
    FormatNumber(score, '0', tmp);
    OutTextXY(HUD_SCN_X, HUD_Y, tmp);

    /* 플레이어가 서 있는 칸을 1(먹은 뒤의 빈 발판)로 바꾼다 */
    map_cur[stage * 240 + player_col * 12 + player_row] = 1;
    DrawTile(player_col, player_row, 1);    /* 배경에서도 먹이를 지운다 */

    /* 이 스테이지의 남은 먹이 개수 감소 */
    food_left[stage]--;
    if (food_left[stage] == 0)
        stage_cleared = 1;                 /* 스테이지 클리어 */
}
/* ============================================================================
   --
   ----------------------------------------------------------------------------
   [배열 표기 규약]

     map_cur [stage*240 + col*12 + row]  : 현재 타일맵   (col 1..20, row 1..11)
     sprite [tile]                      : 16x16 스프라이트 far 포인터 배열
     enemy_col[i]                        : 적 i 의 열(col)   실주소 0x2b4c..
     enemy_row[i]                        : 적 i 의 행(row)   실주소 0x2b54..

   [타일 번호 정리 — 이 배치에서 밝혀진 것]
     1     : 빈 칸(발밑이 1 이면 추락)
     7     : 즉사 타일(가시/함정)
     food_tile: 먹이 타일 번호 (주의: 앞선 분석에서는 "시간"으로 봤지만 여기서는
             타일 번호로 비교된다. 초기값 18 = 0x12)
     0x1c  : 보너스 아이템 A (+500)
     0x1d  : 보너스 아이템 B (+1000)
     0x1e  : 움직이는 발판 발생 타일

   [플레이어 스프라이트 번호 player_sprite]
     0x0a 왼쪽 정지 / 0x0b 왼쪽 걷기   0x0e 오른쪽 정지 / 0x0f 오른쪽 걷기
     0x0d 위(점프) 자세
     0x1f,0x20 왼쪽 먹는 자세(A,B)     0x21,0x22 오른쪽 먹는 자세(A,B)
     0x27..0x2a 추락 애니메이션 4프레임
   ============================================================================ */


/* EatBonus  (원래 오프셋 0x1033)
   역할: 보너스 아이템(타일 0x1c/0x1d)을 먹었을 때 — 먹는 자세로 바꾸고
         상승 음계를 울린 뒤 500/1000점을 더해 점수를 다시 찍고 칸을 비운다. */
void EatBonus(void)
{
    int  eat;           /* [bp-2]     먹는 자세 스프라이트 번호 */
    char tmp[0x100];    /* [bp-0x102] FormatNumber 결과 버퍼 */

    /* 바라보는 방향에 맞는 "먹는 자세" 스프라이트를 고른다 */
    if (player_sprite == 0x0a)
        eat = 0x1f;                 /* 왼쪽을 보고 있음 */
    else
        eat = 0x21;                 /* 오른쪽을 보고 있음 */

    /* 아이템 B(0x1d) 는 한 칸 뒤 스프라이트를 쓴다 */
    if (map_cur[stage * 240 + player_col * 12 + player_row] == 0x1d)
        eat++;

    AcSprite(A_PLAYER, eat);
    /* 먹는 자세는 가락이 끝날 때까지만.  멈춰 서서 기다리지 않고
       EatPosePoll 이 때가 되면 원래 자세로 되돌린다. */
    eat_pose_end = TicksMs() + 8 * EAT_MS;

    /* 도-레-미-파-솔-라-시-도 상승 음계 - 게임을 멈추지 않고 예약만 한다 */
    SoundJingle(EAT_TUNE, 8, EAT_MS);

    /* 점수 가산 (원본은 longint : 0x2B94 하위 / 0x2B96 상위) */
    if (map_cur[stage * 240 + player_col * 12 + player_row] == 0x1c) {
        score += 500;         /* 아이템 A */
        SpawnScore(player_col, player_row, 500);
    } else {
        score += 1000;        /* 아이템 B */
        SpawnScore(player_col, player_row, 1000);
    }
    UpdateHiScore();

    /* 점수 표시 갱신 */
    Bar(HUD_SCN_X, HUD_Y, HUD_SCN_X + 6 * NUM_W, 0xc7);
    FormatNumber(score, '0', tmp);
    OutTextXY(HUD_SCN_X, HUD_Y, tmp);

    /* 먹은 칸을 빈 칸으로 */
    map_cur[stage * 240 + player_col * 12 + player_row] = 1;
    DrawTile(player_col, player_row, 1);    /* 배경에서도 지운다 */
}


/* SpawnPlatform  (원래 오프셋 0x11D1)
   역할: 타일 0x1e 를 밟았을 때 — 효과음을 낸 뒤 플레이어 옆 칸에
         "움직이는 발판"을 생성하고 그 초기 상태(위치/방향/시각)를 세팅한다. */
void SpawnPlatform(void)
{
    /* 삑-삐 두 음 - 게임을 멈추지 않고 예약만 한다 */
    {
        static const int tune[2] = { 0x20b, 0x22a };
        SoundJingle(tune, 2, 150);
    }

    /* 발생 타일은 한 번 쓰면 사라진다 */
    map_cur[stage * 240 + player_col * 12 + player_row] = 1;
    DrawTile(player_col, player_row, 1);    /* 배경에서도 지운다 */

    platform_on = 1;                     /* 움직이는 발판 활성화 */

    /* 화면 오른쪽 절반이면 왼쪽으로, 왼쪽 절반이면 오른쪽으로 내보낸다 */
    if (player_col > 9) {
        platform_sprite = 0x23;              /* 왼쪽으로 가는 발판 스프라이트 */
        platform_col = player_col - 1;        /* 발판 col */
        platform_dir = -1;                /* 발판 진행 방향 */
    } else {
        platform_sprite = 0x26;              /* 오른쪽으로 가는 발판 스프라이트 */
        platform_col = player_col + 1;
        platform_dir = 1;
    }

    platform_row = player_row;                /* 발판 row = 플레이어 row */

    /* 마지막 이동 시각(real 6바이트) 0 으로 */
    platform_last_move = 0.0;               /* 6바이트 real 을 워드 3개로 0 클리어 */

    platform_phase = 1;                     /* 반박자 위상 */

    AcPut(A_PLATFORM, platform_col, platform_row, platform_sprite);
}


/* FallAndDie  (원래 오프셋 0x1289)
   역할: 발밑이 비었을 때의 추락 처리 — 4프레임 애니메이션으로 맨 아래(row 11)
         까지 떨어뜨린 뒤 죽음 플래그를 세운다. */
void FallAndDie(void)
{
    int row;    /* [bp-2] */
    int fall;   /* [원본에 없던 것] 떨어지는 데 걸리는 시간 */
    int fall_vgm;  /* [원본에 없던 것] 애드립으로 우는가 */

    player_sprite = 0x27;                          /* 추락 애니메이션 첫 프레임 */

    /* [원본에 없던 것] 떨어지는 내내 "삐이익~" 하고 음이 미끄러져 내려간다.
       애드립이 있으면 BB05.VGM 을 틀고(게임 곡은 잠깐 멈춘다), 없으면
       예전처럼 피시 스피커로 음을 미끄러뜨린다.  떨어지는 높이에 맞춰
       길이를 잡으므로 높은 데서 떨어질수록 길게 운다.  게임을 멈추지 않는
       소리라 떨어지는 그림과 같이 흐른다. */
    fall = (0x0b - player_row) * 0x7d;
    fall_vgm = VgmPlay(FALL_VGM);
    if (!fall_vgm)
        SoundSlide(1800, 180, fall);

    /* 한 줄씩 아래로 (row+1 .. 10) — 한 줄에 125ms 씩 미끄러진다 */
    for (row = player_row + 1; row <= 0x0a; row++) {

        AcMove(A_PLAYER, player_col, row, player_sprite, 0x7d);

        player_sprite++;                           /* 0x27..0x2a 순환 */
        if (player_sprite == 0x2b)
            player_sprite = 0x27;

        DelayFrames(0x7d);                  /* 125ms */
    }

    /* 바닥(row 11)에 처박힌다 */
    AcMove(A_PLAYER, player_col, 0x0b, player_sprite, 0x7d);
    DelayFrames(0x7d);
    SoundSlideStop();                            /* 바닥에 닿으면 소리도 끊는다 */
    if (fall_vgm) VgmStop();                     /* 애드립으로 울던 것도 끊는다
                                                    (게임 곡은 RestartStage 가
                                                     다시 틀어 준다) */
    player_row = 0x0b;
    player_dead = 1;                             /* 죽음 */
}


/* CheckCurrentCell  (원래 오프셋 0x1358)
   역할: 플레이어가 지금 서 있는 칸을 판정한다 — 함정/적/발판 충돌로 인한 죽음,
         먹이·보너스 획득, 발판 발생, 그리고 발밑이 비었으면 추락. */
void CheckCurrentCell(void)
{
    unsigned char hit;  /* [bp-3]  적/발판과 겹쳤는가 */
    int i;              /* [bp-2]  적 번호 1..4 */

    /* 1) 즉사 타일 */
    if (map_cur[stage * 240 + player_col * 12 + player_row] == 7)
        player_dead = 1;

    /* 2) 적과 같은 칸인가 */
    hit = 0;
    for (i = 1; i <= ENEMY_N(); i++) {
        if (player_col == enemy_col[i] && player_row == enemy_row[i])
            hit = 1;
    }

    /* 3) 움직이는 발판과 같은 칸인가 */
    if (platform_on != 0) {
        if (player_col == platform_col && player_row == platform_row)
            hit = 1;
    }

    if (hit == 1)
        player_dead = 1;

    /* 4) 먹이 타일이면 획득 처리 — food_tile 는 여기서 "먹이 타일 번호"로 쓰인다 (?) */
    if (map_cur[stage * 240 + player_col * 12 + player_row] == food_tile)
        EatFood();

    /* 5) 보너스 아이템 */
    if (map_cur[stage * 240 + player_col * 12 + player_row] == 0x1c ||
        map_cur[stage * 240 + player_col * 12 + player_row] == 0x1d)
        EatBonus();

    /* 6) 움직이는 발판 발생 타일 */
    if (map_cur[stage * 240 + player_col * 12 + player_row] == 0x1e)
        SpawnPlatform();

    /* 7) 발밑(row+1)이 빈 칸이면 추락 */
    if (map_cur[stage * 240 + player_col * 12 + (player_row + 1)] == 1)
        FallAndDie();
}


/* JumpMove  (원래 오프셋 0x14BF, 1128바이트 — 게임에서 가장 큰 루틴)
   역할: 플레이어 이동 본체. 현재 자세(player_sprite)에 따라 위로 제자리 점프(0x0d),
         왼쪽 위 대각선 이동(0x0a), 오른쪽 위 대각선 이동(0x0e)을 반 칸(8픽셀)
         중간 프레임까지 넣어 애니메이션하고, 이동 경로의 적과 충돌하면 죽인다. */
void JumpMove(void)
{
    int enemy;      /* 이동할 행에 해당하는 적 번호 */
    int target;     /* 충돌 검사 대상 열 */

    switch (player_sprite) {

    /* ------------------------------------------------------------------
       [0x0d] 위 — 계단참(row mod 3 == 2)에서만 제자리에서 한 칸 폴짝 뛴다.
       ------------------------------------------------------------------ */
    case 0x0d:
        if (player_row % 3 != 2)
            break;

        move_dir = 1;                             /* 방향 */
        SpawnDustKick(player_col, player_row);    /* 뛰어오르며 먼지 */

        Sound(0x417);
        DelayAnimated(0x4b);
        NoSound();

        /* 한 칸 위로 폴짝 (뒤따르는 대기 시간 내내 미끄러진다) */
        player_row--;
        AcMove(A_PLAYER, player_col, player_row, player_sprite, HOP_MS);

        DelayAnimated(0x32);

        /* 다시 원래 자리로 내려온다 */
        player_row++;
        AcMove(A_PLAYER, player_col, player_row, player_sprite, HOP_MS);
        SpawnDust(player_col, player_row);        /* 내려앉으며 먼지 */

        move_dir = 0;
        break;

    /* ------------------------------------------------------------------
       [0x0a] 왼쪽 — 왼쪽 위로 한 칸 대각선 이동.
       ------------------------------------------------------------------ */
    case 0x0a:
        if (player_col <= 2)                        /* 왼쪽 벽 */
            break;

        move_dir = -1;
        SpawnDustKick(player_col, player_row);    /* 뛰어오르며 먼지 */

        Sound(0x417);
        DelayAnimated(0x32);

        /* 지나갈 두 칸(col-2, col-1)에 담당 적이 있으면 죽음.
           적 번호는 행으로 결정된다 : enemy = 5 - (row+1) div 3
           (row 2->4, row 5->3, row 8->2, row 11->1) */
        enemy = 5 - (player_row + 1) / 3;
        for (target = player_col - 2; target <= player_col - 1; target++) {
            if (enemy_col[enemy] == target) player_dead = 1;
            if (stage > 10 && enemy_col[enemy + 4] == target) player_dead = 1;
        }

        NoSound();

        player_sprite = 0x0b;                          /* 걷는 자세 */

        /* 원본은 반 칸(8픽셀) 중간 프레임 한 장으로 대각선을 흉내냈다.
           여기서는 목표 칸까지 픽셀 단위로 미끄러진다. */
        AcMove(A_PLAYER, player_col - 1, player_row - 1, player_sprite, DIAG_MS);

        DelayAnimated(0x25);

        player_sprite = 0x0a;                          /* 정지 자세로 */

        player_col--;                               /* 왼쪽 위 대각선 한 칸 */
        player_row--;

        AcMove(A_PLAYER, player_col, player_row, player_sprite, 40);
        break;

    /* ------------------------------------------------------------------
       [0x0e] 오른쪽 — 오른쪽 위로 한 칸 대각선 이동. (0x0a 의 좌우 대칭)
       ------------------------------------------------------------------ */
    case 0x0e:
        if (player_col >= 0x12)                     /* 오른쪽 벽 */
            break;

        move_dir = 1;
        SpawnDustKick(player_col, player_row);    /* 뛰어오르며 먼지 */

        Sound(0x417);

        /* 지나갈 두 칸(col+2, col+1)에 담당 적이 있으면 죽음 */
        enemy = 5 - (player_row + 1) / 3;
        for (target = player_col + 1; target <= player_col + 2; target++) {
            if (enemy_col[enemy] == target) player_dead = 1;
            if (stage > 10 && enemy_col[enemy + 4] == target) player_dead = 1;
        }

        DelayAnimated(0x32);
        NoSound();

        player_sprite = 0x0f;                          /* 걷는 자세 */

        AcMove(A_PLAYER, player_col + 1, player_row - 1, player_sprite, DIAG_MS);

        DelayAnimated(0x25);

        player_sprite = 0x0e;                          /* 정지 자세로 */

        player_col++;                               /* 오른쪽 위 대각선 한 칸 */
        player_row--;

        AcMove(A_PLAYER, player_col, player_row, player_sprite, 40);
        break;

    default:
        break;                                  /* 그 밖의 자세는 무시 */
    }
}
/* ============================================================================
   --
               ClimbDown / HandleKey
   ----------------------------------------------------------------------------
   [배열 표기 규약]

     map_cur [stage*240 + col*12 + row]  : 현재 타일맵   (col 1..20, row 1..11)
     sprite [tile]                      : 16x16 스프라이트 far 포인터 배열

   [이 배치에서 새로 밝혀진 것]
     타일 6  : 사다리(줄) — 위/아래 키로 오르내릴 수 있는 유일한 타일
     row % 3 == 2 : 계단참(발판 위) — 좌/우 키는 이 높이에서만 먹힌다
     플레이어 스프라이트 추가분
       0x0c : 왼쪽 아래로 내려가는 걷기 자세
       0x10 : 오른쪽 아래로 내려가는 걷기 자세
     스캔코드 : 0x48 위, 0x50 아래, 0x4b 왼쪽, 0x4d 오른쪽
     스페이스(0x20) = 점프, ESC(0x1b) = 종료

   [LandLeft / LandRight 의 위치]
     메인 루프가 move_dir(방향)가 -1 이면 LandLeft, +1 이면 LandRight 을 부른다.
     즉 JumpMove 가 대각선 위로 올려보낸 뒤 남겨둔 "관성"을 소비해서
     반대로 대각선 아래로 한 칸 내려놓는(착지시키는) 루틴이다.
     끝에서 move_dir 를 0 으로 지우므로 한 번만 실행된다.
   ============================================================================ */


/* LandLeft  (원래 오프셋 0x1927)
   역할: 왼쪽 아래 대각선으로 한 칸 이동 — 반 칸(8픽셀) 중간 프레임을 거쳐
         (col-1, row+1) 로 내려놓고 방향을 지운 뒤 착지 칸을 판정한다. */
void LandLeft(void)
{
    player_sprite = 0x0c;                              /* 왼쪽 아래 걷기 자세 */

    /* 원본은 반 칸 중간 프레임 한 장 — 여기서는 픽셀 단위로 미끄러진다 */
    AcMove(A_PLAYER, player_col - 1, player_row + 1, player_sprite, DIAG_MS);

    DelayAnimated(0x25);

    player_sprite = 0x0a;                              /* 왼쪽 정지 자세로 */

    player_col--;                                   /* 왼쪽 아래 대각선 한 칸 */
    player_row++;

    AcMove(A_PLAYER, player_col, player_row, player_sprite, 40);
    SpawnDust(player_col, player_row);              /* 내려앉으며 먼지 */

    move_dir = 0;                                 /* 관성(방향) 소진 */

    CheckCurrentCell();                                /* 착지한 칸 판정 */
}


/* LandRight  (원래 오프셋 0x1A87)
   역할: 오른쪽 아래 대각선으로 한 칸 이동 — LandLeft 의 좌우 대칭. */
void LandRight(void)
{
    player_sprite = 0x10;                              /* 오른쪽 아래 걷기 자세 */

    AcMove(A_PLAYER, player_col + 1, player_row + 1, player_sprite, DIAG_MS);

    DelayAnimated(0x25);

    player_sprite = 0x0e;                              /* 오른쪽 정지 자세로 */

    player_col++;                                   /* 오른쪽 아래 대각선 한 칸 */
    player_row++;

    AcMove(A_PLAYER, player_col, player_row, player_sprite, 40);
    SpawnDust(player_col, player_row);              /* 내려앉으며 먼지 */

    move_dir = 0;                                 /* 관성(방향) 소진 */

    CheckCurrentCell();                                /* 착지한 칸 판정 */
}


/* ClimbUp  (원래 오프셋 0x1BE7)
   역할: 위쪽 화살표 — 지금 서 있는 칸이 사다리(타일 6)일 때만 한 칸 올라간다. */
void ClimbUp(void)
{
    if (map_cur[stage * 240 + player_col * 12 + player_row] != 6)
        return;                                 /* 사다리가 아니면 무시 */

    climb_phase++;                          /* [원본에 없던 것] 손발을 바꾼다 */

    /* [부드럽게] 오르는 동안 내내 미끄러진다 */
    AcMove(A_PLAYER, player_col, player_row - 1, 0x0d, CLIMB_MS);

    Sound(0x20b);
    DelayAnimated(CLIMB_DELAY);

    player_sprite = 0x0d;                              /* 위(사다리) 자세 */
    player_row--;                                   /* 한 칸 위로 */

    NoSound();

    CheckCurrentCell();                                /* 올라간 칸 판정 */
    DelayAnimated(CLIMB_DELAY);
}


/* WalkLeft  (원래 오프셋 0x1C84)
   역할: 왼쪽 화살표 — 계단참(row%3==2)에서만, 먼저 왼쪽을 보게 하고
         이미 왼쪽을 보고 있었다면 한 칸 왼쪽으로 걷는다. */
void WalkLeft(void)
{
    if (player_row % 3 != 2)                        /* 계단참 높이가 아니면 무시 */
        return;

    /* [부드럽게] 원본은 걸음 중간에 16픽셀을 건너뛰었다.  여기서는 목표 칸을
       걸음 시작에 미리 알려 주고 걸음 전체(약 WALK_MS)에 걸쳐 미끄러지게 한다. */
    {
        int step = (player_col > 1 && player_sprite == 0x0a) ? 1 : 0;
        AcMove(A_PLAYER, player_col - step, player_row,
               (player_col > 1) ? 0x0a : player_sprite, WALK_MS);
    }

    Sound(0x20b);
    DelayAnimated(0x19);

    if (player_col > 1) {                           /* 왼쪽 벽이 아니면 */
        /* 이미 왼쪽을 보고 있었을 때만 실제로 한 칸 이동 */
        if (player_sprite == 0x0a)
            player_col--;

        player_sprite = 0x0a;                          /* 왼쪽 정지 자세 */
    }

    NoSound();

    CheckCurrentCell();                                /* 옮겨간 칸 판정 */
    DelayAnimated(0x19);
}


/* WalkRight  (원래 오프셋 0x1D18)
   역할: 오른쪽 화살표 — WalkLeft 의 좌우 대칭. 오른쪽 끝 한계는
         맨 아래 줄(row 11)에서만 col 19 까지, 그 위에서는 col 18 까지다. */
void WalkRight(void)
{
    if (player_row % 3 != 2)                        /* 계단참 높이가 아니면 무시 */
        return;

    /* [부드럽게] 걸음 시작에 목표 칸을 미리 알려 준다 */
    {
        int wall = ((player_col < 0x14 && player_row == 0x0b) || player_col < 0x13);
        int step = (wall && player_sprite == 0x0e) ? 1 : 0;
        AcMove(A_PLAYER, player_col + step, player_row,
               wall ? 0x0e : player_sprite, WALK_MS);
    }

    Sound(0x20b);
    DelayAnimated(0x19);

    /* 오른쪽 벽 판정 : 맨 아랫줄만 한 칸 더 갈 수 있다 */
    if ((player_col < 0x14 && player_row == 0x0b) || player_col < 0x13) {
        /* 이미 오른쪽을 보고 있었을 때만 실제로 한 칸 이동 */
        if (player_sprite == 0x0e)
            player_col++;

        player_sprite = 0x0e;                          /* 오른쪽 정지 자세 */
    }

    NoSound();

    CheckCurrentCell();                                /* 옮겨간 칸 판정 */
    DelayAnimated(0x19);
}


/* ClimbDown  (원래 오프셋 0x1DBD)
   역할: 아래쪽 화살표 — 바로 아래 칸이 사다리(타일 6)일 때만 한 칸 내려간다. */
void ClimbDown(void)
{
    if (map_cur[stage * 240 + player_col * 12 + (player_row + 1)] != 6)
        return;                                 /* 발밑이 사다리가 아니면 무시 */

    climb_phase++;                          /* [원본에 없던 것] 손발을 바꾼다 */

    /* [부드럽게] 내려가는 동안 내내 미끄러진다 */
    AcMove(A_PLAYER, player_col, player_row + 1, 0x0d, CLIMB_MS);

    Sound(0x20b);
    DelayAnimated(CLIMB_DELAY);

    player_sprite = 0x0d;                              /* 사다리 자세 */
    player_row++;                                   /* 한 칸 아래로 */

    NoSound();

    CheckCurrentCell();                                /* 내려간 칸 판정 */
    DelayAnimated(CLIMB_DELAY);
}


/* HandleKey  (원래 오프셋 0x1E5B)
   역할: 키보드 처리 — 스페이스는 점프, ESC 는 종료, 확장키(선행 0x00)는
         상/하/좌/우 이동 루틴으로 분기한다. */
/* [원본에 없던 것] Alt + 좌/우 화살표로 스테이지를 건너뛴다 (만든 사람용) */
static void JumpStage(int delta)
{
    stage += delta;
    if (stage > STAGE_MAX) stage = 1;          /* 20 다음은 다시 1 */
    if (stage < 1)         stage = STAGE_MAX;

    /* 화면 오른쪽에 나오는 판 번호.  원본은 판이 끝없이 돌아서 통산 횟수를
       셌지만, 20판이 끝인 한글판에서는 지금 판 번호와 같아야 한다. */
    round_no = stage;

    food_tile     = FoodTile(stage);           /* 이 판의 먹이 그림 */
    player_dead   = 0;
    stage_cleared = 0;
    move_dir      = 0;

    RestartStage();
}

void HandleKey(void)
{
    last_key = ReadKey();

    if (last_key == 0x20) {                        /* 스페이스 = 점프 */
        /* 대각선 이동(관성)이 남아 있는 동안에는 점프 불가 */
        if (move_dir != 1 && move_dir != -1)
            JumpMove();
    }
    else if (last_key == 0x1b) {                   /* ESC = 게임 종료 */
        quit_flag = 1;
    }
    /* [원본에 없던 것] P 멈춤 / S 소리 끄기 */
    else if (last_key == 'p' || last_key == 'P') {
        PauseScreen();
    }
    else if (last_key == 's' || last_key == 'S') {
        SoundToggle();
    }
    else if (last_key == 0) {                      /* 확장키 : 스캔코드를 한 번 더 */
        last_key = ReadKey();

        if (last_key == 0x48)                      /* Up    */
            ClimbUp();
        else if (last_key == 0x4b)                 /* Left  */
            WalkLeft();
        else if (last_key == 0x4d)                 /* Right */
            WalkRight();
        else if (last_key == 0x50)                 /* Down  */
            ClimbDown();
        else if (last_key == 0x9b)                 /* Alt+Left  : 이전 판 */
            JumpStage(-1);
        else if (last_key == 0x9d)                 /* Alt+Right : 다음 판 */
            JumpStage(1);
    }

    /* [원본에 없던 것] 이동키를 누르고 있으면 자판이 되풀이 입력을 버퍼에
       계속 밀어 넣는다.  원본은 그걸 하나씩 꺼내 쓰기 때문에, 손을 뗀
       뒤에도 쌓인 만큼 한참 더 움직여서 그대로 함정에 빠지곤 했다.
       한 번 처리하고 나면 남은 것은 버려서 "누른 만큼만" 움직이게 한다.
       (ESC 만은 버리지 않는다 - 게임을 끝낼 수 없게 되면 곤란하다) */
    while (KeyPressed()) {
        char c = ReadKey();
        if (c == 0x1b) {
            last_key = 0x1b;
            quit_flag = 1;
            break;
        }
        if (c == 0)                                /* 확장키는 두 바이트다 */
            ReadKey();
    }
}
/* ============================================================================
   --
               GameLoop / ResetGame / GameOverScreen / main(0x2602)
   ----------------------------------------------------------------------------
   [배열 표기 규약]

     map_cur [stage*240 + col*12 + row]  : 현재 타일맵   (col 1..20, row 1..11)
     map_orig [stage*240 + col*12 + row]  : 원본(무손상) 타일맵
     sprite [tile]                      : 16x16 스프라이트 far 포인터 배열
     stage_param[stage*4 + i]              : 스테이지별 적 i(1..4) 이동속도, 베이스 0x2ae2
     food_left[stage]                    : 스테이지별 "남은" 먹이 개수
     food_total[stage]                    : 스테이지별 "원래" 먹이 개수 (재시작 때 복원용)
     enemy_dir[i] / enemy_col[i] / enemy_row[i] / enemy_sprite[i] / enemy_last_move[i]
                                        : 적 i 의 방향 / 열 / 행 / 스프라이트 / 최종이동시각

   [이 배치에서 새로 밝혀진 것]
     적 4마리의 행(row)은 고정이다 :  적1=11, 적2=8, 적3=5, 적4=2
       (JumpMove 의 enemy = 5 - (row+1) div 3 공식과 정확히 일치)
     적은 언제나 col 1, 오른쪽 방향(+1), 스프라이트 9 로 초기화된다.
     enemy_col[i] 가 0 이면 "이 스테이지에 존재하지 않는 적" 이다.
     food_tile 는 "이번 스테이지의 먹이 타일 번호"(0x12..0x1b 순환)이다.
     스프라이트 0x11 : 플레이어가 죽는 자세
     round_no : 통산 라운드 수(스테이지가 11 에서 1 로 되돌아가도 계속 증가)
   ============================================================================ */


/* StartNewGame  (원래 오프셋 0x1ECB)
   역할: 새 게임 한 판을 시작할 때의 전체 초기화 — 목숨 3 / 점수 0 / 시간 100,
         플레이어와 적 4마리를 제자리에 놓고 시스템 시계를 0 으로 되돌린다. */
void StartNewGame(void)
{
    int i;                                  /* [bp-2]  적 번호 1..4 */

    AcReset();                                     /* 화면에 남은 스프라이트 정리 */

    stage_cleared = 0;                             /* 스테이지 클리어 플래그 */
    player_dead = 0;                             /* 죽음 플래그 */

    lives = 3;                             /* 남은 목숨 */

    score = 0L;                       /* 점수(longint) = 0 */

    time_left = 100;                           /* 남은 시간 */

    player_col = 0x14;                          /* 플레이어 : 오른쪽 아래 구석 */
    player_row = 0x0b;
    player_sprite = 0x0a;                          /* 왼쪽 정지 자세 */
    move_dir = 0;                             /* 관성(방향) 없음 */

    /* 적 4마리의 행(row)은 고정 배치 */
    enemy_row[1] = 0x0b;
    enemy_row[2] = 8;
    enemy_row[3] = 5;
    enemy_row[4] = 2;

    for (i = 1; i <= ENEMY_MAX; i++) {

        /* 5..8 번은 1..4 번과 같은 줄에, 반대쪽에서 반대 방향으로 */
        int second = (i > 4);

        enemy_row[i]    = enemy_row[((i - 1) & 3) + 1];
        enemy_sprite[i] = second ? 8 : 9;        /* 8 왼쪽 / 9 오른쪽을 본다 */
        enemy_dir[i]    = second ? -1 : 1;
        enemy_last_move[i] = 0.0;                /* 마지막 이동 시각(6바이트 real) */

        if (i > ENEMY_N() || ENEMY_SPEED(i) == 0) {
            enemy_col[i] = 0;                /* 이 판에 없는 적 */
            AcHide(i);
        } else {
            enemy_col[i] = second ? ENEMY_ROW2_COL : 1;
            AcPut(i, enemy_col[i], enemy_row[i], enemy_sprite[i]);
        }
    }

    AcHide(A_PLATFORM);
    AcPut(A_PLAYER, player_col, player_row, player_sprite);   /* 플레이어 세우기 */

    /* [원본에 없던 것] 게임 곡을 튼다.  새 게임은 RestartStage 를 거치지
       않고 여기로 들어오므로, 여기에도 걸어 두어야 첫 판부터 곡이 나온다. */
    if (!VgmIsPlaying(STAGE_VGM)) VgmPlay(STAGE_VGM);

    SetTime(0, 0, 0, 0);                    /* 시계 리셋 */
}


/* DeathAnim  (원래 오프셋 0x1FDC)
   역할: 죽는 연출 — 죽는 자세(0x11)로 바꾸고 하강 음계를 울린 뒤 목숨을 하나 줄인다. */
void DeathAnim(void)
{
    SoundJingleStop();          /* 먹는 소리가 남아 있으면 끊는다 */
    SoundSlideStop();           /* 떨어지며 울던 소리도 끊는다 */
    AcSprite(A_PLAYER, 0x11);                      /* 죽는 자세 */
    RenderFrame();                                 /* 그 모습을 화면에 내보낸다 */

    /* 도-시-라-솔-파-미-레-도 하강 음계 (DelayAnimated 가 아니라 순수 Delay) */
    Sound(0x416); Delay(0x19);
    Sound(0x3dc); Delay(0x19);
    Sound(0x370); Delay(0x19);
    Sound(0x310); Delay(0x19);
    Sound(0x2ba); Delay(0x19);
    Sound(0x293); Delay(0x19);
    Sound(0x24b); Delay(0x19);
    Sound(0x20b); Delay(0x19);
    NoSound();

    lives--;                               /* 목숨 하나 소진 */
}


/* RestartStage  (원래 오프셋 0x20BC)
   역할: 현재 스테이지를 처음 상태로 되돌려 다시 그린다 — 맵/먹이수 복원,
         하단 상태표시줄 재출력, 플레이어와 적 재배치, 시계 리셋. */
void RestartStage(void)
{
    int  row;               /* [bp-0x16]  1..11 */
    int  col;               /* [bp-0x14]  1..20 */
    int  i;                 /* [bp-0x12]  적 번호 1..4 */

    /* 죽는 동안 눌린 키를 전부 비운다 */
    while (KeyPressed())
        last_key = ReadKey();

    platform_on = 0;                             /* 움직이는 발판 소멸 */
    eat_pose_end = 0;
    SoundJingleStop();
    AcReset();

    ClearDevice();

    /* 1) 현재 스테이지의 타일맵을 원본에서 복원 */
    for (row = 1; row <= 11; row++)
        for (col = 1; col <= 20; col++)
            map_cur[stage * 240 + col * 12 + row] =
                map_orig[stage * 240 + col * 12 + row];

    /* 2) 남은 먹이 개수도 원래 개수로 복원 */
    food_left[stage] = food_total[stage];

    /* 3) 스테이지 화면 + 하단 상태표시줄 다시 그리기 */
    DrawStage(stage);

    /* 4) 플레이어를 출발 위치로 */
    time_left = 100;                           /* 시간 다시 100 */

    DrawStatusBar();                           /* 상태표시줄 (한글 라벨 + 숫자) */

    player_col = 0x14;
    player_row = 0x0b;
    player_sprite = 0x0a;

    /* 5) 적 4마리 재배치 (행 enemy_row[] 는 그대로 두고 열만 되돌린다) */
    for (i = 1; i <= ENEMY_MAX; i++) {

        int second = (i > 4);

        enemy_row[i]    = enemy_row[((i - 1) & 3) + 1];
        enemy_sprite[i] = second ? 8 : 9;
        enemy_dir[i]    = second ? -1 : 1;
        enemy_last_move[i] = 0.0;

        if (i > ENEMY_N() || ENEMY_SPEED(i) == 0) {
            enemy_col[i] = 0;
            AcHide(i);
        } else {
            enemy_col[i] = second ? ENEMY_ROW2_COL : 1;
            AcPut(i, enemy_col[i], enemy_row[i], enemy_sprite[i]);
        }
    }

    AcPut(A_PLAYER, player_col, player_row, player_sprite);

    RenderFrame();                          /* 새로 그린 화면을 내보낸다 */

    /* [원본에 없던 것] 게임 중에는 BB01.VGM 이 계속 흐른다.  이미 그 곡이
       울리고 있으면 그냥 둔다 - 죽거나 다음 판으로 넘어가도 끊기지 않는다.
       (첫 화면 곡 BB00 이 울리던 중이면 여기서 바뀐다) */
    if (!VgmIsPlaying(STAGE_VGM)) VgmPlay(STAGE_VGM);

    SetTime(0, 0, 0, 0);
}


/* HandleDeath  (원래 오프셋 0x22CB)
   역할: 죽었을 때의 처리 — 죽는 연출을 보이고 잠깐 쉰 뒤 스테이지를 다시 시작한다. */
void HandleDeath(void)
{
    DeathAnim();            /* 죽는 연출 + 목숨 -1 */
    Delay(0x1f4);           /* 500ms */
    player_dead = 0;             /* 죽음 플래그 해제 */
    RestartStage();            /* 스테이지 재시작 */
}


/* [원본에 없던 것] 마지막 판을 다 깼을 때의 축하 화면 */
static void AllClearScreen(void)
{
    char tmp[0x100];
    char line[0x120];
    int  n;

    AcReset();
    ClearDevice();

    SetColor(3);
    SetTextJustify(1, 2);
    OutTextXY(160, 0x32, "축하합니다 !");
    OutTextXY(160, 0x46, "20판을 모두 깼습니다");

    FormatNumber(score, ' ', tmp);
    strcopy(line, "최종 점수 ", 0xff);
    strcat(line, tmp);
    OutTextXY(160, 0x82, line);
    SetTextJustify(0, 2);
    GfxFlip();

    /* [원본과 다름] 축하 팡파르 - 애드립이 있으면 끝맺음 곡(BB04.VGM)을
       끝까지 들려주고, 없으면 예전처럼 피시 스피커로 세 번 운다.
       곡이 흐르는 동안에도 아무 키나 누르면 넘어간다. */
    if (VgmPlay(END_VGM)) {
        long end = TicksMs() + 20000L;
        while (VgmBusy() && TicksMs() < end && !KeyPressed())
            FrameSync(60);
        while (KeyPressed()) ReadKey();
        VgmStop();
    } else {
        for (n = 0; n < 3; n++) {
            Sound(0x20b); Delay(0x64);
            Sound(0x310); Delay(0x64);
            Sound(0x417); Delay(0xc8);
        }
        NoSound();
        Delay(0x3e8);
    }
}


/* StageClear  (원래 오프셋 0x2309)
   역할: 스테이지 클리어 처리 — 다음 먹이/스테이지로 넘기고(11 이면 1 로 되돌리며
         적 속도를 올린다) 남은 시간 x100 보너스와 다음 먹이 그림을 보여 준다. */
void StageClear(void)
{
    char s[7];              /* [bp-8]    string[6] */
    char tmp[0x100];        /* [bp-0x112] FormatNumber / @Concat 임시 버퍼 */
    int  all_clear = 0;     /* [원본에 없던 것] 마지막 판을 깼는가 */
    int  clear_vgm = 0;     /* [원본에 없던 것] 팡파르가 애드립으로 나가는가 */

    SoundJingleStop();          /* 먹는 소리가 남아 있으면 끊는다 */

    player_dead = 0;
    stage_cleared = 0;

    AcReset();
    ClearDevice();

    stage++;               /* 다음 스테이지 */

    /* 다음 판의 먹이 그림.  1~10판은 원본 10가지, 11~20판은 16X16B.DAT 의
       새 10가지(파일이 없으면 예전처럼 원본을 다시 돌려 쓴다) */
    food_tile = FoodTile(stage);
    round_no = stage;      /* 화면에 나오는 판 번호 (원본은 통산 라운드 수) */

    /* [원본과 다름] 원본은 마지막 판을 넘기면 1판으로 되돌아가며 적을 +2
       빠르게 해서 끝없이 돌았다.  여기서는 20판이 끝이므로 축하 화면을
       보이고 게임을 마친다.  (원본 규칙은 bongbong.c 에 그대로 남아 있다) */
    if (stage == STAGE_MAX + 1)
        all_clear = 1;

    /* 남은 시간 x 100 을 점수에 더한다 (longint) */
    score += (long)time_left * 100;
    UpdateHiScore();

    /* "BONUS is +nnnn" — 6자리로 만든 뒤 앞 2자리를 잘라 4자리만 보여 준다 */
    FormatNumber((long)time_left * 100, '0', tmp);
    strcopy(s, tmp, 6);                     /* @StrCopy */
    Delete(s, 1, 2);                        /* @Delete  */

    strcopy(tmp, "보너스 +", 0xff);         /* @Concat */
    strcat(tmp, s);
    SetTextJustify(1, 2);
    OutTextXY(160, 0x51, tmp);              /* [원본과 다름] 한 칸(16픽셀) 위로 */
    SetTextJustify(0, 2);
    GfxFlip();

    /* 클리어 팡파르.  [원본에 없던 것] 애드립이 있으면 BB02.VGM 을 울리고,
       없으면 예전처럼 PC 스피커로 도-미-솔-도 를 낸다. */
    clear_vgm = VgmPlay(CLEAR_VGM);
    if (!clear_vgm) {
        Sound(0x20b); Delay(0x64);
        Sound(0x293); Delay(0x64);
        Sound(0x310); Delay(0x64);
        Sound(0x417); Delay(0x64);
        NoSound();
    } else {
        Delay(0x190);                       /* 팡파르 앞부분은 듣고 넘어간다 */
    }

    /* [원본에 없던 것] 마지막 판을 깼으면 축하 화면을 보이고 게임을 끝낸다 */
    if (all_clear) {
        Delay(0x2bc);
        AllClearScreen();
        stage    = STAGE_MAX;               /* 배열 범위를 넘지 않게 되돌린다 */
        lives    = 0;                       /* GameLoop 을 빠져나간다 */
        move_dir = 0;
        return;
    }

    /* 다음 스테이지의 먹이가 무엇인지 그림으로 보여 준다 */
    OutTextXY(0x90, 0x72, "다음 먹이는");   /* [원본과 다름] 한 칸 위로 */
    PutImage(0xd8, 0x6e, sprite[food_tile], 0 /*CopyPut*/);
    GfxFlip();

    /* [원본과 다름] 팡파르가 끝날 때까지 기다린다 (없으면 예전처럼 1.5초).
       기다리는 동안에도 화면은 계속 돌아가므로 곡이 이어서 울린다. */
    if (clear_vgm) {
        long end = TicksMs() + 3500L;
        while (VgmBusy() && TicksMs() < end) FrameSync(60);
    } else {
        Delay(0x5dc);                       /* 1.5초 */
    }

    RestartStage();                            /* 새 스테이지 화면 세팅 */
}


/* ===========================================================================
   [원본에 없던 것]  최고 점수표 / 멈춤 / 소리 끄기 / 이어하기 / 시범 플레이
   =========================================================================== */

/* ---- 최고 점수표 (BONG.HI) ---------------------------------------------- */

#define HI_N     5               /* 다섯 자리까지 남긴다 */
#define HI_CH    3               /* 이름 세 글자 */

static char hi_name[HI_N][HI_CH + 1];
static long hi_val[HI_N];
static byte hi_dirty;            /* 저장할 것이 있는가 */

static void HiDefault(void)
{
    static const char *nm[HI_N] = { "TKL", "SKY", "DOS", "BNG", "M89" };
    static const long sc[HI_N]  = { 30000L, 24000L, 18000L, 12000L, 6000L };
    int i, k;
    for (i = 0; i < HI_N; i++) {
        for (k = 0; k < HI_CH; k++) hi_name[i][k] = nm[i][k];
        hi_name[i][HI_CH] = '\0';
        hi_val[i] = sc[i];
    }
}

void LoadHiScores(void)
{
    FILE *fp;
    int i, k;

    HiDefault();

    fp = fopen("BONG.HI", "r");
    if (fp) {
        for (i = 0; i < HI_N; i++) {
            char nm[32];
            long v;
            if (fscanf(fp, "%8s %ld", nm, &v) != 2) break;
            for (k = 0; k < HI_CH; k++) hi_name[i][k] = nm[k] ? nm[k] : ' ';
            hi_name[i][HI_CH] = '\0';
            if (v < 0L) v = 0L;
            hi_val[i] = v;
        }
        fclose(fp);
    }
    hi_score = hi_val[0];
    hi_dirty = 0;
}

void SaveHiScores(void)
{
    FILE *fp;
    int i;
    if (!hi_dirty) return;
    fp = fopen("BONG.HI", "w");
    if (!fp) return;                        /* 쓸 수 없으면 조용히 넘어간다 */
    for (i = 0; i < HI_N; i++)
        fprintf(fp, "%s %ld\n", hi_name[i], hi_val[i]);
    fclose(fp);
    hi_dirty = 0;
}

/* 화면 위쪽 "최고" 칸을 지금 점수로 갱신한다 (점수가 바뀔 때마다 부른다) */
static void UpdateHiScore(void)
{
    char tmp[0x100];
    if (score <= hi_score) return;
    hi_score = score;
    SetFillStyle(1, 0);
    Bar(HUD_HIN_X, HUD_Y, HUD_HIN_X + 6 * NUM_W, 0xc7);
    SetColor(3);
    FormatNumber(hi_score, '0', tmp);
    OutTextXY(HUD_HIN_X, HUD_Y, tmp);
}

/* 이 점수가 표에 드는가.  들면 자리(0..4), 아니면 -1 */
static int HiRank(long sc)
{
    int i;
    for (i = 0; i < HI_N; i++)
        if (sc > hi_val[i]) return i;
    return -1;
}

/* 표에 끼워 넣는다 (이름은 빈칸으로 두고 EnterName 이 채운다) */
static void HiInsert(int rank, long sc)
{
    int i, k;
    for (i = HI_N - 1; i > rank; i--) {
        for (k = 0; k <= HI_CH; k++) hi_name[i][k] = hi_name[i-1][k];
        hi_val[i] = hi_val[i-1];
    }
    for (k = 0; k < HI_CH; k++) hi_name[rank][k] = 'A';
    hi_name[rank][HI_CH] = '\0';
    hi_val[rank] = sc;
    hi_dirty = 1;
}

/* 이름 세 글자 넣기.  위/아래로 글자, 좌/우로 자리, 스페이스나 엔터로 끝. */
static const char HI_SET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 .-";

/* 자판으로 친 글자를 이름에 쓸 수 있는가 (A~Z 0~9 빈칸 . -) */
static int NameChar(char c)
{
    int i;
    if (c >= 'a' && c <= 'z') c = (char)(c - 32);
    for (i = 0; i < (int)sizeof HI_SET - 1; i++)
        if (HI_SET[i] == c) return c;
    return 0;
}

/* 이름 세 글자 넣기.
   [고침] 예전에는 화살표로 글자를 한 칸씩 돌려 골라야 해서 번거로웠다.
          이제 자판으로 그냥 친다.  백스페이스로 지우고 엔터로 끝낸다.
          (화살표 좌우로 고쳐 쓸 자리를 옮기는 것은 그대로 둔다)
   [고침] 안내 글이 상자를 삐져나가서, 상자를 넓히고 두 줄로 나눴다. */
#define NAME_BOX_X0  48
#define NAME_BOX_X1 272
#define NAME_BOX_Y0  70
#define NAME_BOX_Y1 162
#define NAME_CELL    24          /* 글자 한 자리의 폭 */
#define NAME_Y      100          /* 이름이 찍히는 줄 */
#define NAME_LINE   118          /* 그 밑줄 */

static void EnterName(int rank)
{
    int pos = 0, i, k;
    char buf[HI_CH + 1];

    for (i = 0; i < HI_CH; i++) buf[i] = ' ';      /* 빈 칸에서 시작 */
    buf[HI_CH] = '\0';

    /* [원본에 없던 것] 이름을 넣는 동안에도 최고 기록 화면 곡이 흐른다.
       (이어지는 RankingScreen 에서 같은 곡이 그대로 이어진다) */
    if (!VgmIsPlaying(HISCORE_VGM)) VgmPlay(HISCORE_VGM);

    for (;;) {
        char c;
        int  x0 = 160 - (HI_CH * NAME_CELL) / 2;

        SetFillStyle(1, 0);
        Bar(NAME_BOX_X0, NAME_BOX_Y0, NAME_BOX_X1, NAME_BOX_Y1);
        SetColor(3);
        Rectangle(NAME_BOX_X0, NAME_BOX_Y0, NAME_BOX_X1, NAME_BOX_Y1);

        SetTextJustify(1, 2);
        OutTextXY(160, 78, "새 기록입니다 !");

        /* 글자 세 자리를 나란히.  지금 칠 자리의 밑줄만 밝다. */
        for (i = 0; i < HI_CH; i++) {
            char s[2];
            s[0] = buf[i]; s[1] = '\0';
            SetColor(3);
            OutTextXY(x0 + i * NAME_CELL + NAME_CELL / 2, NAME_Y, s);
            SetColor(i == (pos < HI_CH ? pos : HI_CH - 1) ? 3 : 1);
            Line(x0 + i * NAME_CELL + 3, NAME_LINE,
                 x0 + i * NAME_CELL + NAME_CELL - 4, NAME_LINE);
        }

        SetColor(3);
        OutTextXY(160, 128, "이름을 치고 엔터를 누르세요");
        OutTextXY(160, 144, "백스페이스 = 한 글자 지우기");
        SetTextJustify(0, 2);
        GfxFlip();

        /* [원본과 다름] 그냥 ReadKey 로 막으면 그동안 곡이 멎는다.
           키가 올 때까지 프레임을 돌리며 기다린다. */
        while (!KeyPressed())
            FrameSync(60);
        c = ReadKey();

        if (c == '\r' || c == 0x1b) break;         /* 엔터 / Esc = 끝 */
        if (c == 8) {                              /* 백스페이스 = 지우기 */
            if (pos > 0) pos--;
            buf[pos] = ' ';
            continue;
        }
        if (c == 0) {                              /* 확장키 - 좌우로 자리 옮기기 */
            c = ReadKey();
            if (c == 0x4b) { if (pos > 0) pos--; }
            else if (c == 0x4d) { if (pos < HI_CH) pos++; }
            else if (c == 0x53 && pos < HI_CH) buf[pos] = ' ';   /* Del */
            continue;
        }
        c = (char)NameChar(c);
        if (c && pos < HI_CH) {                    /* 칠 수 있는 글자 */
            buf[pos] = c;
            pos++;                                 /* 세 글자를 다 치면 끝으로 */
        }
    }

    for (k = 0; k < HI_CH; k++) hi_name[rank][k] = buf[k];
    hi_name[rank][HI_CH] = '\0';
    hi_dirty = 1;
}

/* 검증용 훅 (테스트 하네스에서 이름 넣기 화면을 그려 보려고) */
void SmoothNameHook(int rank) { EnterName(rank); }
const char *SmoothNameGet(int rank) { return hi_name[rank]; }

/* 최고 점수표 보여 주기 */
void RankingScreen(int mark)
{
    char s[40], num[0x100];
    int i, y;

    ClearDevice();
    SetColor(3);
    Rectangle(0, 0, 0x13f, 0xc7);
    SetTextJustify(1, 2);
    OutTextXY(160, 20, "==== 최고 기록 ====");

    y = 48;
    for (i = 0; i < HI_N; i++) {
        int n = 0;
        s[n++] = (char)('1' + i);
        s[n++] = '.';
        s[n++] = ' ';
        s[n++] = hi_name[i][0];
        s[n++] = hi_name[i][1];
        s[n++] = hi_name[i][2];
        s[n++] = ' ';
        s[n++] = ' ';
        FormatNumber(hi_val[i], '0', num);
        {
            int k;
            for (k = 0; num[k] && n < 38; k++) s[n++] = num[k];
        }
        if (i == mark) { s[n++] = ' '; s[n++] = '<'; }
        s[n] = '\0';
        OutTextXY(160, y, s);
        y += 18;
    }

    OutTextXY(160, 0xb0, "아무 키나 누르세요 !");
    SetTextJustify(0, 2);
    GfxFlip();

    while (KeyPressed()) { if (ReadKey() == 0) ReadKey(); }

    /* [원본에 없던 것] 최고 기록 화면 곡 (BB06.VGM, 끝없이 되풀이).
       이름을 넣고 들어왔으면 이미 흐르고 있으므로 그대로 이어 간다.
       키를 기다리는 동안에도 프레임을 돌려야 곡이 흐른다. */
    if (!VgmIsPlaying(HISCORE_VGM)) VgmPlay(HISCORE_VGM);
    while (!KeyPressed())
        FrameSync(60);
    if (ReadKey() == 0) ReadKey();
    VgmStop();
}


/* ---- 멈춤 / 소리 끄기 ---------------------------------------------------- */

/* 멈춘 동안 게임 시계가 흐르지 않게 되돌린다 */
static void ClockResync(void)
{
    int i;
    SetTime(0, 0, 0, 0);
    for (i = 1; i <= ENEMY_MAX; i++) enemy_last_move[i] = 0.0;
    platform_last_move = 0.0;
}

static void PauseScreen(void)
{
    char c;

    NoSound();

    /* [고침] 멈춤 상자도 배경 페이지에는 남기지 않는다 */
    GfxStatic(0);
    SetFillStyle(1, 0);
    Bar(0x78, 0x62, 0xc8, 0x76);
    SetColor(3);
    Rectangle(0x78, 0x62, 0xc8, 0x76);
    SetTextJustify(1, 2);
    OutTextXY(160, 0x67, "멈 춤");
    SetTextJustify(0, 2);
    GfxFlip();

    /* [고침] ReadKey 로 막아 두면 그동안 곡이 멎고 애드립은 마지막 음을
       계속 물고 있다.  키가 올 때까지 프레임을 돌린다. */
    while (!KeyPressed())
        FrameSync(60);

    for (;;) {                              /* 아무 키나 누르면 다시 */
        c = ReadKey();
        if (c == 0) { ReadKey(); break; }
        if (c == 0x1b) { quit_flag = 1; last_key = 0x1b; break; }
        break;
    }

    GfxRestoreRect(0x78, 0x62, 0xc8 - 0x78 + 1, 0x76 - 0x62 + 1);  /* 상자 지우기 */
    GfxStatic(1);
    GfxFlip();
    ClockResync();
}

/* 소리 끄기/켜기를 화면 아래에 잠깐 알린다 */
static void SoundToggle(void)
{
    long end;

    SetSoundOn(!GetSoundOn());
    NoSound();

    /* [고침] 소리를 다시 켜면 게임 곡도 바로 이어서 튼다.
       (SetSoundOn(0) 이 VgmStop 으로 곡을 내려놓기 때문에, 켜 주기만 해서는
        다음 판으로 넘어갈 때까지 아무 소리도 안 났다) */
    if (GetSoundOn() && !VgmIsPlaying(STAGE_VGM))
        VgmPlay(STAGE_VGM);

    /* [고침] 안내 글은 배경 페이지에 남기지 않는다.  예전에는 배경에까지
       찍혀서 상자가 화면에 그대로 붙어 있었다. */
    GfxStatic(0);
    SetFillStyle(1, 0);
    Bar(0x78, 0x62, 0xc8, 0x76);
    SetColor(3);
    Rectangle(0x78, 0x62, 0xc8, 0x76);
    SetTextJustify(1, 2);
    OutTextXY(160, 0x67, GetSoundOn() ? "소리 켬" : "소리 끔");
    SetTextJustify(0, 2);
    GfxFlip();

    /* 기다리는 동안에도 프레임을 돌린다 - 그래야 곡이 흐른다 */
    end = TicksMs() + 700;
    while (TicksMs() < end)
        FrameSync(60);

    GfxRestoreRect(0x78, 0x62, 0xc8 - 0x78 + 1, 0x76 - 0x62 + 1);  /* 상자 지우기 */
    GfxStatic(1);
    GfxFlip();
    ClockResync();
}


/* ---- 이어하기 ------------------------------------------------------------ */

byte continue_flag;              /* 1 이면 GameLoop 이 그 판부터 이어서 한다 */

/* 10초 세면서 기다린다.  키를 누르면 1(이어하기), 시간이 다 가면 0 */
static int ContinueScreen(void)
{
    int n;
    char s[8];

    while (KeyPressed()) { if (ReadKey() == 0) ReadKey(); }

    /* [원본에 없던 것] 열을 세는 동안 조바심 나는 곡이 흐른다 (되풀이).
       여기서 나가면 다음 화면(판 다시 시작 / 게임 끝)이 제 곡으로 바꾼다. */
    VgmPlay(CONT_VGM);

    for (n = 9; n >= 0; n--) {
        long end = TicksMs() + 1000;

        SetFillStyle(1, 0);
        Bar(80, 88, 240, 136);
        SetColor(3);
        Rectangle(80, 88, 240, 136);
        SetTextJustify(1, 2);
        OutTextXY(160,  92, "이어서 하시겠습니까 ?");
        OutTextXY(160, 108, "Y = 예    N = 아니오");
        Str((long)n, 0, s);
        OutTextXY(160, 122, s);
        SetTextJustify(0, 2);
        GfxFlip();

        while (TicksMs() < end) {
            if (KeyPressed()) {
                char c = ReadKey();
                /* [고침] 예전에는 아무 키나 누르면 이어서 했다.  "N" 을 눌러도
                   이어지니 물어보는 뜻이 없었다.  이제 Y/N 만 받는다. */
                if (c == 0) { ReadKey(); continue; }        /* 확장키는 무시 */
                if (c == 'n' || c == 'N' || c == 0x1b) { VgmStop(); return 0; }
                if (c == 'y' || c == 'Y' || c == ' ' || c == '\r') { VgmStop(); return 1; }
                /* 그 밖의 키는 못 들은 척한다 */
            }
            FrameSync(60);
        }
    }
    VgmStop();                          /* 시간이 다 갔다 */
    return 0;
}


/* 검증용 훅 */
void SmoothContHook(void) { (void)ContinueScreen(); }

/* ---- 시범 플레이 (어트랙트 모드) -----------------------------------------
   타이틀에서 잠시 아무 키도 안 누르면 컴퓨터가 대신 플레이해 보여 준다.
   판을 만들 때 쓴 자동 조종을 그대로 게임 안에 옮겨 넣은 것이다.
   사람이 하는 생각과 같은 순서로 움직인다.
     1. 지금 자리에서 남은 먹이까지 가장 짧은 길을 찾는다
     2. 그 첫 수를 두어도 안전한지 - 오리가 닿기 전에 사다리로 뺄 수 있는지 - 본다
     3. 안 되면 사다리 쪽으로 물러나 층을 벗어나고, 오리가 지나가길 기다린다
   -------------------------------------------------------------------------- */

enum { D_L, D_R, D_JL, D_JR, D_U, D_D, D_NONE };

#define DM(c,r)   map_cur[stage * 240 + (c) * 12 + (r)]
#define D_NN      (21 * 12)
#define D_IX(c,r) ((c) * 12 + (r))

static int d_dst[D_NN], d_prv[D_NN], d_mv[D_NN];
static int d_target = -1;

static int d_walkrow(int r) { return r == 2 || r == 5 || r == 8 || r == 11; }

static int d_stand(int c, int r)
{
    if (c < 1 || c > 20) return 0;
    if (c == 20 && r != 11) return 0;
    if (!d_walkrow(r)) return 0;
    if (DM(c, r) == 7) return 0;                /* 압정 */
    if (DM(c, r + 1) == 1) return 0;            /* 발밑이 비었다 */
    return 1;
}

static int d_ladder(int c, int r) { return r >= 1 && r <= 11 && DM(c, r) == 6; }

/* 적(또는 발판)이 그 칸에 닿기까지 걸리는 시간(ms).  1초마다 시계를 되돌리는
   원작 버릇 때문에 적이 가끔 한 칸을 공짜로 더 가므로 한 칸 빨리 온다고 본다. */
static int d_etime(int c, int r)
{
    int best = 1 << 24, i;
    if (!d_walkrow(r)) return best;
    for (i = 1; i <= ENEMY_MAX; i++) {
        int spd = ENEMY_SPEED(i), p, dd, j, step;
        if (!enemy_col[i] || enemy_row[i] != r || !spd) continue;
        p = enemy_col[i]; dd = enemy_dir[i]; step = 1000 / spd;
        if (p == c) return 0;
        for (j = 1; j <= 40; j++) {
            p += dd;
            if (p >= 19) { p = 19; dd = -1; }
            if (p <= 1)  { p = 1;  dd = 1;  }
            if (p == c) { if ((j - 1) * step < best) best = (j - 1) * step; break; }
        }
    }
    if (platform_on && platform_row == r) {
        int p = platform_col, dd = platform_dir, j;
        if (p == c) return 0;
        for (j = 1; j <= 40; j++) {
            p += dd;
            if (p >= 19) { p = 19; dd = -1; }
            if (p <= 1)  { p = 1;  dd = 1;  }
            if (p == c) { if ((j - 1) * 800 < best) best = (j - 1) * 800; break; }
        }
    }
    return best;
}

/* 오리를 피해 빠져나갈 수 있는 칸인가 */
static int d_refuge(int c, int r)
{
    if (DM(c, r) == 6) return 1;                     /* 올라갈 사다리 */
    if (r + 1 <= 11 && DM(c, r + 1) == 6) return 1;  /* 내려갈 사다리 */
    if (r == 11 && c == 20) return 1;                /* 오리가 못 오는 구석 */
    return 0;
}

/* 그 칸에서 피난처까지 걸리는 시간(ms).  적이 막고 있는 길은 빼고 잰다. */
static int d_rtime(int c, int r)
{
    int d[21], i, n, rmax = (r == 11) ? 20 : 19;
    if (!d_walkrow(r)) return 0;
    for (i = 0; i <= 20; i++) d[i] = 1 << 24;
    d[c] = 0;
    for (n = 0; n < 24; n++) {
        int ch = 0, p;
        for (p = 1; p <= 20; p++) {
            if (d[p] >= (1 << 24)) continue;
            if (p > 1 && d_stand(p-1, r) && d[p] + 90 < d[p-1]
                && d_etime(p-1, r) > d[p] + 340) { d[p-1] = d[p] + 90; ch = 1; }
            if (p < rmax && d_stand(p+1, r) && d[p] + 90 < d[p+1]
                && d_etime(p+1, r) > d[p] + 340) { d[p+1] = d[p] + 90; ch = 1; }
            if (p > 2 && d_stand(p-2, r) && d[p] + 250 < d[p-2]
                && d_etime(p-2, r) > d[p] + 500
                && d_etime(p-1, r) > d[p] + 300) { d[p-2] = d[p] + 250; ch = 1; }
            if (p <= 17 && d_stand(p+2, r) && d[p] + 250 < d[p+2]
                && d_etime(p+2, r) > d[p] + 500
                && d_etime(p+1, r) > d[p] + 300) { d[p+2] = d[p] + 250; ch = 1; }
        }
        if (!ch) break;
    }
    {
        int best = 1 << 24, p;
        for (p = 1; p <= 20; p++)
            if (d[p] < best && d_refuge(p, r)) best = d[p];
        return best;
    }
}

/* 그 층에 적이 딱 한 마리이고 그 반대쪽으로 달아날 칸이 넉넉하면, 사다리가
   멀어도 버틸 수 있다.  원본 1~10판은 한 층에 한 마리씩이라 이렇게 논다.
   11판부터는 한 층에 두 마리라 이 조건이 서지 않는다(그대로 사다리로 숨는다). */
static int d_runaway(int c, int r)
{
    int n = 0, side = 0, i, k, room = 0, dir;

    for (i = 1; i <= ENEMY_MAX; i++) {
        if (!enemy_col[i] || enemy_row[i] != r || !ENEMY_SPEED(i)) continue;
        if (enemy_col[i] == c) return 0;
        n++;
        side = (enemy_col[i] < c) ? 1 : -1;      /* 적이 왼쪽이면 오른쪽으로 */
    }
    if (platform_on && platform_row == r) return 0;   /* 발판까지 있으면 그만둔다 */
    if (n == 0) return 1;                        /* 이 층에는 적이 없다 */
    if (n != 1) return 0;                        /* 두 마리면 끼일 수 있다 */

    dir = side;
    for (k = 1; k <= 5; k++) {
        if (!d_stand(c + dir * k, r)) break;
        room++;
    }
    return room >= 3;
}

/* [원본에 없던 것] 한 번 못 깬 판은 두 번째에 조금 대담하게 해 본다.
   여유를 반으로 줄이면 더 아슬아슬하지만 그만큼 멈춰 서는 시간이 준다. */
int d_bold;

/* extra 는 "여유를 이만큼 더 본다".  사다리에서 내려설 때 쓴다 -
   내려서는 데 시간이 걸리는 동안 오리가 다가와 곧바로 다시 피해야 하면
   위아래로 까부는 것처럼 보이기 때문이다. */
/* 그 칸에서 가장 가까운 적까지 몇 칸 떨어져 있나 (적이 없으면 큰 수) */
static int d_edist(int c, int r)
{
    int best = 99, i, d;
    if (!d_walkrow(r)) return best;
    for (i = 1; i <= ENEMY_MAX; i++) {
        if (!enemy_col[i] || enemy_row[i] != r || !ENEMY_SPEED(i)) continue;
        d = enemy_col[i] - c; if (d < 0) d = -d;
        if (d < best) best = d;
    }
    if (platform_on && platform_row == r) {
        d = platform_col - c; if (d < 0) d = -d;
        if (d < best) best = d;
    }
    return best;
}

/* 오리 반대쪽으로 한 발이라도 물러날 자리가 있는 칸인가.
   양옆이 모두 구멍이라 두 칸 뛰어야만 빠져나올 수 있는 "막다른 칸"
   (원본 10판 윗줄 14칸 같은 곳)을 가려내려고 쓴다.  그런 칸은 오리가
   몇 칸 떨어져 있어도 들어가면 그대로 잡힌다. */
static int d_backout(int c, int r)
{
    int i, away = 0, nd = 99, n = 0, d;
    if (!d_walkrow(r)) return 1;
    for (i = 1; i <= ENEMY_MAX; i++) {
        if (!enemy_col[i] || enemy_row[i] != r || !ENEMY_SPEED(i)) continue;
        n++;
        d = enemy_col[i] - c; if (d < 0) d = -d;
        if (d < nd) { nd = d; away = (enemy_col[i] > c) ? -1 : 1; }
    }
    if (platform_on && platform_row == r) {
        d = platform_col - c; if (d < 0) d = -d;
        if (!n || d < nd) { nd = d; away = (platform_col > c) ? -1 : 1; }
        n++;
    }
    if (!n) return 1;                            /* 이 줄에는 오리가 없다 */
    if (d_refuge(c, r)) return 1;                /* 사다리·구석은 언제나 괜찮다 */
    {   /* 오리 반대쪽으로 몇 발이나 물러날 수 있나 (구멍 하나는 뛰어 넘는다).
           한 발짝밖에 못 물러나면 그 다음이 없으니 들어가면 안 되는 칸이다. */
        int k, p = c, steps = 0;
        for (k = 0; k < 4; k++) {
            if      (d_stand(p + away, r))     p += away;
            else if (d_stand(p + away * 2, r)) p += away * 2;
            else break;
            steps++;
            if (d_refuge(p, r)) return 1;        /* 그쪽에 사다리가 있다 */
        }
        return steps >= 2;
    }
}

/* 이만큼(칸) 넘게 떨어져 있으면 사다리가 멀어도 그냥 움직인다.
   사람도 그렇게 논다 - 오리가 저만치 있으면 일단 가고, 가까워지면 그때 뺀다.
   이것이 없으면 "사다리가 900ms 안에 없는 줄" 은 오리가 반대쪽 끝에 있어도
   통째로 위험하다고 보고 한 발짝도 안 뗀다. */
#define D_FAR_CELLS 3            /* 세 칸 넘게 떨어져 있으면 움직인다.
                                    크게 잡을수록 조심스럽게 논다
                                    (2 망설임 적음 / 3 보통 / 4 조심) */

static int d_safe_ex(int c, int r, int extra)
{
    static const int marg[3] = { 800, 450, 250 };
    static const int runm[3] = { 600, 400, 250 };
    /* 발이 이미 사다리(또는 구석)에 닿아 있는 칸.  오리가 와도 그 자리에서
       바로 오르내려 피하면 되므로 다른 칸만큼 넉넉히 볼 까닭이 없다.
       이것이 없으면 "사다리 칸으로 내려서기" 에 1.2초나 요구하게 되어,
       오리가 저쪽 끝에서 돌아서는 그 잠깐의 틈(원본 5판 맨 아랫줄)을
       번번이 놓치고 사다리 위에서만 오르락내리락하게 된다. */
    static const int refm[3] = { 400, 300, 200 };
    /* 물러날 데가 없는 "막다른 칸" 은 들어갔다 나오는 데만도 시간이 걸린다.
       들어갈 때 재는 잣대가 나올 때 재는 잣대보다 느슨하면, 들어가서는
       못 나오고 그 자리에 서서 잡힌다 (원본 9판 윗줄 19칸).  그런 칸은
       이만큼 더 넉넉할 때만 들어간다. */
    static const int pockm[3] = { 700, 500, 300 };
    int rt, et, pk;
    if (!d_walkrow(r)) return 1;                /* 사다리 중간칸은 안전하다 */
    et = d_etime(c, r);
    pk = d_backout(c, r) ? 0 : pockm[d_bold];
    if (!pk && d_edist(c, r) > D_FAR_CELLS && et > 400 + extra)
        return 1;                               /* 충분히 멀고, 물러날 곳도 있다 */
    rt = d_rtime(c, r);
    if (rt == 0 && et > refm[d_bold] + extra) return 1;   /* 바로 피할 수 있는 칸 */
    if (rt <= 900 && et > rt + marg[d_bold] + extra + pk) return 1;
    if (!pk && et > runm[d_bold] + extra && d_runaway(c, r)) return 1;
    return 0;
}

static int d_safe(int c, int r) { return d_safe_ex(c, r, 0); }

/* 남은 먹이까지 가는 길 (걸음 하나를 밀리초로 친다) */
static void d_plan(int sc, int sr)
{
    int i, n;
    for (i = 0; i < D_NN; i++) { d_dst[i] = 1 << 28; d_prv[i] = -1; d_mv[i] = D_NONE; }
    d_dst[D_IX(sc, sr)] = 0;
    for (n = 0; n < 40; n++) {
        int changed = 0, c, r;
        for (c = 1; c <= 20; c++) for (r = 1; r <= 11; r++) {
            int u = D_IX(c, r), d = d_dst[u];
            int to[6], cost[6], mv[6], k, nm = 0;
            if (d >= (1 << 28)) continue;
            if (!(d_stand(c, r) || d_ladder(c, r))) continue;
            if (d_walkrow(r)) {
                int rmax = (r == 11) ? 20 : 19;
                if (c > 1 && d_stand(c-1, r))   { to[nm]=D_IX(c-1,r); cost[nm]=75;  mv[nm]=D_L;  nm++; }
                if (c < rmax && d_stand(c+1, r)){ to[nm]=D_IX(c+1,r); cost[nm]=75;  mv[nm]=D_R;  nm++; }
                if (c > 2 && d_stand(c-2, r))   { to[nm]=D_IX(c-2,r); cost[nm]=250; mv[nm]=D_JL; nm++; }
                if (c <= 17 && d_stand(c+2, r)) { to[nm]=D_IX(c+2,r); cost[nm]=250; mv[nm]=D_JR; nm++; }
                if (DM(c, r) == 6 && r > 1)     { to[nm]=D_IX(c,r-1); cost[nm]=80;  mv[nm]=D_U;  nm++; }
                if (r < 11 && DM(c, r+1) == 6)  { to[nm]=D_IX(c,r+1); cost[nm]=80;  mv[nm]=D_D;  nm++; }
            } else {
                if (r > 1 && (DM(c,r-1) == 6 || d_stand(c, r-1)))
                    { to[nm]=D_IX(c,r-1); cost[nm]=80;  mv[nm]=D_U; nm++; }
                if (r < 11 && (DM(c,r+1) == 6 || d_stand(c, r+1)))
                    { to[nm]=D_IX(c,r+1); cost[nm]=80;  mv[nm]=D_D; nm++; }
            }
            for (k = 0; k < nm; k++) {
                int tc = to[k] / 12, tr = to[k] % 12;
                if (DM(tc, tr) == 0x1e) cost[k] += 4000;   /* 발판 발생 타일은 피한다 */
                if (d + cost[k] < d_dst[to[k]]) {
                    d_dst[to[k]] = d + cost[k];
                    d_prv[to[k]] = u; d_mv[to[k]] = mv[k];
                    changed = 1;
                }
            }
        }
        if (!changed) break;
    }
}

/* [원본에 없던 것] 길 찾기는 오리를 보지 않는다.  그래서 "가장 가까운 먹이"
   가 하필 오리 때문에 못 가는 곳이면, 그쪽만 쳐다보며 같은 자리에서 왔다갔다
   하게 된다 (원본 9판 맨 아랫줄).  한동안 하나도 못 먹으면 그 먹이를 잠시
   미뤄 두고 다른 먹이부터 간다 - 사람도 그렇게 논다. */
#define D_BAN_N 6
static int  d_ban_cell[D_BAN_N];
static long d_ban_till[D_BAN_N];

static void d_ban_clear(void)
{
    int i;
    for (i = 0; i < D_BAN_N; i++) { d_ban_cell[i] = -1; d_ban_till[i] = 0; }
}

static int d_banned(int u)
{
    int i;
    for (i = 0; i < D_BAN_N; i++)
        if (d_ban_cell[i] == u && TicksMs() < d_ban_till[i]) return 1;
    return 0;
}

static void d_ban_add(int u, long ms)
{
    int i, slot = 0;
    for (i = 0; i < D_BAN_N; i++) if (d_ban_cell[i] == u) { slot = i; break; }
    if (i == D_BAN_N)
        for (i = 1; i < D_BAN_N; i++)            /* 가장 먼저 풀릴 자리를 쓴다 */
            if (d_ban_till[i] < d_ban_till[slot]) slot = i;
    d_ban_cell[slot] = u;
    d_ban_till[slot] = TicksMs() + ms;
}

/* 목표(한 번 고르면 먹을 때까지 안 바꾼다) 로 가는 첫 수 */
static int d_first(int sc, int sr)
{
    int c, r, best = -1, bd = 1 << 28, u;

    if (d_target >= 0) {
        int tc = d_target / 12, tr = d_target % 12;
        if (DM(tc, tr) == food_tile && d_dst[d_target] < (1 << 28)
            && !d_banned(d_target))
            { best = d_target; bd = d_dst[d_target]; }
        else d_target = -1;
    }
    if (best < 0) {
        int pass;
        /* 먼저 미뤄 둔 먹이를 빼고 찾고, 그것마저 없으면 다시 다 놓고 찾는다 */
        for (pass = 0; pass < 2 && best < 0; pass++)
            for (c = 1; c <= 20; c++) for (r = 1; r <= 11; r++) {
                if (DM(c, r) != food_tile) continue;
                u = D_IX(c, r);
                if (pass == 0 && d_banned(u)) continue;
                if (d_dst[u] < bd) { bd = d_dst[u]; best = u; }
            }
    }
    if (best < 0 || bd >= (1 << 28)) { d_target = -1; return D_NONE; }
    d_target = best;
    u = best;
    while (d_prv[u] >= 0 && d_prv[u] != D_IX(sc, sr)) u = d_prv[u];
    if (d_prv[u] < 0) return D_NONE;
    return d_mv[u];
}

/* 사다리에 숨어 있다가 다시 층으로 내려/올라설 때는 훨씬 넉넉히 본다.
   같은 잣대(0.4초)로 보면 오리가 한 칸 다가올 때마다 판단이 뒤집혀서
   사다리에서 오르락내리락 까부는 것처럼 보인다.  "오리가 지나갈 때까지
   기다렸다가 내려선다" 가 사람이 하는 행동이기도 하다. */
#define D_LAND_MS   700
extern int d_bold;               /* 0 보통 / 1 대담 / 2 더 대담 */
#define D_LANDV()  (d_bold == 0 ? D_LAND_MS : (d_bold == 1 ? 700 : 400))

/* 사다리로 피한 뒤 곧바로 그 층에 다시 내려서지 않게 막는다.
   내려서자마자 옆으로 못 가서 또 피하면, 보기에 사다리에서 위아래로
   왔다갔다 까부는 것처럼 된다.  오리가 지나갈 만큼은 붙어 있는다. */
#define D_HIDE_MS   300
static int  d_hide_row;          /* 도망쳐 나온 층 (0 이면 없음) */
static long d_hide_until;        /* 그 층으로 다시 내려서지 않을 시각 */

/* 바로 앞에 한 세로 이동을 곧장 되돌리지 않는다 (사다리에서 까부는 것 방지) */
#define D_TURN_MS  250
static int  d_last_v;            /* 마지막 세로 이동 (D_U / D_D / D_NONE) */
static long d_last_v_at;

static int d_v_ok(int mv)
{
    if (mv != D_U && mv != D_D) return 1;
    if (d_last_v == D_NONE) return 1;
    if (d_last_v == mv) return 1;                    /* 같은 방향은 언제나 좋다 */
    return TicksMs() - d_last_v_at >= D_TURN_MS;     /* 되돌리기는 잠시 뒤에 */
}

static int d_land_ok(int rr)
{
    if (rr != d_hide_row) return 1;
    return TicksMs() >= d_hide_until;
}

static int d_movesafe(int mv, int c, int r);

/* 그 칸에 내려서서 바로 무엇이든 할 수 있는가.
   내려서자마자 옆으로 못 가면 또 사다리로 피하게 되고, 그것이 되풀이되면
   위아래로 까부는 것처럼 보인다.  "내려서 봐야 소용없으면 안 내려선다". */
static int d_progress_ok(int c, int r)
{
    if (DM(c, r) == food_tile) return 1;             /* 거기 먹이가 있다 */
    if (d_movesafe(D_L,  c, r)) return 1;
    if (d_movesafe(D_R,  c, r)) return 1;
    if (d_movesafe(D_JL, c, r)) return 1;
    if (d_movesafe(D_JR, c, r)) return 1;
    return 0;
}

/* 그 수를 지금 두어도 되는가.  움직이는 동안은 아직 원래 칸에 있다. */
/* [원본에 없던 것] 사다리에서 층으로 내려(올라)설 때, 가야 할 쪽에 오리가
   끼어 있으면 아직 내려서지 않는다.  같은 줄에서 오리를 앞질러 지나갈 수는
   없으므로, 오리를 쫓아가 봐야 반대쪽 끝까지 밀려났다 돌아오기를 되풀이할
   뿐이다.  오리가 지나가기를 사다리에 붙어 기다렸다가, 길이 트이면 그때
   한 번에 건넌다 - 사람이 하는 것과 같다.
   (원본 5·9판 맨 아랫줄처럼 사다리가 양끝에 하나씩뿐인 줄에서 특히 그렇다) */
static int d_clearside(int c, int r)
{
    int tc, dir, i, n = 0, ec = 0;
    /* 첫 시도에서만 이렇게 기다린다.  한 번 못 깨고 다시 할 때는 기다림을
       버리고 비집고 들어간다 (그렇게 해야 깨지는 판이 있다 - 원본 9판). */
    if (d_bold) return 1;
    if (d_target < 0) return 1;
    tc = d_target / 12;
    if (tc == c) return 1;
    for (i = 1; i <= ENEMY_MAX; i++) {
        if (!enemy_col[i] || enemy_row[i] != r || !ENEMY_SPEED(i)) continue;
        n++; ec = enemy_col[i];
    }
    if (platform_on && platform_row == r) { n++; ec = platform_col; }
    /* 한 마리일 때만 본다.  11판부터는 한 층에 두 마리라 길이 트일 때가
       드물어서, 이 잣대까지 들이대면 사다리에서 내려서지를 못한다. */
    if (n != 1) return 1;
    dir = (tc > c) ? 1 : -1;
    if ((ec - c) * dir > 0 && (tc - ec) * dir > 0) return 0;
    return 1;
}

static int d_movesafe(int mv, int c, int r)
{
    if (d_walkrow(r) && d_etime(c, r) <= 320) return 0;
    switch (mv) {
    case D_L:  return d_etime(c-1, r) > 320 && d_safe(c-1, r);
    case D_R:  return d_etime(c+1, r) > 320 && d_safe(c+1, r);
    case D_JL: return d_etime(c-1, r) > 400 && d_etime(c-2, r) > 500 && d_safe(c-2, r);
    case D_JR: return d_etime(c+1, r) > 400 && d_etime(c+2, r) > 500 && d_safe(c+2, r);
    /* 사다리칸(줄 4,7,10)으로 옮기는 것은 언제나 안전하다.
       층(줄 2,5,8,11)으로 내려/올라설 때만 넉넉히 본다. */
    /* 내려/올라설 때는 여유를 600ms 더 보고, 내려서서 바로 무엇이든
       할 수 있을 때만 내려선다 */
    case D_U:  return !d_walkrow(r-1) || (d_land_ok(r-1) &&
                      d_etime(c, r-1) > D_LANDV() && d_safe_ex(c, r-1, 400) &&
                      d_clearside(c, r-1) && d_progress_ok(c, r-1));
    case D_D:  return !d_walkrow(r+1) || (d_land_ok(r+1) &&
                      d_etime(c, r+1) > D_LANDV() && d_safe_ex(c, r+1, 400) &&
                      d_clearside(c, r+1) && d_progress_ok(c, r+1));
    }
    return 0;
}

/* 도망 : 사다리 쪽으로 물러나고, 사다리에 닿으면 층을 벗어난다 */
static int d_flee(int c, int r)
{
    static const int dc[4]   = { -1, 1, -2, 2 };
    static const int need[4] = { 320, 320, 500, 500 };
    static const int mvv[4]  = { D_L, D_R, D_JL, D_JR };
    int rmax = (r == 11) ? 20 : 19, k, away;
    int bestmv = D_NONE, bestrt = 1 << 24, bestet = 0;
    int deadmv = D_NONE, deadet = 0;             /* 막다른 칸밖에 없을 때 */

    if (!d_walkrow(r)) return D_NONE;

    if (d_refuge(c, r)) {
        if (DM(c, r) == 6 && r > 1) return D_U;
        if (r < 11 && DM(c, r + 1) == 6) return D_D;
        if (r == 11 && c == 20) return D_NONE;      /* 구석은 그대로 있으면 된다 */
    }
    /* 그 층에 오리가 한 마리뿐이면, 그 반대쪽으로 한 칸 물러나는 것은
       "내가 먼저 닿기만 하면" 된다.  사람은 한 칸에 0.09초, 오리는 0.2초라
       같은 쪽으로 달아나도 계속 앞선다.  이것을 빼면 오리가 옆에 왔을 때
       도망갈 곳이 없다고 보고 사다리에 붙어 오르락내리락만 하게 된다. */
    away = 0;
    {
        int i, n = 0, nd = 99;
        for (i = 1; i <= ENEMY_MAX; i++) {
            if (!enemy_col[i] || enemy_row[i] != r || !ENEMY_SPEED(i)) continue;
            n++;
            if (abs(enemy_col[i] - c) < nd) {
                nd = abs(enemy_col[i] - c);
                away = (enemy_col[i] > c) ? -1 : 1;
            }
        }
        if (n != 1 || (platform_on && platform_row == r)) away = 0;
    }

    for (k = 0; k < 4; k++) {
        int nc = c + dc[k], rt, et, req;
        if (nc < 1 || nc > rmax) continue;
        if ((dc[k] == -2 && c < 3) || (dc[k] == 2 && c > 17)) continue;
        if (!d_stand(nc, r)) continue;
        if (dc[k] == -2 && d_etime(c - 1, r) < 400) continue;
        if (dc[k] ==  2 && d_etime(c + 1, r) < 400) continue;
        et = d_etime(nc, r);
        req = need[k] + (d_refuge(nc, r) ? 0 : 200);
        if (away && dc[k] == away) req = 150;    /* 오리 반대쪽 한 칸 */
        else if (away && dc[k] == away * 2 && req > 350) req = 350;  /* 반대쪽 두 칸 */
        if (et < req) continue;
        if (!d_backout(nc, r)) {                 /* 들어가면 더 못 물러나는 칸 */
            if (et > deadet) { deadet = et; deadmv = mvv[k]; }
            continue;
        }
        rt = d_rtime(nc, r);
        if (rt < bestrt || (rt == bestrt && et > bestet)) {
            bestrt = rt; bestet = et; bestmv = mvv[k];
        }
    }
    if (bestmv == D_NONE) return deadmv;         /* 그것밖에 없으면 어쩔 수 없다 */
    return bestmv;
}

/* 사다리로 피해 층을 벗어나는 수였으면, 그 층을 잠시 피해 둔다 */
static void d_mark_hide(int mv, int r)
{
    if (!d_walkrow(r)) return;
    if (mv != D_U && mv != D_D) return;
    d_hide_row = r;
    d_hide_until = TicksMs() + D_HIDE_MS;
}

/* 이번 차례에 둘 수 */
static int DemoThink(void)
{
    int c = player_col, r = player_row, mv, k;

    if (move_dir || stage_cleared || player_dead) return D_NONE;

    d_plan(c, r);
    mv = d_first(c, r);

    if (d_walkrow(r) && r != d_hide_row) d_hide_row = 0;   /* 다른 층으로 옮겼다 */

    if (!d_safe(c, r)) {                        /* 지금 자리가 위험하면 먼저 도망 */
        k = d_flee(c, r);
        if (k != D_NONE) { d_mark_hide(k, r); return k; }
    }
    if (mv != D_NONE && d_v_ok(mv) && d_movesafe(mv, c, r))
        return mv;

    k = d_flee(c, r);                           /* 못 가면 물러나 기다린다 */
    d_mark_hide(k, r);
    return k;
}

/* 고른 수를 실제로 둔다 (키보드를 거치지 않고 곧바로 부른다) */
static void DemoDo(int mv)
{
    switch (mv) {
    case D_L:  WalkLeft();  break;
    case D_R:  WalkRight(); break;
    case D_JL: if (player_sprite == 0x0a) JumpMove(); else WalkLeft();  break;
    case D_JR: if (player_sprite == 0x0e) JumpMove(); else WalkRight(); break;
    case D_U:  d_last_v = D_U; d_last_v_at = TicksMs(); ClimbUp();   break;
    case D_D:  d_last_v = D_D; d_last_v_at = TicksMs(); ClimbDown(); break;
    default:   break;
    }
}

/* 화면 위쪽에 "시범 플레이" 라고 적어 둔다 */
static void DemoBanner(void)
{
    SetTextJustify(1, 2);
    SetColor(3);
    OutTextXY(160, 2, "시범 플레이 - 아무 키나 누르면 시작");
    SetTextJustify(0, 2);
}

/* 시범 플레이 한 판.  키를 누르거나 죽거나 시간이 다 되면 끝난다.
   돌려주는 값 : 1 = 사람이 키를 눌렀다 (게임을 시작해야 한다) */
int demo_end;

/* 시범 플레이 한 판.
   돌려주는 값 : 1 깼다 / 0 죽었거나 시간이 다 됐다 / -1 사람이 키를 눌렀다 */
/* 자동 조종이 이만큼 동안 하나도 못 먹으면 그 판은 포기한다.
   원본 5·6·9판처럼 이 조종 방식과 안 맞는 판에서 100초를 가만히 서 있으면
   보는 사람은 "판 6 에서 멈췄다" 고 느낀다. */
#ifndef DEMO_STUCK_MS
#define DEMO_STUCK_MS  30000L
#endif

int DemoStageTest(int st, long ms);
static char demo_hit;            /* 시범 중에 눌린 키 ('D' 면 전체 시범) */


/* 이만큼 동안 목표를 하나도 못 먹으면 그 먹이는 잠시 미뤄 둔다 */
#define D_SWITCH_MS   6000L
#define D_BAN_MS     12000L

static int DemoStage(int st, long limit_ms)
{
    long end, last_eat, switch_at;
    int  food_was;

    demo_end = 0;
    stage = st;
    round_no = st;
    food_tile = FoodTile(stage);
    player_dead = 0; stage_cleared = 0; move_dir = 0;
    d_target = -1;
    d_ban_clear();
    d_hide_row = 0; d_hide_until = 0;
    d_last_v = D_NONE; d_last_v_at = 0;
    platform_on = 0;
    eat_pose_end = 0;

    /* 적이 서는 줄.  StartNewGame 이 정해 주는 값인데, 시범 플레이는 게임을
       시작하기 전에 도는 것이라 여기서 직접 넣어 준다. */
    enemy_row[1] = 0x0b;
    enemy_row[2] = 8;
    enemy_row[3] = 5;
    enemy_row[4] = 2;

    RestartStage();
    DemoBanner();
    GfxFlip();

    end       = TicksMs() + limit_ms;
    last_eat  = TicksMs();
    switch_at = TicksMs();
    food_was  = food_left[stage];

    while (TicksMs() < end) {

        if (food_left[stage] != food_was) {      /* 하나 먹었다 */
            food_was  = food_left[stage];
            last_eat  = TicksMs();
            switch_at = TicksMs();
        }
        if (TicksMs() - last_eat > DEMO_STUCK_MS)
            return 0;                            /* 통 못 먹는다 - 다음 판으로 */

        /* 한참 째려보기만 했으면 그 먹이는 미루고 다른 데부터 간다 */
        if (TicksMs() - switch_at > D_SWITCH_MS) {
            if (d_target >= 0) d_ban_add(d_target, D_BAN_MS);
            d_target  = -1;
            switch_at = TicksMs();
        }

        FrameSync(120);

        if (KeyPressed()) {                     /* 사람이 왔다 */
            char c = ReadKey();
            demo_hit = 0;
            if (c == 0) c = ReadKey();
            else if (c == 0x1b) { quit_flag = 1; last_key = 0x1b; }
            else if (c == 'd' || c == 'D') demo_hit = 'D';   /* 전체 시범 */
            SoundJingleStop();
            return -1;
        }

        if (stage_cleared) return 1;
        if (player_dead)   { demo_end = (time_left <= 0) ? 3 : 1; return 0; }

        if (move_dir == -1)      LandLeft();
        else if (move_dir == 1)  LandRight();
        else {
            int mvx = DemoThink();
#ifdef DEMO_TRACE2
            {
                static long idle_since = 0; static int lc = -1, lr = -1;
                if (mvx != D_NONE || player_col != lc || player_row != lr) {
                    idle_since = TicksMs(); lc = player_col; lr = player_row;
                    if (mvx != D_NONE)
                        printf("%6ld (%2d,%2d) 수%d\n",
                               TicksMs(), player_col, player_row, mvx);
                } else if (TicksMs() - idle_since > 3000) {
                    int c = player_col, r = player_row, i;
                    idle_since = TicksMs();
                    printf("!! 멈춤 %6ld 칸(%d,%d) 계획수=%d\n", TicksMs(), c, r,
                           (d_plan(c,r), d_first(c,r)));
                    printf("   목표=(%d,%d) 안전=%d\n", d_target/12, d_target%12,
                           d_safe(c,r));
                    if (r < 11) printf("   아래(%d,%d): 땅=%d et=%d safe400=%d prog=%d land_ok=%d v_ok=%d\n",
                           c, r+1, d_stand(c,r+1), d_etime(c,r+1),
                           d_safe_ex(c,r+1,400), d_progress_ok(c,r+1),
                           d_land_ok(r+1), d_v_ok(D_D));
                    if (r > 1) printf("   위(%d,%d): 땅=%d et=%d safe400=%d prog=%d\n",
                           c, r-1, d_stand(c,r-1), d_etime(c,r-1),
                           d_safe_ex(c,r-1,400), d_progress_ok(c,r-1));
                    for (i = 1; i <= ENEMY_MAX; i++)
                        if (enemy_col[i]) printf("   적%d (%d줄 %d칸 %+d 속도%d)\n",
                              i, enemy_row[i], enemy_col[i], enemy_dir[i], ENEMY_SPEED(i));
                    fflush(stdout);
                }
            }
#endif
            DemoDo(mvx);
        }

        EatPosePoll();
        UpdateActors();
        DemoBanner();
        RenderFrame();
    }
    demo_end = 4;
    return 0;                                   /* 시간이 다 됐다 */
}

static void DemoCleanup(void)
{
    SoundJingleStop();
    player_dead = 0; stage_cleared = 0; move_dir = 0;
    platform_on = 0;
    eat_pose_end = 0;
    AcReset();
}

/* 타이틀에서 가만 두면 도는 시범 플레이 - 한 판만 보여 준다.
   돌려주는 값 : 1 = 사람이 키를 눌렀다 (게임을 시작해야 한다) */
int DemoStageTest(int st, long ms) { return DemoStage(st, ms); }

int DemoRun(void)
{
    /* 보여 줄 판을 돌아가며 고른다.  자동 조종이 확실히 깨는 판만 쓴다
       (원본 1~10판은 "한 마리를 앞질러 달아나는" 판이라 사다리로 숨는
        이 조종 방식과 잘 맞지 않는다) */
    static const int demo_list[4] = { 11, 14, 17, 20 };
    static int demo_i = 0;
    int r;

    lives = 3; score = 0L;
    r = DemoStage(demo_list[demo_i], 40000L);
    demo_i = (demo_i + 1) & 3;

    DemoCleanup();
    while (KeyPressed()) { if (ReadKey() == 0) ReadKey(); }
    if (r < 0 && demo_hit == 'D') return 2;     /* D -> 전체 시범 */
    return (r < 0) ? 1 : 0;
}

/* [원본에 없던 것] 타이틀에서 D 를 누르면 도는 "전체 시범".
   1판부터 마지막 판까지 컴퓨터가 차례로 "진짜로 깨서" 넘어간다.
   못 깬 판을 그냥 건너뛰지 않는다 - 깰 때까지 다시 한다.  다시 할 때마다
   여유를 달리 잡아(대담 0 -> 1 -> 2 -> 1 -> 2 ...) 오리와 만나는 때가
   달라지므로, 한 번 안 되던 판도 다음 번에는 깨진다.
   돌려주는 값 : 1 = 사람이 키를 눌러 그만두었다 */
#define DEMO_TRY_MAX 8           /* 이만큼 해도 못 깨면 어쩔 수 없이 넘어간다 */

static int DemoBold(int k)       /* k 번째 시도에서 쓸 대담도 */
{
    if (k == 0) return 0;
    return (k & 1) ? 1 : 2;
}

int DemoAll(void)
{
    int st, k, r, cleared = 0;
    char s[8];

    lives = 3; score = 0L;

    for (st = 1; st <= STAGE_MAX; st++) {
        r = 0;
        for (k = 0; k < DEMO_TRY_MAX && !r; k++) {
#ifdef DEMO_TRACE
            long t0 = TicksMs();
#endif
            d_bold = DemoBold(k);              /* 안 되면 여유를 달리 잡아 본다 */
            /* [중요] 같은 여유로 다시 하면 오리와 만나는 때가 똑같아서 결과도
               똑같다 (실제로 원본 9판이 여덟 번 내리 같은 자리에서 실패했다).
               다시 할 때마다 조금씩 늦게 시작해 오리의 위상을 어긋나게 한다. */
            if (k > 0) DelayFrames(k * 83);
            r = DemoStage(st, 115000L);
#ifdef DEMO_TRACE
            printf("판 %2d 시도 %d(대담%d) : %s  (%ld ms, 남은먹이 %d, 남은시간 %d)\n",
                   st, k + 1, d_bold, r > 0 ? "깼다" : (r < 0 ? "키눌림" : "실패"),
                   TicksMs() - t0, food_left[st], time_left);
            fflush(stdout);
#endif
            if (r < 0) { d_bold = 0; DemoCleanup(); return 1; }
        }
        d_bold = 0;
        if (r) {
            cleared++;
            /* 사람이 깼을 때와 똑같이 보너스 점수 화면과 다음 먹이를 보여 준다.
               (StageClear 가 남은 시간 x 100 을 점수에 더하고, 마지막 판이면
                축하 화면까지 띄운다) */
            StageClear();
            if (KeyPressed()) {                  /* 그 사이에 사람이 왔다 */
                char c = ReadKey();
                if (c == 0) ReadKey();
                else if (c == 0x1b) { quit_flag = 1; last_key = 0x1b; }
                d_bold = 0; DemoCleanup(); lives = 3;
                return 1;
            }
        }
    }

    lives = 3;                                   /* 축하 화면이 0 으로 만든다 */

    DemoCleanup();

    /* 끝났다는 것을 알린다 */
    ClearDevice();
    SetColor(3);
    Rectangle(0, 0, 0x13f, 0xc7);
    SetTextJustify(1, 2);
    OutTextXY(160, 60, "==== 전체 시범 끝 ====");
    Str((long)cleared, 0, s);
    {
        char line[40];
        int n = 0, i;
        for (i = 0; s[i]; i++) line[n++] = s[i];
        line[n++] = ' ';
        line[n] = '\0';
        OutTextXY(160, 84, line);
    }
    OutTextXY(160, 100, "판을 깼습니다");
    OutTextXY(160, 0xb0, "아무 키나 누르세요 !");
    SetTextJustify(0, 2);
    GfxFlip();
    while (KeyPressed()) { if (ReadKey() == 0) ReadKey(); }
    if (ReadKey() == 0) ReadKey();
    return 0;
}


/* GameLoop  (원래 오프셋 0x24D8)
   역할: 한 게임의 메인 루프 — 죽음/클리어를 처리하고, 남은 대각선 관성을 소비하거나
         키 입력을 받고, 적을 한 프레임 굴린다. ESC 나 목숨 0 이면 빠져나온다. */
void GameLoop(void)
{
    /* [원본에 없던 것] 이어하기면 점수와 판을 그대로 두고 목숨만 채운다 */
    if (continue_flag) {
        continue_flag = 0;
        lives = 3;
        player_dead = 0;
        stage_cleared = 0;
        move_dir = 0;
        RestartStage();
    } else {
        StartNewGame();                        /* 새 게임 초기화 */
    }

    do {
        /* [원본에 없던 줄] 루프 속도를 묶는다.  화면 갱신은 GfxFlip 이
           수직귀선(70Hz)에 맞추므로 여기서는 상한만 둔다. */
        FrameSync(120);

        /* 죽었고, 목숨이 남아 있고, 클리어가 아니면 -> 죽음 처리 */
        if (player_dead != 0 && lives != 0 && stage_cleared == 0)
            HandleDeath();

        /* 스테이지 클리어 */
        if (stage_cleared != 0)
            StageClear();

        /* 점프 뒤에 남은 관성이 있으면 그것부터 소비한다 */
        if (move_dir == -1) {
            LandLeft();                    /* 왼쪽 아래로 착지 */
        } else if (move_dir == 1) {
            LandRight();                    /* 오른쪽 아래로 착지 */
        } else if (KeyPressed()) {
            HandleKey();                    /* 키 입력 처리 */
        }

        EatPosePoll();                         /* 먹는 자세 되돌리기 */
        UpdateActors();                        /* 적/발판/시간 한 프레임 */
        RenderFrame();                         /* 화면 한 프레임 */

    } while (last_key != 0x1b && lives != 0);  /* ESC 또는 목숨 0 이면 종료 */
}


/* ResetGame  (원래 오프셋 0x2540)
   역할: 게임을 처음부터 다시 하기 위한 상태 리셋 — 종료플래그/방향/먹이·스테이지 번호. */
void ResetGame(void)
{
    quit_flag = 0;             /* 프로그램 종료 플래그 해제 */
    move_dir = 0;             /* 관성(방향) 없음 */
    food_tile = FoodTile(1);   /* 첫 먹이 타일 번호 */
    stage = 1;             /* 스테이지 1 */
    round_no = 1;             /* 통산 라운드 1 */
    platform_on = 0;             /* 움직이는 발판 없음 */
    AcHide(A_PLATFORM);
    /* [원본에 없던 것] 원본은 상태표시줄을 글자 그대로("...030000...") 찍어서
       시작값이 맞았다.  여기서는 진짜 값을 찍으므로 미리 맞춰 둔다. */
    lives = 3; score = 0L; time_left = 100;
}


/* GameOverScreen  (원래 오프셋 0x2579)
   역할: GAME OVER 화면 — 화면 가운데에 검은 상자와 테두리를 그리고 문구를 띄운 뒤
         아무 키나 기다린다. ESC 를 누르면 프로그램을 끝낸다. */
void GameOverScreen(void)
{
    /* [원본에 없던 것] 게임 곡을 끊고 "게임 끝" 곡으로 바꾼다 */
    VgmPlay(OVER_VGM);

    /* 남아 있던 키 입력을 전부 비운다 */
    while (KeyPressed())
        last_key = ReadKey();

    SetFillStyle(1 /*SolidFill*/, 0 /*검정*/);
    Bar(0x78, 0x62, 0xc8, 0x76);            /* 문구가 들어갈 자리를 지운다 */

    SetColor(3);
    Rectangle(0x78, 0x62, 0xc8, 0x76);      /* 테두리 */

    SetTextJustify(1, 2);
    OutTextXY(160, 0x67, "게 임 끝");
    SetTextJustify(0, 2);
    GfxFlip();

    /* [원본과 다름] 그냥 ReadKey 로 막아 두면 그동안 곡이 흐르지 않는다.
       키가 올 때까지 프레임을 돌리며 기다린다 (그 사이 곡이 이어진다). */
    while (!KeyPressed())
        FrameSync(60);
    last_key = ReadKey();                      /* 아무 키나 기다림 */
    VgmStop();

    if (last_key == 0x1b)                      /* ESC = 프로그램 종료 */
        quit_flag = 1;
}


/* main  (원래 오프셋 0x2602 — 프로그램 본체)
   역할: 그래픽 초기화와 데이터 파일 적재 후, 타이틀->게임->게임오버를 ESC 가
         눌릴 때까지 반복한다. */
int main(void)
{
    /* Turbo Pascal 유닛 초기화 (System / Graph / Crt) */

    graph_driver = 1;                              /* GraphDriver = CGA */
    graph_mode = 1;                              /* GraphMode          */
    InitGraph(&graph_driver, &graph_mode, "C:");        /* BGI 경로 0x25ff */

    /* [원본에 없던 것] 한글 글꼴 */
    if (!LoadTextFonts("HSMALL.FNT", "ESMALL.FNT")) {
        CloseGraph();
        fprintf(stderr, "HSMALL.FNT / ESMALL.FNT 가 같은 폴더에 있어야 합니다.\n");
        return 1;
    }

    /* [원본에 없던 것] 뒤 버퍼 + 배경 페이지 (128KB) */
    if (!GfxOpenPages()) {
        CloseGraph();
        fprintf(stderr, "메모리가 모자라 화면 버퍼를 잡을 수 없습니다.\n");
        return 1;
    }

    ClearDevice();

    hi_score = 30000L;                   /* 하이스코어 초기값 30000 */

    LoadSprites();                            /* 16X16.DAT 스프라이트 적재 */
    LoadExtraSprites();                       /* [원본에 없던 것] 16X16B.DAT */
    FindEyes();                               /* [원본에 없던 것] 눈 자리 찾기 */
    ShadeSprites();                           /* [원본에 없던 것] 입체감 주기 */
    MakeTailFrames();                         /* [원본에 없던 것] 꼬리 흔들 그림 */
    MakeWaveFrames();                         /* [원본에 없던 것] 적 꿈틀 그림 */
    MakeClimbFrames();                        /* [원본에 없던 것] 사다리 손발 */
    MakeSmallIcons();                         /* [원본에 없던 것] 8x8 판 아이콘 */
    MakePlatFrames();                         /* [원본에 없던 것] 층마다 다른 발판 색 */

    food_tile = 0x12;                          /* 첫 먹이 타일 번호 */

    LoadStageData();                            /* B_ST.DAT 스테이지 데이터 적재 */

    LoadHiScores();                           /* [원본에 없던 것] BONG.HI */

    do {
        ResetGame();                        /* 게임 상태 리셋 */

        /* [원본에 없던 것] 타이틀에서
             - 5초 동안 아무 키도 안 누르면 : 컴퓨터가 한 판 보여 준다(어트랙트)
             - D 를 누르면               : 1판부터 끝까지 보여 주는 전체 시범
             - 그 밖의 키                : 게임 시작 */
        {
            int pick = TitleScreen();
            if (quit_flag) break;
            if (pick == 0) {
                int r = DemoRun();
                if (r == 0) continue;       /* 시범만 보이고 다시 타이틀로 */
                if (r == 2) { DemoAll(); if (quit_flag) break; continue; }
                if (quit_flag) break;
            } else if (pick == 2) {
                DemoAll();                  /* 전체 시범 - 끝나면 타이틀로 */
                if (quit_flag) break;
                continue;
            }
        }

        for (;;) {
            DrawStage(stage);              /* 스테이지 그리기 */
            DrawStatusBar();                    /* 하단 상태표시줄 */
            GfxFlip();

            GameLoop();                    /* 게임 메인 루프 */

            if (quit_flag) break;
            if (lives != 0) break;         /* 클리어/중단 */

            /* [원본에 없던 것] 이어하기 */
            if (!ContinueScreen()) break;
            continue_flag = 1;
        }

        if (lives == 0 && !quit_flag) {    /* 목숨이 다 떨어졌으면 */
            int rank;
            GameOverScreen();                    /* GAME OVER 화면 */
            rank = HiRank(score);          /* [원본에 없던 것] 최고 기록 */
            if (rank >= 0) {
                HiInsert(rank, score);
                EnterName(rank);
                SaveHiScores();
            }
            RankingScreen(rank);
            hi_score = hi_val[0];
        }

    } while (quit_flag == 0);                  /* ESC 로 종료할 때까지 반복 */

    SaveHiScores();

    CloseGraph();

    Halt(0);                                /* @Halt */
    return 0;
}
