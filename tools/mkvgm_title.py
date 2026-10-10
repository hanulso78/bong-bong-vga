# -*- coding: utf-8 -*-
"""첫 화면 곡 (BB00.VGM) - 애드립(OPL2) 연주.
   가락·길이·쉼은 원본 비프(melody[11])와 한 음도 다르지 않다.  그 위에
   베이스 / 낮게 깔리는 화음 / 가벼운 드럼을 더해 '연주'로 만들었다.
   약 5초, 되풀이 없음."""
import struct
OP1=[0x00,0x01,0x02,0x08,0x09,0x0A,0x10,0x11,0x12]
OP2=[0x03,0x04,0x05,0x0B,0x0C,0x0D,0x13,0x14,0x15]
SR=44100

def fnb(hz):
    for b in range(8):
        n=int(round(hz*(2**(20-b))/49716.0))
        if n<1024 and (n>=300 or b==7): return n,b
    return 1023,7
def hz(m): return 440.0*(2.0**((m-69)/12.0))
N={'c':0,'d':2,'e':4,'f':5,'g':7,'a':9,'b':11}
def p(s):
    k=N[s[0]]; i=1
    while i<len(s) and s[i] in '+-#': k+=1 if s[i] in '+#' else -1; i+=1
    return (int(s[i:])+1)*12+k

# ---- 원본 TitleScreen 의 melody[11] : (주파수, 길이 ms), 음 사이 30ms 쉼 ----
MEL=[(523,400),(659,400),(784,200),(659,200),(523,400),
     (880,200),(784,200),(880,200),(988,200),(1047,400),(523,200)]
GAP=30
ON=[]                                   # (시작 ms, 길이 ms, 주파수)
t=0
for i,(f,ms) in enumerate(MEL):
    if i: t+=GAP
    ON.append((t,ms,f)); t+=ms
END=t                                   # 3300ms

# ---- 가락 밑에 깔 화음 (시작 ms, 근음, 3화음) ----
CH=[(0,      'c2', ['c4','e4','g4']),
    (ON[2][0],'g2', ['b3','d4','g4']),
    (ON[4][0],'c2', ['c4','e4','g4']),
    (ON[5][0],'f2', ['a3','c4','f4']),
    (ON[7][0],'g2', ['b3','d4','g4']),
    (ON[9][0],'c2', ['c4','e4','g4'])]

# ---- 소리 ----
# 리드 : 되먹임을 조금 준 반송파 - 비프와 닮았지만 모서리가 덜 날카롭다
P_LEAD=((0x21,0x24,0xF1,0x15,0x00),(0x21,0x00,0xF1,0x15,0x00),0x06)
P_SUB =((0x21,0x1A,0xF2,0x26,0x00),(0x21,0x1C,0xF2,0x26,0x00),0x08)  # 한 옥타브 아래
P_PAD =((0x21,0x4F,0x63,0x25,0x00),(0x21,0x22,0x63,0x25,0x00),0x09)  # 낮게 깔리는 화음
P_BASS=((0x20,0x1D,0xF4,0x3F,0x00),(0x21,0x0A,0xF3,0x5F,0x00),0x3E)
P_BD  =((0x00,0x07,0xD8,0x0F,0x00),(0x00,0x00,0xF5,0x0F,0x00),0x38)
P_SD  =((0x01,0x0C,0xF8,0x0F,0x00),None,0x39)
P_TOM =((0x04,0x05,0xF7,0x0F,0x00),(0x0E,0x0C,0xC9,0x0F,0x00),0x3D)

class V:
    def __init__(s): s.d=bytearray(); s.n=0; s.t=0.0
    def w(s,r,v): s.d+=bytes([0x5A,r&0xFF,v&0xFF])
    def wait_to(s,ms):
        k=int(round(ms*SR/1000.0))-s.n
        if k<=0: return
        s.n+=k
        while k>65535: s.d+=bytes([0x61,0xFF,0xFF]); k-=65535
        if k: s.d+=bytes([0x61,k&0xFF,k>>8])
    def patch(s,ch,pt):
        m,c,c0=pt; o1,o2=OP1[ch],OP2[ch]
        for r,x in zip((0x20,0x40,0x60,0x80,0xE0),m): s.w(r+o1,x)
        if c:
            for r,x in zip((0x20,0x40,0x60,0x80,0xE0),c): s.w(r+o2,x)
        s.w(0xC0+ch,c0)
    def on(s,ch,f):
        n,b=fnb(f); s.w(0xA0+ch,n&0xFF); s.w(0xB0+ch,0x20|(b<<2)|(n>>8))
    def off(s,ch): s.w(0xB0+ch,0x00)

ev=[]   # (ms, 우선순위, 함수)
def at(ms, fn, pri=1): ev.append((ms, pri, fn))

v=V()
v.w(0x01,0x20); v.w(0x08,0x00)
v.patch(0,P_LEAD); v.patch(1,P_SUB)
v.patch(2,P_PAD); v.patch(3,P_PAD); v.patch(5,P_PAD)
v.patch(4,P_BASS)
v.patch(6,P_BD); v.patch(7,P_SD); v.patch(8,P_TOM)
v.w(0x40+OP2[7],0x0C)
for ch,m in ((6,36),(7,60),(8,48)):
    f,b=fnb(hz(m)); v.w(0xA0+ch,f&0xFF); v.w(0xB0+ch,(b<<2)|(f>>8))
v.w(0xBD,0x20)

# 가락 + 한 옥타브 아래
for i,(st,ms,f) in enumerate(ON):
    at(st, (lambda f: (lambda: (v.on(0,f), v.on(1,f/2.0))))(f), 1)
    if i < len(ON)-1:
        at(st+ms-8, (lambda: (v.off(0), v.off(1))), 0)
# 마지막 음은 여운을 남긴다
at(END+700, (lambda: (v.off(0), v.off(1))), 0)

# 베이스 + 화음
for j,(st,root,tri) in enumerate(CH):
    nxt = CH[j+1][0] if j+1 < len(CH) else END+700
    at(st, (lambda r: (lambda: v.on(4, hz(p(r)))))(root), 1)
    at(min(nxt-20, st+360), (lambda: v.off(4)), 0)
    for k,ch in enumerate((2,3,5)):
        at(st, (lambda c,nt: (lambda: v.on(c, hz(p(nt)))))(ch,tri[k]), 1)
        at(nxt-20, (lambda c: (lambda: v.off(c)))(ch), 0)

# 드럼 : 음이 바뀔 때마다 하이햇, 악구 머리에 큰북, 마지막에 심벌
def hit(mask):
    return lambda: (v.w(0xBD,0x20&~mask), v.w(0xBD,0x20|mask))
for st,ms,f in ON: at(st, hit(0x01), 2)
for st in (ON[0][0], ON[4][0], ON[9][0]): at(st, hit(0x10), 2)
at(ON[2][0], hit(0x08), 2); at(ON[7][0], hit(0x08), 2)
at(ON[9][0], hit(0x02), 2); at(ON[9][0], hit(0x04), 2)

ev.sort(key=lambda e:(e[0], e[1]))
for ms,pri,fn in ev:
    v.wait_to(ms); fn()
v.wait_to(END+900)
for ch in range(9): v.off(ch)
v.wait_to(END+960)
v.d+=bytes([0x66])

D=0x100; h=bytearray(D); h[0:4]=b'Vgm '
struct.pack_into('<I',h,0x04,D+len(v.d)-4)
struct.pack_into('<I',h,0x08,0x151)
struct.pack_into('<I',h,0x18,v.n)
struct.pack_into('<I',h,0x1C,0); struct.pack_into('<I',h,0x20,0)
struct.pack_into('<I',h,0x34,D-0x34)
struct.pack_into('<I',h,0x50,3579545)
open('/tmp/vgm/BB00.VGM','wb').write(bytes(h)+bytes(v.d))
print("BB00.VGM %d바이트 %.2f초 (되풀이 없음) 쓰기 %d번" % (D+len(v.d), v.n/44100.0, v.d.count(0x5A)))
