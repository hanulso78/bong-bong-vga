# -*- coding: utf-8 -*-
"""최고 기록 화면에 흐르는 곡 (BB06.VGM).
   느긋하고 뿌듯한 느낌. 8마디를 끝없이 되풀이한다. 약 16초."""
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
def below(m,deg):
    o,s=divmod(m,12)
    if s not in SCALE: s-=1
    d=SCALE.index(s)-deg
    while d<0: d+=7; o-=1
    return o*12+SCALE[d]

Q,E,H,W=4,2,8,16
LEAD=[]
def L(bar,seq):
    t=bar*16
    for ln,nt in seq:
        if nt: LEAD.append((t,ln,p(nt)))
        t+=ln
#  C     -  Am    -  F     -  G     -  C     -  Em    -  F  G  -  C
L(0,[(Q,'e5'),(Q,'g5'),(H,'c6')])
L(1,[(Q,'a5'),(E,'g5'),(E,'e5'),(H,'a5')])
L(2,[(Q,'f5'),(Q,'a5'),(E,'c6'),(E,'a5'),(Q,'f5')])
L(3,[(Q,'g5'),(Q,'b5'),(H,'d6')])
L(4,[(Q,'e6'),(E,'d6'),(E,'c6'),(H,'g5')])
L(5,[(Q,'e5'),(Q,'g5'),(E,'b5'),(E,'g5'),(Q,'e5')])
L(6,[(Q,'f5'),(Q,'g5'),(Q,'a5'),(Q,'b5')])
L(7,[(W,'c6')])

BARS=8; PAT=BARS*16
CH=[('c3',['c4','e4','g4']), ('a2',['a3','c4','e4']),
    ('f2',['f3','a3','c4']), ('g2',['g3','b3','d4']),
    ('c3',['c4','e4','g4']), ('e2',['e3','g3','b3']),
    ('f2',['f3','a3','c4']), ('c3',['c4','e4','g4'])]
HARM=[(t,l,below(m,2)) for t,l,m in LEAD]
BASS=[]
for b,(root,tri) in enumerate(CH):
    r=p(root)
    for k in range(4): BASS.append((b*16+k*4,3,r if k%2==0 else r+7))
ARP=[]
for b,(root,tri) in enumerate(CH):
    seq=[tri[0],tri[1],tri[2],tri[1]]
    for k in range(8): ARP.append((b*16+k*2,2,p(seq[k%4])+12))
PAD=[(b*16,16,p(tri[0])) for b,(root,tri) in enumerate(CH)]
def drums():
    ev=[]
    for b in range(BARS):
        o=b*16
        ev += [(o+0,0x10),(o+8,0x10)]     # 큰북
        ev += [(o+4,0x08),(o+12,0x08)]    # 작은북
        for k in range(0,16,4): ev.append((o+k+2,0x01))   # 하이햇
        if b==7: ev += [(o+12,0x02),(o+14,0x04)]
    return ev

P_LEAD=((0x31,0x4A,0xE6,0x26,0x00),(0x31,0x02,0xE6,0x26,0x00),0x0C)
P_HARM=((0x31,0x5C,0xE6,0x36,0x00),(0x31,0x0C,0xE6,0x36,0x01),0x0A)
P_ARP =((0xA3,0xD9,0xF0,0x3F,0x01),(0xE1,0x1E,0xF0,0x5F,0x00),0x30)
P_PAD =((0x21,0x4F,0x53,0x14,0x00),(0x21,0x1C,0x53,0x14,0x00),0x09)
P_BASS=((0x20,0x1D,0xF4,0x2F,0x00),(0x21,0x04,0xF3,0x4F,0x00),0x3E)
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

BPM=112; SPS=int(round(44100*60.0/BPM/4))
v=V(); v.w(0x01,0x20); v.w(0x08,0x00)
v.patch(0,P_LEAD); v.patch(1,P_HARM); v.patch(2,P_ARP)
v.patch(3,P_PAD);  v.patch(4,P_BASS)
v.patch(6,P_BD);   v.patch(7,P_SD);  v.patch(8,P_TOM)
v.w(0x40+OP2[7],0x0A)
for ch,m in ((6,36),(7,60),(8,48)):
    f,b=fnb(m); v.w(0xA0+ch,f&0xFF); v.w(0xB0+ch,(b<<2)|(f>>8))
v.w(0xBD,0x20)
LOOP_BYTE = len(v.d)              # 여기서부터 되풀이한다

ev=[]
def put(lst,ch,g=10):
    for st,ln,m in lst:
        ev.append((st,1,ch,m)); ev.append((st+max(1,ln*g//10),0,ch,0))
put(LEAD,0,9); put(HARM,1,9); put(ARP,2,8); put(PAD,3); put(BASS,4,8)
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
LOOP_SAMPLES = v.n
v.d+=bytes([0x66])

D=0x100; h=bytearray(D); h[0:4]=b'Vgm '
struct.pack_into('<I',h,0x04,D+len(v.d)-4)
struct.pack_into('<I',h,0x08,0x151)
struct.pack_into('<I',h,0x18,v.n)
struct.pack_into('<I',h,0x1C,D+LOOP_BYTE-0x1C)
struct.pack_into('<I',h,0x20,LOOP_SAMPLES)
struct.pack_into('<I',h,0x34,D-0x34)
struct.pack_into('<I',h,0x50,3579545)
open('/tmp/vgm/BB06.VGM','wb').write(bytes(h)+bytes(v.d))
print("BB06.VGM %d바이트 %.1f초 (되풀이 시작 0x%X, %.1f초) 쓰기 %d번"
      % (D+len(v.d), v.n/44100.0, D+LOOP_BYTE, LOOP_SAMPLES/44100.0, v.d.count(0x5A)))
