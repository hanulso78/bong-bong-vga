# -*- coding: utf-8 -*-
"""게임이 끝났을 때 나오는 곡 (BB03.VGM).
   가락이 천천히 내려앉다가 단조 화음으로 길게 멎는다. 되풀이 없음, 약 9초."""
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

Q,E,H,W = 4,2,8,16
LEAD=[]
def L(bar, seq):
    t=bar*16
    for ln,nt in seq:
        if nt: LEAD.append((t,ln,p(nt)))
        t+=ln
# 1마디 : 높은 데서 한 번 울고
L(0,[(Q,'a5'),(E,'c6'),(E,'b5'),(H,'a5')])
# 2마디 : 반음씩 내려앉는다
L(1,[(Q,'g+5'),(Q,'g5'),(Q,'f+5'),(Q,'f5')])
# 3마디 : 더 내려가서
L(2,[(Q,'e5'),(E,'d5'),(E,'c5'),(H,'b4')])
# 4마디 : 낮은 으뜸음으로 길게 멎는다
L(3,[(W,'a4')])

BARS=4; PAT=BARS*16
CH=[('a2',['a3','c4','e4']), ('e2',['e3','g+3','b3']),
    ('f2',['f3','a3','c4']), ('a2',['a3','c4','e4'])]
HARM=[]
for t,l,m in LEAD: HARM.append((t,l,m-12))     # 한 옥타브 아래로 겹친다
BASS=[]
for b,(root,tri) in enumerate(CH):
    r=p(root)
    BASS.append((b*16,8,r)); BASS.append((b*16+8,8,r))
PAD=[]
for b,(root,tri) in enumerate(CH):
    for nt in tri: PAD.append((b*16,16,p(nt)))

P_LEAD=((0x31,0x4A,0xB6,0x16,0x00),(0x31,0x02,0xB6,0x16,0x00),0x0C)
P_HARM=((0x31,0x5C,0xB6,0x26,0x00),(0x31,0x0C,0xB6,0x26,0x01),0x0A)
P_PAD =((0x21,0x4F,0x43,0x14,0x00),(0x21,0x1A,0x43,0x14,0x00),0x09)
P_BASS=((0x20,0x1D,0xB4,0x1F,0x00),(0x21,0x02,0xB3,0x2F,0x00),0x3E)

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
        for r,x in zip((0x20,0x40,0x60,0x80,0xE0),c): s.w(r+o2,x)
        s.w(0xC0+ch,c0)

BPM=100; SPS=int(round(44100*60.0/BPM/4))
v=V(); v.w(0x01,0x20); v.w(0x08,0x00); v.w(0xBD,0x00)
v.patch(0,P_LEAD); v.patch(1,P_HARM); v.patch(4,P_BASS)
for ch in (2,3,5): v.patch(ch,P_PAD)

ev=[]
def put(lst,ch,g=10):
    for st,ln,m in lst:
        ev.append((st,1,ch,m)); ev.append((st+max(1,ln*g//10),0,ch,0))
put(LEAD,0,9); put(HARM,1,9); put(BASS,4,9)
for i,(st,ln,m) in enumerate(PAD): put([(st,ln,m)],2+(i%3 if i%3<2 else 3),10)
ev=[e for e in ev if e[0]<=PAT+16]
ev.sort(key=lambda e:(e[0],e[1]))
now=0;i=0
while i<len(ev):
    st=ev[i][0]
    if st>now: v.wait((st-now)*SPS); now=st
    while i<len(ev) and ev[i][0]==st:
        _,k,a,b=ev[i]; i+=1
        if k==1:
            f,bl=fnb(b); v.w(0xA0+a,f&0xFF); v.w(0xB0+a,0x20|(bl<<2)|(f>>8))
        else: v.w(0xB0+a,0x00)
v.wait(SPS*6)
for ch in range(9): v.w(0xB0+ch,0x00)
v.d+=bytes([0x66])

D=0x100; h=bytearray(D); h[0:4]=b'Vgm '
struct.pack_into('<I',h,0x04,D+len(v.d)-4)
struct.pack_into('<I',h,0x08,0x151)
struct.pack_into('<I',h,0x18,v.n)
struct.pack_into('<I',h,0x1C,0); struct.pack_into('<I',h,0x20,0)
struct.pack_into('<I',h,0x34,D-0x34)
struct.pack_into('<I',h,0x50,3579545)
open('/tmp/vgm/BB03.VGM','wb').write(bytes(h)+bytes(v.d))
print("BB03.VGM %d바이트 %.1f초 (되풀이 없음) 쓰기 %d번" % (D+len(v.d), v.n/44100.0, v.d.count(0x5A)))
