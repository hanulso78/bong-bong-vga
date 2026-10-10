# -*- coding: utf-8 -*-
"""게임을 다 깼을 때 나오는 끝맺음 곡 (BB05.VGM).
   첫 화면 비프 가락을 웅장하게 다시 들려주고 으뜸화음으로 크게 맺는다.
   되풀이 없음, 약 14초."""
import struct
OP1=[0x00,0x01,0x02,0x08,0x09,0x0A,0x10,0x11,0x12]
OP2=[0x03,0x04,0x05,0x0B,0x0C,0x0D,0x13,0x14,0x15]
def fnb(m):
    f=440.0*(2.0**((m-69)/12.0))
    for b in range(8):
        n=int(round(f*(2**(20-b))/49716.0))
        if n<1024 and (n>=300 or b==7): return n,b
    return 1023,7
N={'c':0,'d':2,'e':4,'f':5,'g':7,'a':9,'b':11}
def p(s):
    k=N[s[0]]; i=1
    while i<len(s) and s[i] in '+-#': k+=1 if s[i] in '+#' else -1; i+=1
    return (int(s[i:])+1)*12+k
SCALE=[0,2,4,5,7,9,11]
def below(m, deg):
    o,s=divmod(m,12)
    if s not in SCALE: s-=1
    d=SCALE.index(s)-deg
    while d<0: d+=7; o-=1
    return o*12+SCALE[d]

Q,E,H = 4,2,8
LEAD=[]
def L(bar, seq):
    t=bar*16
    for ln,nt in seq:
        if nt: LEAD.append((t,ln,p(nt)))
        t+=ln
# 1~2마디 : 첫 화면 가락 그대로
L(0,[(Q,'c5'),(Q,'e5'),(E,'g5'),(E,'e5'),(Q,'c5')])
L(1,[(E,'a5'),(E,'g5'),(E,'a5'),(E,'b5'),(Q,'c6'),(E,'c5'),(E,None)])
# 3~4마디 : 한 옥타브 위로 다시 (더 밝게)
L(2,[(Q,'c6'),(Q,'e6'),(E,'g6'),(E,'e6'),(Q,'c6')])
L(3,[(E,'a6'),(E,'g6'),(E,'a6'),(E,'b6'),(Q,'c7'),(E,'g6'),(E,None)])
# 5~6마디 : 올라가는 다리
L(4,[(Q,'f5'),(Q,'g5'),(Q,'a5'),(Q,'b5')])
L(5,[(Q,'c6'),(Q,'d6'),(H,'e6')])
# 7~8마디 : 맺음
L(6,[(E,'g6'),(E,'f6'),(Q,'e6'),(E,'d6'),(E,'e6'),(Q,'f6')])
L(7,[(Q,'g6'),(Q,'e6'),(H,'c6')])

BARS=8; PAT=BARS*16
CH=[('c3',['c4','e4','g4']), ('g2',['g3','b3','d4']),
    ('c3',['c4','e4','g4']), ('g2',['g3','b3','d4']),
    ('f2',['f3','a3','c4']), ('g2',['g3','b3','d4']),
    ('f2',['f3','a3','c4']), ('c3',['c4','e4','g4'])]

HARM=[(t,l,below(m,2)) for t,l,m in LEAD]
BASS=[]
for b,(root,tri) in enumerate(CH):
    r=p(root)
    for k in range(4):
        BASS.append((b*16+k*4, 4, r if k%2==0 else r+7))
ARP=[]
for b,(root,tri) in enumerate(CH):
    seq=[tri[0],tri[1],tri[2],tri[1]]
    for k in range(16):
        ARP.append((b*16+k, 1, p(seq[k%4])))
PAD=[]                                   # 화음을 길게 깔아 웅장하게
for b,(root,tri) in enumerate(CH):
    PAD.append((b*16, 16, p(tri[2])))
def drums():
    ev=[]
    for b in range(BARS):
        o=b*16
        ev += [(o+0,0x10),(o+8,0x10)]
        ev += [(o+4,0x08),(o+12,0x08)]
        for k in range(0,16,2): ev.append((o+k,0x01))
        if b in (3,5): ev += [(o+12,0x04),(o+14,0x04)]
        if b == 7:                        # 마지막 마디 - 심벌과 탐으로 크게
            ev = [e for e in ev if e[0] < o+2]
            ev += [(o,0x10),(o,0x02),(o+8,0x10),(o+8,0x02)]
    return ev

P_LEAD=((0x31,0x4A,0xF6,0x26,0x00),(0x31,0x00,0xF6,0x26,0x03),0x0C)
P_HARM=((0x31,0x5C,0xF6,0x36,0x00),(0x31,0x0A,0xF6,0x36,0x01),0x0A)
P_ARP =((0xA3,0xD9,0xF0,0x3F,0x01),(0xE1,0x1C,0xF0,0x5F,0x00),0x30)
P_PAD =((0x21,0x4F,0x53,0x14,0x00),(0x21,0x18,0x53,0x14,0x00),0x09)
P_BASS=((0x20,0x1D,0xF4,0x2F,0x00),(0x21,0x02,0xF3,0x4F,0x00),0x3E)
P_BELL=((0x01,0x53,0xF8,0x7A,0x00),(0x11,0x12,0xF7,0x6A,0x00),0x0E)
P_BD  =((0x00,0x07,0xD8,0x0F,0x00),(0x00,0x00,0xF5,0x0F,0x00),0x38)
P_SD  =((0x01,0x0C,0xF8,0x0F,0x00),None,0x39)
P_TOM =((0x04,0x05,0xF7,0x0F,0x00),(0x0E,0x0C,0xC9,0x0F,0x00),0x3D)

class V:
    def __init__(s): s.d=bytearray(); s.n=0
    def w(s,r,v): s.d+=bytes([0x5A,r&0xFF,v&0xFF])
    def wait(s,k):
        s.n+=k
        while k>65535: s.d+=bytes([0x61,0xFF,0xFF]); k-=65535
        if k: s.d+=bytes([0x61,k&0xFF,k>>8])
    def patch(s,ch,pt):
        m,c,c0=pt; o1,o2=OP1[ch],OP2[ch]
        for r,x in zip((0x20,0x40,0x60,0x80,0xE0),m): s.w(r+o1,x)
        if c:
            for r,x in zip((0x20,0x40,0x60,0x80,0xE0),c): s.w(r+o2,x)
        s.w(0xC0+ch,c0)

BPM=132; SPS=int(round(44100*60.0/BPM/4))
v=V(); v.w(0x01,0x20); v.w(0x08,0x00)
v.patch(0,P_LEAD); v.patch(1,P_HARM); v.patch(2,P_ARP)
v.patch(3,P_PAD);  v.patch(4,P_BASS); v.patch(5,P_BELL)
v.patch(6,P_BD);   v.patch(7,P_SD);   v.patch(8,P_TOM)
v.w(0x40+OP2[7],0x08)
for ch,m in ((6,36),(7,60),(8,48)):
    f,b=fnb(m); v.w(0xA0+ch,f&0xFF); v.w(0xB0+ch,(b<<2)|(f>>8))
v.w(0xBD,0x20)

ev=[]
def put(lst,ch,g=10):
    for st,ln,m in lst:
        ev.append((st,1,ch,m)); ev.append((st+max(1,ln*g//10),0,ch,0))
put(LEAD,0,9); put(HARM,1,9); put(ARP,2); put(PAD,3); put(BASS,4,9)
put([(b*16,4,p(r[0])+12) for b,r in enumerate([['c6'],['b5'],['c6'],['b5'],
     ['a5'],['b5'],['a5'],['c6']])],5,8)
for st,mask in drums(): ev.append((st,2,mask,0))
ev=[e for e in ev if e[0]<=PAT+8]
ev.sort(key=lambda e:(e[0],e[1]))
now=0;i=0
while i<len(ev):
    st=ev[i][0]
    if st>now: v.wait((st-now)*SPS); now=st
    dm=0
    while i<len(ev) and ev[i][0]==st:
        _,k,a,b=ev[i]; i+=1
        if k==1:
            f,bl=fnb(b); v.w(0xA0+a,f&0xFF); v.w(0xB0+a,0x20|(bl<<2)|(f>>8))
        elif k==0: v.w(0xB0+a,0x00)
        else: dm|=a
    if dm: v.w(0xBD,0x20&~dm); v.w(0xBD,0x20|dm)
v.wait(SPS*8)
for ch in range(9): v.w(0xB0+ch,0x00)
v.d+=bytes([0x66])

D=0x100; h=bytearray(D); h[0:4]=b'Vgm '
struct.pack_into('<I',h,0x04,D+len(v.d)-4)
struct.pack_into('<I',h,0x08,0x151)
struct.pack_into('<I',h,0x18,v.n)
struct.pack_into('<I',h,0x1C,0); struct.pack_into('<I',h,0x20,0)
struct.pack_into('<I',h,0x34,D-0x34)
struct.pack_into('<I',h,0x50,3579545)
open('/tmp/vgm/BB04.VGM','wb').write(bytes(h)+bytes(v.d))
print("BB04.VGM %d바이트 %.1f초 (되풀이 없음) 쓰기 %d번" % (D+len(v.d), v.n/44100.0, v.d.count(0x5A)))
