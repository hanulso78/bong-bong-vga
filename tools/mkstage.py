# -*- coding: utf-8 -*-
"""B_ST.DAT 스테이지 11~20 만들기.  원본 1~10 의 규칙을 따르되,
   "한 층에 오리 두 마리" 를 고려해서 사람이 실제로 깰 수 있게 만든다.

  줄 1     사다리칸(위에 층이 없으므로 전부 '.')
  줄 2,5,8,11   걸어다니는 줄 (아이템/사다리)
  줄 3,6,9      발판 줄 ( 2 왼끝 / 3 오른끝 / 4 가운데 / 5 한 칸 / 6 사다리 )
  줄 4,7,10     사다리칸
  줄 12         코드가 항상 바닥(3)으로 채운다

[게임에서 확인한 움직임]
  - 걷기      : 한 칸.  단, 보고 있지 않은 쪽으로는 먼저 몸을 돌린다.
  - 점프      : 두 칸 건너뛰기(가운데 칸은 건너뛴다).  세 칸 이상은 못 넘는다.
                => 발판 구멍은 폭 1 만 건널 수 있다.  폭 2 부터는 못 넘는다.
  - 사다리    : 위/아래.  사다리 중간칸(줄 4,7,10)에는 오리가 못 온다.
  - 오리      : 자기 층의 1~19 칸을 왕복한다.  구멍/압정/아이템을 무시하고 걷는다.
                같은 칸에 겹치면 죽는다.  점프로 뛰어넘을 수도 없다(지나는 두 칸
                을 검사한다).  => 오리를 피하는 방법은 "사다리로 층을 벗어나는 것"
                뿐이다.  맨 아랫층 20칸은 오리가 못 오므로 그 자체가 피난처다.

[그래서 지킨 규칙]
  1. 오리가 나오는 줄에서는 어느 칸에 서 있든 3수 안에 피난처(사다리/20칸)에
     닿을 수 있어야 한다.
  2. 오리가 나오는 줄의 발판은 폭 1 구멍만 둬서 줄이 토막나지 않게 한다.
  3. 압정은 놓은 뒤 1번 조건을 다시 검사해서, 도망길을 막으면 물린다.
  4. 오리 속도는 원본보다 낮춘다(두 마리씩이므로 게임 쪽에서 다시 2/3 로 준다).
"""
import random

WALK = [11, 8, 5, 2]          # 아래층 -> 위층
PLAT = {11: 12, 8: 9, 5: 6, 2: 3}
LGROUP = (11, 8, 5)           # 사다리 무리 : 아래 줄 w 에서 위 줄 w-3 으로

ESCAPE_MAX = 2                # 피난처까지 허용하는 수(걷기 1, 점프 2)


def new_grid():
    return [['.'] * 21 for _ in range(13)]     # [row][col], 1..20 사용


def make_platform(rng, safe):
    """cols 1..19 중 구멍을 뚫은 solid 마스크.
       safe=True(오리가 다니는 줄) 면 폭 1 구멍만 뚫어 줄이 토막나지 않게 한다."""
    solid = [False] * 21
    for c in range(1, 20):
        solid[c] = True
    used = []

    def far(c, w):
        return all(abs(c - u) > 2 and abs(c + w - 1 - u) > 2 for u in used)

    if not safe:
        for _ in range(rng.randint(1, 2)):        # 못 건너는 넓은 구멍
            for _try in range(40):
                w = rng.choice([2, 2, 3])
                c = rng.randint(3, 17 - w)
                if far(c, w):
                    used += [c, c + w - 1]
                    for k in range(c, c + w):
                        solid[k] = False
                    break

    for _ in range(rng.randint(3, 5)):            # 점프로 건너는 한 칸 구멍
        for _try in range(40):
            c = rng.randint(2, 18)
            if solid[c] and solid[c-1] and solid[c+1] and far(c, 1):
                used.append(c)
                solid[c] = False
                break
    return solid


def lay_tiles(g, row, solid):
    c = 1
    while c <= 19:
        if not solid[c]:
            c += 1; continue
        e = c
        while e + 1 <= 19 and solid[e + 1]:
            e += 1
        if e == c:
            g[row][c] = '5'
        else:
            for k in range(c, e + 1):
                g[row][k] = '4'
            if c != 1:  g[row][c] = '2'
            if e != 19: g[row][e] = '3'
        c = e + 1


def can_stand(g, solid, w, c):
    if c < 1 or c > 20: return False
    if c == 20 and w != 11: return False
    if not solid[PLAT[w]][c]: return False
    return g[w][c] != '7'


def reachable(g, solid):
    """플레이어가 (20,11) 에서 갈 수 있는 (col,row) 집합"""
    start = (20, 11)
    seen = set([start]); stack = [start]
    while stack:
        c, w = stack.pop()
        nxt = []
        if c > 1 and can_stand(g, solid, w, c - 1): nxt.append((c - 1, w))
        if ((c < 20 and w == 11) or c < 19) and can_stand(g, solid, w, c + 1):
            nxt.append((c + 1, w))
        if c > 2 and can_stand(g, solid, w, c - 2): nxt.append((c - 2, w))      # 왼쪽 점프
        if c <= 17 and can_stand(g, solid, w, c + 2): nxt.append((c + 2, w))    # 오른쪽 점프
        if g[w][c] == '6' and w - 3 >= 2 and can_stand(g, solid, w - 3, c):
            nxt.append((c, w - 3))
        if w + 3 <= 11 and g[w+1][c] == '6' and can_stand(g, solid, w + 3, c):
            nxt.append((c, w + 3))
        for p in nxt:
            if p not in seen:
                seen.add(p); stack.append(p)
    return seen


def refuges(g, w):
    """줄 w 에서 오리를 피해 빠져나갈 수 있는 칸들"""
    r = set()
    for c in range(1, 21):
        if g[w][c] == '6':                       # 위로 오르는 사다리
            r.add(c)
        if w + 1 <= 11 and g[w+1][c] == '6':     # 아래로 내려가는 사다리
            r.add(c)
    if w == 11:
        r.add(20)                                # 오리는 19칸까지만 온다
    return r


def escape_cost(g, solid, w, c, ref):
    """칸 c 에서 피난처까지 몇 수인가 (걷기 1, 점프 2).  못 가면 큰 수."""
    if c in ref: return 0
    best = 99
    dist = {c: 0}
    front = [c]
    while front:
        nf = []
        for p in front:
            d = dist[p]
            if d >= ESCAPE_MAX: continue
            for step, cost in ((-1, 1), (1, 1), (-2, 2), (2, 2)):
                q = p + step
                nd = d + cost
                if nd > ESCAPE_MAX: continue
                if not can_stand(g, solid, w, q): continue
                if q > 19 and w != 11: continue
                if nd < dist.get(q, 99):
                    dist[q] = nd
                    if q in ref: best = min(best, nd)
                    else: nf.append(q)
        front = nf
    return best


def row_safe(g, solid, w, reach):
    """오리가 나오는 줄 w 가 사람이 살아남을 만한가"""
    ref = refuges(g, w)
    if not ref: return False
    for c in range(1, 21):
        if (c, w) not in reach: continue
        if not can_stand(g, solid, w, c): continue
        if escape_cost(g, solid, w, c, ref) > ESCAPE_MAX:
            return False
    return True


def uncovered(g, solid, w, reach):
    ref = refuges(g, w)
    bad = []
    for c in range(1, 21):
        if (c, w) not in reach: continue
        if not can_stand(g, solid, w, c): continue
        if not ref or escape_cost(g, solid, w, c, ref) > ESCAPE_MAX:
            bad.append(c)
    return bad


def put_ladder(g, w, c):
    g[w][c] = '6'; g[w-1][c] = '6'; g[w-2][c] = '6'


def build(rng, enemy_rows):
    g = new_grid()
    solid = {}
    for w in (8, 5, 2):
        pr = PLAT[w]
        solid[pr] = make_platform(rng, w in enemy_rows)
        lay_tiles(g, pr, solid[pr])
    solid[12] = [True] * 21
    for c in range(1, 21):
        g[12][c] = '3'

    # ---- 사다리 놓기 -------------------------------------------------------
    # 무리 w 는 줄 w(올라감) 와 줄 w-3(내려옴) 에 피난처를 만들어 준다.
    cand = {}
    for w in LGROUP:
        hi = 16 if w == 11 else 20    # 맨 아래층 사다리는 출발 자리에서 멀리
        cand[w] = [c for c in range(2, hi)
                   if solid[PLAT[w]][c] and solid[w-2][c]]
        if len(cand[w]) < 2:
            return None

    chosen = {w: [] for w in LGROUP}

    def place(w, c):
        if c not in cand[w]: return False
        if any(abs(c - p) < 3 for p in chosen[w]): return False
        if len(chosen[w]) >= 6: return False
        chosen[w].append(c); put_ladder(g, w, c)
        return True

    for w in LGROUP:                            # 기본 2~3 개
        pool = cand[w][:]; rng.shuffle(pool)
        want = rng.randint(2, 3)
        for c in pool:
            if len(chosen[w]) >= want: break
            place(w, c)
        if not chosen[w]: return None

    # 오리 줄의 빈 구석을 메울 때까지 사다리를 더 놓는다
    for _round in range(16):
        reach = reachable(g, solid)
        need = {}
        for w in enemy_rows:
            bad = uncovered(g, solid, w, reach)
            if bad: need[w] = bad
        if not need: break
        best = None
        for w, bad in need.items():
            for grp in (w, w + 3):              # 줄 w 에 피난처를 주는 두 무리
                if grp not in LGROUP: continue
                for c in cand[grp]:
                    if any(abs(c - p) < 3 for p in chosen[grp]): continue
                    if len(chosen[grp]) >= 6: continue
                    gain = sum(1 for b in bad if abs(b - c) <= ESCAPE_MAX)
                    if gain and (best is None or gain > best[0]):
                        best = (gain, grp, c)
        if best is None: return None
        place(best[1], best[2])
    else:
        return None

    reach = reachable(g, solid)
    for w in enemy_rows:
        if not row_safe(g, solid, w, reach):
            return None
    return g, solid, chosen


ITEM = '#abc'


def place_items(g, solid, rng, n_food, nospike, enemy_rows):
    cells = []
    for w in WALK:
        for c in range(1, 21):
            if w == 11 and c == 20: continue          # 플레이어 출발 자리
            if g[w][c] != '.': continue
            if not solid[PLAT[w]][c]: continue
            cells.append((c, w))
    rng.shuffle(cells)
    reach = reachable(g, solid)
    good = [p for p in cells if p in reach]
    if len(good) < n_food + 10:
        return False
    for i in range(n_food):
        c, w = good[i]; g[w][c] = '#'
    rest = good[n_food:]
    for i in range(rng.randint(2, 3)):                # 보너스 항아리
        c, w = rest.pop(); g[w][c] = rng.choice('ab')
    for _ in range(rng.randint(1, 2)):                # 움직이는 발판 발생 타일
        for i in range(len(rest)):                    # 오리 줄에는 두지 않는다
            c, w = rest.pop()
            if w in enemy_rows: continue
            g[w][c] = 'c'; break

    for i in range(rng.randint(4, 7)):                # 즉사 기둥
        for _ in range(len(rest)):
            c, w = rest.pop()
            if not spike_ok(g, solid, c, w, nospike, enemy_rows): continue
            g[w][c] = '7'
            if w in enemy_rows and not row_safe(g, solid, w, reachable(g, solid)):
                g[w][c] = '.'                         # 도망길을 막으면 물린다
                continue
            break
    return True


def spike_ok(g, solid, c, w, nospike, enemy_rows):
    # 출발 자리 바로 옆(맨 아래층 오른쪽 끝)에는 두지 않는다
    if w == 11 and c >= 17: return False
    # 사다리를 타고 올라가 서는 칸에는 두지 않는다 (피할 수 없이 죽는다)
    if (c, w) in nospike: return False
    # 옆칸에 이미 압정이 있으면 두지 않는다 - 두 개가 붙으면
    # 점프(두 칸 건너뛰기)로도 넘을 수 없다
    if c > 1 and g[w][c-1] == '7': return False
    if c < 20 and g[w][c+1] == '7': return False
    # 피난처 바로 옆은 비워 둔다 - 도망쳐 들어갈 입구를 막으면 안 된다
    if w in enemy_rows:
        ref = refuges(g, w)
        if any(abs(c - r) <= 1 for r in ref): return False
    return True


def break_runs(g, solid, rng, nospike, enemy_rows):
    """아이템이 셋 이상 줄줄이 붙어 있으면 가운데를 즉사 기둥으로 바꿔 끊는다.
       (점프는 두 칸 건너뛰기라 기둥 하나는 넘어갈 수 있다)"""
    for w in WALK:
        c = 1
        while c <= 20:
            if g[w][c] not in ITEM:
                c += 1; continue
            e = c
            while e + 1 <= 20 and g[w][e+1] in ITEM:
                e += 1
            L = e - c + 1
            if L >= 3:
                spots = [c + L // 2] if L < 6 else [c + L // 3, c + 2 * L // 3]
                for p in spots:
                    # 가운데부터 찾고, 안 되면 줄 안 어디든 (셋 연속만 끊으면 된다)
                    order = [p, p - 1, p + 1] + list(range(c, e + 1))
                    for q in order:
                        if not (c <= q <= e): continue
                        if g[w][q] not in ITEM: continue
                        if not spike_ok(g, solid, q, w, nospike, enemy_rows): continue
                        keep = g[w][q]
                        g[w][q] = '7'
                        if w in enemy_rows and not row_safe(g, solid, w,
                                                           reachable(g, solid)):
                            g[w][q] = keep       # 도망길을 막으면 물린다
                            continue
                        break
            c = e + 1


def tour_cost(g, solid, foods):
    """(20,11) 에서 출발해 가까운 먹이부터 주워 먹을 때 드는 수 - 시간 가늠용"""
    nodes = [(20, 11)] + list(foods)
    def bfs(src):
        d = {src: 0}; st = [src]
        while st:
            c, w = st.pop(); dd = d[(c, w)]
            nx = []
            if c > 1 and can_stand(g, solid, w, c-1): nx.append(((c-1, w), 1))
            if ((c < 20 and w == 11) or c < 19) and can_stand(g, solid, w, c+1):
                nx.append(((c+1, w), 1))
            if c > 2 and can_stand(g, solid, w, c-2): nx.append(((c-2, w), 2))
            if c <= 17 and can_stand(g, solid, w, c+2): nx.append(((c+2, w), 2))
            if g[w][c] == '6' and w-3 >= 2 and can_stand(g, solid, w-3, c):
                nx.append(((c, w-3), 3))
            if w+3 <= 11 and g[w+1][c] == '6' and can_stand(g, solid, w+3, c):
                nx.append(((c, w+3), 3))
            for p, k in nx:
                if dd + k < d.get(p, 9999):
                    d[p] = dd + k; st.append(p)
        return d
    dmap = {n: bfs(n) for n in nodes}
    cur = (20, 11); left = set(foods); total = 0
    while left:
        nxt = min(left, key=lambda p: dmap[cur].get(p, 9999))
        total += dmap[cur].get(nxt, 9999)
        cur = nxt; left.discard(nxt)
    return total


def make_stage(seed, n_food, enemy_rows):
    rng = random.Random(seed)
    for attempt in range(6000):
        r = build(rng, enemy_rows)
        if not r: continue
        g, solid, lad = r
        nospike = set()
        for w, cols in lad.items():
            for c in cols:
                nospike.add((c, w - 3))       # 사다리를 타고 올라가 서는 칸
                nospike.add((c, w))           # 사다리 밑칸 (내려와 서는 칸)
        if not place_items(g, solid, rng, n_food, nospike, enemy_rows): continue
        break_runs(g, solid, rng, nospike, enemy_rows)
        # 먹이가 전부 닿는지 다시 확인 (즉사 기둥이 길을 막았을 수도 있다)
        reach = reachable(g, solid)
        foods = [(c, w) for w in WALK for c in range(1, 21) if g[w][c] == '#']
        if len(foods) < n_food - 3: continue
        if not all(p in reach for p in foods): continue
        if len(reach) < 45: continue        # 너무 좁은 판은 버린다
        if any(not row_safe(g, solid, w, reach) for w in enemy_rows): continue
        # 100초 안에 다 주울 수 있는가 (한 수 약 0.15초, 오리 피하는 여유 절반)
        if tour_cost(g, solid, foods) > 260: continue
        return g
    return None


# 오리 속도.  줄 차례는 11, 8, 5, 2.  0 이면 그 줄에는 오리가 없다.
# 11판부터는 한 줄에 두 마리가 나오고 게임이 다시 2/3 로 늦추므로
# 실제 속도는 4->2, 5->3, 6->4, 7->4 칸/초 (사람은 약 7칸/초).
SPEED = {
    11: (4, 0, 4, 0), 12: (4, 0, 5, 0), 13: (5, 0, 4, 4), 14: (5, 0, 5, 0),
    15: (5, 4, 0, 5), 16: (5, 0, 5, 4), 17: (6, 0, 5, 0), 18: (6, 5, 0, 5),
    19: (6, 0, 6, 5), 20: (7, 5, 0, 6),
}
FOOD = {11: 13, 12: 14, 13: 14, 14: 15, 15: 14, 16: 15, 17: 15, 18: 16, 19: 15, 20: 16}

if __name__ == '__main__':
    out = []
    for st in range(11, 21):
        sp = SPEED[st]
        erows = [WALK[i] for i in range(4) if sp[i]]
        g = None
        for s in range(40):
            g = make_stage(7000 + st * 97 + s, FOOD[st], erows)
            if g: break
        if g is None:
            raise SystemExit("스테이지 %d 만들기 실패" % st)
        out.append('{ stage %d }' % st)
        for r in range(1, 12):
            out.append(''.join(g[r][1:21]))
        n = sum(''.join(g[r][1:21]).count('#') for r in range(1, 12))
        out.append('%d %d %d %d %d' % (sp + (n,)))
        print('stage %2d  오리줄 %s  먹이 %d' % (st, erows, n))
    open('/tmp/gen/new_stages.txt', 'w').write('\r\n'.join(out) + '\r\n')
    print('총 %d 줄' % len(out))
