# -*- coding: utf-8 -*-
"""good-old-game-sound-generator 가 만들어 준 MML 을 OPL2 VGM 으로 굽는다.
   악기(음색)는 BB01.VGM 이 쓰던 것을 그대로 가져와 다른 곡들과 소리가 어울린다."""
import struct, re, sys

# ---- MML 파싱 ---------------------------------------------------------------
STEP = {'c':0,'d':2,'e':4,'f':5,'g':7,'a':9,'b':11}

def parse_mml(s):
    """(시작16분, 길이16분, midi) 목록.  l/o/></v/r/^/+- 를 다룬다."""
    s = re.sub(r'@[a-zA-Z]+', '', s)          # @tone @d 같은 것
    s = re.sub(r's\d+', '', s)                # 시드
    s = s.replace(' ', '')
    i, t, octv, deflen = 0, 0, 4, 16
    out = []
    def readlen(i):
        m = re.match(r'(\d+)?(\.*)', s[i:])
        num, dots = m.group(1), m.group(2)
        ln = 16 // int(num) if num else 16 // deflen    # 시간 단위는 16분음표
        add, half = ln, ln
        for _ in dots:
            half //= 2; add += half
        return add, i + len(m.group(0))
    while i < len(s):
        c = s[i]
        if c == 'l':
            m = re.match(r'l(\d+)(\.*)', s[i:]); deflen = int(m.group(1)); i += len(m.group(0))
        elif c == 'o':
            m = re.match(r'o(\d+)', s[i:]); octv = int(m.group(1)); i += len(m.group(0))
        elif c == '>': octv += 1; i += 1
        elif c == '<': octv -= 1; i += 1
        elif c == 'v':
            m = re.match(r'v(\d+)', s[i:]); i += len(m.group(0))
        elif c == 't':
            m = re.match(r't(\d+)', s[i:]); i += len(m.group(0))
        elif c == 'r':
            i += 1; ln, i = readlen(i); t += ln
        elif c == '^':
            i += 1; ln, i = readlen(i)
            if out: out[-1][1] += ln
            t += ln
        elif c in STEP:
            n = STEP[c]; i += 1
            while i < len(s) and s[i] in '+-#':
                n += 1 if s[i] in '+#' else -1; i += 1
            ln, i = readlen(i)
            out.append([t, ln, (octv + 1) * 12 + n])
            t += ln
        else:
            i += 1
    return out, t

# ---- OPL2 ------------------------------------------------------------------
OP1 = [0x00,0x01,0x02,0x08,0x09,0x0A,0x10,0x11,0x12]
OP2 = [0x03,0x04,0x05,0x0B,0x0C,0x0D,0x13,0x14,0x15]

def fnum_block(midi):
    f = 440.0 * (2.0 ** ((midi - 69) / 12.0))
    for blk in range(8):
        fn = int(round(f * (2 ** (20 - blk)) / 49716.0))
        if fn < 1024:
            if fn >= 300 or blk == 7: return fn, blk
    return 1023, 7

class Vgm:
    def __init__(self):
        self.d = bytearray(); self.samples = 0
    def w(self, reg, val):
        self.d += bytes([0x5A, reg & 0xFF, val & 0xFF])
    def wait(self, n):
        self.samples += n
        while n > 65535:
            self.d += bytes([0x61, 0xFF, 0xFF]); n -= 65535
        if n > 0: self.d += bytes([0x61, n & 0xFF, n >> 8])
    def patch(self, ch, mod, car, c0):
        o1, o2 = OP1[ch], OP2[ch]
        for r, v in zip((0x20,0x40,0x60,0x80,0xE0), mod): self.w(r + o1, v)
        for r, v in zip((0x20,0x40,0x60,0x80,0xE0), car): self.w(r + o2, v)
        self.w(0xC0 + ch, c0)

# BB01.VGM 에서 뽑아 온 음색 (모듈레이터 20/40/60/80/E0, 캐리어 같은 순서, C0)
P_LEAD  = ((0xF4,0x52,0xA4,0x0F,0x01), (0xF1,0x00,0x45,0x0F,0x00), 0x3C)
P_LEAD2 = ((0xF4,0x56,0xA4,0x0F,0x01), (0xF1,0x08,0x45,0x0F,0x00), 0x3C)
P_ARP   = ((0xA3,0xD9,0x90,0x0F,0x01), (0xE1,0x0D,0x90,0x0F,0x00), 0x30)
P_BASS  = ((0x20,0x1D,0xB4,0x0F,0x00), (0x21,0x00,0xC3,0x0F,0x00), 0x3E)
# 리듬부 (BB01 과 같은 값)
P_BD    = ((0x00,0x07,0xD8,0x0F,0x00), (0x00,0x00,0xF5,0x0F,0x00), 0x38)
P_SDHH  = ((0x01,0x0C,0xF8,0x0F,0x00), None, 0x39)
P_TOMTC = ((0x04,0x05,0xF7,0x0F,0x00), (0x0E,0x0C,0xC9,0x0F,0x00), 0x3D)

# ---- 곡 만들기 --------------------------------------------------------------
LEAD_MML = "@synth v40 l2. o4 r16a r8a8 g r8g8 g r8g8 f+ r8f+16"
ARP_MML  = ("@select v25 l16 o4 <b>eaa8<a8a8 a8a>df+d+e agdd8c8d8 rd8d<g+a+>c "
            "dddd8c8d8 <g8>ccfd+g gf+c+f+8<b8b8 b8b>c+8ff+")

BPM      = 160                       # 박진감 있게
SPB16    = int(round(44100 * 60.0 / BPM / 4))   # 16분음표 한 칸의 샘플 수
BARS     = 4                         # 만들어 준 것이 4마디(64칸)
PAT      = BARS * 16

lead, _ = parse_mml(LEAD_MML)
arp,  _ = parse_mml(ARP_MML)
lead = [n for n in lead if n[0] < PAT]
arp  = [n for n in arp  if n[0] < PAT]

# 베이스 : 각 마디 첫 음(과 3박)을 두 옥타브 내려 8분음표로 쿵쿵 찍는다
bass = []
for bar in range(BARS):
    roots = [n[2] for n in arp if bar*16 <= n[0] < bar*16+16]
    root = (min(roots) if roots else 57) - 12
    while root > 45: root -= 12
    while root < 33: root += 12
    for k in range(8):                      # 8분음표로 계속 - 달리는 느낌
        bass.append([bar*16 + k*2, 2, root if k % 4 != 2 else root + 7])

# 드럼 : 킥은 매 박, 스네어는 2·4박, 하이햇은 8분 (뒤 절반은 16분으로 조인다)
def drums(second):
    ev = []                                  # (칸, 비트마스크)
    for bar in range(BARS):
        b = bar * 16
        for beat in range(4):
            ev.append((b + beat*4, 0x10))                    # BD
            if beat % 2 == 1: ev.append((b + beat*4, 0x08))  # SD
        step = 1 if second else 2
        for k in range(0, 16, step):
            ev.append((b + k, 0x01))                         # HH
        if second and bar == BARS - 1:                       # 마지막 마디 필인
            for k in (12, 13, 14, 15): ev.append((b + k, 0x04))
    return ev

def build():
    v = Vgm()
    v.w(0x01, 0x20)                          # 웨이브폼 쓰기 허용
    v.w(0x08, 0x00)
    v.patch(0, *P_LEAD); v.patch(1, *P_LEAD2)
    v.patch(2, *P_ARP);  v.patch(3, *P_ARP)
    v.patch(4, *P_BASS); v.patch(5, *P_BASS)
    o1, o2 = OP1[6], OP2[6]
    for r, x in zip((0x20,0x40,0x60,0x80,0xE0), P_BD[0]): v.w(r+o1, x)
    for r, x in zip((0x20,0x40,0x60,0x80,0xE0), P_BD[1]): v.w(r+o2, x)
    v.w(0xC0+6, P_BD[2])
    o1 = OP1[7]
    for r, x in zip((0x20,0x40,0x60,0x80,0xE0), P_SDHH[0]): v.w(r+o1, x)
    v.w(0x40+OP2[7], 0x08)
    v.w(0xC0+7, P_SDHH[2])
    o1, o2 = OP1[8], OP2[8]
    for r, x in zip((0x20,0x40,0x60,0x80,0xE0), P_TOMTC[0]): v.w(r+o1, x)
    for r, x in zip((0x20,0x40,0x60,0x80,0xE0), P_TOMTC[1]): v.w(r+o2, x)
    v.w(0xC0+8, P_TOMTC[2])
    # 리듬부 음높이
    for ch, midi in ((6, 36), (7, 60), (8, 48)):
        fn, blk = fnum_block(midi)
        v.w(0xA0+ch, fn & 0xFF); v.w(0xB0+ch, (blk << 2) | (fn >> 8))
    v.w(0xBD, 0x20)                          # 리듬 모드 켜기

    loop_at = len(v.d)                       # 여기부터 되풀이

    for second in (0, 1):                    # 같은 패턴 두 번, 두 번째는 더 촘촘히
        track = []                           # (칸, 종류, 값)
        for st, ln, m in lead:
            track.append((st, 'on', 0, m)); track.append((min(st+ln, PAT), 'off', 0, 0))
            if second:                       # 두 번째 바퀴는 한 옥타브 겹쳐 두껍게
                track.append((st, 'on', 1, m+12)); track.append((min(st+ln, PAT), 'off', 1, 0))
        for k, (st, ln, m) in enumerate(arp):
            ch = 2 + (k & 1)
            track.append((st, 'on', ch, m)); track.append((min(st+ln, PAT), 'off', ch, 0))
        for st, ln, m in bass:
            track.append((st, 'on', 4, m)); track.append((min(st+ln, PAT), 'off', 4, 0))
        for st, mask in drums(second):
            track.append((st, 'drum', mask, 0))

        track.sort(key=lambda e: (e[0], 0 if e[1] == 'off' else 1))
        now = 0
        rhy = 0x20
        idx = 0
        while idx < len(track):
            st = track[idx][0]
            if st > now:
                v.wait((st - now) * SPB16); now = st
            drum_mask = 0
            while idx < len(track) and track[idx][0] == st:
                _, kind, a, b = track[idx]; idx += 1
                if kind == 'on':
                    fn, blk = fnum_block(b)
                    v.w(0xA0+a, fn & 0xFF)
                    v.w(0xB0+a, 0x20 | (blk << 2) | (fn >> 8))
                elif kind == 'off':
                    v.w(0xB0+a, 0x00)
                else:
                    drum_mask |= a
            if drum_mask:
                v.w(0xBD, rhy & ~drum_mask)   # 다시 치려면 껐다 켠다
                v.w(0xBD, rhy | drum_mask)
        if PAT > now: v.wait((PAT - now) * SPB16)
    v.d += bytes([0x66])
    return v, loop_at

v, loop_at = build()
DATA = 0x100
hdr = bytearray(DATA)
hdr[0:4] = b'Vgm '
struct.pack_into('<I', hdr, 0x04, DATA + len(v.d) - 4)
struct.pack_into('<I', hdr, 0x08, 0x151)
struct.pack_into('<I', hdr, 0x18, v.samples)
struct.pack_into('<I', hdr, 0x1C, DATA + loop_at - 0x1C)
struct.pack_into('<I', hdr, 0x20, v.samples)
struct.pack_into('<I', hdr, 0x34, DATA - 0x34)
struct.pack_into('<I', hdr, 0x50, 3579545)
open('/tmp/vgm/BB02.VGM','wb').write(bytes(hdr) + bytes(v.d))
print("BB02.VGM  %d 바이트  %d 샘플 = %.1f초  되풀이 시작 0x%x  쓰기 %d번"
      % (DATA+len(v.d), v.samples, v.samples/44100.0, DATA+loop_at, v.d.count(0x5A)))
