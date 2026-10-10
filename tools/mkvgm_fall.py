# -*- coding: utf-8 -*-
"""너구리가 떨어질 때 나는 "삐이익~" 소리 (BB06.VGM).
   높은 음에서 낮은 음으로 미끄러져 내려가며 살짝 떨린다. 되풀이 없음, 약 1.6초."""
import struct, math
OP1=[0x00,0x01,0x02,0x08,0x09,0x0A,0x10,0x11,0x12]
OP2=[0x03,0x04,0x05,0x0B,0x0C,0x0D,0x13,0x14,0x15]

def fnb(hz):
    for b in range(8):
        n=int(round(hz*(2**(20-b))/49716.0))
        if n<1024 and (n>=280 or b==7): return n,b
    return 1023,7

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

# 호루라기 같은 맑은 소리 : 변조기는 아주 약하게, 반송파는 계속 울린다
P_WHI=((0x21,0x2A,0xF0,0x0F,0x00),(0x21,0x00,0xF0,0x0F,0x00),0x06)
# 한 옥타브 위에서 살짝 겹쳐 주는 소리 (쇳소리)
P_THIN=((0x21,0x3A,0xF0,0x1F,0x01),(0x21,0x16,0xF0,0x1F,0x00),0x04)

HZ0, HZ1, DUR = 1900.0, 170.0, 1.62     # 처음/끝 주파수, 길이(초)
STEP = 0.008                            # 8ms 마다 음을 바꾼다
SR   = 44100

v=V()
v.w(0x01,0x20); v.w(0x08,0x00); v.w(0xBD,0x00)
v.patch(0,P_WHI); v.patch(1,P_THIN)

n=int(DUR/STEP)
first=True
for i in range(n):
    t=i*STEP
    r=t/DUR
    hz=HZ0*((HZ1/HZ0)**r)                       # 지수로 미끄러져 내려간다
    hz*=1.0+0.035*math.sin(2*math.pi*7.5*t)     # 살짝 떨림
    for ch,mul in ((0,1.0),(1,2.0)):
        f,b=fnb(hz*mul)
        v.w(0xA0+ch,f&0xFF); v.w(0xB0+ch,0x20|(b<<2)|(f>>8))
    if first:
        v.w(0x40+OP2[0],0x00); v.w(0x40+OP2[1],0x16); first=False
    else:
        # 뒤로 갈수록 조금씩 작아진다
        v.w(0x40+OP2[0], min(0x3F,int(0x00+0x1E*r)))
        v.w(0x40+OP2[1], min(0x3F,int(0x16+0x24*r)))
    v.wait(int(STEP*SR))
for ch in (0,1): v.w(0xB0+ch,0x00)
v.wait(int(0.05*SR))
v.d+=bytes([0x66])

D=0x100; h=bytearray(D); h[0:4]=b'Vgm '
struct.pack_into('<I',h,0x04,D+len(v.d)-4)
struct.pack_into('<I',h,0x08,0x151)
struct.pack_into('<I',h,0x18,v.n)
struct.pack_into('<I',h,0x1C,0); struct.pack_into('<I',h,0x20,0)
struct.pack_into('<I',h,0x34,D-0x34)
struct.pack_into('<I',h,0x50,3579545)
open('/tmp/vgm/BB05.VGM','wb').write(bytes(h)+bytes(v.d))
print("BB05.VGM %d바이트 %.2f초 쓰기 %d번" % (D+len(v.d), v.n/44100.0, v.d.count(0x5A)))
