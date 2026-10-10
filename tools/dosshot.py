# -*- coding: utf-8 -*-
"""DOSBox 를 창 없이 돌려 대본(BBTEST)대로 키를 넣고 화면을 찍는다.
   python tools/dosshot.py 대본.txt 모을폴더 [exe]
   exe 는 이 폴더 기준 (기본 bongvga.exe).  찍힌 BMP 는 PNG 로 바꿔 모을폴더에 둔다.
   대본 줄: <ms> key <글자|0x16진수> / <ms> shot <이름> / <ms> quit   (tp.c 참고)"""
import os, sys, glob, shutil, subprocess, tempfile
from PIL import Image

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
DOSBOX = os.path.abspath(os.path.join(ROOT, '..', '..', 'dosbox', 'dosbox.exe'))

script, outdir = sys.argv[1], os.path.abspath(sys.argv[2])
exe = sys.argv[3] if len(sys.argv) > 3 else 'bongvga.exe'

run = tempfile.mkdtemp(prefix='bbshot')                 # 최고 점수 파일이 섞이지 않게 따로 돌린다
for f in ['16X16.DAT', '16X16B.DAT', 'B_ST.DAT', 'SOUND.DAT', 'HSMALL.FNT', 'ESMALL.FNT', 'CWSDPMI.EXE']:
    shutil.copy(os.path.join(ROOT, f), run)
shutil.copy(os.path.join(ROOT, exe), os.path.join(run, 'GAME.EXE'))
shutil.copy(script, os.path.join(run, 'BBTEST.TXT'))
conf = os.path.join(run, 'run.conf')
open(conf, 'w').write('''[dosbox]
machine=svga_s3
memsize=32
[cpu]
core=auto
cycles=max
[mixer]
nosound=true
[autoexec]
mount C "%s"
C:
set BBTEST=BBTEST.TXT
GAME.EXE
exit
''' % run)
env = dict(os.environ, SDL_VIDEODRIVER='dummy', SDL_AUDIODRIVER='dummy')
subprocess.run([DOSBOX, '-conf', conf, '-noconsole'], cwd=os.path.dirname(DOSBOX), env=env, timeout=600)
os.makedirs(outdir, exist_ok=True)
n = 0
for fn in sorted(glob.glob(os.path.join(run, '*.BMP'))):
    name = os.path.splitext(os.path.basename(fn))[0].lower()
    Image.open(fn).convert('RGB').resize((640, 400), Image.NEAREST).save(os.path.join(outdir, name + '.png'))
    n += 1
shutil.rmtree(run, ignore_errors=True)
print('%d shots -> %s' % (n, outdir))
