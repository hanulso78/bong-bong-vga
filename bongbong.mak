# ============================================================================
#  BONG BONG  -  djgpp 빌드
# ----------------------------------------------------------------------------
#  ..\..\djgpp.bat 로 연 창에서 이 폴더로 와서
#
#      make -f bongbong.mak            둘 다 만든다
#      make -f bongbong.mak vga        BONGBONG.EXE  원판 + VGA + 일정한 속도
#      make -f bongbong.mak hangul     BONGVGA.EXE   한글 개선판
#      make -f bongbong.mak clean
#
#  (bongbong.mak 을 makefile 로 이름만 바꾸면 그냥 make 로도 된다)
#
#  ---------------------------------------------------------------------------
#  BONGBONG.EXE - 1989년 원판 그대로.  두 가지만 다르다.
#       1) 화면을 VGA 모드 13h 로 그린다.
#          djgpp 는 32비트 보호모드라 BGI(CGA.BGI)를 못 쓴다.  터보 파스칼의
#          Graph/Crt/Dos/System 호출을 tp.c 에서 VGA 로 다시 구현했다.
#          쓰는 색은 원판과 같은 CGA 팔레트 4색(검정/하늘/자홍/흰색)뿐이라
#          화면은 원판과 똑같이 보인다.
#       2) 기계가 빨라도 같은 속도로 돈다.
#          원판은 게임 루프가 최대 속도로 돌아서 빠른 PC 에서는 손도 못 댈
#          만큼 빨라졌다.  여기서는 PIT 를 읽는 uclock() 으로 시간을 재서
#          초당 GAME_FPS(70) 번으로 묶는다(FrameSync).  Delay 도 빈 루프가
#          아니라 진짜 시계를 쓴다.
#       그 밖의 것 - 스테이지 10개, 영문 표시, 16픽셀씩 뚝뚝 끊기는 움직임,
#       원본 소리 - 은 모두 원판 그대로다.
#       필요한 파일 : CWSDPMI.EXE, 16X16.DAT, B_ST.DAT
#
#  BONGVGA.EXE  - 한글 개선판.  부드러운 움직임, 입체 음영, 한글, 20 스테이지,
#       최고점수 저장, 멈춤/소리끄기, 이어하기, 시범 플레이 등이 들어 있다.
#       필요한 파일 : CWSDPMI.EXE, 16X16.DAT, 16X16B.DAT, B_ST.DAT,
#                     HSMALL.FNT, ESMALL.FNT,
#                     SOUND.DAT (애드립 곡 일곱 개를 묶은 것)
# ============================================================================

CC      = gcc
CFLAGS  = -O2 -Wall
LDLIBS  = -lm

all: bongbong.exe bongvga.exe

vga:    bongbong.exe
hangul: bongvga.exe

# ---- 원판 + VGA + 일정한 속도 ---------------------------------------------
bongbong.exe: bongbong.o tp.o
	$(CC) $(CFLAGS) -o bongbong.exe bongbong.o tp.o $(LDLIBS)
	strip bongbong.exe

# ---- 한글 개선판 -----------------------------------------------------------
bongvga.exe: bongsmth.o tp.o
	$(CC) $(CFLAGS) -o bongvga.exe bongsmth.o tp.o $(LDLIBS)
	strip bongvga.exe

bongbong.o: bongbong.c tp.h
	$(CC) $(CFLAGS) -c bongbong.c -o bongbong.o

bongsmth.o: bongbong_smooth.c tp.h
	$(CC) $(CFLAGS) -c bongbong_smooth.c -o bongsmth.o

tp.o: tp.c tp.h
	$(CC) $(CFLAGS) -c tp.c -o tp.o

clean:
	rm -f bongbong.o bongsmth.o tp.o bongbong.exe bongvga.exe

.PHONY: all vga hangul clean
