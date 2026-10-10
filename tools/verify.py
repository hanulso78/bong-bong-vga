# -*- coding: utf-8 -*-
"""만든 B_ST.DAT 을 게임과 똑같은 방식으로 읽어 검사한다."""
import sys
raw = open(sys.argv[1],'rb').read().decode('latin1')
pos = 0
def ReadChar():
    global pos
    if pos >= len(raw): return chr(26)
    c = raw[pos]
    if c in '\r\n': return '\r'
    pos += 1; return c
def Readln():
    global pos
    while pos < len(raw):
        c = raw[pos]; pos += 1
        if c == '\n': return
def ReadInt():
    global pos
    while pos < len(raw) and raw[pos] in ' \t\r\n': pos += 1
    s = ''
    while pos < len(raw) and (raw[pos].isdigit() or (not s and raw[pos]=='-')):
        s += raw[pos]; pos += 1
    return int(s) if s else 0

NST = int(sys.argv[2])
tile = {}          # (stage,col,row) -> 값
food_n = {}; param = {}
for st in range(1, NST+1):
    Readln()
    for r in range(1, 12):
        for c in range(1, 21):
            ch = ReadChar()
            if c == 20: Readln()
            if ch == '.': v = 1
            else:
                try: v = ord(ch) - ord('0')
                except: v = 0
            if ch == '#': v = 0x12 + (st-1) % 10
            elif ch == 'a': v = 0x1c
            elif ch == 'b': v = 0x1d
            elif ch == 'c': v = 0x1e
            tile[(st,c,r)] = v
    p = [ReadInt() for _ in range(4)]
    n = ReadInt(); Readln()
    param[st] = p; food_n[st] = n
    for c in range(1, 21): tile[(st,c,12)] = 3

WALK=[11,8,5,2]; PLAT={11:12,8:9,5:6,2:3}
bad = 0
for st in range(1, NST+1):
    T = lambda c,r: tile[(st,c,r)]
    def stand(w,c):
        return 1 <= c <= 20 and T(c,PLAT[w]) != 1 and T(c,w) != 7
    seen={(20,11)}; stk=[(20,11)]
    while stk:
        c,w = stk.pop(); nx=[]
        if c>1 and stand(w,c-1): nx.append((c-1,w))
        if ((c<20 and w==11) or c<19) and stand(w,c+1): nx.append((c+1,w))
        if c>2 and stand(w,c-2): nx.append((c-2,w))
        if c<=17 and stand(w,c+2): nx.append((c+2,w))
        if T(c,w)==6 and w-3>=2 and stand(w-3,c): nx.append((c,w-3))
        if w+3<=11 and T(c,w+1)==6 and stand(w+3,c): nx.append((c,w+3))
        for p in nx:
            if p not in seen: seen.add(p); stk.append(p)
    fd = [(c,w) for w in WALK for c in range(1,21) if T(c,w)==0x12+(st-1)%10]
    unreach = [p for p in fd if p not in seen]
    lv = len(seen)
    ok = (len(fd)==food_n[st]) and not unreach
    if not ok: bad += 1
    print('stage %2d  먹이 %2d/%2d  닿는칸 %3d  속도 %-12s %s'
          % (st, len(fd), food_n[st], lv, ' '.join(map(str,param[st])),
             'OK' if ok else '!! 못 닿는 먹이 %s' % unreach))
print('실패 %d 개' % bad)
