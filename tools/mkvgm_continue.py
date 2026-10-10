# -*- coding: utf-8 -*-
"""이어하기 화면 곡 (BB07.VGM).
   10초를 세는 화면이라 시계처럼 째깍대는 박자 위에 조바심 나는 가락을 얹었다.
   6마디 약 10.3초, 끝나면 처음으로 되풀이한다."""
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

Q,E,S,H,W=4,2,1,8,16
LEAD=[]
def L(bar,seq):
    t=bar*16
    for ln,nt in seq:
        if nt: LEAD.append((t,ln,p(nt)))
        t+=ln
# 1~2마디 : 다 단조로 내려오는 짧은 동기
L(0,[(E,'g5'),(E,'f5'),(Q,'e-5'),(E,'d5'),(E,'e-5'),(Q,'c5')])
L(1,[(E,'g5'),(E,'f5'),(Q,'e-5'),(H,'d5')])
# 3~4마디 : 단3도 위로 올려 조바심을 더한다
L(2,[(E,'b-5'),(E,'a-5'),(Q,'g5'),(E,'f5'),(E,'g5'),(Q,'e-5')])
L(3,[(E,'b-5'),(E,'a-5'),(Q,'g5'),(H,'f5')])
# 5~6마디 : 반음씩 치고 올라가 딸림화음에서 멎는다 (풀리지 않는 느낌)
L(4,[(E,'c6'),(E,'d6'),(E,'e-6'),(E,'e6'),(E,'f6'),(E,'f+6'),(Q,'g6')])
L(5,[(Q,'g6'),(Q,'f6'),(H,'d6')])

BARS=6; PAT=BARS*16
CH=[('c2',['c4','e-4','g4']), ('c2',['c4','e-4','g4']),
    ('a-1',['a-3','c4','e-4']), ('a-1',['a-3','c4','e-4']),
    ('g1',['g3','b3','d4']),   ('g1',['g3','b3','d4'])]
BASS=[]
for b,(root,tri) in enumerate(CH):
    r=p(root)
    for k in range(8):                      # 8분음표로 두근대는 베이스
        BASS.append((b*16+k*2, 2, r if k % 4 != 3 else r+7))
PAD=[]
for b,(root,tri) in enumerate(CH):
    if b % 2 == 0:
        for nt in tri: PAD.append((b*16, 32, p(nt)))
def drums():
    ev=[]
    for b in range(BARS):
        o=b*16
        for k in range(0,16,4): ev.append((o+k,0x01))      # 째깍 (하이햇)
        ev += [(o+0,0x10),(o+8,0x10)]                      # 큰북
        ev += [(o+4,0x08),(o+12,0x08)]                     # 작은북
        if b >= 4:
            for k in range(2,16,4): ev.append((o+k,0x01))  # 뒤로 갈수록 촘촘히
        if b == 5: ev += [(o+12,0x04),(o+14,0x04)]         # 탐으로 몰아친다
    return ev

P_LEAD=((0x31,0x46,0xF7,0x36,0x00),(0x31,0x00,0xF7,0x36,0x02),0x0C)
P_PAD =((0x21,0x4F,0x53,0x25,0x00),(0x21,0x1E,0x53,0x25,0x00),0x09)
P_BASS=((0x20,0x1B,0xF6,0x4F,0x00),(0x21,0x02,0xF5,0x6F,0x00),0x3E)
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

BPM=140; SPS=int(round(44100*60.0/BPM/4))
v=V(); v.w(0x01,0x20); v.w(0x08,0x00)
v.patch(0,P_LEAD); v.patch(1,P_PAD); v.patch(2,P_PAD); v.patch(3,P_PAD)
v.patch(4,P_BASS); v.patch(6,P_BD); v.patch(7,P_SD); v.patch(8,P_TOM)
v.w(0x40+OP2[7],0x08)
for ch,m in ((6,36),(7,60),(8,48)):
    f,b=fnb(m); v.w(0xA0+ch,f&0xFF); v.w(0xB0+ch,(b<<2)|(f>>8))
v.w(0xBD,0x20)
LOOP_BYTE=len(v.d)

ev=[]
def put(lst,ch,g=10):
    for st,ln,m in lst:
        ev.append((st,1,ch,m)); ev.append((st+max(1,ln*g//10),0,ch,0))
put(LEAD,0,9); put(BASS,4,7)
for i,(st,ln,m) in enumerate(PAD): put([(st,ln,m)],1+(i%3))
for st,mask in drums(): ev.append((st,2,mask,0))
ev=[e for e in ev if e[0]<PAT]
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
v.wait((PAT-now)*SPS)
LOOP_SAMPLES=v.n
v.d+=bytes([0x66])

D=0x100; h=bytearray(D); h[0:4]=b'Vgm '
struct.pack_into('<I',h,0x04,D+len(v.d)-4)
struct.pack_into('<I',h,0x08,0x151)
struct.pack_into('<I',h,0x18,v.n)
struct.pack_into('<I',h,0x1C,D+LOOP_BYTE-0x1C)
struct.pack_into('<I',h,0x20,LOOP_SAMPLES)
struct.pack_into('<I',h,0x34,D-0x34)
struct.pack_into('<I',h,0x50,3579545)
open('/tmp/vgm/BB07.VGM','wb').write(bytes(h)+bytes(v.d))
print("BB07.VGM %d바이트 %.1f초 (되풀이) 쓰기 %d번" % (D+len(v.d), v.n/44100.0, v.d.count(0x5A)))
