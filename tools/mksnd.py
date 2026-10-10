# -*- coding: utf-8 -*-
"""BB0n.VGM 여러 개를 SOUND.DAT 한 덩어리로 묶는다.
   쓰는 법 :  python mksnd.py [곡이 있는 폴더] [만들 파일]
       0..7    "BBSND10"           (마지막 바이트는 0)
       8..9    곡 수                (2바이트, 낮은 자리 먼저)
       10..    곡마다 20바이트      이름 12 / 시작 위치 4 / 길이 4
       그 뒤   VGM 자료 (손대지 않은 원본 그대로)
"""
import os, struct, sys

NAMES = ["BB00.VGM",   # 첫 화면
         "BB01.VGM",   # 게임 중 (되풀이)
         "BB02.VGM",   # 판 깼을 때
         "BB03.VGM",   # 게임 끝
         "BB04.VGM",   # 20판 다 깼을 때
         "BB05.VGM",   # 떨어질 때 "삐이익~"
         "BB06.VGM",   # 최고 기록 (되풀이)
         "BB07.VGM"]   # 이어하기 (되풀이)

src = sys.argv[1] if len(sys.argv) > 1 else "."
out = sys.argv[2] if len(sys.argv) > 2 else os.path.join(src, "SOUND.DAT")

have = [(n, open(os.path.join(src, n), "rb").read())
        for n in NAMES if os.path.exists(os.path.join(src, n))]
if not have:
    print("곡을 하나도 못 찾았습니다:", src); sys.exit(1)

head = 8 + 2 + 20 * len(have)
toc, data, off = bytearray(), bytearray(), head
for name, body in have:
    if body[:4] != b'Vgm ':
        print("VGM 이 아닙니다:", name); sys.exit(1)
    toc += name.encode("ascii").ljust(12, b'\0')
    toc += struct.pack("<II", off, len(body))
    data += body
    off += len(body)

with open(out, "wb") as f:
    f.write(b"BBSND10\0")
    f.write(struct.pack("<H", len(have)))
    f.write(bytes(toc))
    f.write(bytes(data))

print("%s %d바이트 - 곡 %d개" % (out, head + len(data), len(have)))
for name, body in have:
    print("   %-10s %6d바이트" % (name, len(body)))
