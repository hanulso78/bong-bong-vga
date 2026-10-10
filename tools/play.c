/* 자동 플레이 검사기 ------------------------------------------------------
   11~20 판을 "사람처럼" 움직여서 정말 깰 수 있는지 확인한다.
   - 게임 코드(bongbong_smooth.c)는 하나도 고치지 않고 키만 넣어 준다.
   - 가상 시계를 켜서 게임 안 시간 비율은 그대로 두고 빨리 돌린다.
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../tp.h"

extern int  graph_driver, graph_mode, food_tile, stage, lives, time_left, round_no;
extern int  player_col, player_row, player_sprite, move_dir;
extern int  enemy_col[9], enemy_row[9], enemy_dir[9], stage_param[88];
extern int  map_cur[5100], map_orig[5100];
extern int  food_left[22], food_total[22];
extern long score, hi_score;
extern unsigned char stage_cleared, player_dead, quit_flag, platform_on;
extern int platform_col, platform_row, platform_dir;
extern int (*stub_bot)(void);

void LoadSprites(void); void LoadExtraSprites(void); int FoodTile(int); void LoadStageData(void); void DrawStage(int);
void DrawStatusBar(void); void StartNewGame(void); void RestartStage(void);
void UpdateActors(void); void HandleDeath(void);
void LandLeft(void); void LandRight(void); void HandleKey(void);
extern void SmoothShadeHook(void); extern void SmoothRenderHook(void);
void stub_vclock(int);
void stub_dump(const char *);

#define M(c,r)  map_cur[stage*240 + (c)*12 + (r)]

static int is_walk(int r) { return r == 2 || r == 5 || r == 8 || r == 11; }

static int standable(int c, int r)
{
    if (c < 1 || c > 20) return 0;
    if (c == 20 && r != 11) return 0;
    if (!is_walk(r)) return 0;
    if (M(c, r) == 7) return 0;          /* 압정 */
    if (M(c, r + 1) == 1) return 0;      /* 발밑이 비었다 */
    return 1;
}

static int onladder(int c, int r) { return r >= 1 && r <= 11 && M(c, r) == 6; }

/* ---- 적 예측 ------------------------------------------------------------ */
static int espeed(int i)
{
    int v = stage_param[stage * 4 + ((i - 1) & 3) + 1];
    if (!v) return 0;
    if (stage > 10) { v = v * 2 / 3; if (v < 1) v = 1; }
    return v;
}

/* 적 i 가 앞으로 ms 밀리초 안에 c 칸을 밟을 수 있나 (벽에서 되돌아오는 것 포함) */
static int swept(int start, int dir, int k, int c)
{
    int p = start, d = dir, j;
    if (p == c) return 1;
    for (j = 0; j < k; j++) {
        p += d;
        if (p >= 19) { p = 19; d = -1; }
        if (p <= 1)  { p = 1;  d = 1;  }
        if (p == c) return 1;
    }
    return 0;
}

static int hot(int c, int r, int ms)
{
    int i;
    if (!is_walk(r)) return 0;            /* 사다리 중간칸에는 적이 못 온다 */
    for (i = 1; i <= 8; i++) {
        int spd = espeed(i), k;
        if (!enemy_col[i] || enemy_row[i] != r || !spd) continue;
        k = ms * spd / 1000 + 1;
        if (swept(enemy_col[i], enemy_dir[i], k, c)) return 1;
    }
    /* 움직이는 발판도 닿으면 죽는다 (0.8초에 한 칸) */
    if (platform_on && platform_row == r) {
        int k = ms / 800 + 1;
        if (swept(platform_col, platform_dir, k, c)) return 1;
    }
    return 0;
}

/* ---- 길 찾기 ------------------------------------------------------------ */
#define NN   (21 * 12)
#define IX(c,r) ((c) * 12 + (r))
enum { MV_L, MV_R, MV_JL, MV_JR, MV_U, MV_D, MV_NONE };

static int dst_[NN], prv_[NN], pmv_[NN];

static void relax(int *heapdummy) { (void)heapdummy; }

static void plan(int sc, int sr)
{
    int i, n;
    for (i = 0; i < NN; i++) { dst_[i] = 1 << 28; prv_[i] = -1; pmv_[i] = MV_NONE; }
    dst_[IX(sc, sr)] = 0;
    /* 칸이 250개뿐이라 그냥 되풀이해서 느슨하게 만든다 (Bellman-Ford) */
    for (n = 0; n < 60; n++) {
        int changed = 0, c, r;
        for (c = 1; c <= 20; c++) for (r = 1; r <= 11; r++) {
            int u = IX(c, r), d = dst_[u];
            int to[6], cost[6], k, nmv = 0, mv[6];
            if (d >= (1 << 28)) continue;
            if (!(standable(c, r) || onladder(c, r))) continue;
            if (is_walk(r)) {
                int rmax = (r == 11) ? 20 : 19;
                if (c - 1 >= 1 && standable(c - 1, r)) { to[nmv]=IX(c-1,r); cost[nmv]=75;  mv[nmv]=MV_L;  nmv++; }
                if (c + 1 <= rmax && standable(c + 1, r)) { to[nmv]=IX(c+1,r); cost[nmv]=75;  mv[nmv]=MV_R;  nmv++; }
                if (c >= 3 && standable(c - 2, r))     { to[nmv]=IX(c-2,r); cost[nmv]=250; mv[nmv]=MV_JL; nmv++; }
                if (c <= 17 && standable(c + 2, r))    { to[nmv]=IX(c+2,r); cost[nmv]=250; mv[nmv]=MV_JR; nmv++; }
                if (M(c, r) == 6 && r - 1 >= 1)        { to[nmv]=IX(c,r-1); cost[nmv]=80;  mv[nmv]=MV_U;  nmv++; }
                if (r + 1 <= 11 && M(c, r + 1) == 6)   { to[nmv]=IX(c,r+1); cost[nmv]=80;  mv[nmv]=MV_D;  nmv++; }
            } else {
                if (r - 1 >= 1 && (M(c,r-1) == 6 || standable(c, r-1)))
                    { to[nmv]=IX(c,r-1); cost[nmv]=80; mv[nmv]=MV_U; nmv++; }
                if (r + 1 <= 11 && (M(c,r+1) == 6 || standable(c, r+1)))
                    { to[nmv]=IX(c,r+1); cost[nmv]=80; mv[nmv]=MV_D; nmv++; }
            }
            for (k = 0; k < nmv; k++) {
                int tc = to[k] / 12, tr = to[k] % 12;
                if (M(tc, tr) == 0x1e) cost[k] += 4000;   /* 발판 발생 타일은 피한다 */
                if (d + cost[k] < dst_[to[k]]) {
                    dst_[to[k]] = d + cost[k];
                    prv_[to[k]] = u; pmv_[to[k]] = mv[k];
                    changed = 1;
                }
            }
        }
        if (!changed) break;
    }
    relax(0);
}

/* 목표(가장 가까운 먹이/보너스) 로 가는 첫 수 */
static int cur_target = -1;
void bot_reset_target(void) { cur_target = -1; }

static int first_move(int sc, int sr, int *tgt_out)
{
    int c, r, best = -1, bd = 1 << 28, u, mv = MV_NONE;
    /* 한 번 고른 먹이는 먹을 때까지 바꾸지 않는다 - 안 그러면 제자리에서 갈팡질팡 */
    if (cur_target >= 0) {
        int tc = cur_target / 12, tr = cur_target % 12;
        if (M(tc, tr) == food_tile && dst_[cur_target] < (1 << 28))
            { best = cur_target; bd = dst_[cur_target]; }
        else cur_target = -1;
    }
    if (best < 0)
    for (c = 1; c <= 20; c++) for (r = 1; r <= 11; r++) {
        int t = M(c, r);
        if (t != food_tile) continue;
        u = IX(c, r);
        if (dst_[u] < bd) { bd = dst_[u]; best = u; }
    }
    if (best < 0 || bd >= (1 << 28)) { cur_target = -1; return MV_NONE; }
    cur_target = best;
    if (tgt_out) *tgt_out = best;
    u = best;
    while (prv_[u] >= 0 && prv_[u] != IX(sc, sr)) u = prv_[u];
    if (prv_[u] < 0) return MV_NONE;
    mv = pmv_[u];
    return mv;
}

/* ---- "도망칠 수 있는가" 로 안전을 판단한다 --------------------------------
   사람이 하는 생각 그대로다 : 지금 칸에서 가장 가까운 사다리까지 걸리는 시간보다
   적이 나에게 닿는 시간이 넉넉히 길면 안전하다. */

static int is_refuge(int c, int r)
{
    if (M(c, r) == 6) return 1;                       /* 올라갈 사다리 */
    if (r + 1 <= 11 && M(c, r + 1) == 6) return 1;    /* 내려갈 사다리 */
    if (r == 11 && c == 20) return 1;                 /* 오리가 못 오는 구석 */
    return 0;
}

static int enemy_time(int c, int r);

/* 칸 c 에서 피난처까지 걸리는 시간(ms).  적이 막고 있는 길은 빼고 잰다.
   못 가면 큰 수 */
static int refuge_time(int c, int r)
{
    int d[21], i, n;
    if (!is_walk(r)) return 0;
    for (i = 0; i <= 20; i++) d[i] = 1 << 24;
    d[c] = 0;
    for (n = 0; n < 24; n++) {
        int ch = 0, p;
        for (p = 1; p <= 20; p++) {
            int rmax = (r == 11) ? 20 : 19;
            if (d[p] >= (1 << 24)) continue;
            if (p > 1   && standable(p-1, r) && d[p] +  90 < d[p-1]
                && enemy_time(p-1, r) > d[p] + 340) { d[p-1] = d[p] +  90; ch = 1; }
            if (p < rmax && standable(p+1, r) && d[p] +  90 < d[p+1]
                && enemy_time(p+1, r) > d[p] + 340) { d[p+1] = d[p] +  90; ch = 1; }
            if (p > 2   && standable(p-2, r) && d[p] + 250 < d[p-2]
                && enemy_time(p-2, r) > d[p] + 500
                && enemy_time(p-1, r) > d[p] + 300) { d[p-2] = d[p] + 250; ch = 1; }
            if (p <= 17 && standable(p+2, r) && d[p] + 250 < d[p+2]
                && enemy_time(p+2, r) > d[p] + 500
                && enemy_time(p+1, r) > d[p] + 300) { d[p+2] = d[p] + 250; ch = 1; }
        }
        if (!ch) break;
    }
    {
        int best = 1 << 24, p;
        for (p = 1; p <= 20; p++)
            if (d[p] < best && is_refuge(p, r)) best = d[p];
        return best;
    }
}

/* 적(또는 발판)이 칸 c 에 닿기까지 걸리는 시간(ms).  아무도 못 오면 큰 수 */
static int enemy_time(int c, int r)
{
    int best = 1 << 24, i;
    if (!is_walk(r)) return best;
    for (i = 1; i <= 8; i++) {
        int spd = espeed(i), p, dd, j, step;
        if (!enemy_col[i] || enemy_row[i] != r || !spd) continue;
        p = enemy_col[i]; dd = enemy_dir[i]; step = 1000 / spd;
        if (p == c) return 0;
        for (j = 1; j <= 40; j++) {
            p += dd;
            if (p >= 19) { p = 19; dd = -1; }
            if (p <= 1)  { p = 1;  dd = 1;  }
            /* 1초마다 시계를 되돌리는 원작 버릇 때문에 적이 가끔 한 칸을
               공짜로 더 간다.  한 칸 빨리 온다고 보고 여유를 둔다. */
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

/* 그 칸에 서 있어도 되는가 - 적이 오기 전에 사다리로 뺄 수 있으면 된다 */
/* 그 층에 적이 딱 한 마리이면 반대쪽으로 달아나면 된다 (원본 1~10판) */
static int runaway_ok(int c, int r)
{
    int n = 0, side = 0, i, k, room = 0;
    for (i = 1; i <= 8; i++) {
        if (!enemy_col[i] || enemy_row[i] != r || !espeed(i)) continue;
        if (enemy_col[i] == c) return 0;
        n++;
        side = (enemy_col[i] < c) ? 1 : -1;
    }
    if (platform_on && platform_row == r) return 0;
    if (n == 0) return 1;
    if (n != 1) return 0;
    for (k = 1; k <= 5; k++) { if (!standable(c + side * k, r)) break; room++; }
    return room >= 3;
}

/* 가장 가까운 적까지 몇 칸인가 */
static int edist(int c, int r)
{
    int best = 99, i, d;
    if (!is_walk(r)) return best;
    for (i = 1; i <= 8; i++) {
        if (!enemy_col[i] || enemy_row[i] != r || !espeed(i)) continue;
        d = enemy_col[i] - c; if (d < 0) d = -d;
        if (d < best) best = d;
    }
    if (platform_on && platform_row == r) {
        d = platform_col - c; if (d < 0) d = -d;
        if (d < best) best = d;
    }
    return best;
}

static int safe_ex(int c, int r, int extra)
{
    int rt, et;
    if (!is_walk(r)) return 1;
    et = enemy_time(c, r);
    if (edist(c, r) > 3 && et > 400 + extra) return 1;   /* 충분히 멀다 */
    rt = refuge_time(c, r);
    if (rt <= 900 && et > rt + 800 + extra) return 1;
    if (et > 600 + extra && runaway_ok(c, r)) return 1;
    return 0;
}

static int safe_cell(int c, int r) { return safe_ex(c, r, 0); }

/* 사다리로 피한 뒤 곧바로 그 층에 다시 내려서지 않게 막는다 */
#define HIDE_MS 300
#define TURN_MS 250
static int  hide_row; static long hide_until;
static int  last_v = MV_NONE; static long last_v_at;
static int land_ok(int rr) { return rr != hide_row || TicksMs() >= hide_until; }
static int v_ok(int mv)
{
    if (mv != MV_U && mv != MV_D) return 1;
    if (last_v == MV_NONE || last_v == mv) return 1;
    return TicksMs() - last_v_at >= TURN_MS;
}

static int move_safe(int mv, int c, int r);

/* 내려서서 바로 무엇이든 할 수 있는가 (내려서 봐야 소용없으면 안 내려선다) */
static int progress_ok(int c, int r)
{
    if (M(c, r) == food_tile) return 1;
    if (move_safe(MV_L,  c, r)) return 1;
    if (move_safe(MV_R,  c, r)) return 1;
    if (move_safe(MV_JL, c, r)) return 1;
    if (move_safe(MV_JR, c, r)) return 1;
    return 0;
}

/* 이 수를 지금 해도 안전한가.
   움직이는 동안(약 40ms) 은 아직 원래 칸에 있으므로 원래 칸도 같이 본다. */
static int move_safe(int mv, int c, int r)
{
    if (is_walk(r) && enemy_time(c, r) <= 320) return 0;   /* 움직이다 밟힌다 */
    switch (mv) {
    case MV_L:  return enemy_time(c-1, r) > 320 && safe_cell(c-1, r);
    case MV_R:  return enemy_time(c+1, r) > 320 && safe_cell(c+1, r);
    case MV_JL: return enemy_time(c-1, r) > 400 && enemy_time(c-2, r) > 500
                       && safe_cell(c-2, r);
    case MV_JR: return enemy_time(c+1, r) > 400 && enemy_time(c+2, r) > 500
                       && safe_cell(c+2, r);
    /* 사다리에서 층으로 내려/올라설 때는 넉넉히 본다 (오르락내리락 방지) */
    case MV_U:  return !is_walk(r-1) || (land_ok(r-1) &&
                       enemy_time(c, r-1) > 700 && safe_ex(c, r-1, 400) &&
                       progress_ok(c, r-1));
    case MV_D:  return !is_walk(r+1) || (land_ok(r+1) &&
                       enemy_time(c, r+1) > 700 && safe_ex(c, r+1, 400) &&
                       progress_ok(c, r+1));
    }
    return 0;
}

static int key_for(int mv, int c, int r)
{
    switch (mv) {
    case MV_L:  return 0x4B00;
    case MV_R:  return 0x4D00;
    case MV_JL: return (player_sprite == 0x0a) ? 0x3920 : 0x4B00;   /* 먼저 왼쪽을 본다 */
    case MV_JR: return (player_sprite == 0x0e) ? 0x3920 : 0x4D00;
    case MV_U:  return 0x4800;
    case MV_D:  return 0x5000;
    }
    (void)c; (void)r;
    return 0;
}

/* 도망 : 사다리 쪽으로 한 칸씩, 사다리에 닿으면 층을 벗어난다 */
static int flee_key(int c, int r)
{
    int rmax = (r == 11) ? 20 : 19, k, away;
    int bestk = 0, bestrt = 1 << 24, bestet = 0;
    struct { int dc, need, mid; } opt[4] = {
        { -1, 320, 0 }, { 1, 320, 0 }, { -2, 500, -1 }, { 2, 500, 1 } };

    if (!is_walk(r)) return 0;                     /* 이미 안전한 곳 */

    if (is_refuge(c, r)) {                         /* 사다리 위 - 바로 벗어난다 */
        if (M(c, r) == 6 && r - 1 >= 1) return 0x4800;
        if (r + 1 <= 11 && M(c, r + 1) == 6) return 0x5000;
        if (r == 11 && c == 20) return 0;          /* 구석은 그대로 있으면 된다 */
    }
    /* 그 층에 오리가 한 마리뿐이면 반대쪽으로 한 칸 물러나는 것은
       "내가 먼저 닿기만 하면" 된다 (사람 0.09초/칸, 오리 0.2초/칸) */
    {
        int i, n = 0, nd = 99;
        away = 0;
        for (i = 1; i <= 8; i++) {
            if (!enemy_col[i] || enemy_row[i] != r || !espeed(i)) continue;
            n++;
            if (abs(enemy_col[i] - c) < nd) {
                nd = abs(enemy_col[i] - c);
                away = (enemy_col[i] > c) ? -1 : 1;
            }
        }
        if (n != 1 || (platform_on && platform_row == r)) away = 0;
    }

    for (k = 0; k < 4; k++) {
        int nc = c + opt[k].dc, rt, et, req;
        if (nc < 1 || nc > rmax) continue;
        if (abs(opt[k].dc) == 2 && (c < 3 || c > 17)) continue;
        if (!standable(nc, r)) continue;
        if (opt[k].mid && enemy_time(c + opt[k].mid, r) < 400) continue;
        et = enemy_time(nc, r);
        req = opt[k].need + (is_refuge(nc, r) ? 0 : 200);
        if (away && opt[k].dc == away) req = 150;
        if (et < req) continue;
        rt = refuge_time(nc, r);
        if (rt < bestrt || (rt == bestrt && et > bestet)) {
            bestrt = rt; bestet = et;
            if (abs(opt[k].dc) == 2)          /* 두 칸은 점프 - 먼저 그쪽을 본다 */
                bestk = (opt[k].dc < 0) ? ((player_sprite == 0x0a) ? 0x3920 : 0x4B00)
                                        : ((player_sprite == 0x0e) ? 0x3920 : 0x4D00);
            else
                bestk = (opt[k].dc < 0) ? 0x4B00 : 0x4D00;
        }
    }
    return bestk;
}

static long bot_calls = 0, bot_waits = 0;

/* 최근에 무엇을 했는지 남겨 둔다 (죽으면 되돌아본다) */
#define LOGN 16
static struct { long t; int c, r, key, sp, mv, st; int e[9]; } lg[LOGN];
static int lgi = 0;
static void logit(int key)
{
    int i, pv = (lgi + LOGN - 1) % LOGN;
    if (lg[pv].t && lg[pv].key == key && lg[pv].c == player_col
        && lg[pv].r == player_row) return;          /* 같은 상황은 한 번만 */
    lg[lgi].t = TicksMs(); lg[lgi].c = player_col; lg[lgi].r = player_row;
    lg[lgi].key = key; lg[lgi].sp = player_sprite; lg[lgi].mv = -1; lg[lgi].st = -1;
    for (i = 1; i <= 8; i++) lg[lgi].e[i] = enemy_col[i] ? enemy_col[i] : -1;
    lgi = (lgi + 1) % LOGN;
}
static void dumplog(void)
{
    int k, i;
    for (k = 0; k < LOGN; k++) {
        int j = (lgi + k) % LOGN;
        if (!lg[j].t) continue;
        printf("      %6ld ms  칸(%2d,%2d) 자세%02x 키%04x 수%d 설%d 적:",
               lg[j].t, lg[j].c, lg[j].r, lg[j].sp, lg[j].key, lg[j].mv, lg[j].st);
        for (i = 1; i <= 8; i++) if (lg[j].e[i] > 0) printf(" %d:%d", i, lg[j].e[i]);
        printf("\n");
    }
}

static int bot_key(void)
{
    int c = player_col, r = player_row, mv, k;

    if (move_dir || stage_cleared || player_dead) return 0;
    bot_calls++;

    /* 사다리 중간칸이면 적이 못 온다 - 길을 다시 세운다 */
    plan(c, r);
    mv = first_move(c, r, 0);

    /* 지금 칸이 위험하면 무조건 도망부터 */
    if (!safe_cell(c, r)) {
        k = flee_key(c, r);
        if (k) { logit(k);
                 if (k == 0x4800 || k == 0x5000) {
                     hide_row = r; hide_until = TicksMs() + HIDE_MS;
                     last_v = (k == 0x4800) ? MV_U : MV_D; last_v_at = TicksMs(); }
                 return k; }
    }

    if (mv != MV_NONE && v_ok(mv) && move_safe(mv, c, r)) {
        int kk = key_for(mv, c, r);
        if (mv == MV_U || mv == MV_D) { last_v = mv; last_v_at = TicksMs(); }
        logit(kk);
        lg[(lgi + LOGN - 1) % LOGN].mv = mv;
        lg[(lgi + LOGN - 1) % LOGN].st = standable(c+1, r) * 2 + standable(c-1, r);
        return kk;
    }

    /* 못 가면 : 위험하면 도망, 아니면 기다린다 */
    /* 갈 수 없으면 사다리로 물러나 층을 벗어나고 적이 지나가길 기다린다 */
    if (is_walk(r)) {
        k = flee_key(c, r);
        if (k) { logit(k);
                 if (k == 0x4800 || k == 0x5000) {
                     hide_row = r; hide_until = TicksMs() + HIDE_MS;
                     last_v = (k == 0x4800) ? MV_U : MV_D; last_v_at = TicksMs(); }
                 return k; }
    }

    bot_waits++;
    if (is_walk(r) && !safe_cell(c, r)) {        /* 위험한데도 못 움직인다 */
        logit(0xFFFF);
        lg[(lgi + LOGN - 1) % LOGN].mv = refuge_time(c, r);
        lg[(lgi + LOGN - 1) % LOGN].st = enemy_time(c, r);
    }
    return 0;
}

/* ---- 한 판 돌려 보기 ---------------------------------------------------- */
static int play_stage(int st, int maxlife, int verbose)
{
    long guard = 0;
    stage = st; round_no = st;
    food_tile = FoodTile(st);
    lives = maxlife; score = 0; player_dead = 0; stage_cleared = 0;
    move_dir = 0; quit_flag = 0;
    RestartStage();
    bot_reset_target();
    hide_row = 0; hide_until = 0; last_v = MV_NONE; last_v_at = 0;

    while (guard++ < 4000000L) {
        FrameSync(120);
        if (player_dead && lives && !stage_cleared) {
            if (verbose) {
                int i; char why[160]; int n = 0;
                n += sprintf(why + n, "죽음: 칸(%d,%d) 타일 %02x 시간 %d",
                             player_col, player_row,
                             M(player_col, player_row), time_left);
                if (M(player_col, player_row) == 7) n += sprintf(why + n, " [압정]");
                if (M(player_col, player_row + 1) == 1) n += sprintf(why + n, " [추락]");
                for (i = 1; i <= 8; i++)
                    if (enemy_col[i])
                        n += sprintf(why + n, " 적%d(%d줄%d칸%+d)",
                                     i, enemy_row[i], enemy_col[i], enemy_dir[i]);
                if (platform_on)
                    n += sprintf(why + n, " 발판(%d줄%d칸)", platform_row, platform_col);
                if (time_left <= 0) n += sprintf(why + n, " [시간초과]");
                printf("   %s  남은먹이 %d\n", why, food_left[stage]); dumplog();
            }
            HandleDeath();
        }
        if (stage_cleared) return 1;
        if (!lives) return 0;
        if (move_dir == -1)      LandLeft();
        else if (move_dir == 1)  LandRight();
        else if (KeyPressed())   HandleKey();
        UpdateActors();
        SmoothRenderHook();
        if (verbose && (guard % 2000) == 0)
            printf("   ... %ld 프레임  칸(%d,%d) 남은먹이 %d 시간 %d 목숨 %d\n",
                   guard, player_col, player_row, food_left[stage], time_left, lives);
    }
    return -1;
}

int main(int argc, char **argv)
{
    int s, from = 11, to = 20, tries = 3, ok_all = 1;
    if (argc > 1) from = atoi(argv[1]);
    if (argc > 2) to   = atoi(argv[2]);

    graph_driver = 1; graph_mode = 1;
    InitGraph(&graph_driver, &graph_mode, "C:");
    LoadTextFonts("HSMALL.FNT", "ESMALL.FNT");
    GfxOpenPages();
    ClearDevice(); hi_score = 30000L;
    LoadSprites(); LoadExtraSprites(); SmoothShadeHook();
    food_tile = 0x12; LoadStageData();
    StartNewGame();
    stub_bot = bot_key;
    stub_vclock(1);

    for (s = from; s <= to; s++) {
        int t, got = 0;
        for (t = 0; t < tries && !got; t++) {
            long w0 = bot_waits;
            int  r = play_stage(s, 3, 1);
            if (r == 1) {
                printf("판 %2d  깼다 (남은 시간 %d, 목숨 %d, 멈춰선 횟수 %ld)\n",
                       s, time_left, lives, bot_waits - w0);
                got = 1;
            } else if (r == 0) {
                printf("판 %2d  목숨 3개를 다 썼다 (시도 %d)\n", s, t + 1);
            } else {
                printf("판 %2d  시간 초과로 검사를 멈췄다 (시도 %d)\n", s, t + 1);
            }
        }
        if (!got) { ok_all = 0; }
    }
    printf(ok_all ? "\n전부 클리어\n" : "\n못 깬 판이 있다\n");
    return ok_all ? 0 : 1;
}
