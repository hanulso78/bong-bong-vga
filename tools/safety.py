# -*- coding: utf-8 -*-
"""만든 B_ST.DAT 을 다시 읽어서 "오리를 피할 수 있는가" 를 따로 검사한다.
   (mkstage.py 와 별개로 짠 검사기 - 서로 베끼지 않는다)"""
import sys

raw = open(sys.argv[1], 'rb').read().decode('latin1').replace('\r\n', '\n').split('\n')
blocks = []
i = 0
while i < len(raw):
    if not raw[i].startswith('{'):
        i += 1; continue
    blocks.append((raw[i].strip(), raw[i+1:i+12], raw[i+12].split()))
    i += 13

WALK = [11, 8, 5, 2]
PLAT = {11: 12, 8: 9, 5: 6, 2: 3}
LIMIT = 3
bad_total = 0

for st, (name, rows, par) in enumerate(blocks, 1):
    g = {r + 1: ' ' + rows[r] for r in range(11)}     # 1-기반
    g[12] = ' ' + '3' * 20
    sp = [int(x) for x in par[:4]]
    erows = [WALK[k] for k in range(4) if sp[k]]

    def stand(w, c):
        if c < 1 or c > 20: return False
        if c == 20 and w != 11: return False
        return g[PLAT[w]][c] != '.' and g[w][c] != '7'

    # 갈 수 있는 칸
    seen = {(20, 11)}; stk = [(20, 11)]
    while stk:
        c, w = stk.pop(); nx = []
        if stand(w, c-1): nx.append((c-1, w))
        if ((c < 20 and w == 11) or c < 19) and stand(w, c+1): nx.append((c+1, w))
        if c > 2 and stand(w, c-2): nx.append((c-2, w))
        if c <= 17 and stand(w, c+2): nx.append((c+2, w))
        if g[w][c] == '6' and w-3 >= 2 and stand(w-3, c): nx.append((c, w-3))
        if w+3 <= 11 and g[w+1][c] == '6' and stand(w+3, c): nx.append((c, w+3))
        for p in nx:
            if p not in seen: seen.add(p); stk.append(p)

    worst = 0; worstcell = None
    for w in erows:
        ref = set()
        for c in range(1, 21):
            if g[w][c] == '6': ref.add(c)
            if w + 1 <= 11 and g[w+1][c] == '6': ref.add(c)
        if w == 11: ref.add(20)
        for c in range(1, 21):
            if (c, w) not in seen or not stand(w, c): continue
            if c in ref: continue
            # 걷기 1 / 점프 2 로 피난처까지 몇 수
            d = {c: 0}; front = [c]; best = 99
            while front:
                nf = []
                for p in front:
                    for step, k in ((-1, 1), (1, 1), (-2, 2), (2, 2)):
                        q = p + step; nd = d[p] + k
                        if nd > LIMIT or not stand(w, q): continue
                        if nd < d.get(q, 99):
                            d[q] = nd
                            if q in ref: best = min(best, nd)
                            else: nf.append(q)
                front = nf
            if best > worst: worst, worstcell = best, (c, w)
    ok = worst <= LIMIT
    if not ok: bad_total += 1
    print('stage %2d  오리줄 %-12s 피난처까지 최악 %s수 %s  %s'
          % (st, str(erows), worst if worst < 99 else '못감',
             worstcell if worstcell else '', 'OK' if ok else '!! 갇히는 칸'))
print('위험한 판 %d 개' % bad_total)
