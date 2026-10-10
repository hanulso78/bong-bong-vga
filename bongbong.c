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
 *  이 파일 자체는 원본 어셈블리를 옮긴 게임 로직만 담는다.
 *      make        - BONGBONG.EXE 를 만든다
 * ========================================================================== */

#include <stddef.h>
#include <string.h>

#include "tp.h"

/* ============================ 전역 변수 ====================================
   (원본에 심볼이 없어 DS 오프셋으로만 알 수 있었다. 오프셋을 주석에 남긴다) */

FILE_T  file_sprites;        /* 0x028C  16X16.DAT 파일 변수 */
FILE_T  file_stages;         /* 0x038C  B_ST.DAT 파일 변수 */

void  * sprite[43];          /* 0x0488  16x16 그림 42장의 버퍼 포인터 [1..42] */
int     sprite_size;         /* 0x0534  그림 한 장의 바이트 크기 */
#define SPR_FLOOR  sprite[4]    /* 0x0498  바닥 타일     */
#define SPR_LIFE   sprite[14]   /* 0x04C0  목숨 아이콘   */

int     map_cur[2700];       /* 0x033C  현재 타일맵  [stage*240 + col*12 + row] */
int     map_orig[2700];      /* 0x15FC  원본 타일맵(스테이지 재시작용) */

int     graph_driver;        /* 0x0286  InitGraph driver */
int     graph_mode;          /* 0x0288  InitGraph mode */
byte    last_key;            /* 0x028A  마지막으로 읽은 키 */

int     stage;               /* 0x2AE4  현재 스테이지 1..10 */
int     round_no;            /* 0x2AE6  통산 라운드 수(11판째면 스테이지는 1로 돌아가도 계속 증가) */
int     lives;               /* 0x2AE8  남은 목숨(3에서 시작, 0이면 게임 끝) */
long    score;               /* 0x2B94  점수 (longint) */
long    hi_score;            /* 0x2B90  하이스코어 (longint, 초기 30000) */
int     time_left;           /* 0x2B8E  남은 시간(100에서 감소) */

int     food_tile;           /* 0x2AEA  이번 판 먹이 타일 번호(0x12..0x1B) */
int     food_left[12];       /* 0x2AB4  스테이지별 남은 먹이 수 */
int     food_total[12];      /* 0x2AC8  스테이지별 먹이 총수(원본) */
int     stage_param[48];     /* 배열 밑번지 0x2AE2, 자료는 0x2AEC 부터. 스테이지별 적 속도 [stage*4+i] */

int     player_col;          /* 0x2B82  플레이어 칸(col) 1..20 */
int     player_row;          /* 0x2B84  플레이어 줄(row) 1..12 */
int     player_sprite;       /* 0x2AE0  플레이어 자세(그림 번호) */
int     move_dir;            /* 0x2B8C  플레이어 이동 방향 -1 왼쪽 / 0 정지 / +1 오른쪽 */

/* 적 4마리 (인덱스 1..4) */
int     enemy_col[8];        /* 0x2B4A  적의 칸(col) */
int     enemy_row[8];        /* 0x2B52  적의 줄(row) - 11/8/5/2 고정 */
int     enemy_dir[8];        /* 0x2B3A  적의 진행 방향 (+1/-1) */
int     enemy_sprite[8];     /* 0x2B5A  적의 그림 번호 */
double  enemy_last_move[8];  /* 0x2B5E  적이 마지막으로 움직인 시각(원본 6바이트 real) */

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
void LoadSprites(void);
void LoadStageData(void);
int  GridToPixel(int idx);
void DrawTile(int col, int row, int img);
void DrawTitleRow(int img);
void TitleScreen(void);
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
                    v = food_tile + stage - 1;            /* 시간+스테이지 의존 타일 */

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

        if (stage == 10) break;
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
    int y = 0x82;               /* [bp-4] : 130 */
    int i;                      /* [bp-2] */
    int x;

    for (i = 0; ; i++) {
        x = ((i << 4) << 1) + 8;            /* i*32 + 8 */
        PutImage(x, y, sprite[img], 0);
        if (i == 9) break;                  /* 10개 */
    }
}


/* TitleScreen  (원래 오프셋 0x0500)
   역할: 타이틀/인트로 화면 - 제목·제작자·조작키를 출력하고 캐릭터를 좌우로
         움직이며 멜로디를 연주한 뒤 키 입력을 기다린다(Esc 면 종료 플래그). */
void TitleScreen(void)
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

    ClearDevice();

    SetLineStyle(0, 3, 1);                  /* ? 3인자 그래픽 설정(선/채우기 스타일) */
    Rectangle(0, 0, 0x13f, 0xc7);           /* 화면 전체 테두리 (319,199) */

    OutTextXY(0x73, 0x1e, "[Bong Bong]");           /* 115, 30  */
    OutTextXY(0x50, 0x32, "Programed By Lee T.K."); /*  80, 50  */
    OutTextXY(0x5a, 0x3c, "Yonsei COMSCI 1989");    /*  90, 60  */
    OutTextXY(0x64, 0x50, " ==== Key ====");        /* 100, 80  */
    OutTextXY(0x64, 0x5a, " [Esc]  : Quit");        /* 100, 90  */
    OutTextXY(0x64, 0x64, " Cursor : Move");        /* 100, 100 */
    OutTextXY(0x64, 0x6e, " Space  : jump");        /* 100, 110 */

    /* 캐릭터 줄을 그려놓고, 음이 바뀔 때마다 10번<->14번 스프라이트로 토글하여
       춤추는 것처럼 보이게 한다 (t = 24 - t). */
    t = 10;
    DrawTitleRow(t);
    NoSound();

    for (n = 0; n < 11; n++) {
        if (n > 0) {
            t = 0x18 - t;                   /* 10 <-> 14 */
            DrawTitleRow(t);
            Delay(0x1e);                    /* 30ms */
        }
        Sound(melody[n].hz);
        Delay(melody[n].ms);
        NoSound();
    }

    OutTextXY(0x64, 0xaa, "Press Any Key !");       /* 100, 170 */

    last_key = ReadKey();
    if (last_key == 0x1b)                      /* Esc */
        quit_flag = 1;                         /* 종료 플래그 */
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
                PutImage((col - 1) * 16, (row - 1) * 16, sprite[tile], 0 /*CopyPut*/);
        }
    }

    /* 3) 맨 아래(y=176) 바닥 타일 한 줄 */
    for (col = 1; col <= 20; col++)
        PutImage((col - 1) * 16, 0xb0, SPR_FLOOR, 0 /*CopyPut*/);

    /* 4) 우측 20번 열에 "지나온 스테이지" 아이콘을 스테이지-1 개 만큼 세로로 */
    col = 20;
    SetColor(3);
    row = 0;
    if (stage != 1) {
        last = stage - 1;
        if (1 <= last) {
            for (row = 1; row <= last; row++)
                DrawTile(col, row, row + 17);      /* 스프라이트 18,19,20... */
            row = last;
        }
    }

    /* 5) 놀이판과 우측 정보란을 가르는 세로선 */
    Line(0x130, 0, 0x130, 0xa0);

    /* 6) 아이콘 바로 아래에 수치(round_no) 출력 */
    Str((long)round_no, 0, s /*string[6]*/);          /* @Str */
    OutTextXY(0x130, row * 16 + 4, s);
}


/* DrawStatusBar  (원래 오프셋 0x09D2)
   역할: 화면 하단 상태표시줄(HI-SCORE / SCORE / TIME / 목숨)의 바탕 틀을 그린다. */
void DrawStatusBar(void)
{
    static const char HUD[] = "HI-SCORE030000 SCORE000000 TIME100   3";   /* 0x09AB */

    SetTextJustify(0, 2);           /* LeftText, TopText */
    SetTextStyle(0, 0, 1);          /* 기본 폰트, 가로, 1배 */

    /* 목숨 아이콘을 XOR 로 찍는다 */
    PutImage(0x114, 0xb9, SPR_LIFE, 1 /*XORPut*/);

    OutTextXY(0, 0xbe, HUD);

    SetFillStyle(1, 0);             /* ? SolidFill, 검정 — 이후 Bar 로 지울 때 쓰는 설정 */
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

    /* ---------------- 적 4마리 ---------------- */
    for (i = 1; i <= 4; i++) {

        /* 이 스테이지에서 속도가 0 인 적은 존재하지 않는 것 */
        if (stage_param[stage * 4 + i] == 0)
            continue;

        /* 속도 * (현재시각 - 마지막이동시각) / 100 >= 1 이 되면 한 칸 이동 */
        if ((double)stage_param[stage * 4 + i]
              * ((double)now_csec - enemy_last_move[i]) / 100.0 < 1.0)
            continue;

        enemy_last_move[i] = (double)now_csec;       /* 마지막 이동 시각 갱신 */

        /* 있던 자리에 원래 타일을 다시 찍어 지운다 */
        DrawTile(enemy_col[i], enemy_row[i],
                  map_cur[stage * 240 + enemy_col[i] * 12 + enemy_row[i]]);

        /* 좌우로 한 칸 이동 후 다시 그린다 */
        enemy_col[i] += enemy_dir[i];
        DrawTile(enemy_col[i], enemy_row[i], enemy_sprite[i]);

        /* 플레이어와 같은 칸이면 죽음 */
        if (enemy_col[i] == player_col && enemy_row[i] == player_row)
            player_dead = 1;

        /* 좌우 끝(1 또는 19)에 닿으면 방향과 좌/우 스프라이트를 뒤집는다 */
        if (enemy_col[i] == 0x13 || enemy_col[i] == 1) {
            enemy_dir[i] = -enemy_dir[i];
            enemy_sprite[i] = 0x11 - enemy_sprite[i];   /* 짝을 이루는 두 스프라이트(합 17) */
        }
    }

    /* ---------------- 움직이는 발판 ---------------- */
    if (platform_on != 0) {

        /* 2 * (현재시각 - 마지막이동시각) / 100 >= 0.8 => 0.4초마다 한 박자 */
        if (2.0 * ((double)now_csec - platform_last_move) / 100.0 >= 0.8) {

            platform_last_move = (double)now_csec;

            /* 있던 자리 지우기 */
            DrawTile(platform_col, platform_row,
                      map_cur[stage * 240 + platform_col * 12 + platform_row]);

            /* 두 박자에 한 칸씩 움직이며 스프라이트를 2프레임으로 번갈아 쓴다 */
            if (platform_phase != 0) {
                platform_sprite -= platform_dir;           /* 애니메이션 프레임 되돌리기 */
                platform_phase  = 0;
            } else {
                platform_sprite += platform_dir;           /* 애니메이션 프레임 진행 */
                platform_col += platform_dir;           /* 실제로 한 칸 이동 */
                platform_phase  = 1;
            }

            DrawTile(platform_col, platform_row, platform_sprite);

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

        /* 시계를 되돌렸으니 각 객체의 "마지막 이동 시각"도 0 으로 */
        for (i = 1; i <= 4; i++)
            enemy_last_move[i] = 0.0;
        platform_last_move = 0.0;

        time_left--;                           /* 남은 시간 */

        /* 시간 표시 영역을 지우고 다시 찍는다 */
        Bar(0xf8, 0xbe, 0x110, 0xc7);
        FormatNumber((long)time_left, '0', tmp);
        strcopy(buf, tmp, 6);               /* @StrCopy */
        Delete(buf, 1, 3);                  /* 앞 3자리 버리고 3자리만 표시 */
        OutTextXY(0xf8, 0xbe, buf);

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
        Delay(ms / k);          /* @LongDiv : ms div k */
        UpdateActors();
    }
}


/* EatFood  (원래 오프셋 0x0F06)
   역할: 먹이를 하나 먹었을 때 — 도레미 상승음 연주, 점수 +100 표시,
         해당 칸을 빈 칸으로 바꾸고 남은 먹이가 0 이면 스테이지 클리어 플래그를 세운다. */
void EatFood(void)
{
    char tmp[0x100];    /* [bp-0x100] FormatNumber 결과 버퍼 */

    /* C-D-E-F-G-A-B-C 상승 음계 */
    Sound(0x20b); DelayAnimated(0x19);
    Sound(0x24b); DelayAnimated(0x19);
    Sound(0x293); DelayAnimated(0x19);
    Sound(0x2ba); DelayAnimated(0x19);
    Sound(0x310); DelayAnimated(0x19);
    Sound(0x370); DelayAnimated(0x19);
    Sound(0x3dc); DelayAnimated(0x19);
    Sound(0x417); DelayAnimated(0x19);
    NoSound();

    /* 점수 +100 (원본은 longint : 0x2B94 하위 / 0x2B96 상위) */
    score += 100;

    /* 점수 표시 갱신 */
    Bar(0xa0, 0xbe, 0xd0, 0xc7);
    FormatNumber(score, '0', tmp);
    OutTextXY(0xa0, 0xbe, tmp);

    /* 플레이어가 서 있는 칸을 1(먹은 뒤의 빈 발판)로 바꾼다 */
    map_cur[stage * 240 + player_col * 12 + player_row] = 1;

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

    DrawTile(player_col, player_row, eat);

    /* 도-레-미-파-솔-라-시-도 상승 음계 */
    Sound(0x20b); DelayAnimated(0x19);
    Sound(0x24b); DelayAnimated(0x19);
    Sound(0x293); DelayAnimated(0x19);
    Sound(0x2ba); DelayAnimated(0x19);
    Sound(0x310); DelayAnimated(0x19);
    Sound(0x370); DelayAnimated(0x19);
    Sound(0x3dc); DelayAnimated(0x19);
    Sound(0x417); DelayAnimated(0x19);
    NoSound();

    /* 원래 자세로 되돌린다 */
    DrawTile(player_col, player_row, player_sprite);

    /* 점수 가산 (원본은 longint : 0x2B94 하위 / 0x2B96 상위) */
    if (map_cur[stage * 240 + player_col * 12 + player_row] == 0x1c)
        score += 500;         /* 아이템 A */
    else
        score += 1000;        /* 아이템 B */

    /* 점수 표시 갱신 */
    Bar(0xa0, 0xbe, 0xd0, 0xc7);
    FormatNumber(score, '0', tmp);
    OutTextXY(0xa0, 0xbe, tmp);

    /* 먹은 칸을 빈 칸으로 */
    map_cur[stage * 240 + player_col * 12 + player_row] = 1;
}


/* SpawnPlatform  (원래 오프셋 0x11D1)
   역할: 타일 0x1e 를 밟았을 때 — 효과음을 낸 뒤 플레이어 옆 칸에
         "움직이는 발판"을 생성하고 그 초기 상태(위치/방향/시각)를 세팅한다. */
void SpawnPlatform(void)
{
    /* 삑-삐 두 음 */
    Sound(0x20b); DelayAnimated(0xc8);
    Sound(0x22a); DelayAnimated(0x64);
    NoSound();

    /* 발생 타일은 한 번 쓰면 사라진다 */
    map_cur[stage * 240 + player_col * 12 + player_row] = 1;

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

    DrawTile(platform_col, platform_row, platform_sprite);
}


/* FallAndDie  (원래 오프셋 0x1289)
   역할: 발밑이 비었을 때의 추락 처리 — 4프레임 애니메이션으로 맨 아래(row 11)
         까지 떨어뜨린 뒤 죽음 플래그를 세운다. */
void FallAndDie(void)
{
    int row;    /* [bp-2] */

    /* 서 있던 칸을 원래 타일로 되돌려 지운다 */
    DrawTile(player_col, player_row,
              map_cur[stage * 240 + player_col * 12 + player_row]);

    player_sprite = 0x27;                          /* 추락 애니메이션 첫 프레임 */

    /* 한 줄씩 아래로 (row+1 .. 10) */
    for (row = player_row + 1; row <= 0x0a; row++) {

        DrawTile(player_col, row, player_sprite);     /* 떨어지는 모습 그리기 */

        player_sprite++;                           /* 0x27..0x2a 순환 */
        if (player_sprite == 0x2b)
            player_sprite = 0x27;

        Delay(0x7d);                        /* 125ms */

        DrawTile(player_col, row,              /* 지우기 */
                  map_cur[stage * 240 + player_col * 12 + row]);
    }

    /* 바닥(row 11)에 처박힌다 */
    DrawTile(player_col, 0x0b, player_sprite);
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

    /* 2) 적 4마리와 같은 칸인가 */
    hit = 0;
    for (i = 1; i <= 4; i++) {
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

        Sound(0x417);
        DelayAnimated(0x4b);
        NoSound();

        /* 있던 자리 지우고 한 칸 위에 그린다 */
        DrawTile(player_col, player_row,
                  map_cur[stage * 240 + player_col * 12 + player_row]);
        player_row--;
        DrawTile(player_col, player_row, player_sprite);

        DelayAnimated(0x32);

        /* 다시 원래 자리로 내려온다 */
        DrawTile(player_col, player_row,
                  map_cur[stage * 240 + player_col * 12 + player_row]);
        player_row++;
        DrawTile(player_col, player_row, player_sprite);

        move_dir = 0;
        break;

    /* ------------------------------------------------------------------
       [0x0a] 왼쪽 — 왼쪽 위로 한 칸 대각선 이동.
       ------------------------------------------------------------------ */
    case 0x0a:
        if (player_col <= 2)                        /* 왼쪽 벽 */
            break;

        move_dir = -1;

        Sound(0x417);
        DelayAnimated(0x32);

        /* 지나갈 두 칸(col-2, col-1)에 담당 적이 있으면 죽음.
           적 번호는 행으로 결정된다 : enemy = 5 - (row+1) div 3
           (row 2->4, row 5->3, row 8->2, row 11->1) */
        target = player_col - 2;
        enemy  = 5 - (player_row + 1) / 3;
        if (enemy_col[enemy] == target) {
            player_dead = 1;
        } else {
            target = player_col - 1;
            enemy  = 5 - (player_row + 1) / 3;
            if (enemy_col[enemy] == target)
                player_dead = 1;
        }

        NoSound();

        player_sprite = 0x0b;                          /* 걷는 자세 */

        /* 있던 자리 지우기 */
        DrawTile(player_col, player_row,
                  map_cur[stage * 240 + player_col * 12 + player_row]);

        /* 반 칸(8픽셀) 왼쪽 위에 중간 프레임을 찍는다 */
        PutImage(GridToPixel(player_col) - 8, GridToPixel(player_row) - 8,
                 sprite[player_sprite], 0 /*CopyPut*/);

        DelayAnimated(0x25);

        /* 중간 프레임이 걸쳐 있던 2x2 칸을 원래 타일로 복구 */
        DrawTile(player_col - 1, player_row - 1,
                  map_cur[stage * 240 + (player_col - 1) * 12 + (player_row - 1)]);
        DrawTile(player_col,     player_row - 1,
                  map_cur[stage * 240 + player_col * 12 + (player_row - 1)]);
        DrawTile(player_col - 1, player_row,
                  map_cur[stage * 240 + (player_col - 1) * 12 + player_row]);
        DrawTile(player_col,     player_row,
                  map_cur[stage * 240 + player_col * 12 + player_row]);

        player_sprite = 0x0a;                          /* 정지 자세로 */

        player_col--;                               /* 왼쪽 위 대각선 한 칸 */
        player_row--;

        DrawTile(player_col, player_row, player_sprite);
        break;

    /* ------------------------------------------------------------------
       [0x0e] 오른쪽 — 오른쪽 위로 한 칸 대각선 이동. (0x0a 의 좌우 대칭)
       ------------------------------------------------------------------ */
    case 0x0e:
        if (player_col >= 0x12)                     /* 오른쪽 벽 */
            break;

        move_dir = 1;

        Sound(0x417);

        /* 지나갈 두 칸(col+2, col+1)에 담당 적이 있으면 죽음 */
        target = player_col + 2;
        enemy  = 5 - (player_row + 1) / 3;
        if (enemy_col[enemy] == target) {
            player_dead = 1;
        } else {
            target = player_col + 1;
            enemy  = 5 - (player_row + 1) / 3;
            if (enemy_col[enemy] == target)
                player_dead = 1;
        }

        DelayAnimated(0x32);
        NoSound();

        player_sprite = 0x0f;                          /* 걷는 자세 */

        DrawTile(player_col, player_row,
                  map_cur[stage * 240 + player_col * 12 + player_row]);

        /* 반 칸(8픽셀) 오른쪽 위에 중간 프레임 */
        PutImage(GridToPixel(player_col) + 8, GridToPixel(player_row) - 8,
                 sprite[player_sprite], 0 /*CopyPut*/);

        DelayAnimated(0x25);

        /* 걸쳐 있던 2x2 칸 복구 */
        DrawTile(player_col,     player_row - 1,
                  map_cur[stage * 240 + player_col * 12 + (player_row - 1)]);
        DrawTile(player_col + 1, player_row - 1,
                  map_cur[stage * 240 + (player_col + 1) * 12 + (player_row - 1)]);
        DrawTile(player_col,     player_row,
                  map_cur[stage * 240 + player_col * 12 + player_row]);
        DrawTile(player_col + 1, player_row,
                  map_cur[stage * 240 + (player_col + 1) * 12 + player_row]);

        player_sprite = 0x0e;                          /* 정지 자세로 */

        player_col++;                               /* 오른쪽 위 대각선 한 칸 */
        player_row--;

        DrawTile(player_col, player_row, player_sprite);
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

    /* 있던 자리 지우기 */
    DrawTile(player_col, player_row,
              map_cur[stage * 240 + player_col * 12 + player_row]);

    /* 반 칸(8픽셀) 왼쪽 아래에 중간 프레임을 찍는다 */
    PutImage(GridToPixel(player_col) - 8, GridToPixel(player_row) + 8,
             sprite[player_sprite], 0 /*CopyPut*/);

    DelayAnimated(0x25);

    /* 중간 프레임이 걸쳐 있던 2x2 칸을 원래 타일로 복구 */
    DrawTile(player_col - 1, player_row,
              map_cur[stage * 240 + (player_col - 1) * 12 + player_row]);
    DrawTile(player_col,     player_row,
              map_cur[stage * 240 + player_col * 12 + player_row]);
    DrawTile(player_col - 1, player_row + 1,
              map_cur[stage * 240 + (player_col - 1) * 12 + (player_row + 1)]);
    DrawTile(player_col,     player_row + 1,
              map_cur[stage * 240 + player_col * 12 + (player_row + 1)]);

    player_sprite = 0x0a;                              /* 왼쪽 정지 자세로 */

    player_col--;                                   /* 왼쪽 아래 대각선 한 칸 */
    player_row++;

    DrawTile(player_col, player_row, player_sprite);

    move_dir = 0;                                 /* 관성(방향) 소진 */

    CheckCurrentCell();                                /* 착지한 칸 판정 */
}


/* LandRight  (원래 오프셋 0x1A87)
   역할: 오른쪽 아래 대각선으로 한 칸 이동 — LandLeft 의 좌우 대칭. */
void LandRight(void)
{
    player_sprite = 0x10;                              /* 오른쪽 아래 걷기 자세 */

    /* 있던 자리 지우기 */
    DrawTile(player_col, player_row,
              map_cur[stage * 240 + player_col * 12 + player_row]);

    /* 반 칸(8픽셀) 오른쪽 아래에 중간 프레임을 찍는다 */
    PutImage(GridToPixel(player_col) + 8, GridToPixel(player_row) + 8,
             sprite[player_sprite], 0 /*CopyPut*/);

    DelayAnimated(0x25);

    /* 중간 프레임이 걸쳐 있던 2x2 칸을 원래 타일로 복구 */
    DrawTile(player_col,     player_row,
              map_cur[stage * 240 + player_col * 12 + player_row]);
    DrawTile(player_col + 1, player_row,
              map_cur[stage * 240 + (player_col + 1) * 12 + player_row]);
    DrawTile(player_col,     player_row + 1,
              map_cur[stage * 240 + player_col * 12 + (player_row + 1)]);
    DrawTile(player_col + 1, player_row + 1,
              map_cur[stage * 240 + (player_col + 1) * 12 + (player_row + 1)]);

    player_sprite = 0x0e;                              /* 오른쪽 정지 자세로 */

    player_col++;                                   /* 오른쪽 아래 대각선 한 칸 */
    player_row++;

    DrawTile(player_col, player_row, player_sprite);

    move_dir = 0;                                 /* 관성(방향) 소진 */

    CheckCurrentCell();                                /* 착지한 칸 판정 */
}


/* ClimbUp  (원래 오프셋 0x1BE7)
   역할: 위쪽 화살표 — 지금 서 있는 칸이 사다리(타일 6)일 때만 한 칸 올라간다. */
void ClimbUp(void)
{
    if (map_cur[stage * 240 + player_col * 12 + player_row] != 6)
        return;                                 /* 사다리가 아니면 무시 */

    Sound(0x20b);
    DelayAnimated(0x19);

    player_sprite = 0x0d;                              /* 위(사다리) 자세 */

    /* 있던 자리 지우고 한 칸 위에 그린다 */
    DrawTile(player_col, player_row,
              map_cur[stage * 240 + player_col * 12 + player_row]);
    player_row--;
    DrawTile(player_col, player_row, player_sprite);

    NoSound();

    CheckCurrentCell();                                /* 올라간 칸 판정 */
    DelayAnimated(0x19);
}


/* WalkLeft  (원래 오프셋 0x1C84)
   역할: 왼쪽 화살표 — 계단참(row%3==2)에서만, 먼저 왼쪽을 보게 하고
         이미 왼쪽을 보고 있었다면 한 칸 왼쪽으로 걷는다. */
void WalkLeft(void)
{
    if (player_row % 3 != 2)                        /* 계단참 높이가 아니면 무시 */
        return;

    Sound(0x20b);
    DelayAnimated(0x19);

    if (player_col > 1) {                           /* 왼쪽 벽이 아니면 */
        /* 있던 자리 지우기 */
        DrawTile(player_col, player_row,
                  map_cur[stage * 240 + player_col * 12 + player_row]);

        /* 이미 왼쪽을 보고 있었을 때만 실제로 한 칸 이동 */
        if (player_sprite == 0x0a)
            player_col--;

        player_sprite = 0x0a;                          /* 왼쪽 정지 자세 */
    }

    /* 벽에 막혔더라도 현재 자세는 다시 찍는다 */
    DrawTile(player_col, player_row, player_sprite);

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

    Sound(0x20b);
    DelayAnimated(0x19);

    /* 오른쪽 벽 판정 : 맨 아랫줄만 한 칸 더 갈 수 있다 */
    if ((player_col < 0x14 && player_row == 0x0b) || player_col < 0x13) {
        /* 있던 자리 지우기 */
        DrawTile(player_col, player_row,
                  map_cur[stage * 240 + player_col * 12 + player_row]);

        /* 이미 오른쪽을 보고 있었을 때만 실제로 한 칸 이동 */
        if (player_sprite == 0x0e)
            player_col++;

        player_sprite = 0x0e;                          /* 오른쪽 정지 자세 */
    }

    /* 벽에 막혔더라도 현재 자세는 다시 찍는다 */
    DrawTile(player_col, player_row, player_sprite);

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

    Sound(0x20b);
    DelayAnimated(0x19);

    player_sprite = 0x0d;                              /* 사다리 자세 */

    /* 있던 자리 지우고 한 칸 아래에 그린다 */
    DrawTile(player_col, player_row,
              map_cur[stage * 240 + player_col * 12 + player_row]);
    player_row++;
    DrawTile(player_col, player_row, player_sprite);

    NoSound();

    CheckCurrentCell();                                /* 내려간 칸 판정 */
    DelayAnimated(0x19);
}


/* HandleKey  (원래 오프셋 0x1E5B)
   역할: 키보드 처리 — 스페이스는 점프, ESC 는 종료, 확장키(선행 0x00)는
         상/하/좌/우 이동 루틴으로 분기한다. */
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

    for (i = 1; i <= 4; i++) {

        enemy_sprite[i] = 9;                    /* 오른쪽을 보는 적 스프라이트 */
        enemy_dir[i] = 1;                    /* 진행 방향 = 오른쪽 */

        enemy_last_move[i] = 0.0;                  /* 마지막 이동 시각(6바이트 real) */

        if (stage_param[stage * 4 + i] == 0) {
            enemy_col[i] = 0;                /* 속도 0 = 이 스테이지에 없는 적 */
        } else {
            enemy_col[i] = 1;                /* 왼쪽 끝(col 1)에서 출발 */
            DrawTile(enemy_col[i], enemy_row[i], enemy_sprite[i]);
        }
    }

    DrawTile(player_col, player_row, player_sprite);      /* 플레이어 그리기 */

    SetTime(0, 0, 0, 0);                    /* 시계 리셋 */
}


/* DeathAnim  (원래 오프셋 0x1FDC)
   역할: 죽는 연출 — 죽는 자세(0x11)로 바꾸고 하강 음계를 울린 뒤 목숨을 하나 줄인다. */
void DeathAnim(void)
{
    DrawTile(player_col, player_row, 0x11);        /* 죽는 자세 */

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
    static const char HUD[] = "HI-SCORE       SCORE       TIME100    ";  /* 0x2095 */

    int  row;               /* [bp-0x16]  1..11 */
    int  col;               /* [bp-0x14]  1..20 */
    int  i;                 /* [bp-0x12]  적 번호 1..4 */
    char s[7];              /* [bp-0x10]  string[6] */
    char tmp[0x100];        /* [bp-0x11a] FormatNumber 결과 임시 버퍼 */

    /* 죽는 동안 눌린 키를 전부 비운다 */
    while (KeyPressed())
        last_key = ReadKey();

    platform_on = 0;                             /* 움직이는 발판 소멸 */

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

    OutTextXY(0, 0xbe, HUD);

    FormatNumber(hi_score, '0', tmp);       /* 하이스코어 */
    OutTextXY(0x40, 0xbe, tmp);

    FormatNumber(score, '0', tmp);       /* 현재 점수 */
    OutTextXY(0xa0, 0xbe, tmp);

    Str((long)lives, 0, s);                /* @Str : 남은 목숨 */
    PutImage(0x114, 0xb9, SPR_LIFE, 1 /*XORPut*/);     /* 목숨 아이콘 */
    OutTextXY(0x128, 0xbe, s);

    /* 4) 플레이어를 출발 위치로 */
    time_left = 100;                           /* 시간 다시 100 */
    player_col = 0x14;
    player_row = 0x0b;
    player_sprite = 0x0a;

    /* 5) 적 4마리 재배치 (행 enemy_row[] 는 그대로 두고 열만 되돌린다) */
    for (i = 1; i <= 4; i++) {

        enemy_sprite[i] = 9;
        enemy_dir[i] = 1;

        enemy_last_move[i] = 0.0;

        if (stage_param[stage * 4 + i] == 0) {
            enemy_col[i] = 0;
        } else {
            enemy_col[i] = 1;
            DrawTile(enemy_col[i], enemy_row[i], enemy_sprite[i]);
        }
    }

    DrawTile(player_col, player_row, player_sprite);

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


/* StageClear  (원래 오프셋 0x2309)
   역할: 스테이지 클리어 처리 — 다음 먹이/스테이지로 넘기고(11 이면 1 로 되돌리며
         적 속도를 올린다) 남은 시간 x100 보너스와 다음 먹이 그림을 보여 준다. */
void StageClear(void)
{
    int  i;                 /* [bp-0x12] */
    char s[7];              /* [bp-8]    string[6] */
    char tmp[0x100];        /* [bp-0x112] FormatNumber / @Concat 임시 버퍼 */

    player_dead = 0;
    stage_cleared = 0;

    ClearDevice();

    /* 다음 먹이 타일 번호로 (0x12..0x1b 순환) */
    food_tile++;
    if (food_tile == 0x1c)
        food_tile = 0x12;

    stage++;               /* 다음 스테이지 */
    round_no++;               /* 통산 라운드 수 */

    /* 스테이지 10 을 넘기면 1 로 되돌아가되 적이 빨라진다 */
    if (stage == 0x0b) {

        stage = 1;

        for (i = 1; i <= 4; i++) {

            /* 존재하는 적만 속도 +2 (최대 0x10 으로 제한) */
            if (stage_param[stage * 4 + i] != 0)
                stage_param[stage * 4 + i] = stage_param[stage * 4 + i] + 2;

            if (stage_param[stage * 4 + i] > 0x10)
                stage_param[stage * 4 + i] = 0x10;
        }
    }

    /* 남은 시간 x 100 을 점수에 더한다 (longint) */
    score += (long)time_left * 100;

    /* "BONUS is +nnnn" — 6자리로 만든 뒤 앞 2자리를 잘라 4자리만 보여 준다 */
    FormatNumber((long)time_left * 100, '0', tmp);
    strcopy(s, tmp, 6);                     /* @StrCopy */
    Delete(s, 1, 2);                        /* @Delete  */

    strcopy(tmp, "BONUS is +", 0xff);       /* @Concat : 'BONUS is +' + s */
    strcat(tmp, s);
    OutTextXY(0x64, 0x61, tmp);

    /* 클리어 팡파르 */
    Sound(0x20b); Delay(0x64);
    Sound(0x293); Delay(0x64);
    Sound(0x310); Delay(0x64);
    Sound(0x417); Delay(0x64);
    NoSound();

    /* 다음 스테이지의 먹이가 무엇인지 그림으로 보여 준다 */
    OutTextXY(0x78, 0x82, "NEXT Item is");                  /* 0x22fc */
    PutImage(0xd8, 0x7e, sprite[food_tile], 0 /*CopyPut*/);

    Delay(0x5dc);                           /* 1.5초 */

    RestartStage();                            /* 새 스테이지 화면 세팅 */
}


/* GameLoop  (원래 오프셋 0x24D8)
   역할: 한 게임의 메인 루프 — 죽음/클리어를 처리하고, 남은 대각선 관성을 소비하거나
         키 입력을 받고, 적을 한 프레임 굴린다. ESC 나 목숨 0 이면 빠져나온다. */
void GameLoop(void)
{
    StartNewGame();                            /* 새 게임 초기화 */

    do {
        /* [원본에 없던 한 줄] 루프를 초당 GAME_FPS 번으로 묶는다.
           원본은 최대 속도로 돌아서 빠른 기계일수록 게임이 빨라졌다. */
        FrameSync(GAME_FPS);

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

        UpdateActors();                        /* 적/발판/시간 한 프레임 */

    } while (last_key != 0x1b && lives != 0);  /* ESC 또는 목숨 0 이면 종료 */
}


/* ResetGame  (원래 오프셋 0x2540)
   역할: 게임을 처음부터 다시 하기 위한 상태 리셋 — 종료플래그/방향/먹이·스테이지 번호. */
void ResetGame(void)
{
    quit_flag = 0;             /* 프로그램 종료 플래그 해제 */
    move_dir = 0;             /* 관성(방향) 없음 */
    food_tile = 0x12;          /* 첫 먹이 타일 번호 */
    stage = 1;             /* 스테이지 1 */
    round_no = 1;             /* 통산 라운드 1 */
    platform_on = 0;             /* 움직이는 발판 없음 */
}


/* GameOverScreen  (원래 오프셋 0x2579)
   역할: GAME OVER 화면 — 화면 가운데에 검은 상자와 테두리를 그리고 문구를 띄운 뒤
         아무 키나 기다린다. ESC 를 누르면 프로그램을 끝낸다. */
void GameOverScreen(void)
{
    /* 남아 있던 키 입력을 전부 비운다 */
    while (KeyPressed())
        last_key = ReadKey();

    SetFillStyle(1 /*SolidFill*/, 0 /*검정*/);
    Bar(0x78, 0x64, 0xc8, 0x6e);            /* 문구가 들어갈 자리를 지운다 */

    SetColor(3);
    Rectangle(0x78, 0x64, 0xc8, 0x6e);      /* 테두리 */

    OutTextXY(0x7d, 0x66, "GAME OVER");     /* 0x256f */

    last_key = ReadKey();                      /* 아무 키나 기다림 */

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
    ClearDevice();

    hi_score = 30000L;                   /* 하이스코어 초기값 30000 */

    LoadSprites();                            /* 16X16.DAT 스프라이트 적재 */

    food_tile = 0x12;                          /* 첫 먹이 타일 번호 */

    LoadStageData();                            /* B_ST.DAT 스테이지 데이터 적재 */

    do {
        ResetGame();                        /* 게임 상태 리셋 */
        TitleScreen();                        /* 타이틀 화면 */

        DrawStage(stage);                  /* 스테이지 1 그리기 */
        DrawStatusBar();                        /* 하단 상태표시줄 */

        GameLoop();                        /* 게임 메인 루프 */

        if (lives == 0)                    /* 목숨이 다 떨어졌으면 */
            GameOverScreen();                    /* GAME OVER 화면 */

    } while (quit_flag == 0);                  /* ESC 로 종료할 때까지 반복 */

    CloseGraph();

    Halt(0);                                /* @Halt */
    return 0;
}
