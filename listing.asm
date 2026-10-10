
;===== proc_000A  (len 466) =====
000A  55               push   bp
000B  89e5             mov    bp, sp
000D  b80e00           mov    ax, 0xe
0010  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
0015  83ec0e           sub    sp, 0xe
0018  bf8c02           mov    di, 0x28c
001B  1e               push   ds
001C  57               push   di
001D  bf0000           mov    di, 0
0020  0e               push   cs
0021  57               push   di
0022  9ac80c0d06       lcall  0x60d, 0xcc8   ; @Val
0027  bf8c02           mov    di, 0x28c
002A  1e               push   ds
002B  57               push   di
002C  9a380d0d06       lcall  0x60d, 0xd38   ; @Assign
0031  9a0e020d06       lcall  0x60d, 0x20e   ; @IOCheck
0036  bf8c02           mov    di, 0x28c
0039  1e               push   ds
003A  57               push   di
003B  9aeb0f0d06       lcall  0x60d, 0xfeb   ; @Read
0040  9a0e020d06       lcall  0x60d, 0x20e   ; @IOCheck
0045  b80100           mov    ax, 1
0048  50               push   ax
0049  b80100           mov    ax, 1
004C  50               push   ax
004D  b81000           mov    ax, 0x10
0050  50               push   ax
0051  b81000           mov    ax, 0x10
0054  50               push   ax
0055  9a000ed302       lcall  0x2d3, 0xe00   ; ImageSize?
005A  a33405           mov    word ptr [0x534], ax
005D  c746f20100       mov    word ptr [bp - 0xe], 1
0062  eb03             jmp    0x67
0064  ff46f2           inc    word ptr [bp - 0xe]
0067  c746f80100       mov    word ptr [bp - 8], 1
006C  eb03             jmp    0x71
006E  ff46f8           inc    word ptr [bp - 8]
0071  c746fa0100       mov    word ptr [bp - 6], 1
0076  eb03             jmp    0x7b
0078  ff46fa           inc    word ptr [bp - 6]
007B  837efa10         cmp    word ptr [bp - 6], 0x10
007F  7519             jne    0x9a
0081  bf8c02           mov    di, 0x28c
0084  1e               push   ds
0085  57               push   di
0086  9a59100d06       lcall  0x60d, 0x1059   ; @BlockRead
008B  8846ff           mov    byte ptr [bp - 1], al
008E  9aeb0f0d06       lcall  0x60d, 0xfeb   ; @Read
0093  9a0e020d06       lcall  0x60d, 0x20e   ; @IOCheck
0098  eb2e             jmp    0xc8
009A  bf8c02           mov    di, 0x28c
009D  1e               push   ds
009E  57               push   di
009F  9a59100d06       lcall  0x60d, 0x1059   ; @BlockRead
00A4  8846ff           mov    byte ptr [bp - 1], al
00A7  9a33100d06       lcall  0x60d, 0x1033   ; @FileChk
00AC  9a0e020d06       lcall  0x60d, 0x20e   ; @IOCheck
00B1  bf8c02           mov    di, 0x28c
00B4  1e               push   ds
00B5  57               push   di
00B6  9a59100d06       lcall  0x60d, 0x1059   ; @BlockRead
00BB  8846fe           mov    byte ptr [bp - 2], al
00BE  9a33100d06       lcall  0x60d, 0x1033   ; @FileChk
00C3  9a0e020d06       lcall  0x60d, 0x20e   ; @IOCheck
00C8  8a46ff           mov    al, byte ptr [bp - 1]
00CB  3c2e             cmp    al, 0x2e
00CD  7507             jne    0xd6
00CF  31c0             xor    ax, ax
00D1  8946fc           mov    word ptr [bp - 4], ax
00D4  eb1f             jmp    0xf5
00D6  3c31             cmp    al, 0x31
00D8  7507             jne    0xe1
00DA  c746fc0100       mov    word ptr [bp - 4], 1
00DF  eb14             jmp    0xf5
00E1  3c32             cmp    al, 0x32
00E3  7507             jne    0xec
00E5  c746fc0200       mov    word ptr [bp - 4], 2
00EA  eb09             jmp    0xf5
00EC  3c33             cmp    al, 0x33
00EE  7505             jne    0xf5
00F0  c746fc0300       mov    word ptr [bp - 4], 3
00F5  837ef212         cmp    word ptr [bp - 0xe], 0x12
00F9  7c1c             jl     0x117
00FB  8b46f2           mov    ax, word ptr [bp - 0xe]
00FE  2d1200           sub    ax, 0x12
0101  b90400           mov    cx, 4
0104  d3e0             shl    ax, cl
0106  0346fa           add    ax, word ptr [bp - 6]
0109  8946f6           mov    word ptr [bp - 0xa], ax
010C  8b46f8           mov    ax, word ptr [bp - 8]
010F  051400           add    ax, 0x14
0112  8946f4           mov    word ptr [bp - 0xc], ax
0115  eb15             jmp    0x12c
0117  8b46f2           mov    ax, word ptr [bp - 0xe]
011A  48               dec    ax
011B  b90400           mov    cx, 4
011E  d3e0             shl    ax, cl
0120  0346fa           add    ax, word ptr [bp - 6]
0123  8946f6           mov    word ptr [bp - 0xa], ax
0126  8b46f8           mov    ax, word ptr [bp - 8]
0129  8946f4           mov    word ptr [bp - 0xc], ax
012C  837ef223         cmp    word ptr [bp - 0xe], 0x23
0130  7c1a             jl     0x14c
0132  8b46f2           mov    ax, word ptr [bp - 0xe]
0135  2d2300           sub    ax, 0x23
0138  b90400           mov    cx, 4
013B  d3e0             shl    ax, cl
013D  0346fa           add    ax, word ptr [bp - 6]
0140  8946f6           mov    word ptr [bp - 0xa], ax
0143  8b46f8           mov    ax, word ptr [bp - 8]
0146  052800           add    ax, 0x28
0149  8946f4           mov    word ptr [bp - 0xc], ax
014C  ff76f6           push   word ptr [bp - 0xa]
014F  ff76f4           push   word ptr [bp - 0xc]
0152  ff76fc           push   word ptr [bp - 4]
0155  9a2818d302       lcall  0x2d3, 0x1828   ; PutPixel
015A  837efa10         cmp    word ptr [bp - 6], 0x10
015E  7403             je     0x163
0160  e915ff           jmp    0x78
0163  837ef810         cmp    word ptr [bp - 8], 0x10
0167  7403             je     0x16c
0169  e902ff           jmp    0x6e
016C  bf8c02           mov    di, 0x28c
016F  1e               push   ds
0170  57               push   di
0171  9aeb0f0d06       lcall  0x60d, 0xfeb   ; @Read
0176  9a0e020d06       lcall  0x60d, 0x20e   ; @IOCheck
017B  8b7ef2           mov    di, word ptr [bp - 0xe]
017E  d1e7             shl    di, 1
0180  d1e7             shl    di, 1
0182  81c78804         add    di, 0x488
0186  1e               push   ds
0187  57               push   di
0188  ff363405         push   word ptr [0x534]
018C  9a29030d06       lcall  0x60d, 0x329   ; @Randomize
0191  8b46f6           mov    ax, word ptr [bp - 0xa]
0194  2d0f00           sub    ax, 0xf
0197  50               push   ax
0198  8b46f4           mov    ax, word ptr [bp - 0xc]
019B  2d0f00           sub    ax, 0xf
019E  50               push   ax
019F  ff76f6           push   word ptr [bp - 0xa]
01A2  ff76f4           push   word ptr [bp - 0xc]
01A5  8b7ef2           mov    di, word ptr [bp - 0xe]
01A8  d1e7             shl    di, 1
01AA  d1e7             shl    di, 1
01AC  c4bd8804         les    di, ptr [di + 0x488]
01B0  06               push   es
01B1  57               push   di
01B2  9a3f18d302       lcall  0x2d3, 0x183f   ; GetImage
01B7  837ef22a         cmp    word ptr [bp - 0xe], 0x2a
01BB  7403             je     0x1c0
01BD  e9a4fe           jmp    0x64
01C0  bf8c02           mov    di, 0x28c
01C3  1e               push   ds
01C4  57               push   di
01C5  9a910d0d06       lcall  0x60d, 0xd91   ; @Reset
01CA  9a0e020d06       lcall  0x60d, 0x20e   ; @IOCheck
01CF  89ec             mov    sp, bp
01D1  5d               pop    bp
01D2  c3               ret    
01D3  STR 'B_ST.DAT'

;===== proc_01DC  (len 496) =====
01DC  55               push   bp
01DD  89e5             mov    bp, sp
01DF  b80a00           mov    ax, 0xa
01E2  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
01E7  83ec0a           sub    sp, 0xa
01EA  bf8c03           mov    di, 0x38c
01ED  1e               push   ds
01EE  57               push   di
01EF  bfd301           mov    di, 0x1d3
01F2  0e               push   cs
01F3  57               push   di
01F4  9ac80c0d06       lcall  0x60d, 0xcc8   ; @Val
01F9  bf8c03           mov    di, 0x38c
01FC  1e               push   ds
01FD  57               push   di
01FE  9a380d0d06       lcall  0x60d, 0xd38   ; @Assign
0203  9a0e020d06       lcall  0x60d, 0x20e   ; @IOCheck
0208  c706e42a0100     mov    word ptr [0x2ae4], 1
020E  eb04             jmp    0x214
0210  ff06e42a         inc    word ptr [0x2ae4]
0214  bf8c03           mov    di, 0x38c
0217  1e               push   ds
0218  57               push   di
0219  9aeb0f0d06       lcall  0x60d, 0xfeb   ; @Read
021E  9a0e020d06       lcall  0x60d, 0x20e   ; @IOCheck
0223  c746fc0100       mov    word ptr [bp - 4], 1
0228  eb03             jmp    0x22d
022A  ff46fc           inc    word ptr [bp - 4]
022D  c746fe0100       mov    word ptr [bp - 2], 1
0232  eb03             jmp    0x237
0234  ff46fe           inc    word ptr [bp - 2]
0237  837efe14         cmp    word ptr [bp - 2], 0x14
023B  7519             jne    0x256
023D  bf8c03           mov    di, 0x38c
0240  1e               push   ds
0241  57               push   di
0242  9a59100d06       lcall  0x60d, 0x1059   ; @BlockRead
0247  8846f9           mov    byte ptr [bp - 7], al
024A  9aeb0f0d06       lcall  0x60d, 0xfeb   ; @Read
024F  9a0e020d06       lcall  0x60d, 0x20e   ; @IOCheck
0254  eb17             jmp    0x26d
0256  bf8c03           mov    di, 0x38c
0259  1e               push   ds
025A  57               push   di
025B  9a59100d06       lcall  0x60d, 0x1059   ; @BlockRead
0260  8846f9           mov    byte ptr [bp - 7], al
0263  9a33100d06       lcall  0x60d, 0x1033   ; @FileChk
0268  9a0e020d06       lcall  0x60d, 0x20e   ; @IOCheck
026D  807ef92e         cmp    byte ptr [bp - 7], 0x2e
0271  7507             jne    0x27a
0273  c746f60100       mov    word ptr [bp - 0xa], 1
0278  eb0b             jmp    0x285
027A  8a46f9           mov    al, byte ptr [bp - 7]
027D  30e4             xor    ah, ah
027F  2d3000           sub    ax, 0x30
0282  8946f6           mov    word ptr [bp - 0xa], ax
0285  807ef923         cmp    byte ptr [bp - 7], 0x23
0289  750b             jne    0x296
028B  a1ea2a           mov    ax, word ptr [0x2aea]
028E  0306e42a         add    ax, word ptr [0x2ae4]
0292  48               dec    ax
0293  8946f6           mov    word ptr [bp - 0xa], ax
0296  8a46f9           mov    al, byte ptr [bp - 7]
0299  3c61             cmp    al, 0x61
029B  7507             jne    0x2a4
029D  c746f61c00       mov    word ptr [bp - 0xa], 0x1c
02A2  eb14             jmp    0x2b8
02A4  3c62             cmp    al, 0x62
02A6  7507             jne    0x2af
02A8  c746f61d00       mov    word ptr [bp - 0xa], 0x1d
02AD  eb09             jmp    0x2b8
02AF  3c63             cmp    al, 0x63
02B1  7505             jne    0x2b8
02B3  c746f61e00       mov    word ptr [bp - 0xa], 0x1e
02B8  ff76f6           push   word ptr [bp - 0xa]
02BB  8b46fc           mov    ax, word ptr [bp - 4]
02BE  d1e0             shl    ax, 1
02C0  8bd8             mov    bx, ax
02C2  8b46fe           mov    ax, word ptr [bp - 2]
02C5  ba1800           mov    dx, 0x18
02C8  f7e2             mul    dx
02CA  8bc8             mov    cx, ax
02CC  a1e42a           mov    ax, word ptr [0x2ae4]
02CF  bae001           mov    dx, 0x1e0
02D2  f7e2             mul    dx
02D4  8bf8             mov    di, ax
02D6  03f9             add    di, cx
02D8  03fb             add    di, bx
02DA  58               pop    ax
02DB  89853c03         mov    word ptr [di + 0x33c], ax
02DF  ff76f6           push   word ptr [bp - 0xa]
02E2  8b46fc           mov    ax, word ptr [bp - 4]
02E5  d1e0             shl    ax, 1
02E7  8bd8             mov    bx, ax
02E9  8b46fe           mov    ax, word ptr [bp - 2]
02EC  ba1800           mov    dx, 0x18
02EF  f7e2             mul    dx
02F1  8bc8             mov    cx, ax
02F3  a1e42a           mov    ax, word ptr [0x2ae4]
02F6  bae001           mov    dx, 0x1e0
02F9  f7e2             mul    dx
02FB  8bf8             mov    di, ax
02FD  03f9             add    di, cx
02FF  03fb             add    di, bx
0301  58               pop    ax
0302  8985fc15         mov    word ptr [di + 0x15fc], ax
0306  837efe14         cmp    word ptr [bp - 2], 0x14
030A  7403             je     0x30f
030C  e925ff           jmp    0x234
030F  837efc0b         cmp    word ptr [bp - 4], 0xb
0313  7403             je     0x318
0315  e912ff           jmp    0x22a
0318  c746fa0100       mov    word ptr [bp - 6], 1
031D  eb03             jmp    0x322
031F  ff46fa           inc    word ptr [bp - 6]
0322  bf8c03           mov    di, 0x38c
0325  1e               push   ds
0326  57               push   di
0327  9a1a110d06       lcall  0x60d, 0x111a   ; @Close
032C  8bd0             mov    dx, ax
032E  8b46fa           mov    ax, word ptr [bp - 6]
0331  d1e0             shl    ax, 1
0333  8b3ee42a         mov    di, word ptr [0x2ae4]
0337  b103             mov    cl, 3
0339  d3e7             shl    di, cl
033B  03f8             add    di, ax
033D  8995e22a         mov    word ptr [di + 0x2ae2], dx
0341  9a33100d06       lcall  0x60d, 0x1033   ; @FileChk
0346  9a0e020d06       lcall  0x60d, 0x20e   ; @IOCheck
034B  837efa04         cmp    word ptr [bp - 6], 4
034F  75ce             jne    0x31f
0351  bf8c03           mov    di, 0x38c
0354  1e               push   ds
0355  57               push   di
0356  9a1a110d06       lcall  0x60d, 0x111a   ; @Close
035B  8b3ee42a         mov    di, word ptr [0x2ae4]
035F  d1e7             shl    di, 1
0361  8985b42a         mov    word ptr [di + 0x2ab4], ax
0365  9aeb0f0d06       lcall  0x60d, 0xfeb   ; @Read
036A  9a0e020d06       lcall  0x60d, 0x20e   ; @IOCheck
036F  8b3ee42a         mov    di, word ptr [0x2ae4]
0373  d1e7             shl    di, 1
0375  8b85b42a         mov    ax, word ptr [di + 0x2ab4]
0379  8b3ee42a         mov    di, word ptr [0x2ae4]
037D  d1e7             shl    di, 1
037F  8985c82a         mov    word ptr [di + 0x2ac8], ax
0383  c746fe0100       mov    word ptr [bp - 2], 1
0388  eb03             jmp    0x38d
038A  ff46fe           inc    word ptr [bp - 2]
038D  8b46fe           mov    ax, word ptr [bp - 2]
0390  ba1800           mov    dx, 0x18
0393  f7e2             mul    dx
0395  8bc8             mov    cx, ax
0397  a1e42a           mov    ax, word ptr [0x2ae4]
039A  bae001           mov    dx, 0x1e0
039D  f7e2             mul    dx
039F  8bf8             mov    di, ax
03A1  03f9             add    di, cx
03A3  c78554030300     mov    word ptr [di + 0x354], 3
03A9  837efe14         cmp    word ptr [bp - 2], 0x14
03AD  75db             jne    0x38a
03AF  833ee42a0a       cmp    word ptr [0x2ae4], 0xa
03B4  7403             je     0x3b9
03B6  e957fe           jmp    0x210
03B9  bf8c03           mov    di, 0x38c
03BC  1e               push   ds
03BD  57               push   di
03BE  9a910d0d06       lcall  0x60d, 0xd91   ; @Reset
03C3  9a0e020d06       lcall  0x60d, 0x20e   ; @IOCheck
03C8  89ec             mov    sp, bp
03CA  5d               pop    bp
03CB  c3               ret    

;===== proc_03CC  (len 35) =====
03CC  55               push   bp
03CD  89e5             mov    bp, sp
03CF  b80200           mov    ax, 2
03D2  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
03D7  83ec02           sub    sp, 2
03DA  8b4604           mov    ax, word ptr [bp + 4]
03DD  48               dec    ax
03DE  b90400           mov    cx, 4
03E1  d3e0             shl    ax, cl
03E3  8946fe           mov    word ptr [bp - 2], ax
03E6  8b46fe           mov    ax, word ptr [bp - 2]
03E9  89ec             mov    sp, bp
03EB  5d               pop    bp
03EC  c20200           ret    2

;===== proc_03EF  (len 65) =====
03EF  55               push   bp
03F0  89e5             mov    bp, sp
03F2  b80400           mov    ax, 4
03F5  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
03FA  83ec04           sub    sp, 4
03FD  ff7608           push   word ptr [bp + 8]
0400  e8c9ff           call   0x3cc
0403  8946fe           mov    word ptr [bp - 2], ax
0406  ff7606           push   word ptr [bp + 6]
0409  e8c0ff           call   0x3cc
040C  8946fc           mov    word ptr [bp - 4], ax
040F  ff76fe           push   word ptr [bp - 2]
0412  ff76fc           push   word ptr [bp - 4]
0415  8b7e04           mov    di, word ptr [bp + 4]
0418  d1e7             shl    di, 1
041A  d1e7             shl    di, 1
041C  c4bd8804         les    di, ptr [di + 0x488]
0420  06               push   es
0421  57               push   di
0422  31c0             xor    ax, ax
0424  50               push   ax
0425  9a340ed302       lcall  0x2d3, 0xe34   ; PutImage
042A  89ec             mov    sp, bp
042C  5d               pop    bp
042D  c20600           ret    6

;===== proc_0430  (len 208) =====
0430  55               push   bp
0431  89e5             mov    bp, sp
0433  b80400           mov    ax, 4
0436  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
043B  83ec04           sub    sp, 4
043E  c746fc8200       mov    word ptr [bp - 4], 0x82
0443  31c0             xor    ax, ax
0445  8946fe           mov    word ptr [bp - 2], ax
0448  eb03             jmp    0x44d
044A  ff46fe           inc    word ptr [bp - 2]
044D  8b46fe           mov    ax, word ptr [bp - 2]
0450  b90400           mov    cx, 4
0453  d3e0             shl    ax, cl
0455  d1e0             shl    ax, 1
0457  050800           add    ax, 8
045A  50               push   ax
045B  ff76fc           push   word ptr [bp - 4]
045E  8b7e04           mov    di, word ptr [bp + 4]
0461  d1e7             shl    di, 1
0463  d1e7             shl    di, 1
0465  c4bd8804         les    di, ptr [di + 0x488]
0469  06               push   es
046A  57               push   di
046B  31c0             xor    ax, ax
046D  50               push   ax
046E  9a340ed302       lcall  0x2d3, 0xe34   ; PutImage
0473  837efe09         cmp    word ptr [bp - 2], 9
0477  75d1             jne    0x44a
0479  89ec             mov    sp, bp
047B  5d               pop    bp
047C  c20200           ret    2
047F  STR '[Bong Bong]'
048B  STR 'Programed By Lee T.K.'
04A1  STR 'Yonsei COMSCI 1989'
04B4  STR ' ==== Key ===='
04C3  STR ' [Esc]  : Quit'
04D2  STR ' Cursor : Move'
04E1  STR ' Space  : jump'
04F0  STR 'Press Any Key !'

;===== proc_0500  (len 731) =====
0500  55               push   bp
0501  89e5             mov    bp, sp
0503  b80200           mov    ax, 2
0506  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
050B  83ec02           sub    sp, 2
050E  9a280bd302       lcall  0x2d3, 0xb28   ; ClearDevice
0513  31c0             xor    ax, ax
0515  50               push   ax
0516  b80300           mov    ax, 3
0519  50               push   ax
051A  b80100           mov    ax, 1
051D  50               push   ax
051E  9af30bd302       lcall  0x2d3, 0xbf3   ; SetFillStyle
0523  31c0             xor    ax, ax
0525  50               push   ax
0526  31c0             xor    ax, ax
0528  50               push   ax
0529  b83f01           mov    ax, 0x13f
052C  50               push   ax
052D  b8c700           mov    ax, 0xc7
0530  50               push   ax
0531  9a2f0cd302       lcall  0x2d3, 0xc2f   ; Rectangle?
0536  b87300           mov    ax, 0x73
0539  50               push   ax
053A  b81e00           mov    ax, 0x1e
053D  50               push   ax
053E  bf7f04           mov    di, 0x47f
0541  0e               push   cs
0542  57               push   di
0543  9a5617d302       lcall  0x2d3, 0x1756   ; OutTextXY
0548  b85000           mov    ax, 0x50
054B  50               push   ax
054C  b83200           mov    ax, 0x32
054F  50               push   ax
0550  bf8b04           mov    di, 0x48b
0553  0e               push   cs
0554  57               push   di
0555  9a5617d302       lcall  0x2d3, 0x1756   ; OutTextXY
055A  b85a00           mov    ax, 0x5a
055D  50               push   ax
055E  b83c00           mov    ax, 0x3c
0561  50               push   ax
0562  bfa104           mov    di, 0x4a1
0565  0e               push   cs
0566  57               push   di
0567  9a5617d302       lcall  0x2d3, 0x1756   ; OutTextXY
056C  b86400           mov    ax, 0x64
056F  50               push   ax
0570  b85000           mov    ax, 0x50
0573  50               push   ax
0574  bfb404           mov    di, 0x4b4
0577  0e               push   cs
0578  57               push   di
0579  9a5617d302       lcall  0x2d3, 0x1756   ; OutTextXY
057E  b86400           mov    ax, 0x64
0581  50               push   ax
0582  b85a00           mov    ax, 0x5a
0585  50               push   ax
0586  bfc304           mov    di, 0x4c3
0589  0e               push   cs
058A  57               push   di
058B  9a5617d302       lcall  0x2d3, 0x1756   ; OutTextXY
0590  b86400           mov    ax, 0x64
0593  50               push   ax
0594  b86400           mov    ax, 0x64
0597  50               push   ax
0598  bfd204           mov    di, 0x4d2
059B  0e               push   cs
059C  57               push   di
059D  9a5617d302       lcall  0x2d3, 0x1756   ; OutTextXY
05A2  b86400           mov    ax, 0x64
05A5  50               push   ax
05A6  b86e00           mov    ax, 0x6e
05A9  50               push   ax
05AA  bfe104           mov    di, 0x4e1
05AD  0e               push   cs
05AE  57               push   di
05AF  9a5617d302       lcall  0x2d3, 0x1756   ; OutTextXY
05B4  c746fe0a00       mov    word ptr [bp - 2], 0xa
05B9  ff76fe           push   word ptr [bp - 2]
05BC  e871fe           call   0x430
05BF  9af3027102       lcall  0x271, 0x2f3   ; NoSound
05C4  b80b02           mov    ax, 0x20b
05C7  50               push   ax
05C8  9ac6027102       lcall  0x271, 0x2c6   ; Sound
05CD  b89001           mov    ax, 0x190
05D0  50               push   ax
05D1  9a9e027102       lcall  0x271, 0x29e   ; Delay
05D6  9af3027102       lcall  0x271, 0x2f3   ; NoSound
05DB  b81800           mov    ax, 0x18
05DE  2b46fe           sub    ax, word ptr [bp - 2]
05E1  8946fe           mov    word ptr [bp - 2], ax
05E4  ff76fe           push   word ptr [bp - 2]
05E7  e846fe           call   0x430
05EA  b81e00           mov    ax, 0x1e
05ED  50               push   ax
05EE  9a9e027102       lcall  0x271, 0x29e   ; Delay
05F3  b89302           mov    ax, 0x293
05F6  50               push   ax
05F7  9ac6027102       lcall  0x271, 0x2c6   ; Sound
05FC  b89001           mov    ax, 0x190
05FF  50               push   ax
0600  9a9e027102       lcall  0x271, 0x29e   ; Delay
0605  9af3027102       lcall  0x271, 0x2f3   ; NoSound
060A  b81800           mov    ax, 0x18
060D  2b46fe           sub    ax, word ptr [bp - 2]
0610  8946fe           mov    word ptr [bp - 2], ax
0613  ff76fe           push   word ptr [bp - 2]
0616  e817fe           call   0x430
0619  b81e00           mov    ax, 0x1e
061C  50               push   ax
061D  9a9e027102       lcall  0x271, 0x29e   ; Delay
0622  b81003           mov    ax, 0x310
0625  50               push   ax
0626  9ac6027102       lcall  0x271, 0x2c6   ; Sound
062B  b8c800           mov    ax, 0xc8
062E  50               push   ax
062F  9a9e027102       lcall  0x271, 0x29e   ; Delay
0634  9af3027102       lcall  0x271, 0x2f3   ; NoSound
0639  b81800           mov    ax, 0x18
063C  2b46fe           sub    ax, word ptr [bp - 2]
063F  8946fe           mov    word ptr [bp - 2], ax
0642  ff76fe           push   word ptr [bp - 2]
0645  e8e8fd           call   0x430
0648  b81e00           mov    ax, 0x1e
064B  50               push   ax
064C  9a9e027102       lcall  0x271, 0x29e   ; Delay
0651  b89302           mov    ax, 0x293
0654  50               push   ax
0655  9ac6027102       lcall  0x271, 0x2c6   ; Sound
065A  b8c800           mov    ax, 0xc8
065D  50               push   ax
065E  9a9e027102       lcall  0x271, 0x29e   ; Delay
0663  9af3027102       lcall  0x271, 0x2f3   ; NoSound
0668  b81800           mov    ax, 0x18
066B  2b46fe           sub    ax, word ptr [bp - 2]
066E  8946fe           mov    word ptr [bp - 2], ax
0671  ff76fe           push   word ptr [bp - 2]
0674  e8b9fd           call   0x430
0677  b81e00           mov    ax, 0x1e
067A  50               push   ax
067B  9a9e027102       lcall  0x271, 0x29e   ; Delay
0680  b80b02           mov    ax, 0x20b
0683  50               push   ax
0684  9ac6027102       lcall  0x271, 0x2c6   ; Sound
0689  b89001           mov    ax, 0x190
068C  50               push   ax
068D  9a9e027102       lcall  0x271, 0x29e   ; Delay
0692  9af3027102       lcall  0x271, 0x2f3   ; NoSound
0697  b81800           mov    ax, 0x18
069A  2b46fe           sub    ax, word ptr [bp - 2]
069D  8946fe           mov    word ptr [bp - 2], ax
06A0  ff76fe           push   word ptr [bp - 2]
06A3  e88afd           call   0x430
06A6  b81e00           mov    ax, 0x1e
06A9  50               push   ax
06AA  9a9e027102       lcall  0x271, 0x29e   ; Delay
06AF  b87003           mov    ax, 0x370
06B2  50               push   ax
06B3  9ac6027102       lcall  0x271, 0x2c6   ; Sound
06B8  b8c800           mov    ax, 0xc8
06BB  50               push   ax
06BC  9a9e027102       lcall  0x271, 0x29e   ; Delay
06C1  9af3027102       lcall  0x271, 0x2f3   ; NoSound
06C6  b81800           mov    ax, 0x18
06C9  2b46fe           sub    ax, word ptr [bp - 2]
06CC  8946fe           mov    word ptr [bp - 2], ax
06CF  ff76fe           push   word ptr [bp - 2]
06D2  e85bfd           call   0x430
06D5  b81e00           mov    ax, 0x1e
06D8  50               push   ax
06D9  9a9e027102       lcall  0x271, 0x29e   ; Delay
06DE  b81003           mov    ax, 0x310
06E1  50               push   ax
06E2  9ac6027102       lcall  0x271, 0x2c6   ; Sound
06E7  b8c800           mov    ax, 0xc8
06EA  50               push   ax
06EB  9a9e027102       lcall  0x271, 0x29e   ; Delay
06F0  9af3027102       lcall  0x271, 0x2f3   ; NoSound
06F5  b81800           mov    ax, 0x18
06F8  2b46fe           sub    ax, word ptr [bp - 2]
06FB  8946fe           mov    word ptr [bp - 2], ax
06FE  ff76fe           push   word ptr [bp - 2]
0701  e82cfd           call   0x430
0704  b81e00           mov    ax, 0x1e
0707  50               push   ax
0708  9a9e027102       lcall  0x271, 0x29e   ; Delay
070D  b87003           mov    ax, 0x370
0710  50               push   ax
0711  9ac6027102       lcall  0x271, 0x2c6   ; Sound
0716  b8c800           mov    ax, 0xc8
0719  50               push   ax
071A  9a9e027102       lcall  0x271, 0x29e   ; Delay
071F  9af3027102       lcall  0x271, 0x2f3   ; NoSound
0724  b81800           mov    ax, 0x18
0727  2b46fe           sub    ax, word ptr [bp - 2]
072A  8946fe           mov    word ptr [bp - 2], ax
072D  ff76fe           push   word ptr [bp - 2]
0730  e8fdfc           call   0x430
0733  b81e00           mov    ax, 0x1e
0736  50               push   ax
0737  9a9e027102       lcall  0x271, 0x29e   ; Delay
073C  b8dc03           mov    ax, 0x3dc
073F  50               push   ax
0740  9ac6027102       lcall  0x271, 0x2c6   ; Sound
0745  b8c800           mov    ax, 0xc8
0748  50               push   ax
0749  9a9e027102       lcall  0x271, 0x29e   ; Delay
074E  9af3027102       lcall  0x271, 0x2f3   ; NoSound
0753  b81800           mov    ax, 0x18
0756  2b46fe           sub    ax, word ptr [bp - 2]
0759  8946fe           mov    word ptr [bp - 2], ax
075C  ff76fe           push   word ptr [bp - 2]
075F  e8cefc           call   0x430
0762  b81e00           mov    ax, 0x1e
0765  50               push   ax
0766  9a9e027102       lcall  0x271, 0x29e   ; Delay
076B  b81704           mov    ax, 0x417
076E  50               push   ax
076F  9ac6027102       lcall  0x271, 0x2c6   ; Sound
0774  b89001           mov    ax, 0x190
0777  50               push   ax
0778  9a9e027102       lcall  0x271, 0x29e   ; Delay
077D  9af3027102       lcall  0x271, 0x2f3   ; NoSound
0782  b81800           mov    ax, 0x18
0785  2b46fe           sub    ax, word ptr [bp - 2]
0788  8946fe           mov    word ptr [bp - 2], ax
078B  ff76fe           push   word ptr [bp - 2]
078E  e89ffc           call   0x430
0791  b81e00           mov    ax, 0x1e
0794  50               push   ax
0795  9a9e027102       lcall  0x271, 0x29e   ; Delay
079A  b80b02           mov    ax, 0x20b
079D  50               push   ax
079E  9ac6027102       lcall  0x271, 0x2c6   ; Sound
07A3  b8c800           mov    ax, 0xc8
07A6  50               push   ax
07A7  9a9e027102       lcall  0x271, 0x29e   ; Delay
07AC  9af3027102       lcall  0x271, 0x2f3   ; NoSound
07B1  b86400           mov    ax, 0x64
07B4  50               push   ax
07B5  b8aa00           mov    ax, 0xaa
07B8  50               push   ax
07B9  bff004           mov    di, 0x4f0
07BC  0e               push   cs
07BD  57               push   di
07BE  9a5617d302       lcall  0x2d3, 0x1756   ; OutTextXY
07C3  9a0c037102       lcall  0x271, 0x30c   ; ReadKey
07C8  a28a02           mov    byte ptr [0x28a], al
07CB  803e8a021b       cmp    byte ptr [0x28a], 0x1b
07D0  7505             jne    0x7d7
07D2  c606a62b01       mov    byte ptr [0x2ba6], 1
07D7  89ec             mov    sp, bp
07D9  5d               pop    bp
07DA  c3               ret    

;===== proc_07DB  (len 503) =====
07DB  55               push   bp
07DC  89e5             mov    bp, sp
07DE  b80e00           mov    ax, 0xe
07E1  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
07E6  83ec0e           sub    sp, 0xe
07E9  9a280bd302       lcall  0x2d3, 0xb28   ; ClearDevice
07EE  c746fc0100       mov    word ptr [bp - 4], 1
07F3  eb03             jmp    0x7f8
07F5  ff46fc           inc    word ptr [bp - 4]
07F8  c746fe0100       mov    word ptr [bp - 2], 1
07FD  eb03             jmp    0x802
07FF  ff46fe           inc    word ptr [bp - 2]
0802  8b46fc           mov    ax, word ptr [bp - 4]
0805  d1e0             shl    ax, 1
0807  8bd8             mov    bx, ax
0809  8b46fe           mov    ax, word ptr [bp - 2]
080C  ba1800           mov    dx, 0x18
080F  f7e2             mul    dx
0811  8bc8             mov    cx, ax
0813  8b4604           mov    ax, word ptr [bp + 4]
0816  bae001           mov    dx, 0x1e0
0819  f7e2             mul    dx
081B  8bf8             mov    di, ax
081D  03f9             add    di, cx
081F  03fb             add    di, bx
0821  ffb5fc15         push   word ptr [di + 0x15fc]
0825  8b46fc           mov    ax, word ptr [bp - 4]
0828  d1e0             shl    ax, 1
082A  8bd8             mov    bx, ax
082C  8b46fe           mov    ax, word ptr [bp - 2]
082F  ba1800           mov    dx, 0x18
0832  f7e2             mul    dx
0834  8bc8             mov    cx, ax
0836  8b4604           mov    ax, word ptr [bp + 4]
0839  bae001           mov    dx, 0x1e0
083C  f7e2             mul    dx
083E  8bf8             mov    di, ax
0840  03f9             add    di, cx
0842  03fb             add    di, bx
0844  58               pop    ax
0845  89853c03         mov    word ptr [di + 0x33c], ax
0849  837efe14         cmp    word ptr [bp - 2], 0x14
084D  75b0             jne    0x7ff
084F  837efc0b         cmp    word ptr [bp - 4], 0xb
0853  75a0             jne    0x7f5
0855  c746fc0100       mov    word ptr [bp - 4], 1
085A  eb03             jmp    0x85f
085C  ff46fc           inc    word ptr [bp - 4]
085F  c746fe0100       mov    word ptr [bp - 2], 1
0864  eb03             jmp    0x869
0866  ff46fe           inc    word ptr [bp - 2]
0869  8b46fc           mov    ax, word ptr [bp - 4]
086C  d1e0             shl    ax, 1
086E  8bd8             mov    bx, ax
0870  8b46fe           mov    ax, word ptr [bp - 2]
0873  ba1800           mov    dx, 0x18
0876  f7e2             mul    dx
0878  8bc8             mov    cx, ax
087A  a1e42a           mov    ax, word ptr [0x2ae4]
087D  bae001           mov    dx, 0x1e0
0880  f7e2             mul    dx
0882  8bf8             mov    di, ax
0884  03f9             add    di, cx
0886  03fb             add    di, bx
0888  83bd3c0300       cmp    word ptr [di + 0x33c], 0
088D  7449             je     0x8d8
088F  8b46fe           mov    ax, word ptr [bp - 2]
0892  48               dec    ax
0893  b90400           mov    cx, 4
0896  d3e0             shl    ax, cl
0898  50               push   ax
0899  8b46fc           mov    ax, word ptr [bp - 4]
089C  48               dec    ax
089D  b90400           mov    cx, 4
08A0  d3e0             shl    ax, cl
08A2  50               push   ax
08A3  8b46fc           mov    ax, word ptr [bp - 4]
08A6  d1e0             shl    ax, 1
08A8  8bd8             mov    bx, ax
08AA  8b46fe           mov    ax, word ptr [bp - 2]
08AD  ba1800           mov    dx, 0x18
08B0  f7e2             mul    dx
08B2  8bc8             mov    cx, ax
08B4  a1e42a           mov    ax, word ptr [0x2ae4]
08B7  bae001           mov    dx, 0x1e0
08BA  f7e2             mul    dx
08BC  8bf8             mov    di, ax
08BE  03f9             add    di, cx
08C0  03fb             add    di, bx
08C2  8bbd3c03         mov    di, word ptr [di + 0x33c]
08C6  d1e7             shl    di, 1
08C8  d1e7             shl    di, 1
08CA  c4bd8804         les    di, ptr [di + 0x488]
08CE  06               push   es
08CF  57               push   di
08D0  31c0             xor    ax, ax
08D2  50               push   ax
08D3  9a340ed302       lcall  0x2d3, 0xe34   ; PutImage
08D8  837efe14         cmp    word ptr [bp - 2], 0x14
08DC  7588             jne    0x866
08DE  837efc0b         cmp    word ptr [bp - 4], 0xb
08E2  7403             je     0x8e7
08E4  e975ff           jmp    0x85c
08E7  c746fe0100       mov    word ptr [bp - 2], 1
08EC  eb03             jmp    0x8f1
08EE  ff46fe           inc    word ptr [bp - 2]
08F1  8b46fe           mov    ax, word ptr [bp - 2]
08F4  48               dec    ax
08F5  b90400           mov    cx, 4
08F8  d3e0             shl    ax, cl
08FA  50               push   ax
08FB  b8b000           mov    ax, 0xb0
08FE  50               push   ax
08FF  c43e9804         les    di, ptr [0x498]
0903  06               push   es
0904  57               push   di
0905  31c0             xor    ax, ax
0907  50               push   ax
0908  9a340ed302       lcall  0x2d3, 0xe34   ; PutImage
090D  837efe14         cmp    word ptr [bp - 2], 0x14
0911  75db             jne    0x8ee
0913  c746fe1400       mov    word ptr [bp - 2], 0x14
0918  b80300           mov    ax, 3
091B  50               push   ax
091C  9a0616d302       lcall  0x2d3, 0x1606   ; SetColor
0921  31c0             xor    ax, ax
0923  8946fc           mov    word ptr [bp - 4], ax
0926  833ee42a01       cmp    word ptr [0x2ae4], 1
092B  7433             je     0x960
092D  a1e42a           mov    ax, word ptr [0x2ae4]
0930  40               inc    ax
0931  2d0200           sub    ax, 2
0934  8946f2           mov    word ptr [bp - 0xe], ax
0937  b80100           mov    ax, 1
093A  3b46f2           cmp    ax, word ptr [bp - 0xe]
093D  7f21             jg     0x960
093F  8946fc           mov    word ptr [bp - 4], ax
0942  eb03             jmp    0x947
0944  ff46fc           inc    word ptr [bp - 4]
0947  ff76fe           push   word ptr [bp - 2]
094A  ff76fc           push   word ptr [bp - 4]
094D  8b46fc           mov    ax, word ptr [bp - 4]
0950  051200           add    ax, 0x12
0953  48               dec    ax
0954  50               push   ax
0955  e897fa           call   0x3ef
0958  8b46fc           mov    ax, word ptr [bp - 4]
095B  3b46f2           cmp    ax, word ptr [bp - 0xe]
095E  75e4             jne    0x944
0960  b83001           mov    ax, 0x130
0963  50               push   ax
0964  31c0             xor    ax, ax
0966  50               push   ax
0967  b83001           mov    ax, 0x130
096A  50               push   ax
096B  b8a000           mov    ax, 0xa0
096E  50               push   ax
096F  9acc14d302       lcall  0x2d3, 0x14cc   ; Line
0974  a1e62a           mov    ax, word ptr [0x2ae6]
0977  99               cdq    
0978  52               push   dx
0979  50               push   ax
097A  31c0             xor    ax, ax
097C  50               push   ax
097D  8d7ef4           lea    di, [bp - 0xc]
0980  16               push   ss
0981  57               push   di
0982  b80600           mov    ax, 6
0985  50               push   ax
0986  9a4c0c0d06       lcall  0x60d, 0xc4c   ; @Str
098B  b83001           mov    ax, 0x130
098E  50               push   ax
098F  8b46fc           mov    ax, word ptr [bp - 4]
0992  b90400           mov    cx, 4
0995  d3e0             shl    ax, cl
0997  050400           add    ax, 4
099A  50               push   ax
099B  8d7ef4           lea    di, [bp - 0xc]
099E  16               push   ss
099F  57               push   di
09A0  9a5617d302       lcall  0x2d3, 0x1756   ; OutTextXY
09A5  89ec             mov    sp, bp
09A7  5d               pop    bp
09A8  c20200           ret    2
09AB  STR 'HI-SCORE030000 SCORE000000 TIME100   3'

;===== proc_09D2  (len 93) =====
09D2  55               push   bp
09D3  89e5             mov    bp, sp
09D5  31c0             xor    ax, ax
09D7  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
09DC  31c0             xor    ax, ax
09DE  50               push   ax
09DF  b80200           mov    ax, 2
09E2  50               push   ax
09E3  9aef0ed302       lcall  0x2d3, 0xeef   ; SetTextJustify
09E8  31c0             xor    ax, ax
09EA  50               push   ax
09EB  31c0             xor    ax, ax
09ED  50               push   ax
09EE  b80100           mov    ax, 1
09F1  50               push   ax
09F2  9a330fd302       lcall  0x2d3, 0xf33   ; OutText?
09F7  b81401           mov    ax, 0x114
09FA  50               push   ax
09FB  b8b900           mov    ax, 0xb9
09FE  50               push   ax
09FF  c43ec004         les    di, ptr [0x4c0]
0A03  06               push   es
0A04  57               push   di
0A05  b80100           mov    ax, 1
0A08  50               push   ax
0A09  9a340ed302       lcall  0x2d3, 0xe34   ; PutImage
0A0E  31c0             xor    ax, ax
0A10  50               push   ax
0A11  b8be00           mov    ax, 0xbe
0A14  50               push   ax
0A15  bfab09           mov    di, 0x9ab
0A18  0e               push   cs
0A19  57               push   di
0A1A  9a5617d302       lcall  0x2d3, 0x1756   ; OutTextXY
0A1F  b80100           mov    ax, 1
0A22  50               push   ax
0A23  31c0             xor    ax, ax
0A25  50               push   ax
0A26  9a780cd302       lcall  0x2d3, 0xc78   ; SetTextStyle?
0A2B  89ec             mov    sp, bp
0A2D  5d               pop    bp
0A2E  c3               ret    

;===== proc_0A2F  (len 227) =====
0A2F  55               push   bp
0A30  89e5             mov    bp, sp
0A32  b81001           mov    ax, 0x110
0A35  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
0A3A  81ec1001         sub    sp, 0x110
0A3E  c646f200         mov    byte ptr [bp - 0xe], 0
0A42  31c0             xor    ax, ax
0A44  8946fc           mov    word ptr [bp - 4], ax
0A47  c44604           les    ax, ptr [bp + 4]
0A4A  8cc2             mov    dx, es
0A4C  b90a00           mov    cx, 0xa
0A4F  31db             xor    bx, bx
0A51  9a94020d06       lcall  0x60d, 0x294   ; @LongDiv
0A56  89c8             mov    ax, cx
0A58  89da             mov    dx, bx
0A5A  8946fe           mov    word ptr [bp - 2], ax
0A5D  c44604           les    ax, ptr [bp + 4]
0A60  8cc2             mov    dx, es
0A62  b90a00           mov    cx, 0xa
0A65  31db             xor    bx, bx
0A67  9a94020d06       lcall  0x60d, 0x294   ; @LongDiv
0A6C  894604           mov    word ptr [bp + 4], ax
0A6F  895606           mov    word ptr [bp + 6], dx
0A72  8dbef2fe         lea    di, [bp - 0x10e]
0A76  16               push   ss
0A77  57               push   di
0A78  8b46fe           mov    ax, word ptr [bp - 2]
0A7B  053000           add    ax, 0x30
0A7E  50               push   ax
0A7F  9a4f070d06       lcall  0x60d, 0x74f   ; @Concat
0A84  8d7ef2           lea    di, [bp - 0xe]
0A87  16               push   ss
0A88  57               push   di
0A89  9ac1060d06       lcall  0x60d, 0x6c1   ; @StrCmp
0A8E  8d7ef2           lea    di, [bp - 0xe]
0A91  16               push   ss
0A92  57               push   di
0A93  b80600           mov    ax, 6
0A96  50               push   ax
0A97  9a4e060d06       lcall  0x60d, 0x64e   ; @StrCopy
0A9C  8b46fc           mov    ax, word ptr [bp - 4]
0A9F  40               inc    ax
0AA0  8946fc           mov    word ptr [bp - 4], ax
0AA3  8b4604           mov    ax, word ptr [bp + 4]
0AA6  0b4606           or     ax, word ptr [bp + 6]
0AA9  759c             jne    0xa47
0AAB  837efc06         cmp    word ptr [bp - 4], 6
0AAF  7448             je     0xaf9
0AB1  b80600           mov    ax, 6
0AB4  2b46fc           sub    ax, word ptr [bp - 4]
0AB7  8946f0           mov    word ptr [bp - 0x10], ax
0ABA  b80100           mov    ax, 1
0ABD  3b46f0           cmp    ax, word ptr [bp - 0x10]
0AC0  7f37             jg     0xaf9
0AC2  8946fa           mov    word ptr [bp - 6], ax
0AC5  eb03             jmp    0xaca
0AC7  ff46fa           inc    word ptr [bp - 6]
0ACA  8dbef0fe         lea    di, [bp - 0x110]
0ACE  16               push   ss
0ACF  57               push   di
0AD0  8a4608           mov    al, byte ptr [bp + 8]
0AD3  50               push   ax
0AD4  9a4f070d06       lcall  0x60d, 0x74f   ; @Concat
0AD9  8d7ef2           lea    di, [bp - 0xe]
0ADC  16               push   ss
0ADD  57               push   di
0ADE  9ac1060d06       lcall  0x60d, 0x6c1   ; @StrCmp
0AE3  8d7ef2           lea    di, [bp - 0xe]
0AE6  16               push   ss
0AE7  57               push   di
0AE8  b80600           mov    ax, 6
0AEB  50               push   ax
0AEC  9a4e060d06       lcall  0x60d, 0x64e   ; @StrCopy
0AF1  8b46fa           mov    ax, word ptr [bp - 6]
0AF4  3b46f0           cmp    ax, word ptr [bp - 0x10]
0AF7  75ce             jne    0xac7
0AF9  8d7ef2           lea    di, [bp - 0xe]
0AFC  16               push   ss
0AFD  57               push   di
0AFE  c47e0a           les    di, ptr [bp + 0xa]
0B01  06               push   es
0B02  57               push   di
0B03  b80600           mov    ax, 6
0B06  50               push   ax
0B07  9a4e060d06       lcall  0x60d, 0x64e   ; @StrCopy
0B0C  89ec             mov    sp, bp
0B0E  5d               pop    bp
0B0F  c20600           ret    6

;===== proc_0B12  (len 922) =====
0B12  55               push   bp
0B13  89e5             mov    bp, sp
0B15  b81001           mov    ax, 0x110
0B18  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
0B1D  81ec1001         sub    sp, 0x110
0B21  8d7ef2           lea    di, [bp - 0xe]
0B24  16               push   ss
0B25  57               push   di
0B26  8d7ef0           lea    di, [bp - 0x10]
0B29  16               push   ss
0B2A  57               push   di
0B2B  bf9a2b           mov    di, 0x2b9a
0B2E  1e               push   ds
0B2F  57               push   di
0B30  bf9c2b           mov    di, 0x2b9c
0B33  1e               push   ds
0B34  57               push   di
0B35  9a36006902       lcall  0x269, 0x36   ; GetTime
0B3A  c746fe0100       mov    word ptr [bp - 2], 1
0B3F  eb03             jmp    0xb44
0B41  ff46fe           inc    word ptr [bp - 2]
0B44  8b46fe           mov    ax, word ptr [bp - 2]
0B47  d1e0             shl    ax, 1
0B49  8b3ee42a         mov    di, word ptr [0x2ae4]
0B4D  b103             mov    cl, 3
0B4F  d3e7             shl    di, cl
0B51  03f8             add    di, ax
0B53  83bde22a00       cmp    word ptr [di + 0x2ae2], 0
0B58  7503             jne    0xb5d
0B5A  e96a01           jmp    0xcc7
0B5D  8b46fe           mov    ax, word ptr [bp - 2]
0B60  d1e0             shl    ax, 1
0B62  8b3ee42a         mov    di, word ptr [0x2ae4]
0B66  b103             mov    cl, 3
0B68  d3e7             shl    di, cl
0B6A  03f8             add    di, ax
0B6C  8b85e22a         mov    ax, word ptr [di + 0x2ae2]
0B70  99               cdq    
0B71  9a140b0d06       lcall  0x60d, 0xb14   ; @Rand?
0B76  52               push   dx
0B77  53               push   bx
0B78  50               push   ax
0B79  a19c2b           mov    ax, word ptr [0x2b9c]
0B7C  31d2             xor    dx, dx
0B7E  9a140b0d06       lcall  0x60d, 0xb14   ; @Rand?
0B83  8b7efe           mov    di, word ptr [bp - 2]
0B86  d1e7             shl    di, 1
0B88  8bf7             mov    si, di
0B8A  d1e7             shl    di, 1
0B8C  03fe             add    di, si
0B8E  8b8d5e2b         mov    cx, word ptr [di + 0x2b5e]
0B92  8bb5602b         mov    si, word ptr [di + 0x2b60]
0B96  8bbd622b         mov    di, word ptr [di + 0x2b62]
0B9A  9af40a0d06       lcall  0x60d, 0xaf4   ; @f0
0B9F  59               pop    cx
0BA0  5e               pop    si
0BA1  5f               pop    di
0BA2  9a000b0d06       lcall  0x60d, 0xb00   ; @f1
0BA7  b98700           mov    cx, 0x87
0BAA  31f6             xor    si, si
0BAC  bf0048           mov    di, 0x4800
0BAF  9a060b0d06       lcall  0x60d, 0xb06   ; @f2
0BB4  b98100           mov    cx, 0x81
0BB7  31f6             xor    si, si
0BB9  31ff             xor    di, di
0BBB  9a100b0d06       lcall  0x60d, 0xb10   ; @f3
0BC0  7303             jae    0xbc5
0BC2  e90201           jmp    0xcc7
0BC5  a19c2b           mov    ax, word ptr [0x2b9c]
0BC8  31d2             xor    dx, dx
0BCA  9a140b0d06       lcall  0x60d, 0xb14   ; @Rand?
0BCF  8b7efe           mov    di, word ptr [bp - 2]
0BD2  d1e7             shl    di, 1
0BD4  8bf7             mov    si, di
0BD6  d1e7             shl    di, 1
0BD8  03fe             add    di, si
0BDA  89855e2b         mov    word ptr [di + 0x2b5e], ax
0BDE  899d602b         mov    word ptr [di + 0x2b60], bx
0BE2  8995622b         mov    word ptr [di + 0x2b62], dx
0BE6  8b7efe           mov    di, word ptr [bp - 2]
0BE9  d1e7             shl    di, 1
0BEB  ffb54a2b         push   word ptr [di + 0x2b4a]
0BEF  8b7efe           mov    di, word ptr [bp - 2]
0BF2  d1e7             shl    di, 1
0BF4  ffb5522b         push   word ptr [di + 0x2b52]
0BF8  8b7efe           mov    di, word ptr [bp - 2]
0BFB  d1e7             shl    di, 1
0BFD  8b85522b         mov    ax, word ptr [di + 0x2b52]
0C01  d1e0             shl    ax, 1
0C03  8bd8             mov    bx, ax
0C05  8b7efe           mov    di, word ptr [bp - 2]
0C08  d1e7             shl    di, 1
0C0A  8b854a2b         mov    ax, word ptr [di + 0x2b4a]
0C0E  ba1800           mov    dx, 0x18
0C11  f7e2             mul    dx
0C13  8bc8             mov    cx, ax
0C15  a1e42a           mov    ax, word ptr [0x2ae4]
0C18  bae001           mov    dx, 0x1e0
0C1B  f7e2             mul    dx
0C1D  8bf8             mov    di, ax
0C1F  03f9             add    di, cx
0C21  03fb             add    di, bx
0C23  ffb53c03         push   word ptr [di + 0x33c]
0C27  e8c5f7           call   0x3ef
0C2A  8b7efe           mov    di, word ptr [bp - 2]
0C2D  d1e7             shl    di, 1
0C2F  8b854a2b         mov    ax, word ptr [di + 0x2b4a]
0C33  8b7efe           mov    di, word ptr [bp - 2]
0C36  d1e7             shl    di, 1
0C38  03853a2b         add    ax, word ptr [di + 0x2b3a]
0C3C  8b7efe           mov    di, word ptr [bp - 2]
0C3F  d1e7             shl    di, 1
0C41  89854a2b         mov    word ptr [di + 0x2b4a], ax
0C45  8b7efe           mov    di, word ptr [bp - 2]
0C48  d1e7             shl    di, 1
0C4A  ffb54a2b         push   word ptr [di + 0x2b4a]
0C4E  8b7efe           mov    di, word ptr [bp - 2]
0C51  d1e7             shl    di, 1
0C53  ffb5522b         push   word ptr [di + 0x2b52]
0C57  8b7efe           mov    di, word ptr [bp - 2]
0C5A  d1e7             shl    di, 1
0C5C  ffb55a2b         push   word ptr [di + 0x2b5a]
0C60  e88cf7           call   0x3ef
0C63  8b7efe           mov    di, word ptr [bp - 2]
0C66  d1e7             shl    di, 1
0C68  8b854a2b         mov    ax, word ptr [di + 0x2b4a]
0C6C  3b06822b         cmp    ax, word ptr [0x2b82]
0C70  7514             jne    0xc86
0C72  8b7efe           mov    di, word ptr [bp - 2]
0C75  d1e7             shl    di, 1
0C77  8b85522b         mov    ax, word ptr [di + 0x2b52]
0C7B  3b06842b         cmp    ax, word ptr [0x2b84]
0C7F  7505             jne    0xc86
0C81  c6069f2b01       mov    byte ptr [0x2b9f], 1
0C86  8b7efe           mov    di, word ptr [bp - 2]
0C89  d1e7             shl    di, 1
0C8B  83bd4a2b13       cmp    word ptr [di + 0x2b4a], 0x13
0C90  740c             je     0xc9e
0C92  8b7efe           mov    di, word ptr [bp - 2]
0C95  d1e7             shl    di, 1
0C97  83bd4a2b01       cmp    word ptr [di + 0x2b4a], 1
0C9C  7529             jne    0xcc7
0C9E  8b7efe           mov    di, word ptr [bp - 2]
0CA1  d1e7             shl    di, 1
0CA3  8b853a2b         mov    ax, word ptr [di + 0x2b3a]
0CA7  f7d8             neg    ax
0CA9  8b7efe           mov    di, word ptr [bp - 2]
0CAC  d1e7             shl    di, 1
0CAE  89853a2b         mov    word ptr [di + 0x2b3a], ax
0CB2  b81100           mov    ax, 0x11
0CB5  8b7efe           mov    di, word ptr [bp - 2]
0CB8  d1e7             shl    di, 1
0CBA  2b855a2b         sub    ax, word ptr [di + 0x2b5a]
0CBE  8b7efe           mov    di, word ptr [bp - 2]
0CC1  d1e7             shl    di, 1
0CC3  89855a2b         mov    word ptr [di + 0x2b5a], ax
0CC7  837efe04         cmp    word ptr [bp - 2], 4
0CCB  7403             je     0xcd0
0CCD  e971fe           jmp    0xb41
0CD0  803ea02b00       cmp    byte ptr [0x2ba0], 0
0CD5  7503             jne    0xcda
0CD7  e90801           jmp    0xde2
0CDA  a19c2b           mov    ax, word ptr [0x2b9c]
0CDD  31d2             xor    dx, dx
0CDF  9a140b0d06       lcall  0x60d, 0xb14   ; @Rand?
0CE4  8b0e7c2b         mov    cx, word ptr [0x2b7c]
0CE8  8b367e2b         mov    si, word ptr [0x2b7e]
0CEC  8b3e802b         mov    di, word ptr [0x2b80]
0CF0  9af40a0d06       lcall  0x60d, 0xaf4   ; @f0
0CF5  b98200           mov    cx, 0x82
0CF8  31f6             xor    si, si
0CFA  31ff             xor    di, di
0CFC  9a000b0d06       lcall  0x60d, 0xb00   ; @f1
0D01  b98700           mov    cx, 0x87
0D04  31f6             xor    si, si
0D06  bf0048           mov    di, 0x4800
0D09  9a060b0d06       lcall  0x60d, 0xb06   ; @f2
0D0E  b980cd           mov    cx, 0xcd80
0D11  becccc           mov    si, 0xcccc
0D14  bfcc4c           mov    di, 0x4ccc
0D17  9a100b0d06       lcall  0x60d, 0xb10   ; @f3
0D1C  7303             jae    0xd21
0D1E  e9c100           jmp    0xde2
0D21  a19c2b           mov    ax, word ptr [0x2b9c]
0D24  31d2             xor    dx, dx
0D26  9a140b0d06       lcall  0x60d, 0xb14   ; @Rand?
0D2B  a37c2b           mov    word ptr [0x2b7c], ax
0D2E  891e7e2b         mov    word ptr [0x2b7e], bx
0D32  8916802b         mov    word ptr [0x2b80], dx
0D36  ff36862b         push   word ptr [0x2b86]
0D3A  ff36882b         push   word ptr [0x2b88]
0D3E  a1882b           mov    ax, word ptr [0x2b88]
0D41  d1e0             shl    ax, 1
0D43  8bd8             mov    bx, ax
0D45  a1862b           mov    ax, word ptr [0x2b86]
0D48  ba1800           mov    dx, 0x18
0D4B  f7e2             mul    dx
0D4D  8bc8             mov    cx, ax
0D4F  a1e42a           mov    ax, word ptr [0x2ae4]
0D52  bae001           mov    dx, 0x1e0
0D55  f7e2             mul    dx
0D57  8bf8             mov    di, ax
0D59  03f9             add    di, cx
0D5B  03fb             add    di, bx
0D5D  ffb53c03         push   word ptr [di + 0x33c]
0D61  e88bf6           call   0x3ef
0D64  803ea12b00       cmp    byte ptr [0x2ba1], 0
0D69  7411             je     0xd7c
0D6B  a1e22a           mov    ax, word ptr [0x2ae2]
0D6E  2b068a2b         sub    ax, word ptr [0x2b8a]
0D72  a3e22a           mov    word ptr [0x2ae2], ax
0D75  c606a12b00       mov    byte ptr [0x2ba1], 0
0D7A  eb19             jmp    0xd95
0D7C  a1e22a           mov    ax, word ptr [0x2ae2]
0D7F  03068a2b         add    ax, word ptr [0x2b8a]
0D83  a3e22a           mov    word ptr [0x2ae2], ax
0D86  a1862b           mov    ax, word ptr [0x2b86]
0D89  03068a2b         add    ax, word ptr [0x2b8a]
0D8D  a3862b           mov    word ptr [0x2b86], ax
0D90  c606a12b01       mov    byte ptr [0x2ba1], 1
0D95  ff36862b         push   word ptr [0x2b86]
0D99  ff36882b         push   word ptr [0x2b88]
0D9D  ff36e22a         push   word ptr [0x2ae2]
0DA1  e84bf6           call   0x3ef
0DA4  a1862b           mov    ax, word ptr [0x2b86]
0DA7  3b06822b         cmp    ax, word ptr [0x2b82]
0DAB  750e             jne    0xdbb
0DAD  a1882b           mov    ax, word ptr [0x2b88]
0DB0  3b06842b         cmp    ax, word ptr [0x2b84]
0DB4  7505             jne    0xdbb
0DB6  c6069f2b01       mov    byte ptr [0x2b9f], 1
0DBB  833e862b13       cmp    word ptr [0x2b86], 0x13
0DC0  7407             je     0xdc9
0DC2  833e862b01       cmp    word ptr [0x2b86], 1
0DC7  7519             jne    0xde2
0DC9  803ea12b00       cmp    byte ptr [0x2ba1], 0
0DCE  7512             jne    0xde2
0DD0  a18a2b           mov    ax, word ptr [0x2b8a]
0DD3  f7d8             neg    ax
0DD5  a38a2b           mov    word ptr [0x2b8a], ax
0DD8  b84900           mov    ax, 0x49
0DDB  2b06e22a         sub    ax, word ptr [0x2ae2]
0DDF  a3e22a           mov    word ptr [0x2ae2], ax
0DE2  833e9a2b01       cmp    word ptr [0x2b9a], 1
0DE7  7303             jae    0xdec
0DE9  e9bc00           jmp    0xea8
0DEC  31c0             xor    ax, ax
0DEE  50               push   ax
0DEF  31c0             xor    ax, ax
0DF1  50               push   ax
0DF2  31c0             xor    ax, ax
0DF4  50               push   ax
0DF5  31c0             xor    ax, ax
0DF7  50               push   ax
0DF8  9a5b006902       lcall  0x269, 0x5b   ; SetTime
0DFD  c746fe0100       mov    word ptr [bp - 2], 1
0E02  eb03             jmp    0xe07
0E04  ff46fe           inc    word ptr [bp - 2]
0E07  8b7efe           mov    di, word ptr [bp - 2]
0E0A  d1e7             shl    di, 1
0E0C  8bf7             mov    si, di
0E0E  d1e7             shl    di, 1
0E10  03fe             add    di, si
0E12  c7855e2b0000     mov    word ptr [di + 0x2b5e], 0
0E18  c785602b0000     mov    word ptr [di + 0x2b60], 0
0E1E  c785622b0000     mov    word ptr [di + 0x2b62], 0
0E24  837efe04         cmp    word ptr [bp - 2], 4
0E28  75da             jne    0xe04
0E2A  c7067c2b0000     mov    word ptr [0x2b7c], 0
0E30  c7067e2b0000     mov    word ptr [0x2b7e], 0
0E36  c706802b0000     mov    word ptr [0x2b80], 0
0E3C  a18e2b           mov    ax, word ptr [0x2b8e]
0E3F  48               dec    ax
0E40  a38e2b           mov    word ptr [0x2b8e], ax
0E43  b8f800           mov    ax, 0xf8
0E46  50               push   ax
0E47  b8be00           mov    ax, 0xbe
0E4A  50               push   ax
0E4B  b81001           mov    ax, 0x110
0E4E  50               push   ax
0E4F  b8c700           mov    ax, 0xc7
0E52  50               push   ax
0E53  9a1415d302       lcall  0x2d3, 0x1514   ; Bar
0E58  8dbef0fe         lea    di, [bp - 0x110]
0E5C  16               push   ss
0E5D  57               push   di
0E5E  b030             mov    al, 0x30
0E60  50               push   ax
0E61  a18e2b           mov    ax, word ptr [0x2b8e]
0E64  99               cdq    
0E65  52               push   dx
0E66  50               push   ax
0E67  e8c5fb           call   0xa2f
0E6A  8d7ef4           lea    di, [bp - 0xc]
0E6D  16               push   ss
0E6E  57               push   di
0E6F  b80600           mov    ax, 6
0E72  50               push   ax
0E73  9a4e060d06       lcall  0x60d, 0x64e   ; @StrCopy
0E78  8d7ef4           lea    di, [bp - 0xc]
0E7B  16               push   ss
0E7C  57               push   di
0E7D  b80100           mov    ax, 1
0E80  50               push   ax
0E81  b80300           mov    ax, 3
0E84  50               push   ax
0E85  9ad0070d06       lcall  0x60d, 0x7d0   ; @Pos
0E8A  b8f800           mov    ax, 0xf8
0E8D  50               push   ax
0E8E  b8be00           mov    ax, 0xbe
0E91  50               push   ax
0E92  8d7ef4           lea    di, [bp - 0xc]
0E95  16               push   ss
0E96  57               push   di
0E97  9a5617d302       lcall  0x2d3, 0x1756   ; OutTextXY
0E9C  833e8e2b00       cmp    word ptr [0x2b8e], 0
0EA1  7505             jne    0xea8
0EA3  c6069f2b01       mov    byte ptr [0x2b9f], 1
0EA8  89ec             mov    sp, bp
0EAA  5d               pop    bp
0EAB  c3               ret    

;===== proc_0EAC  (len 90) =====
0EAC  55               push   bp
0EAD  89e5             mov    bp, sp
0EAF  b80600           mov    ax, 6
0EB2  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
0EB7  83ec06           sub    sp, 6
0EBA  8b4604           mov    ax, word ptr [bp + 4]
0EBD  31d2             xor    dx, dx
0EBF  b90c00           mov    cx, 0xc
0EC2  f7f1             div    cx
0EC4  8946fe           mov    word ptr [bp - 2], ax
0EC7  8b46fe           mov    ax, word ptr [bp - 2]
0ECA  8946fa           mov    word ptr [bp - 6], ax
0ECD  b80100           mov    ax, 1
0ED0  3b46fa           cmp    ax, word ptr [bp - 6]
0ED3  7f2b             jg     0xf00
0ED5  8946fc           mov    word ptr [bp - 4], ax
0ED8  eb03             jmp    0xedd
0EDA  ff46fc           inc    word ptr [bp - 4]
0EDD  8b46fc           mov    ax, word ptr [bp - 4]
0EE0  99               cdq    
0EE1  8bc8             mov    cx, ax
0EE3  8bda             mov    bx, dx
0EE5  8b4604           mov    ax, word ptr [bp + 4]
0EE8  31d2             xor    dx, dx
0EEA  9a94020d06       lcall  0x60d, 0x294   ; @LongDiv
0EEF  50               push   ax
0EF0  9a9e027102       lcall  0x271, 0x29e   ; Delay
0EF5  e81afc           call   0xb12
0EF8  8b46fc           mov    ax, word ptr [bp - 4]
0EFB  3b46fa           cmp    ax, word ptr [bp - 6]
0EFE  75da             jne    0xeda
0F00  89ec             mov    sp, bp
0F02  5d               pop    bp
0F03  c20200           ret    2

;===== proc_0F06  (len 301) =====
0F06  55               push   bp
0F07  89e5             mov    bp, sp
0F09  b80001           mov    ax, 0x100
0F0C  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
0F11  81ec0001         sub    sp, 0x100
0F15  b80b02           mov    ax, 0x20b
0F18  50               push   ax
0F19  9ac6027102       lcall  0x271, 0x2c6   ; Sound
0F1E  b81900           mov    ax, 0x19
0F21  50               push   ax
0F22  e887ff           call   0xeac
0F25  b84b02           mov    ax, 0x24b
0F28  50               push   ax
0F29  9ac6027102       lcall  0x271, 0x2c6   ; Sound
0F2E  b81900           mov    ax, 0x19
0F31  50               push   ax
0F32  e877ff           call   0xeac
0F35  b89302           mov    ax, 0x293
0F38  50               push   ax
0F39  9ac6027102       lcall  0x271, 0x2c6   ; Sound
0F3E  b81900           mov    ax, 0x19
0F41  50               push   ax
0F42  e867ff           call   0xeac
0F45  b8ba02           mov    ax, 0x2ba
0F48  50               push   ax
0F49  9ac6027102       lcall  0x271, 0x2c6   ; Sound
0F4E  b81900           mov    ax, 0x19
0F51  50               push   ax
0F52  e857ff           call   0xeac
0F55  b81003           mov    ax, 0x310
0F58  50               push   ax
0F59  9ac6027102       lcall  0x271, 0x2c6   ; Sound
0F5E  b81900           mov    ax, 0x19
0F61  50               push   ax
0F62  e847ff           call   0xeac
0F65  b87003           mov    ax, 0x370
0F68  50               push   ax
0F69  9ac6027102       lcall  0x271, 0x2c6   ; Sound
0F6E  b81900           mov    ax, 0x19
0F71  50               push   ax
0F72  e837ff           call   0xeac
0F75  b8dc03           mov    ax, 0x3dc
0F78  50               push   ax
0F79  9ac6027102       lcall  0x271, 0x2c6   ; Sound
0F7E  b81900           mov    ax, 0x19
0F81  50               push   ax
0F82  e827ff           call   0xeac
0F85  b81704           mov    ax, 0x417
0F88  50               push   ax
0F89  9ac6027102       lcall  0x271, 0x2c6   ; Sound
0F8E  b81900           mov    ax, 0x19
0F91  50               push   ax
0F92  e817ff           call   0xeac
0F95  9af3027102       lcall  0x271, 0x2f3   ; NoSound
0F9A  c406942b         les    ax, ptr [0x2b94]
0F9E  8cc2             mov    dx, es
0FA0  056400           add    ax, 0x64
0FA3  83d200           adc    dx, 0
0FA6  a3942b           mov    word ptr [0x2b94], ax
0FA9  8916962b         mov    word ptr [0x2b96], dx
0FAD  b8a000           mov    ax, 0xa0
0FB0  50               push   ax
0FB1  b8be00           mov    ax, 0xbe
0FB4  50               push   ax
0FB5  b8d000           mov    ax, 0xd0
0FB8  50               push   ax
0FB9  b8c700           mov    ax, 0xc7
0FBC  50               push   ax
0FBD  9a1415d302       lcall  0x2d3, 0x1514   ; Bar
0FC2  b8a000           mov    ax, 0xa0
0FC5  50               push   ax
0FC6  b8be00           mov    ax, 0xbe
0FC9  50               push   ax
0FCA  8dbe00ff         lea    di, [bp - 0x100]
0FCE  16               push   ss
0FCF  57               push   di
0FD0  b030             mov    al, 0x30
0FD2  50               push   ax
0FD3  ff36962b         push   word ptr [0x2b96]
0FD7  ff36942b         push   word ptr [0x2b94]
0FDB  e851fa           call   0xa2f
0FDE  9a5617d302       lcall  0x2d3, 0x1756   ; OutTextXY
0FE3  a1842b           mov    ax, word ptr [0x2b84]
0FE6  d1e0             shl    ax, 1
0FE8  8bd8             mov    bx, ax
0FEA  a1822b           mov    ax, word ptr [0x2b82]
0FED  ba1800           mov    dx, 0x18
0FF0  f7e2             mul    dx
0FF2  8bc8             mov    cx, ax
0FF4  a1e42a           mov    ax, word ptr [0x2ae4]
0FF7  bae001           mov    dx, 0x1e0
0FFA  f7e2             mul    dx
0FFC  8bf8             mov    di, ax
0FFE  03f9             add    di, cx
1000  03fb             add    di, bx
1002  c7853c030100     mov    word ptr [di + 0x33c], 1
1008  8b3ee42a         mov    di, word ptr [0x2ae4]
100C  d1e7             shl    di, 1
100E  8b85b42a         mov    ax, word ptr [di + 0x2ab4]
1012  48               dec    ax
1013  8b3ee42a         mov    di, word ptr [0x2ae4]
1017  d1e7             shl    di, 1
1019  8985b42a         mov    word ptr [di + 0x2ab4], ax
101D  8b3ee42a         mov    di, word ptr [0x2ae4]
1021  d1e7             shl    di, 1
1023  83bdb42a00       cmp    word ptr [di + 0x2ab4], 0
1028  7505             jne    0x102f
102A  c6069e2b01       mov    byte ptr [0x2b9e], 1
102F  89ec             mov    sp, bp
1031  5d               pop    bp
1032  c3               ret    

;===== proc_1033  (len 414) =====
1033  55               push   bp
1034  89e5             mov    bp, sp
1036  b80201           mov    ax, 0x102
1039  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
103E  81ec0201         sub    sp, 0x102
1042  833ee02a0a       cmp    word ptr [0x2ae0], 0xa
1047  7507             jne    0x1050
1049  c746fe1f00       mov    word ptr [bp - 2], 0x1f
104E  eb05             jmp    0x1055
1050  c746fe2100       mov    word ptr [bp - 2], 0x21
1055  a1842b           mov    ax, word ptr [0x2b84]
1058  d1e0             shl    ax, 1
105A  8bd8             mov    bx, ax
105C  a1822b           mov    ax, word ptr [0x2b82]
105F  ba1800           mov    dx, 0x18
1062  f7e2             mul    dx
1064  8bc8             mov    cx, ax
1066  a1e42a           mov    ax, word ptr [0x2ae4]
1069  bae001           mov    dx, 0x1e0
106C  f7e2             mul    dx
106E  8bf8             mov    di, ax
1070  03f9             add    di, cx
1072  03fb             add    di, bx
1074  83bd3c031d       cmp    word ptr [di + 0x33c], 0x1d
1079  7507             jne    0x1082
107B  8b46fe           mov    ax, word ptr [bp - 2]
107E  40               inc    ax
107F  8946fe           mov    word ptr [bp - 2], ax
1082  ff36822b         push   word ptr [0x2b82]
1086  ff36842b         push   word ptr [0x2b84]
108A  ff76fe           push   word ptr [bp - 2]
108D  e85ff3           call   0x3ef
1090  b80b02           mov    ax, 0x20b
1093  50               push   ax
1094  9ac6027102       lcall  0x271, 0x2c6   ; Sound
1099  b81900           mov    ax, 0x19
109C  50               push   ax
109D  e80cfe           call   0xeac
10A0  b84b02           mov    ax, 0x24b
10A3  50               push   ax
10A4  9ac6027102       lcall  0x271, 0x2c6   ; Sound
10A9  b81900           mov    ax, 0x19
10AC  50               push   ax
10AD  e8fcfd           call   0xeac
10B0  b89302           mov    ax, 0x293
10B3  50               push   ax
10B4  9ac6027102       lcall  0x271, 0x2c6   ; Sound
10B9  b81900           mov    ax, 0x19
10BC  50               push   ax
10BD  e8ecfd           call   0xeac
10C0  b8ba02           mov    ax, 0x2ba
10C3  50               push   ax
10C4  9ac6027102       lcall  0x271, 0x2c6   ; Sound
10C9  b81900           mov    ax, 0x19
10CC  50               push   ax
10CD  e8dcfd           call   0xeac
10D0  b81003           mov    ax, 0x310
10D3  50               push   ax
10D4  9ac6027102       lcall  0x271, 0x2c6   ; Sound
10D9  b81900           mov    ax, 0x19
10DC  50               push   ax
10DD  e8ccfd           call   0xeac
10E0  b87003           mov    ax, 0x370
10E3  50               push   ax
10E4  9ac6027102       lcall  0x271, 0x2c6   ; Sound
10E9  b81900           mov    ax, 0x19
10EC  50               push   ax
10ED  e8bcfd           call   0xeac
10F0  b8dc03           mov    ax, 0x3dc
10F3  50               push   ax
10F4  9ac6027102       lcall  0x271, 0x2c6   ; Sound
10F9  b81900           mov    ax, 0x19
10FC  50               push   ax
10FD  e8acfd           call   0xeac
1100  b81704           mov    ax, 0x417
1103  50               push   ax
1104  9ac6027102       lcall  0x271, 0x2c6   ; Sound
1109  b81900           mov    ax, 0x19
110C  50               push   ax
110D  e89cfd           call   0xeac
1110  9af3027102       lcall  0x271, 0x2f3   ; NoSound
1115  ff36822b         push   word ptr [0x2b82]
1119  ff36842b         push   word ptr [0x2b84]
111D  ff36e02a         push   word ptr [0x2ae0]
1121  e8cbf2           call   0x3ef
1124  a1842b           mov    ax, word ptr [0x2b84]
1127  d1e0             shl    ax, 1
1129  8bd8             mov    bx, ax
112B  a1822b           mov    ax, word ptr [0x2b82]
112E  ba1800           mov    dx, 0x18
1131  f7e2             mul    dx
1133  8bc8             mov    cx, ax
1135  a1e42a           mov    ax, word ptr [0x2ae4]
1138  bae001           mov    dx, 0x1e0
113B  f7e2             mul    dx
113D  8bf8             mov    di, ax
113F  03f9             add    di, cx
1141  03fb             add    di, bx
1143  83bd3c031c       cmp    word ptr [di + 0x33c], 0x1c
1148  7515             jne    0x115f
114A  c406942b         les    ax, ptr [0x2b94]
114E  8cc2             mov    dx, es
1150  05f401           add    ax, 0x1f4
1153  83d200           adc    dx, 0
1156  a3942b           mov    word ptr [0x2b94], ax
1159  8916962b         mov    word ptr [0x2b96], dx
115D  eb13             jmp    0x1172
115F  c406942b         les    ax, ptr [0x2b94]
1163  8cc2             mov    dx, es
1165  05e803           add    ax, 0x3e8
1168  83d200           adc    dx, 0
116B  a3942b           mov    word ptr [0x2b94], ax
116E  8916962b         mov    word ptr [0x2b96], dx
1172  b8a000           mov    ax, 0xa0
1175  50               push   ax
1176  b8be00           mov    ax, 0xbe
1179  50               push   ax
117A  b8d000           mov    ax, 0xd0
117D  50               push   ax
117E  b8c700           mov    ax, 0xc7
1181  50               push   ax
1182  9a1415d302       lcall  0x2d3, 0x1514   ; Bar
1187  b8a000           mov    ax, 0xa0
118A  50               push   ax
118B  b8be00           mov    ax, 0xbe
118E  50               push   ax
118F  8dbefefe         lea    di, [bp - 0x102]
1193  16               push   ss
1194  57               push   di
1195  b030             mov    al, 0x30
1197  50               push   ax
1198  ff36962b         push   word ptr [0x2b96]
119C  ff36942b         push   word ptr [0x2b94]
11A0  e88cf8           call   0xa2f
11A3  9a5617d302       lcall  0x2d3, 0x1756   ; OutTextXY
11A8  a1842b           mov    ax, word ptr [0x2b84]
11AB  d1e0             shl    ax, 1
11AD  8bd8             mov    bx, ax
11AF  a1822b           mov    ax, word ptr [0x2b82]
11B2  ba1800           mov    dx, 0x18
11B5  f7e2             mul    dx
11B7  8bc8             mov    cx, ax
11B9  a1e42a           mov    ax, word ptr [0x2ae4]
11BC  bae001           mov    dx, 0x1e0
11BF  f7e2             mul    dx
11C1  8bf8             mov    di, ax
11C3  03f9             add    di, cx
11C5  03fb             add    di, bx
11C7  c7853c030100     mov    word ptr [di + 0x33c], 1
11CD  89ec             mov    sp, bp
11CF  5d               pop    bp
11D0  c3               ret    

;===== proc_11D1  (len 184) =====
11D1  55               push   bp
11D2  89e5             mov    bp, sp
11D4  31c0             xor    ax, ax
11D6  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
11DB  b80b02           mov    ax, 0x20b
11DE  50               push   ax
11DF  9ac6027102       lcall  0x271, 0x2c6   ; Sound
11E4  b8c800           mov    ax, 0xc8
11E7  50               push   ax
11E8  e8c1fc           call   0xeac
11EB  b82a02           mov    ax, 0x22a
11EE  50               push   ax
11EF  9ac6027102       lcall  0x271, 0x2c6   ; Sound
11F4  b86400           mov    ax, 0x64
11F7  50               push   ax
11F8  e8b1fc           call   0xeac
11FB  9af3027102       lcall  0x271, 0x2f3   ; NoSound
1200  a1842b           mov    ax, word ptr [0x2b84]
1203  d1e0             shl    ax, 1
1205  8bd8             mov    bx, ax
1207  a1822b           mov    ax, word ptr [0x2b82]
120A  ba1800           mov    dx, 0x18
120D  f7e2             mul    dx
120F  8bc8             mov    cx, ax
1211  a1e42a           mov    ax, word ptr [0x2ae4]
1214  bae001           mov    dx, 0x1e0
1217  f7e2             mul    dx
1219  8bf8             mov    di, ax
121B  03f9             add    di, cx
121D  03fb             add    di, bx
121F  c7853c030100     mov    word ptr [di + 0x33c], 1
1225  c606a02b01       mov    byte ptr [0x2ba0], 1
122A  833e822b09       cmp    word ptr [0x2b82], 9
122F  7e15             jle    0x1246
1231  c706e22a2300     mov    word ptr [0x2ae2], 0x23
1237  a1822b           mov    ax, word ptr [0x2b82]
123A  48               dec    ax
123B  a3862b           mov    word ptr [0x2b86], ax
123E  c7068a2bffff     mov    word ptr [0x2b8a], 0xffff
1244  eb13             jmp    0x1259
1246  c706e22a2600     mov    word ptr [0x2ae2], 0x26
124C  a1822b           mov    ax, word ptr [0x2b82]
124F  40               inc    ax
1250  a3862b           mov    word ptr [0x2b86], ax
1253  c7068a2b0100     mov    word ptr [0x2b8a], 1
1259  a1842b           mov    ax, word ptr [0x2b84]
125C  a3882b           mov    word ptr [0x2b88], ax
125F  c7067c2b0000     mov    word ptr [0x2b7c], 0
1265  c7067e2b0000     mov    word ptr [0x2b7e], 0
126B  c706802b0000     mov    word ptr [0x2b80], 0
1271  c606a12b01       mov    byte ptr [0x2ba1], 1
1276  ff36862b         push   word ptr [0x2b86]
127A  ff36882b         push   word ptr [0x2b88]
127E  ff36e22a         push   word ptr [0x2ae2]
1282  e86af1           call   0x3ef
1285  89ec             mov    sp, bp
1287  5d               pop    bp
1288  c3               ret    

;===== proc_1289  (len 207) =====
1289  55               push   bp
128A  89e5             mov    bp, sp
128C  b80200           mov    ax, 2
128F  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
1294  83ec02           sub    sp, 2
1297  ff36822b         push   word ptr [0x2b82]
129B  ff36842b         push   word ptr [0x2b84]
129F  a1842b           mov    ax, word ptr [0x2b84]
12A2  d1e0             shl    ax, 1
12A4  8bd8             mov    bx, ax
12A6  a1822b           mov    ax, word ptr [0x2b82]
12A9  ba1800           mov    dx, 0x18
12AC  f7e2             mul    dx
12AE  8bc8             mov    cx, ax
12B0  a1e42a           mov    ax, word ptr [0x2ae4]
12B3  bae001           mov    dx, 0x1e0
12B6  f7e2             mul    dx
12B8  8bf8             mov    di, ax
12BA  03f9             add    di, cx
12BC  03fb             add    di, bx
12BE  ffb53c03         push   word ptr [di + 0x33c]
12C2  e82af1           call   0x3ef
12C5  c706e02a2700     mov    word ptr [0x2ae0], 0x27
12CB  a1842b           mov    ax, word ptr [0x2b84]
12CE  40               inc    ax
12CF  3d0a00           cmp    ax, 0xa
12D2  7f66             jg     0x133a
12D4  8946fe           mov    word ptr [bp - 2], ax
12D7  eb03             jmp    0x12dc
12D9  ff46fe           inc    word ptr [bp - 2]
12DC  ff36822b         push   word ptr [0x2b82]
12E0  ff76fe           push   word ptr [bp - 2]
12E3  ff36e02a         push   word ptr [0x2ae0]
12E7  e805f1           call   0x3ef
12EA  a1e02a           mov    ax, word ptr [0x2ae0]
12ED  40               inc    ax
12EE  a3e02a           mov    word ptr [0x2ae0], ax
12F1  833ee02a2b       cmp    word ptr [0x2ae0], 0x2b
12F6  7506             jne    0x12fe
12F8  c706e02a2700     mov    word ptr [0x2ae0], 0x27
12FE  b87d00           mov    ax, 0x7d
1301  50               push   ax
1302  9a9e027102       lcall  0x271, 0x29e   ; Delay
1307  ff36822b         push   word ptr [0x2b82]
130B  ff76fe           push   word ptr [bp - 2]
130E  8b46fe           mov    ax, word ptr [bp - 2]
1311  d1e0             shl    ax, 1
1313  8bd8             mov    bx, ax
1315  a1822b           mov    ax, word ptr [0x2b82]
1318  ba1800           mov    dx, 0x18
131B  f7e2             mul    dx
131D  8bc8             mov    cx, ax
131F  a1e42a           mov    ax, word ptr [0x2ae4]
1322  bae001           mov    dx, 0x1e0
1325  f7e2             mul    dx
1327  8bf8             mov    di, ax
1329  03f9             add    di, cx
132B  03fb             add    di, bx
132D  ffb53c03         push   word ptr [di + 0x33c]
1331  e8bbf0           call   0x3ef
1334  837efe0a         cmp    word ptr [bp - 2], 0xa
1338  759f             jne    0x12d9
133A  ff36822b         push   word ptr [0x2b82]
133E  b80b00           mov    ax, 0xb
1341  50               push   ax
1342  ff36e02a         push   word ptr [0x2ae0]
1346  e8a6f0           call   0x3ef
1349  c706842b0b00     mov    word ptr [0x2b84], 0xb
134F  c6069f2b01       mov    byte ptr [0x2b9f], 1
1354  89ec             mov    sp, bp
1356  5d               pop    bp
1357  c3               ret    

;===== proc_1358  (len 359) =====
1358  55               push   bp
1359  89e5             mov    bp, sp
135B  b80400           mov    ax, 4
135E  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
1363  83ec04           sub    sp, 4
1366  a1842b           mov    ax, word ptr [0x2b84]
1369  d1e0             shl    ax, 1
136B  8bd8             mov    bx, ax
136D  a1822b           mov    ax, word ptr [0x2b82]
1370  ba1800           mov    dx, 0x18
1373  f7e2             mul    dx
1375  8bc8             mov    cx, ax
1377  a1e42a           mov    ax, word ptr [0x2ae4]
137A  bae001           mov    dx, 0x1e0
137D  f7e2             mul    dx
137F  8bf8             mov    di, ax
1381  03f9             add    di, cx
1383  03fb             add    di, bx
1385  83bd3c0307       cmp    word ptr [di + 0x33c], 7
138A  7505             jne    0x1391
138C  c6069f2b01       mov    byte ptr [0x2b9f], 1
1391  c646fd00         mov    byte ptr [bp - 3], 0
1395  c746fe0100       mov    word ptr [bp - 2], 1
139A  eb03             jmp    0x139f
139C  ff46fe           inc    word ptr [bp - 2]
139F  a1822b           mov    ax, word ptr [0x2b82]
13A2  8b7efe           mov    di, word ptr [bp - 2]
13A5  d1e7             shl    di, 1
13A7  3b854a2b         cmp    ax, word ptr [di + 0x2b4a]
13AB  7512             jne    0x13bf
13AD  a1842b           mov    ax, word ptr [0x2b84]
13B0  8b7efe           mov    di, word ptr [bp - 2]
13B3  d1e7             shl    di, 1
13B5  3b85522b         cmp    ax, word ptr [di + 0x2b52]
13B9  7504             jne    0x13bf
13BB  c646fd01         mov    byte ptr [bp - 3], 1
13BF  837efe04         cmp    word ptr [bp - 2], 4
13C3  75d7             jne    0x139c
13C5  803ea02b00       cmp    byte ptr [0x2ba0], 0
13CA  7416             je     0x13e2
13CC  a1822b           mov    ax, word ptr [0x2b82]
13CF  3b06862b         cmp    ax, word ptr [0x2b86]
13D3  750d             jne    0x13e2
13D5  a1842b           mov    ax, word ptr [0x2b84]
13D8  3b06882b         cmp    ax, word ptr [0x2b88]
13DC  7504             jne    0x13e2
13DE  c646fd01         mov    byte ptr [bp - 3], 1
13E2  807efd01         cmp    byte ptr [bp - 3], 1
13E6  7505             jne    0x13ed
13E8  c6069f2b01       mov    byte ptr [0x2b9f], 1
13ED  a1842b           mov    ax, word ptr [0x2b84]
13F0  d1e0             shl    ax, 1
13F2  8bd8             mov    bx, ax
13F4  a1822b           mov    ax, word ptr [0x2b82]
13F7  ba1800           mov    dx, 0x18
13FA  f7e2             mul    dx
13FC  8bc8             mov    cx, ax
13FE  a1e42a           mov    ax, word ptr [0x2ae4]
1401  bae001           mov    dx, 0x1e0
1404  f7e2             mul    dx
1406  8bf8             mov    di, ax
1408  03f9             add    di, cx
140A  03fb             add    di, bx
140C  8b853c03         mov    ax, word ptr [di + 0x33c]
1410  3b06ea2a         cmp    ax, word ptr [0x2aea]
1414  7503             jne    0x1419
1416  e8edfa           call   0xf06
1419  a1842b           mov    ax, word ptr [0x2b84]
141C  d1e0             shl    ax, 1
141E  8bd8             mov    bx, ax
1420  a1822b           mov    ax, word ptr [0x2b82]
1423  ba1800           mov    dx, 0x18
1426  f7e2             mul    dx
1428  8bc8             mov    cx, ax
142A  a1e42a           mov    ax, word ptr [0x2ae4]
142D  bae001           mov    dx, 0x1e0
1430  f7e2             mul    dx
1432  8bf8             mov    di, ax
1434  03f9             add    di, cx
1436  03fb             add    di, bx
1438  83bd3c031c       cmp    word ptr [di + 0x33c], 0x1c
143D  7426             je     0x1465
143F  a1842b           mov    ax, word ptr [0x2b84]
1442  d1e0             shl    ax, 1
1444  8bd8             mov    bx, ax
1446  a1822b           mov    ax, word ptr [0x2b82]
1449  ba1800           mov    dx, 0x18
144C  f7e2             mul    dx
144E  8bc8             mov    cx, ax
1450  a1e42a           mov    ax, word ptr [0x2ae4]
1453  bae001           mov    dx, 0x1e0
1456  f7e2             mul    dx
1458  8bf8             mov    di, ax
145A  03f9             add    di, cx
145C  03fb             add    di, bx
145E  83bd3c031d       cmp    word ptr [di + 0x33c], 0x1d
1463  7503             jne    0x1468
1465  e8cbfb           call   0x1033
1468  a1842b           mov    ax, word ptr [0x2b84]
146B  d1e0             shl    ax, 1
146D  8bd8             mov    bx, ax
146F  a1822b           mov    ax, word ptr [0x2b82]
1472  ba1800           mov    dx, 0x18
1475  f7e2             mul    dx
1477  8bc8             mov    cx, ax
1479  a1e42a           mov    ax, word ptr [0x2ae4]
147C  bae001           mov    dx, 0x1e0
147F  f7e2             mul    dx
1481  8bf8             mov    di, ax
1483  03f9             add    di, cx
1485  03fb             add    di, bx
1487  83bd3c031e       cmp    word ptr [di + 0x33c], 0x1e
148C  7503             jne    0x1491
148E  e840fd           call   0x11d1
1491  a1842b           mov    ax, word ptr [0x2b84]
1494  40               inc    ax
1495  d1e0             shl    ax, 1
1497  8bd8             mov    bx, ax
1499  a1822b           mov    ax, word ptr [0x2b82]
149C  ba1800           mov    dx, 0x18
149F  f7e2             mul    dx
14A1  8bc8             mov    cx, ax
14A3  a1e42a           mov    ax, word ptr [0x2ae4]
14A6  bae001           mov    dx, 0x1e0
14A9  f7e2             mul    dx
14AB  8bf8             mov    di, ax
14AD  03f9             add    di, cx
14AF  03fb             add    di, bx
14B1  83bd3c0301       cmp    word ptr [di + 0x33c], 1
14B6  7503             jne    0x14bb
14B8  e8cefd           call   0x1289
14BB  89ec             mov    sp, bp
14BD  5d               pop    bp
14BE  c3               ret    

;===== proc_14BF  (len 1128) =====
14BF  55               push   bp
14C0  89e5             mov    bp, sp
14C2  31c0             xor    ax, ax
14C4  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
14C9  a1e02a           mov    ax, word ptr [0x2ae0]
14CC  3d0d00           cmp    ax, 0xd
14CF  7403             je     0x14d4
14D1  e9c400           jmp    0x1598
14D4  a1842b           mov    ax, word ptr [0x2b84]
14D7  99               cdq    
14D8  b90300           mov    cx, 3
14DB  f7f9             idiv   cx
14DD  92               xchg   dx, ax
14DE  3d0200           cmp    ax, 2
14E1  7403             je     0x14e6
14E3  e9af00           jmp    0x1595
14E6  c7068c2b0100     mov    word ptr [0x2b8c], 1
14EC  b81704           mov    ax, 0x417
14EF  50               push   ax
14F0  9ac6027102       lcall  0x271, 0x2c6   ; Sound
14F5  b84b00           mov    ax, 0x4b
14F8  50               push   ax
14F9  e8b0f9           call   0xeac
14FC  9af3027102       lcall  0x271, 0x2f3   ; NoSound
1501  ff36822b         push   word ptr [0x2b82]
1505  ff36842b         push   word ptr [0x2b84]
1509  a1842b           mov    ax, word ptr [0x2b84]
150C  d1e0             shl    ax, 1
150E  8bd8             mov    bx, ax
1510  a1822b           mov    ax, word ptr [0x2b82]
1513  ba1800           mov    dx, 0x18
1516  f7e2             mul    dx
1518  8bc8             mov    cx, ax
151A  a1e42a           mov    ax, word ptr [0x2ae4]
151D  bae001           mov    dx, 0x1e0
1520  f7e2             mul    dx
1522  8bf8             mov    di, ax
1524  03f9             add    di, cx
1526  03fb             add    di, bx
1528  ffb53c03         push   word ptr [di + 0x33c]
152C  e8c0ee           call   0x3ef
152F  a1842b           mov    ax, word ptr [0x2b84]
1532  48               dec    ax
1533  a3842b           mov    word ptr [0x2b84], ax
1536  ff36822b         push   word ptr [0x2b82]
153A  ff36842b         push   word ptr [0x2b84]
153E  ff36e02a         push   word ptr [0x2ae0]
1542  e8aaee           call   0x3ef
1545  b83200           mov    ax, 0x32
1548  50               push   ax
1549  e860f9           call   0xeac
154C  ff36822b         push   word ptr [0x2b82]
1550  ff36842b         push   word ptr [0x2b84]
1554  a1842b           mov    ax, word ptr [0x2b84]
1557  d1e0             shl    ax, 1
1559  8bd8             mov    bx, ax
155B  a1822b           mov    ax, word ptr [0x2b82]
155E  ba1800           mov    dx, 0x18
1561  f7e2             mul    dx
1563  8bc8             mov    cx, ax
1565  a1e42a           mov    ax, word ptr [0x2ae4]
1568  bae001           mov    dx, 0x1e0
156B  f7e2             mul    dx
156D  8bf8             mov    di, ax
156F  03f9             add    di, cx
1571  03fb             add    di, bx
1573  ffb53c03         push   word ptr [di + 0x33c]
1577  e875ee           call   0x3ef
157A  a1842b           mov    ax, word ptr [0x2b84]
157D  40               inc    ax
157E  a3842b           mov    word ptr [0x2b84], ax
1581  ff36822b         push   word ptr [0x2b82]
1585  ff36842b         push   word ptr [0x2b84]
1589  ff36e02a         push   word ptr [0x2ae0]
158D  e85fee           call   0x3ef
1590  31c0             xor    ax, ax
1592  a38c2b           mov    word ptr [0x2b8c], ax
1595  e98b03           jmp    0x1923
1598  3d0a00           cmp    ax, 0xa
159B  7403             je     0x15a0
159D  e9bf01           jmp    0x175f
15A0  833e822b02       cmp    word ptr [0x2b82], 2
15A5  7f03             jg     0x15aa
15A7  e9b201           jmp    0x175c
15AA  c7068c2bffff     mov    word ptr [0x2b8c], 0xffff
15B0  b81704           mov    ax, 0x417
15B3  50               push   ax
15B4  9ac6027102       lcall  0x271, 0x2c6   ; Sound
15B9  b83200           mov    ax, 0x32
15BC  50               push   ax
15BD  e8ecf8           call   0xeac
15C0  a1822b           mov    ax, word ptr [0x2b82]
15C3  2d0200           sub    ax, 2
15C6  8bd8             mov    bx, ax
15C8  a1842b           mov    ax, word ptr [0x2b84]
15CB  40               inc    ax
15CC  99               cdq    
15CD  b90300           mov    cx, 3
15D0  f7f9             idiv   cx
15D2  8bd0             mov    dx, ax
15D4  b80500           mov    ax, 5
15D7  2bc2             sub    ax, dx
15D9  8bf8             mov    di, ax
15DB  d1e7             shl    di, 1
15DD  8b854a2b         mov    ax, word ptr [di + 0x2b4a]
15E1  3bc3             cmp    ax, bx
15E3  7423             je     0x1608
15E5  a1822b           mov    ax, word ptr [0x2b82]
15E8  48               dec    ax
15E9  8bd8             mov    bx, ax
15EB  a1842b           mov    ax, word ptr [0x2b84]
15EE  40               inc    ax
15EF  99               cdq    
15F0  b90300           mov    cx, 3
15F3  f7f9             idiv   cx
15F5  8bd0             mov    dx, ax
15F7  b80500           mov    ax, 5
15FA  2bc2             sub    ax, dx
15FC  8bf8             mov    di, ax
15FE  d1e7             shl    di, 1
1600  8b854a2b         mov    ax, word ptr [di + 0x2b4a]
1604  3bc3             cmp    ax, bx
1606  7505             jne    0x160d
1608  c6069f2b01       mov    byte ptr [0x2b9f], 1
160D  9af3027102       lcall  0x271, 0x2f3   ; NoSound
1612  c706e02a0b00     mov    word ptr [0x2ae0], 0xb
1618  ff36822b         push   word ptr [0x2b82]
161C  ff36842b         push   word ptr [0x2b84]
1620  a1842b           mov    ax, word ptr [0x2b84]
1623  d1e0             shl    ax, 1
1625  8bd8             mov    bx, ax
1627  a1822b           mov    ax, word ptr [0x2b82]
162A  ba1800           mov    dx, 0x18
162D  f7e2             mul    dx
162F  8bc8             mov    cx, ax
1631  a1e42a           mov    ax, word ptr [0x2ae4]
1634  bae001           mov    dx, 0x1e0
1637  f7e2             mul    dx
1639  8bf8             mov    di, ax
163B  03f9             add    di, cx
163D  03fb             add    di, bx
163F  ffb53c03         push   word ptr [di + 0x33c]
1643  e8a9ed           call   0x3ef
1646  ff36822b         push   word ptr [0x2b82]
164A  e87fed           call   0x3cc
164D  2d0800           sub    ax, 8
1650  50               push   ax
1651  ff36842b         push   word ptr [0x2b84]
1655  e874ed           call   0x3cc
1658  2d0800           sub    ax, 8
165B  50               push   ax
165C  8b3ee02a         mov    di, word ptr [0x2ae0]
1660  d1e7             shl    di, 1
1662  d1e7             shl    di, 1
1664  c4bd8804         les    di, ptr [di + 0x488]
1668  06               push   es
1669  57               push   di
166A  31c0             xor    ax, ax
166C  50               push   ax
166D  9a340ed302       lcall  0x2d3, 0xe34   ; PutImage
1672  b82500           mov    ax, 0x25
1675  50               push   ax
1676  e833f8           call   0xeac
1679  a1822b           mov    ax, word ptr [0x2b82]
167C  48               dec    ax
167D  50               push   ax
167E  a1842b           mov    ax, word ptr [0x2b84]
1681  48               dec    ax
1682  50               push   ax
1683  a1842b           mov    ax, word ptr [0x2b84]
1686  48               dec    ax
1687  d1e0             shl    ax, 1
1689  8bd8             mov    bx, ax
168B  a1822b           mov    ax, word ptr [0x2b82]
168E  48               dec    ax
168F  ba1800           mov    dx, 0x18
1692  f7e2             mul    dx
1694  8bc8             mov    cx, ax
1696  a1e42a           mov    ax, word ptr [0x2ae4]
1699  bae001           mov    dx, 0x1e0
169C  f7e2             mul    dx
169E  8bf8             mov    di, ax
16A0  03f9             add    di, cx
16A2  03fb             add    di, bx
16A4  ffb53c03         push   word ptr [di + 0x33c]
16A8  e844ed           call   0x3ef
16AB  ff36822b         push   word ptr [0x2b82]
16AF  a1842b           mov    ax, word ptr [0x2b84]
16B2  48               dec    ax
16B3  50               push   ax
16B4  a1842b           mov    ax, word ptr [0x2b84]
16B7  48               dec    ax
16B8  d1e0             shl    ax, 1
16BA  8bd8             mov    bx, ax
16BC  a1822b           mov    ax, word ptr [0x2b82]
16BF  ba1800           mov    dx, 0x18
16C2  f7e2             mul    dx
16C4  8bc8             mov    cx, ax
16C6  a1e42a           mov    ax, word ptr [0x2ae4]
16C9  bae001           mov    dx, 0x1e0
16CC  f7e2             mul    dx
16CE  8bf8             mov    di, ax
16D0  03f9             add    di, cx
16D2  03fb             add    di, bx
16D4  ffb53c03         push   word ptr [di + 0x33c]
16D8  e814ed           call   0x3ef
16DB  a1822b           mov    ax, word ptr [0x2b82]
16DE  48               dec    ax
16DF  50               push   ax
16E0  ff36842b         push   word ptr [0x2b84]
16E4  a1842b           mov    ax, word ptr [0x2b84]
16E7  d1e0             shl    ax, 1
16E9  8bd8             mov    bx, ax
16EB  a1822b           mov    ax, word ptr [0x2b82]
16EE  48               dec    ax
16EF  ba1800           mov    dx, 0x18
16F2  f7e2             mul    dx
16F4  8bc8             mov    cx, ax
16F6  a1e42a           mov    ax, word ptr [0x2ae4]
16F9  bae001           mov    dx, 0x1e0
16FC  f7e2             mul    dx
16FE  8bf8             mov    di, ax
1700  03f9             add    di, cx
1702  03fb             add    di, bx
1704  ffb53c03         push   word ptr [di + 0x33c]
1708  e8e4ec           call   0x3ef
170B  ff36822b         push   word ptr [0x2b82]
170F  ff36842b         push   word ptr [0x2b84]
1713  a1842b           mov    ax, word ptr [0x2b84]
1716  d1e0             shl    ax, 1
1718  8bd8             mov    bx, ax
171A  a1822b           mov    ax, word ptr [0x2b82]
171D  ba1800           mov    dx, 0x18
1720  f7e2             mul    dx
1722  8bc8             mov    cx, ax
1724  a1e42a           mov    ax, word ptr [0x2ae4]
1727  bae001           mov    dx, 0x1e0
172A  f7e2             mul    dx
172C  8bf8             mov    di, ax
172E  03f9             add    di, cx
1730  03fb             add    di, bx
1732  ffb53c03         push   word ptr [di + 0x33c]
1736  e8b6ec           call   0x3ef
1739  c706e02a0a00     mov    word ptr [0x2ae0], 0xa
173F  a1822b           mov    ax, word ptr [0x2b82]
1742  48               dec    ax
1743  a3822b           mov    word ptr [0x2b82], ax
1746  a1842b           mov    ax, word ptr [0x2b84]
1749  48               dec    ax
174A  a3842b           mov    word ptr [0x2b84], ax
174D  ff36822b         push   word ptr [0x2b82]
1751  ff36842b         push   word ptr [0x2b84]
1755  ff36e02a         push   word ptr [0x2ae0]
1759  e893ec           call   0x3ef
175C  e9c401           jmp    0x1923
175F  3d0e00           cmp    ax, 0xe
1762  7403             je     0x1767
1764  e9bc01           jmp    0x1923
1767  833e822b12       cmp    word ptr [0x2b82], 0x12
176C  7c03             jl     0x1771
176E  e9b201           jmp    0x1923
1771  c7068c2b0100     mov    word ptr [0x2b8c], 1
1777  b81704           mov    ax, 0x417
177A  50               push   ax
177B  9ac6027102       lcall  0x271, 0x2c6   ; Sound
1780  a1822b           mov    ax, word ptr [0x2b82]
1783  050200           add    ax, 2
1786  8bd8             mov    bx, ax
1788  a1842b           mov    ax, word ptr [0x2b84]
178B  40               inc    ax
178C  99               cdq    
178D  b90300           mov    cx, 3
1790  f7f9             idiv   cx
1792  8bd0             mov    dx, ax
1794  b80500           mov    ax, 5
1797  2bc2             sub    ax, dx
1799  8bf8             mov    di, ax
179B  d1e7             shl    di, 1
179D  8b854a2b         mov    ax, word ptr [di + 0x2b4a]
17A1  3bc3             cmp    ax, bx
17A3  7423             je     0x17c8
17A5  a1822b           mov    ax, word ptr [0x2b82]
17A8  40               inc    ax
17A9  8bd8             mov    bx, ax
17AB  a1842b           mov    ax, word ptr [0x2b84]
17AE  40               inc    ax
17AF  99               cdq    
17B0  b90300           mov    cx, 3
17B3  f7f9             idiv   cx
17B5  8bd0             mov    dx, ax
17B7  b80500           mov    ax, 5
17BA  2bc2             sub    ax, dx
17BC  8bf8             mov    di, ax
17BE  d1e7             shl    di, 1
17C0  8b854a2b         mov    ax, word ptr [di + 0x2b4a]
17C4  3bc3             cmp    ax, bx
17C6  7505             jne    0x17cd
17C8  c6069f2b01       mov    byte ptr [0x2b9f], 1
17CD  b83200           mov    ax, 0x32
17D0  50               push   ax
17D1  e8d8f6           call   0xeac
17D4  9af3027102       lcall  0x271, 0x2f3   ; NoSound
17D9  c706e02a0f00     mov    word ptr [0x2ae0], 0xf
17DF  ff36822b         push   word ptr [0x2b82]
17E3  ff36842b         push   word ptr [0x2b84]
17E7  a1842b           mov    ax, word ptr [0x2b84]
17EA  d1e0             shl    ax, 1
17EC  8bd8             mov    bx, ax
17EE  a1822b           mov    ax, word ptr [0x2b82]
17F1  ba1800           mov    dx, 0x18
17F4  f7e2             mul    dx
17F6  8bc8             mov    cx, ax
17F8  a1e42a           mov    ax, word ptr [0x2ae4]
17FB  bae001           mov    dx, 0x1e0
17FE  f7e2             mul    dx
1800  8bf8             mov    di, ax
1802  03f9             add    di, cx
1804  03fb             add    di, bx
1806  ffb53c03         push   word ptr [di + 0x33c]
180A  e8e2eb           call   0x3ef
180D  ff36822b         push   word ptr [0x2b82]
1811  e8b8eb           call   0x3cc
1814  050800           add    ax, 8
1817  50               push   ax
1818  ff36842b         push   word ptr [0x2b84]
181C  e8adeb           call   0x3cc
181F  2d0800           sub    ax, 8
1822  50               push   ax
1823  8b3ee02a         mov    di, word ptr [0x2ae0]
1827  d1e7             shl    di, 1
1829  d1e7             shl    di, 1
182B  c4bd8804         les    di, ptr [di + 0x488]
182F  06               push   es
1830  57               push   di
1831  31c0             xor    ax, ax
1833  50               push   ax
1834  9a340ed302       lcall  0x2d3, 0xe34   ; PutImage
1839  b82500           mov    ax, 0x25
183C  50               push   ax
183D  e86cf6           call   0xeac
1840  ff36822b         push   word ptr [0x2b82]
1844  a1842b           mov    ax, word ptr [0x2b84]
1847  48               dec    ax
1848  50               push   ax
1849  a1842b           mov    ax, word ptr [0x2b84]
184C  48               dec    ax
184D  d1e0             shl    ax, 1
184F  8bd8             mov    bx, ax
1851  a1822b           mov    ax, word ptr [0x2b82]
1854  ba1800           mov    dx, 0x18
1857  f7e2             mul    dx
1859  8bc8             mov    cx, ax
185B  a1e42a           mov    ax, word ptr [0x2ae4]
185E  bae001           mov    dx, 0x1e0
1861  f7e2             mul    dx
1863  8bf8             mov    di, ax
1865  03f9             add    di, cx
1867  03fb             add    di, bx
1869  ffb53c03         push   word ptr [di + 0x33c]
186D  e87feb           call   0x3ef
1870  a1822b           mov    ax, word ptr [0x2b82]
1873  40               inc    ax
1874  50               push   ax
1875  a1842b           mov    ax, word ptr [0x2b84]
1878  48               dec    ax
1879  50               push   ax
187A  a1842b           mov    ax, word ptr [0x2b84]
187D  48               dec    ax
187E  d1e0             shl    ax, 1
1880  8bd8             mov    bx, ax
1882  a1822b           mov    ax, word ptr [0x2b82]
1885  40               inc    ax
1886  ba1800           mov    dx, 0x18
1889  f7e2             mul    dx
188B  8bc8             mov    cx, ax
188D  a1e42a           mov    ax, word ptr [0x2ae4]
1890  bae001           mov    dx, 0x1e0
1893  f7e2             mul    dx
1895  8bf8             mov    di, ax
1897  03f9             add    di, cx
1899  03fb             add    di, bx
189B  ffb53c03         push   word ptr [di + 0x33c]
189F  e84deb           call   0x3ef
18A2  ff36822b         push   word ptr [0x2b82]
18A6  ff36842b         push   word ptr [0x2b84]
18AA  a1842b           mov    ax, word ptr [0x2b84]
18AD  d1e0             shl    ax, 1
18AF  8bd8             mov    bx, ax
18B1  a1822b           mov    ax, word ptr [0x2b82]
18B4  ba1800           mov    dx, 0x18
18B7  f7e2             mul    dx
18B9  8bc8             mov    cx, ax
18BB  a1e42a           mov    ax, word ptr [0x2ae4]
18BE  bae001           mov    dx, 0x1e0
18C1  f7e2             mul    dx
18C3  8bf8             mov    di, ax
18C5  03f9             add    di, cx
18C7  03fb             add    di, bx
18C9  ffb53c03         push   word ptr [di + 0x33c]
18CD  e81feb           call   0x3ef
18D0  a1822b           mov    ax, word ptr [0x2b82]
18D3  40               inc    ax
18D4  50               push   ax
18D5  ff36842b         push   word ptr [0x2b84]
18D9  a1842b           mov    ax, word ptr [0x2b84]
18DC  d1e0             shl    ax, 1
18DE  8bd8             mov    bx, ax
18E0  a1822b           mov    ax, word ptr [0x2b82]
18E3  40               inc    ax
18E4  ba1800           mov    dx, 0x18
18E7  f7e2             mul    dx
18E9  8bc8             mov    cx, ax
18EB  a1e42a           mov    ax, word ptr [0x2ae4]
18EE  bae001           mov    dx, 0x1e0
18F1  f7e2             mul    dx
18F3  8bf8             mov    di, ax
18F5  03f9             add    di, cx
18F7  03fb             add    di, bx
18F9  ffb53c03         push   word ptr [di + 0x33c]
18FD  e8efea           call   0x3ef
1900  c706e02a0e00     mov    word ptr [0x2ae0], 0xe
1906  a1822b           mov    ax, word ptr [0x2b82]
1909  40               inc    ax
190A  a3822b           mov    word ptr [0x2b82], ax
190D  a1842b           mov    ax, word ptr [0x2b84]
1910  48               dec    ax
1911  a3842b           mov    word ptr [0x2b84], ax
1914  ff36822b         push   word ptr [0x2b82]
1918  ff36842b         push   word ptr [0x2b84]
191C  ff36e02a         push   word ptr [0x2ae0]
1920  e8ccea           call   0x3ef
1923  89ec             mov    sp, bp
1925  5d               pop    bp
1926  c3               ret    

;===== proc_1927  (len 352) =====
1927  55               push   bp
1928  89e5             mov    bp, sp
192A  31c0             xor    ax, ax
192C  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
1931  c706e02a0c00     mov    word ptr [0x2ae0], 0xc
1937  ff36822b         push   word ptr [0x2b82]
193B  ff36842b         push   word ptr [0x2b84]
193F  a1842b           mov    ax, word ptr [0x2b84]
1942  d1e0             shl    ax, 1
1944  8bd8             mov    bx, ax
1946  a1822b           mov    ax, word ptr [0x2b82]
1949  ba1800           mov    dx, 0x18
194C  f7e2             mul    dx
194E  8bc8             mov    cx, ax
1950  a1e42a           mov    ax, word ptr [0x2ae4]
1953  bae001           mov    dx, 0x1e0
1956  f7e2             mul    dx
1958  8bf8             mov    di, ax
195A  03f9             add    di, cx
195C  03fb             add    di, bx
195E  ffb53c03         push   word ptr [di + 0x33c]
1962  e88aea           call   0x3ef
1965  ff36822b         push   word ptr [0x2b82]
1969  e860ea           call   0x3cc
196C  2d0800           sub    ax, 8
196F  50               push   ax
1970  ff36842b         push   word ptr [0x2b84]
1974  e855ea           call   0x3cc
1977  050800           add    ax, 8
197A  50               push   ax
197B  8b3ee02a         mov    di, word ptr [0x2ae0]
197F  d1e7             shl    di, 1
1981  d1e7             shl    di, 1
1983  c4bd8804         les    di, ptr [di + 0x488]
1987  06               push   es
1988  57               push   di
1989  31c0             xor    ax, ax
198B  50               push   ax
198C  9a340ed302       lcall  0x2d3, 0xe34   ; PutImage
1991  b82500           mov    ax, 0x25
1994  50               push   ax
1995  e814f5           call   0xeac
1998  a1822b           mov    ax, word ptr [0x2b82]
199B  48               dec    ax
199C  50               push   ax
199D  ff36842b         push   word ptr [0x2b84]
19A1  a1842b           mov    ax, word ptr [0x2b84]
19A4  d1e0             shl    ax, 1
19A6  8bd8             mov    bx, ax
19A8  a1822b           mov    ax, word ptr [0x2b82]
19AB  48               dec    ax
19AC  ba1800           mov    dx, 0x18
19AF  f7e2             mul    dx
19B1  8bc8             mov    cx, ax
19B3  a1e42a           mov    ax, word ptr [0x2ae4]
19B6  bae001           mov    dx, 0x1e0
19B9  f7e2             mul    dx
19BB  8bf8             mov    di, ax
19BD  03f9             add    di, cx
19BF  03fb             add    di, bx
19C1  ffb53c03         push   word ptr [di + 0x33c]
19C5  e827ea           call   0x3ef
19C8  ff36822b         push   word ptr [0x2b82]
19CC  ff36842b         push   word ptr [0x2b84]
19D0  a1842b           mov    ax, word ptr [0x2b84]
19D3  d1e0             shl    ax, 1
19D5  8bd8             mov    bx, ax
19D7  a1822b           mov    ax, word ptr [0x2b82]
19DA  ba1800           mov    dx, 0x18
19DD  f7e2             mul    dx
19DF  8bc8             mov    cx, ax
19E1  a1e42a           mov    ax, word ptr [0x2ae4]
19E4  bae001           mov    dx, 0x1e0
19E7  f7e2             mul    dx
19E9  8bf8             mov    di, ax
19EB  03f9             add    di, cx
19ED  03fb             add    di, bx
19EF  ffb53c03         push   word ptr [di + 0x33c]
19F3  e8f9e9           call   0x3ef
19F6  a1822b           mov    ax, word ptr [0x2b82]
19F9  48               dec    ax
19FA  50               push   ax
19FB  a1842b           mov    ax, word ptr [0x2b84]
19FE  40               inc    ax
19FF  50               push   ax
1A00  a1842b           mov    ax, word ptr [0x2b84]
1A03  40               inc    ax
1A04  d1e0             shl    ax, 1
1A06  8bd8             mov    bx, ax
1A08  a1822b           mov    ax, word ptr [0x2b82]
1A0B  48               dec    ax
1A0C  ba1800           mov    dx, 0x18
1A0F  f7e2             mul    dx
1A11  8bc8             mov    cx, ax
1A13  a1e42a           mov    ax, word ptr [0x2ae4]
1A16  bae001           mov    dx, 0x1e0
1A19  f7e2             mul    dx
1A1B  8bf8             mov    di, ax
1A1D  03f9             add    di, cx
1A1F  03fb             add    di, bx
1A21  ffb53c03         push   word ptr [di + 0x33c]
1A25  e8c7e9           call   0x3ef
1A28  ff36822b         push   word ptr [0x2b82]
1A2C  a1842b           mov    ax, word ptr [0x2b84]
1A2F  40               inc    ax
1A30  50               push   ax
1A31  a1842b           mov    ax, word ptr [0x2b84]
1A34  40               inc    ax
1A35  d1e0             shl    ax, 1
1A37  8bd8             mov    bx, ax
1A39  a1822b           mov    ax, word ptr [0x2b82]
1A3C  ba1800           mov    dx, 0x18
1A3F  f7e2             mul    dx
1A41  8bc8             mov    cx, ax
1A43  a1e42a           mov    ax, word ptr [0x2ae4]
1A46  bae001           mov    dx, 0x1e0
1A49  f7e2             mul    dx
1A4B  8bf8             mov    di, ax
1A4D  03f9             add    di, cx
1A4F  03fb             add    di, bx
1A51  ffb53c03         push   word ptr [di + 0x33c]
1A55  e897e9           call   0x3ef
1A58  c706e02a0a00     mov    word ptr [0x2ae0], 0xa
1A5E  a1822b           mov    ax, word ptr [0x2b82]
1A61  48               dec    ax
1A62  a3822b           mov    word ptr [0x2b82], ax
1A65  a1842b           mov    ax, word ptr [0x2b84]
1A68  40               inc    ax
1A69  a3842b           mov    word ptr [0x2b84], ax
1A6C  ff36822b         push   word ptr [0x2b82]
1A70  ff36842b         push   word ptr [0x2b84]
1A74  ff36e02a         push   word ptr [0x2ae0]
1A78  e874e9           call   0x3ef
1A7B  31c0             xor    ax, ax
1A7D  a38c2b           mov    word ptr [0x2b8c], ax
1A80  e8d5f8           call   0x1358
1A83  89ec             mov    sp, bp
1A85  5d               pop    bp
1A86  c3               ret    

;===== proc_1A87  (len 352) =====
1A87  55               push   bp
1A88  89e5             mov    bp, sp
1A8A  31c0             xor    ax, ax
1A8C  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
1A91  c706e02a1000     mov    word ptr [0x2ae0], 0x10
1A97  ff36822b         push   word ptr [0x2b82]
1A9B  ff36842b         push   word ptr [0x2b84]
1A9F  a1842b           mov    ax, word ptr [0x2b84]
1AA2  d1e0             shl    ax, 1
1AA4  8bd8             mov    bx, ax
1AA6  a1822b           mov    ax, word ptr [0x2b82]
1AA9  ba1800           mov    dx, 0x18
1AAC  f7e2             mul    dx
1AAE  8bc8             mov    cx, ax
1AB0  a1e42a           mov    ax, word ptr [0x2ae4]
1AB3  bae001           mov    dx, 0x1e0
1AB6  f7e2             mul    dx
1AB8  8bf8             mov    di, ax
1ABA  03f9             add    di, cx
1ABC  03fb             add    di, bx
1ABE  ffb53c03         push   word ptr [di + 0x33c]
1AC2  e82ae9           call   0x3ef
1AC5  ff36822b         push   word ptr [0x2b82]
1AC9  e800e9           call   0x3cc
1ACC  050800           add    ax, 8
1ACF  50               push   ax
1AD0  ff36842b         push   word ptr [0x2b84]
1AD4  e8f5e8           call   0x3cc
1AD7  050800           add    ax, 8
1ADA  50               push   ax
1ADB  8b3ee02a         mov    di, word ptr [0x2ae0]
1ADF  d1e7             shl    di, 1
1AE1  d1e7             shl    di, 1
1AE3  c4bd8804         les    di, ptr [di + 0x488]
1AE7  06               push   es
1AE8  57               push   di
1AE9  31c0             xor    ax, ax
1AEB  50               push   ax
1AEC  9a340ed302       lcall  0x2d3, 0xe34   ; PutImage
1AF1  b82500           mov    ax, 0x25
1AF4  50               push   ax
1AF5  e8b4f3           call   0xeac
1AF8  ff36822b         push   word ptr [0x2b82]
1AFC  ff36842b         push   word ptr [0x2b84]
1B00  a1842b           mov    ax, word ptr [0x2b84]
1B03  d1e0             shl    ax, 1
1B05  8bd8             mov    bx, ax
1B07  a1822b           mov    ax, word ptr [0x2b82]
1B0A  ba1800           mov    dx, 0x18
1B0D  f7e2             mul    dx
1B0F  8bc8             mov    cx, ax
1B11  a1e42a           mov    ax, word ptr [0x2ae4]
1B14  bae001           mov    dx, 0x1e0
1B17  f7e2             mul    dx
1B19  8bf8             mov    di, ax
1B1B  03f9             add    di, cx
1B1D  03fb             add    di, bx
1B1F  ffb53c03         push   word ptr [di + 0x33c]
1B23  e8c9e8           call   0x3ef
1B26  a1822b           mov    ax, word ptr [0x2b82]
1B29  40               inc    ax
1B2A  50               push   ax
1B2B  ff36842b         push   word ptr [0x2b84]
1B2F  a1842b           mov    ax, word ptr [0x2b84]
1B32  d1e0             shl    ax, 1
1B34  8bd8             mov    bx, ax
1B36  a1822b           mov    ax, word ptr [0x2b82]
1B39  40               inc    ax
1B3A  ba1800           mov    dx, 0x18
1B3D  f7e2             mul    dx
1B3F  8bc8             mov    cx, ax
1B41  a1e42a           mov    ax, word ptr [0x2ae4]
1B44  bae001           mov    dx, 0x1e0
1B47  f7e2             mul    dx
1B49  8bf8             mov    di, ax
1B4B  03f9             add    di, cx
1B4D  03fb             add    di, bx
1B4F  ffb53c03         push   word ptr [di + 0x33c]
1B53  e899e8           call   0x3ef
1B56  ff36822b         push   word ptr [0x2b82]
1B5A  a1842b           mov    ax, word ptr [0x2b84]
1B5D  40               inc    ax
1B5E  50               push   ax
1B5F  a1842b           mov    ax, word ptr [0x2b84]
1B62  40               inc    ax
1B63  d1e0             shl    ax, 1
1B65  8bd8             mov    bx, ax
1B67  a1822b           mov    ax, word ptr [0x2b82]
1B6A  ba1800           mov    dx, 0x18
1B6D  f7e2             mul    dx
1B6F  8bc8             mov    cx, ax
1B71  a1e42a           mov    ax, word ptr [0x2ae4]
1B74  bae001           mov    dx, 0x1e0
1B77  f7e2             mul    dx
1B79  8bf8             mov    di, ax
1B7B  03f9             add    di, cx
1B7D  03fb             add    di, bx
1B7F  ffb53c03         push   word ptr [di + 0x33c]
1B83  e869e8           call   0x3ef
1B86  a1822b           mov    ax, word ptr [0x2b82]
1B89  40               inc    ax
1B8A  50               push   ax
1B8B  a1842b           mov    ax, word ptr [0x2b84]
1B8E  40               inc    ax
1B8F  50               push   ax
1B90  a1842b           mov    ax, word ptr [0x2b84]
1B93  40               inc    ax
1B94  d1e0             shl    ax, 1
1B96  8bd8             mov    bx, ax
1B98  a1822b           mov    ax, word ptr [0x2b82]
1B9B  40               inc    ax
1B9C  ba1800           mov    dx, 0x18
1B9F  f7e2             mul    dx
1BA1  8bc8             mov    cx, ax
1BA3  a1e42a           mov    ax, word ptr [0x2ae4]
1BA6  bae001           mov    dx, 0x1e0
1BA9  f7e2             mul    dx
1BAB  8bf8             mov    di, ax
1BAD  03f9             add    di, cx
1BAF  03fb             add    di, bx
1BB1  ffb53c03         push   word ptr [di + 0x33c]
1BB5  e837e8           call   0x3ef
1BB8  c706e02a0e00     mov    word ptr [0x2ae0], 0xe
1BBE  a1822b           mov    ax, word ptr [0x2b82]
1BC1  40               inc    ax
1BC2  a3822b           mov    word ptr [0x2b82], ax
1BC5  a1842b           mov    ax, word ptr [0x2b84]
1BC8  40               inc    ax
1BC9  a3842b           mov    word ptr [0x2b84], ax
1BCC  ff36822b         push   word ptr [0x2b82]
1BD0  ff36842b         push   word ptr [0x2b84]
1BD4  ff36e02a         push   word ptr [0x2ae0]
1BD8  e814e8           call   0x3ef
1BDB  31c0             xor    ax, ax
1BDD  a38c2b           mov    word ptr [0x2b8c], ax
1BE0  e875f7           call   0x1358
1BE3  89ec             mov    sp, bp
1BE5  5d               pop    bp
1BE6  c3               ret    

;===== proc_1BE7  (len 157) =====
1BE7  55               push   bp
1BE8  89e5             mov    bp, sp
1BEA  31c0             xor    ax, ax
1BEC  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
1BF1  a1842b           mov    ax, word ptr [0x2b84]
1BF4  d1e0             shl    ax, 1
1BF6  8bd8             mov    bx, ax
1BF8  a1822b           mov    ax, word ptr [0x2b82]
1BFB  ba1800           mov    dx, 0x18
1BFE  f7e2             mul    dx
1C00  8bc8             mov    cx, ax
1C02  a1e42a           mov    ax, word ptr [0x2ae4]
1C05  bae001           mov    dx, 0x1e0
1C08  f7e2             mul    dx
1C0A  8bf8             mov    di, ax
1C0C  03f9             add    di, cx
1C0E  03fb             add    di, bx
1C10  83bd3c0306       cmp    word ptr [di + 0x33c], 6
1C15  7569             jne    0x1c80
1C17  b80b02           mov    ax, 0x20b
1C1A  50               push   ax
1C1B  9ac6027102       lcall  0x271, 0x2c6   ; Sound
1C20  b81900           mov    ax, 0x19
1C23  50               push   ax
1C24  e885f2           call   0xeac
1C27  c706e02a0d00     mov    word ptr [0x2ae0], 0xd
1C2D  ff36822b         push   word ptr [0x2b82]
1C31  ff36842b         push   word ptr [0x2b84]
1C35  a1842b           mov    ax, word ptr [0x2b84]
1C38  d1e0             shl    ax, 1
1C3A  8bd8             mov    bx, ax
1C3C  a1822b           mov    ax, word ptr [0x2b82]
1C3F  ba1800           mov    dx, 0x18
1C42  f7e2             mul    dx
1C44  8bc8             mov    cx, ax
1C46  a1e42a           mov    ax, word ptr [0x2ae4]
1C49  bae001           mov    dx, 0x1e0
1C4C  f7e2             mul    dx
1C4E  8bf8             mov    di, ax
1C50  03f9             add    di, cx
1C52  03fb             add    di, bx
1C54  ffb53c03         push   word ptr [di + 0x33c]
1C58  e894e7           call   0x3ef
1C5B  a1842b           mov    ax, word ptr [0x2b84]
1C5E  48               dec    ax
1C5F  a3842b           mov    word ptr [0x2b84], ax
1C62  ff36822b         push   word ptr [0x2b82]
1C66  ff36842b         push   word ptr [0x2b84]
1C6A  ff36e02a         push   word ptr [0x2ae0]
1C6E  e87ee7           call   0x3ef
1C71  9af3027102       lcall  0x271, 0x2f3   ; NoSound
1C76  e8dff6           call   0x1358
1C79  b81900           mov    ax, 0x19
1C7C  50               push   ax
1C7D  e82cf2           call   0xeac
1C80  89ec             mov    sp, bp
1C82  5d               pop    bp
1C83  c3               ret    

;===== proc_1C84  (len 148) =====
1C84  55               push   bp
1C85  89e5             mov    bp, sp
1C87  31c0             xor    ax, ax
1C89  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
1C8E  a1842b           mov    ax, word ptr [0x2b84]
1C91  99               cdq    
1C92  b90300           mov    cx, 3
1C95  f7f9             idiv   cx
1C97  92               xchg   dx, ax
1C98  3d0200           cmp    ax, 2
1C9B  7577             jne    0x1d14
1C9D  b80b02           mov    ax, 0x20b
1CA0  50               push   ax
1CA1  9ac6027102       lcall  0x271, 0x2c6   ; Sound
1CA6  b81900           mov    ax, 0x19
1CA9  50               push   ax
1CAA  e8fff1           call   0xeac
1CAD  833e822b01       cmp    word ptr [0x2b82], 1
1CB2  7e42             jle    0x1cf6
1CB4  ff36822b         push   word ptr [0x2b82]
1CB8  ff36842b         push   word ptr [0x2b84]
1CBC  a1842b           mov    ax, word ptr [0x2b84]
1CBF  d1e0             shl    ax, 1
1CC1  8bd8             mov    bx, ax
1CC3  a1822b           mov    ax, word ptr [0x2b82]
1CC6  ba1800           mov    dx, 0x18
1CC9  f7e2             mul    dx
1CCB  8bc8             mov    cx, ax
1CCD  a1e42a           mov    ax, word ptr [0x2ae4]
1CD0  bae001           mov    dx, 0x1e0
1CD3  f7e2             mul    dx
1CD5  8bf8             mov    di, ax
1CD7  03f9             add    di, cx
1CD9  03fb             add    di, bx
1CDB  ffb53c03         push   word ptr [di + 0x33c]
1CDF  e80de7           call   0x3ef
1CE2  833ee02a0a       cmp    word ptr [0x2ae0], 0xa
1CE7  7507             jne    0x1cf0
1CE9  a1822b           mov    ax, word ptr [0x2b82]
1CEC  48               dec    ax
1CED  a3822b           mov    word ptr [0x2b82], ax
1CF0  c706e02a0a00     mov    word ptr [0x2ae0], 0xa
1CF6  ff36822b         push   word ptr [0x2b82]
1CFA  ff36842b         push   word ptr [0x2b84]
1CFE  ff36e02a         push   word ptr [0x2ae0]
1D02  e8eae6           call   0x3ef
1D05  9af3027102       lcall  0x271, 0x2f3   ; NoSound
1D0A  e84bf6           call   0x1358
1D0D  b81900           mov    ax, 0x19
1D10  50               push   ax
1D11  e898f1           call   0xeac
1D14  89ec             mov    sp, bp
1D16  5d               pop    bp
1D17  c3               ret    

;===== proc_1D18  (len 165) =====
1D18  55               push   bp
1D19  89e5             mov    bp, sp
1D1B  31c0             xor    ax, ax
1D1D  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
1D22  a1842b           mov    ax, word ptr [0x2b84]
1D25  99               cdq    
1D26  b90300           mov    cx, 3
1D29  f7f9             idiv   cx
1D2B  92               xchg   dx, ax
1D2C  3d0200           cmp    ax, 2
1D2F  7403             je     0x1d34
1D31  e98500           jmp    0x1db9
1D34  b80b02           mov    ax, 0x20b
1D37  50               push   ax
1D38  9ac6027102       lcall  0x271, 0x2c6   ; Sound
1D3D  b81900           mov    ax, 0x19
1D40  50               push   ax
1D41  e868f1           call   0xeac
1D44  833e822b14       cmp    word ptr [0x2b82], 0x14
1D49  7d07             jge    0x1d52
1D4B  833e842b0b       cmp    word ptr [0x2b84], 0xb
1D50  7407             je     0x1d59
1D52  833e822b13       cmp    word ptr [0x2b82], 0x13
1D57  7d42             jge    0x1d9b
1D59  ff36822b         push   word ptr [0x2b82]
1D5D  ff36842b         push   word ptr [0x2b84]
1D61  a1842b           mov    ax, word ptr [0x2b84]
1D64  d1e0             shl    ax, 1
1D66  8bd8             mov    bx, ax
1D68  a1822b           mov    ax, word ptr [0x2b82]
1D6B  ba1800           mov    dx, 0x18
1D6E  f7e2             mul    dx
1D70  8bc8             mov    cx, ax
1D72  a1e42a           mov    ax, word ptr [0x2ae4]
1D75  bae001           mov    dx, 0x1e0
1D78  f7e2             mul    dx
1D7A  8bf8             mov    di, ax
1D7C  03f9             add    di, cx
1D7E  03fb             add    di, bx
1D80  ffb53c03         push   word ptr [di + 0x33c]
1D84  e868e6           call   0x3ef
1D87  833ee02a0e       cmp    word ptr [0x2ae0], 0xe
1D8C  7507             jne    0x1d95
1D8E  a1822b           mov    ax, word ptr [0x2b82]
1D91  40               inc    ax
1D92  a3822b           mov    word ptr [0x2b82], ax
1D95  c706e02a0e00     mov    word ptr [0x2ae0], 0xe
1D9B  ff36822b         push   word ptr [0x2b82]
1D9F  ff36842b         push   word ptr [0x2b84]
1DA3  ff36e02a         push   word ptr [0x2ae0]
1DA7  e845e6           call   0x3ef
1DAA  9af3027102       lcall  0x271, 0x2f3   ; NoSound
1DAF  e8a6f5           call   0x1358
1DB2  b81900           mov    ax, 0x19
1DB5  50               push   ax
1DB6  e8f3f0           call   0xeac
1DB9  89ec             mov    sp, bp
1DBB  5d               pop    bp
1DBC  c3               ret    

;===== proc_1DBD  (len 158) =====
1DBD  55               push   bp
1DBE  89e5             mov    bp, sp
1DC0  31c0             xor    ax, ax
1DC2  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
1DC7  a1842b           mov    ax, word ptr [0x2b84]
1DCA  40               inc    ax
1DCB  d1e0             shl    ax, 1
1DCD  8bd8             mov    bx, ax
1DCF  a1822b           mov    ax, word ptr [0x2b82]
1DD2  ba1800           mov    dx, 0x18
1DD5  f7e2             mul    dx
1DD7  8bc8             mov    cx, ax
1DD9  a1e42a           mov    ax, word ptr [0x2ae4]
1DDC  bae001           mov    dx, 0x1e0
1DDF  f7e2             mul    dx
1DE1  8bf8             mov    di, ax
1DE3  03f9             add    di, cx
1DE5  03fb             add    di, bx
1DE7  83bd3c0306       cmp    word ptr [di + 0x33c], 6
1DEC  7569             jne    0x1e57
1DEE  b80b02           mov    ax, 0x20b
1DF1  50               push   ax
1DF2  9ac6027102       lcall  0x271, 0x2c6   ; Sound
1DF7  b81900           mov    ax, 0x19
1DFA  50               push   ax
1DFB  e8aef0           call   0xeac
1DFE  c706e02a0d00     mov    word ptr [0x2ae0], 0xd
1E04  ff36822b         push   word ptr [0x2b82]
1E08  ff36842b         push   word ptr [0x2b84]
1E0C  a1842b           mov    ax, word ptr [0x2b84]
1E0F  d1e0             shl    ax, 1
1E11  8bd8             mov    bx, ax
1E13  a1822b           mov    ax, word ptr [0x2b82]
1E16  ba1800           mov    dx, 0x18
1E19  f7e2             mul    dx
1E1B  8bc8             mov    cx, ax
1E1D  a1e42a           mov    ax, word ptr [0x2ae4]
1E20  bae001           mov    dx, 0x1e0
1E23  f7e2             mul    dx
1E25  8bf8             mov    di, ax
1E27  03f9             add    di, cx
1E29  03fb             add    di, bx
1E2B  ffb53c03         push   word ptr [di + 0x33c]
1E2F  e8bde5           call   0x3ef
1E32  a1842b           mov    ax, word ptr [0x2b84]
1E35  40               inc    ax
1E36  a3842b           mov    word ptr [0x2b84], ax
1E39  ff36822b         push   word ptr [0x2b82]
1E3D  ff36842b         push   word ptr [0x2b84]
1E41  ff36e02a         push   word ptr [0x2ae0]
1E45  e8a7e5           call   0x3ef
1E48  9af3027102       lcall  0x271, 0x2f3   ; NoSound
1E4D  e808f5           call   0x1358
1E50  b81900           mov    ax, 0x19
1E53  50               push   ax
1E54  e855f0           call   0xeac
1E57  89ec             mov    sp, bp
1E59  5d               pop    bp
1E5A  c3               ret    

;===== proc_1E5B  (len 112) =====
1E5B  55               push   bp
1E5C  89e5             mov    bp, sp
1E5E  b80200           mov    ax, 2
1E61  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
1E66  83ec02           sub    sp, 2
1E69  9a0c037102       lcall  0x271, 0x30c   ; ReadKey
1E6E  a28a02           mov    byte ptr [0x28a], al
1E71  a08a02           mov    al, byte ptr [0x28a]
1E74  3c20             cmp    al, 0x20
1E76  7513             jne    0x1e8b
1E78  833e8c2b01       cmp    word ptr [0x2b8c], 1
1E7D  740a             je     0x1e89
1E7F  833e8c2bff       cmp    word ptr [0x2b8c], -1
1E84  7403             je     0x1e89
1E86  e836f6           call   0x14bf
1E89  eb3c             jmp    0x1ec7
1E8B  3c1b             cmp    al, 0x1b
1E8D  7507             jne    0x1e96
1E8F  c606a62b01       mov    byte ptr [0x2ba6], 1
1E94  eb31             jmp    0x1ec7
1E96  3c00             cmp    al, 0
1E98  752d             jne    0x1ec7
1E9A  9a0c037102       lcall  0x271, 0x30c   ; ReadKey
1E9F  a28a02           mov    byte ptr [0x28a], al
1EA2  a08a02           mov    al, byte ptr [0x28a]
1EA5  3c48             cmp    al, 0x48
1EA7  7505             jne    0x1eae
1EA9  e83bfd           call   0x1be7
1EAC  eb19             jmp    0x1ec7
1EAE  3c4b             cmp    al, 0x4b
1EB0  7505             jne    0x1eb7
1EB2  e8cffd           call   0x1c84
1EB5  eb10             jmp    0x1ec7
1EB7  3c4d             cmp    al, 0x4d
1EB9  7505             jne    0x1ec0
1EBB  e85afe           call   0x1d18
1EBE  eb07             jmp    0x1ec7
1EC0  3c50             cmp    al, 0x50
1EC2  7503             jne    0x1ec7
1EC4  e8f6fe           call   0x1dbd
1EC7  89ec             mov    sp, bp
1EC9  5d               pop    bp
1ECA  c3               ret    

;===== proc_1ECB  (len 273) =====
1ECB  55               push   bp
1ECC  89e5             mov    bp, sp
1ECE  b80600           mov    ax, 6
1ED1  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
1ED6  83ec06           sub    sp, 6
1ED9  c6069e2b00       mov    byte ptr [0x2b9e], 0
1EDE  c6069f2b00       mov    byte ptr [0x2b9f], 0
1EE3  c706e82a0300     mov    word ptr [0x2ae8], 3
1EE9  31c0             xor    ax, ax
1EEB  a3942b           mov    word ptr [0x2b94], ax
1EEE  a3962b           mov    word ptr [0x2b96], ax
1EF1  c7068e2b6400     mov    word ptr [0x2b8e], 0x64
1EF7  c706822b1400     mov    word ptr [0x2b82], 0x14
1EFD  c706842b0b00     mov    word ptr [0x2b84], 0xb
1F03  c706e02a0a00     mov    word ptr [0x2ae0], 0xa
1F09  31c0             xor    ax, ax
1F0B  a38c2b           mov    word ptr [0x2b8c], ax
1F0E  c706542b0b00     mov    word ptr [0x2b54], 0xb
1F14  c706562b0800     mov    word ptr [0x2b56], 8
1F1A  c706582b0500     mov    word ptr [0x2b58], 5
1F20  c7065a2b0200     mov    word ptr [0x2b5a], 2
1F26  c746fe0100       mov    word ptr [bp - 2], 1
1F2B  eb03             jmp    0x1f30
1F2D  ff46fe           inc    word ptr [bp - 2]
1F30  8b7efe           mov    di, word ptr [bp - 2]
1F33  d1e7             shl    di, 1
1F35  c7855a2b0900     mov    word ptr [di + 0x2b5a], 9
1F3B  8b7efe           mov    di, word ptr [bp - 2]
1F3E  d1e7             shl    di, 1
1F40  c7853a2b0100     mov    word ptr [di + 0x2b3a], 1
1F46  8b7efe           mov    di, word ptr [bp - 2]
1F49  d1e7             shl    di, 1
1F4B  8bf7             mov    si, di
1F4D  d1e7             shl    di, 1
1F4F  03fe             add    di, si
1F51  c7855e2b0000     mov    word ptr [di + 0x2b5e], 0
1F57  c785602b0000     mov    word ptr [di + 0x2b60], 0
1F5D  c785622b0000     mov    word ptr [di + 0x2b62], 0
1F63  8b46fe           mov    ax, word ptr [bp - 2]
1F66  d1e0             shl    ax, 1
1F68  8b3ee42a         mov    di, word ptr [0x2ae4]
1F6C  b103             mov    cl, 3
1F6E  d3e7             shl    di, cl
1F70  03f8             add    di, ax
1F72  83bde22a00       cmp    word ptr [di + 0x2ae2], 0
1F77  750d             jne    0x1f86
1F79  8b7efe           mov    di, word ptr [bp - 2]
1F7C  d1e7             shl    di, 1
1F7E  31c0             xor    ax, ax
1F80  89854a2b         mov    word ptr [di + 0x2b4a], ax
1F84  eb29             jmp    0x1faf
1F86  8b7efe           mov    di, word ptr [bp - 2]
1F89  d1e7             shl    di, 1
1F8B  c7854a2b0100     mov    word ptr [di + 0x2b4a], 1
1F91  8b7efe           mov    di, word ptr [bp - 2]
1F94  d1e7             shl    di, 1
1F96  ffb54a2b         push   word ptr [di + 0x2b4a]
1F9A  8b7efe           mov    di, word ptr [bp - 2]
1F9D  d1e7             shl    di, 1
1F9F  ffb5522b         push   word ptr [di + 0x2b52]
1FA3  8b7efe           mov    di, word ptr [bp - 2]
1FA6  d1e7             shl    di, 1
1FA8  ffb55a2b         push   word ptr [di + 0x2b5a]
1FAC  e840e4           call   0x3ef
1FAF  837efe04         cmp    word ptr [bp - 2], 4
1FB3  7403             je     0x1fb8
1FB5  e975ff           jmp    0x1f2d
1FB8  ff36822b         push   word ptr [0x2b82]
1FBC  ff36842b         push   word ptr [0x2b84]
1FC0  ff36e02a         push   word ptr [0x2ae0]
1FC4  e828e4           call   0x3ef
1FC7  31c0             xor    ax, ax
1FC9  50               push   ax
1FCA  31c0             xor    ax, ax
1FCC  50               push   ax
1FCD  31c0             xor    ax, ax
1FCF  50               push   ax
1FD0  31c0             xor    ax, ax
1FD2  50               push   ax
1FD3  9a5b006902       lcall  0x269, 0x5b   ; SetTime
1FD8  89ec             mov    sp, bp
1FDA  5d               pop    bp
1FDB  c3               ret    

;===== proc_1FDC  (len 224) =====
1FDC  55               push   bp
1FDD  89e5             mov    bp, sp
1FDF  31c0             xor    ax, ax
1FE1  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
1FE6  ff36822b         push   word ptr [0x2b82]
1FEA  ff36842b         push   word ptr [0x2b84]
1FEE  b81100           mov    ax, 0x11
1FF1  50               push   ax
1FF2  e8fae3           call   0x3ef
1FF5  b81604           mov    ax, 0x416
1FF8  50               push   ax
1FF9  9ac6027102       lcall  0x271, 0x2c6   ; Sound
1FFE  b81900           mov    ax, 0x19
2001  50               push   ax
2002  9a9e027102       lcall  0x271, 0x29e   ; Delay
2007  b8dc03           mov    ax, 0x3dc
200A  50               push   ax
200B  9ac6027102       lcall  0x271, 0x2c6   ; Sound
2010  b81900           mov    ax, 0x19
2013  50               push   ax
2014  9a9e027102       lcall  0x271, 0x29e   ; Delay
2019  b87003           mov    ax, 0x370
201C  50               push   ax
201D  9ac6027102       lcall  0x271, 0x2c6   ; Sound
2022  b81900           mov    ax, 0x19
2025  50               push   ax
2026  9a9e027102       lcall  0x271, 0x29e   ; Delay
202B  b81003           mov    ax, 0x310
202E  50               push   ax
202F  9ac6027102       lcall  0x271, 0x2c6   ; Sound
2034  b81900           mov    ax, 0x19
2037  50               push   ax
2038  9a9e027102       lcall  0x271, 0x29e   ; Delay
203D  b8ba02           mov    ax, 0x2ba
2040  50               push   ax
2041  9ac6027102       lcall  0x271, 0x2c6   ; Sound
2046  b81900           mov    ax, 0x19
2049  50               push   ax
204A  9a9e027102       lcall  0x271, 0x29e   ; Delay
204F  b89302           mov    ax, 0x293
2052  50               push   ax
2053  9ac6027102       lcall  0x271, 0x2c6   ; Sound
2058  b81900           mov    ax, 0x19
205B  50               push   ax
205C  9a9e027102       lcall  0x271, 0x29e   ; Delay
2061  b84b02           mov    ax, 0x24b
2064  50               push   ax
2065  9ac6027102       lcall  0x271, 0x2c6   ; Sound
206A  b81900           mov    ax, 0x19
206D  50               push   ax
206E  9a9e027102       lcall  0x271, 0x29e   ; Delay
2073  b80b02           mov    ax, 0x20b
2076  50               push   ax
2077  9ac6027102       lcall  0x271, 0x2c6   ; Sound
207C  b81900           mov    ax, 0x19
207F  50               push   ax
2080  9a9e027102       lcall  0x271, 0x29e   ; Delay
2085  9af3027102       lcall  0x271, 0x2f3   ; NoSound
208A  a1e82a           mov    ax, word ptr [0x2ae8]
208D  48               dec    ax
208E  a3e82a           mov    word ptr [0x2ae8], ax
2091  89ec             mov    sp, bp
2093  5d               pop    bp
2094  c3               ret    
2095  STR 'HI-SCORE       SCORE       TIME100    '

;===== proc_20BC  (len 527) =====
20BC  55               push   bp
20BD  89e5             mov    bp, sp
20BF  b81a01           mov    ax, 0x11a
20C2  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
20C7  81ec1a01         sub    sp, 0x11a
20CB  9afa027102       lcall  0x271, 0x2fa   ; KeyPressed
20D0  08c0             or     al, al
20D2  740a             je     0x20de
20D4  9a0c037102       lcall  0x271, 0x30c   ; ReadKey
20D9  a28a02           mov    byte ptr [0x28a], al
20DC  ebed             jmp    0x20cb
20DE  c606a02b00       mov    byte ptr [0x2ba0], 0
20E3  9a280bd302       lcall  0x2d3, 0xb28   ; ClearDevice
20E8  c746ea0100       mov    word ptr [bp - 0x16], 1
20ED  eb03             jmp    0x20f2
20EF  ff46ea           inc    word ptr [bp - 0x16]
20F2  c746ec0100       mov    word ptr [bp - 0x14], 1
20F7  eb03             jmp    0x20fc
20F9  ff46ec           inc    word ptr [bp - 0x14]
20FC  8b46ea           mov    ax, word ptr [bp - 0x16]
20FF  d1e0             shl    ax, 1
2101  8bd8             mov    bx, ax
2103  8b46ec           mov    ax, word ptr [bp - 0x14]
2106  ba1800           mov    dx, 0x18
2109  f7e2             mul    dx
210B  8bc8             mov    cx, ax
210D  a1e42a           mov    ax, word ptr [0x2ae4]
2110  bae001           mov    dx, 0x1e0
2113  f7e2             mul    dx
2115  8bf8             mov    di, ax
2117  03f9             add    di, cx
2119  03fb             add    di, bx
211B  ffb5fc15         push   word ptr [di + 0x15fc]
211F  8b46ea           mov    ax, word ptr [bp - 0x16]
2122  d1e0             shl    ax, 1
2124  8bd8             mov    bx, ax
2126  8b46ec           mov    ax, word ptr [bp - 0x14]
2129  ba1800           mov    dx, 0x18
212C  f7e2             mul    dx
212E  8bc8             mov    cx, ax
2130  a1e42a           mov    ax, word ptr [0x2ae4]
2133  bae001           mov    dx, 0x1e0
2136  f7e2             mul    dx
2138  8bf8             mov    di, ax
213A  03f9             add    di, cx
213C  03fb             add    di, bx
213E  58               pop    ax
213F  89853c03         mov    word ptr [di + 0x33c], ax
2143  837eec14         cmp    word ptr [bp - 0x14], 0x14
2147  75b0             jne    0x20f9
2149  837eea0b         cmp    word ptr [bp - 0x16], 0xb
214D  75a0             jne    0x20ef
214F  8b3ee42a         mov    di, word ptr [0x2ae4]
2153  d1e7             shl    di, 1
2155  8b85c82a         mov    ax, word ptr [di + 0x2ac8]
2159  8b3ee42a         mov    di, word ptr [0x2ae4]
215D  d1e7             shl    di, 1
215F  8985b42a         mov    word ptr [di + 0x2ab4], ax
2163  ff36e42a         push   word ptr [0x2ae4]
2167  e871e6           call   0x7db
216A  31c0             xor    ax, ax
216C  50               push   ax
216D  b8be00           mov    ax, 0xbe
2170  50               push   ax
2171  bf9520           mov    di, 0x2095
2174  0e               push   cs
2175  57               push   di
2176  9a5617d302       lcall  0x2d3, 0x1756   ; OutTextXY
217B  b84000           mov    ax, 0x40
217E  50               push   ax
217F  b8be00           mov    ax, 0xbe
2182  50               push   ax
2183  8dbee6fe         lea    di, [bp - 0x11a]
2187  16               push   ss
2188  57               push   di
2189  b030             mov    al, 0x30
218B  50               push   ax
218C  ff36922b         push   word ptr [0x2b92]
2190  ff36902b         push   word ptr [0x2b90]
2194  e898e8           call   0xa2f
2197  9a5617d302       lcall  0x2d3, 0x1756   ; OutTextXY
219C  b8a000           mov    ax, 0xa0
219F  50               push   ax
21A0  b8be00           mov    ax, 0xbe
21A3  50               push   ax
21A4  8dbee6fe         lea    di, [bp - 0x11a]
21A8  16               push   ss
21A9  57               push   di
21AA  b030             mov    al, 0x30
21AC  50               push   ax
21AD  ff36962b         push   word ptr [0x2b96]
21B1  ff36942b         push   word ptr [0x2b94]
21B5  e877e8           call   0xa2f
21B8  9a5617d302       lcall  0x2d3, 0x1756   ; OutTextXY
21BD  a1e82a           mov    ax, word ptr [0x2ae8]
21C0  99               cdq    
21C1  52               push   dx
21C2  50               push   ax
21C3  31c0             xor    ax, ax
21C5  50               push   ax
21C6  8d7ef0           lea    di, [bp - 0x10]
21C9  16               push   ss
21CA  57               push   di
21CB  b80600           mov    ax, 6
21CE  50               push   ax
21CF  9a4c0c0d06       lcall  0x60d, 0xc4c   ; @Str
21D4  b81401           mov    ax, 0x114
21D7  50               push   ax
21D8  b8b900           mov    ax, 0xb9
21DB  50               push   ax
21DC  c43ec004         les    di, ptr [0x4c0]
21E0  06               push   es
21E1  57               push   di
21E2  b80100           mov    ax, 1
21E5  50               push   ax
21E6  9a340ed302       lcall  0x2d3, 0xe34   ; PutImage
21EB  b82801           mov    ax, 0x128
21EE  50               push   ax
21EF  b8be00           mov    ax, 0xbe
21F2  50               push   ax
21F3  8d7ef0           lea    di, [bp - 0x10]
21F6  16               push   ss
21F7  57               push   di
21F8  9a5617d302       lcall  0x2d3, 0x1756   ; OutTextXY
21FD  c7068e2b6400     mov    word ptr [0x2b8e], 0x64
2203  c706822b1400     mov    word ptr [0x2b82], 0x14
2209  c706842b0b00     mov    word ptr [0x2b84], 0xb
220F  c706e02a0a00     mov    word ptr [0x2ae0], 0xa
2215  c746ee0100       mov    word ptr [bp - 0x12], 1
221A  eb03             jmp    0x221f
221C  ff46ee           inc    word ptr [bp - 0x12]
221F  8b7eee           mov    di, word ptr [bp - 0x12]
2222  d1e7             shl    di, 1
2224  c7855a2b0900     mov    word ptr [di + 0x2b5a], 9
222A  8b7eee           mov    di, word ptr [bp - 0x12]
222D  d1e7             shl    di, 1
222F  c7853a2b0100     mov    word ptr [di + 0x2b3a], 1
2235  8b7eee           mov    di, word ptr [bp - 0x12]
2238  d1e7             shl    di, 1
223A  8bf7             mov    si, di
223C  d1e7             shl    di, 1
223E  03fe             add    di, si
2240  c7855e2b0000     mov    word ptr [di + 0x2b5e], 0
2246  c785602b0000     mov    word ptr [di + 0x2b60], 0
224C  c785622b0000     mov    word ptr [di + 0x2b62], 0
2252  8b46ee           mov    ax, word ptr [bp - 0x12]
2255  d1e0             shl    ax, 1
2257  8b3ee42a         mov    di, word ptr [0x2ae4]
225B  b103             mov    cl, 3
225D  d3e7             shl    di, cl
225F  03f8             add    di, ax
2261  83bde22a00       cmp    word ptr [di + 0x2ae2], 0
2266  750d             jne    0x2275
2268  8b7eee           mov    di, word ptr [bp - 0x12]
226B  d1e7             shl    di, 1
226D  31c0             xor    ax, ax
226F  89854a2b         mov    word ptr [di + 0x2b4a], ax
2273  eb29             jmp    0x229e
2275  8b7eee           mov    di, word ptr [bp - 0x12]
2278  d1e7             shl    di, 1
227A  c7854a2b0100     mov    word ptr [di + 0x2b4a], 1
2280  8b7eee           mov    di, word ptr [bp - 0x12]
2283  d1e7             shl    di, 1
2285  ffb54a2b         push   word ptr [di + 0x2b4a]
2289  8b7eee           mov    di, word ptr [bp - 0x12]
228C  d1e7             shl    di, 1
228E  ffb5522b         push   word ptr [di + 0x2b52]
2292  8b7eee           mov    di, word ptr [bp - 0x12]
2295  d1e7             shl    di, 1
2297  ffb55a2b         push   word ptr [di + 0x2b5a]
229B  e851e1           call   0x3ef
229E  837eee04         cmp    word ptr [bp - 0x12], 4
22A2  7403             je     0x22a7
22A4  e975ff           jmp    0x221c
22A7  ff36822b         push   word ptr [0x2b82]
22AB  ff36842b         push   word ptr [0x2b84]
22AF  ff36e02a         push   word ptr [0x2ae0]
22B3  e839e1           call   0x3ef
22B6  31c0             xor    ax, ax
22B8  50               push   ax
22B9  31c0             xor    ax, ax
22BB  50               push   ax
22BC  31c0             xor    ax, ax
22BE  50               push   ax
22BF  31c0             xor    ax, ax
22C1  50               push   ax
22C2  9a5b006902       lcall  0x269, 0x5b   ; SetTime
22C7  89ec             mov    sp, bp
22C9  5d               pop    bp
22CA  c3               ret    

;===== proc_22CB  (len 62) =====
22CB  55               push   bp
22CC  89e5             mov    bp, sp
22CE  b80a00           mov    ax, 0xa
22D1  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
22D6  83ec0a           sub    sp, 0xa
22D9  e800fd           call   0x1fdc
22DC  b8f401           mov    ax, 0x1f4
22DF  50               push   ax
22E0  9a9e027102       lcall  0x271, 0x29e   ; Delay
22E5  c6069f2b00       mov    byte ptr [0x2b9f], 0
22EA  e8cffd           call   0x20bc
22ED  89ec             mov    sp, bp
22EF  5d               pop    bp
22F0  c3               ret    
22F1  STR 'BONUS is +'
22FC  STR 'NEXT Item is'

;===== proc_2309  (len 463) =====
2309  55               push   bp
230A  89e5             mov    bp, sp
230C  b81201           mov    ax, 0x112
230F  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
2314  81ec1201         sub    sp, 0x112
2318  c6069f2b00       mov    byte ptr [0x2b9f], 0
231D  c6069e2b00       mov    byte ptr [0x2b9e], 0
2322  9a280bd302       lcall  0x2d3, 0xb28   ; ClearDevice
2327  a1ea2a           mov    ax, word ptr [0x2aea]
232A  40               inc    ax
232B  a3ea2a           mov    word ptr [0x2aea], ax
232E  833eea2a1c       cmp    word ptr [0x2aea], 0x1c
2333  7506             jne    0x233b
2335  c706ea2a1200     mov    word ptr [0x2aea], 0x12
233B  a1e42a           mov    ax, word ptr [0x2ae4]
233E  40               inc    ax
233F  a3e42a           mov    word ptr [0x2ae4], ax
2342  a1e62a           mov    ax, word ptr [0x2ae6]
2345  40               inc    ax
2346  a3e62a           mov    word ptr [0x2ae6], ax
2349  833ee42a0b       cmp    word ptr [0x2ae4], 0xb
234E  7403             je     0x2353
2350  e98200           jmp    0x23d5
2353  c706e42a0100     mov    word ptr [0x2ae4], 1
2359  c746ee0100       mov    word ptr [bp - 0x12], 1
235E  eb03             jmp    0x2363
2360  ff46ee           inc    word ptr [bp - 0x12]
2363  8b46ee           mov    ax, word ptr [bp - 0x12]
2366  d1e0             shl    ax, 1
2368  8b3ee42a         mov    di, word ptr [0x2ae4]
236C  b103             mov    cl, 3
236E  d3e7             shl    di, cl
2370  03f8             add    di, ax
2372  83bde22a00       cmp    word ptr [di + 0x2ae2], 0
2377  742b             je     0x23a4
2379  8b46ee           mov    ax, word ptr [bp - 0x12]
237C  d1e0             shl    ax, 1
237E  8b3ee42a         mov    di, word ptr [0x2ae4]
2382  b103             mov    cl, 3
2384  d3e7             shl    di, cl
2386  03f8             add    di, ax
2388  8b85e22a         mov    ax, word ptr [di + 0x2ae2]
238C  050200           add    ax, 2
238F  8bd0             mov    dx, ax
2391  8b46ee           mov    ax, word ptr [bp - 0x12]
2394  d1e0             shl    ax, 1
2396  8b3ee42a         mov    di, word ptr [0x2ae4]
239A  b103             mov    cl, 3
239C  d3e7             shl    di, cl
239E  03f8             add    di, ax
23A0  8995e22a         mov    word ptr [di + 0x2ae2], dx
23A4  8b46ee           mov    ax, word ptr [bp - 0x12]
23A7  d1e0             shl    ax, 1
23A9  8b3ee42a         mov    di, word ptr [0x2ae4]
23AD  b103             mov    cl, 3
23AF  d3e7             shl    di, cl
23B1  03f8             add    di, ax
23B3  83bde22a10       cmp    word ptr [di + 0x2ae2], 0x10
23B8  7e15             jle    0x23cf
23BA  8b46ee           mov    ax, word ptr [bp - 0x12]
23BD  d1e0             shl    ax, 1
23BF  8b3ee42a         mov    di, word ptr [0x2ae4]
23C3  b103             mov    cl, 3
23C5  d3e7             shl    di, cl
23C7  03f8             add    di, ax
23C9  c785e22a1000     mov    word ptr [di + 0x2ae2], 0x10
23CF  837eee04         cmp    word ptr [bp - 0x12], 4
23D3  758b             jne    0x2360
23D5  a18e2b           mov    ax, word ptr [0x2b8e]
23D8  b96400           mov    cx, 0x64
23DB  f7e9             imul   cx
23DD  99               cdq    
23DE  0306942b         add    ax, word ptr [0x2b94]
23E2  1316962b         adc    dx, word ptr [0x2b96]
23E6  a3942b           mov    word ptr [0x2b94], ax
23E9  8916962b         mov    word ptr [0x2b96], dx
23ED  8dbeeefe         lea    di, [bp - 0x112]
23F1  16               push   ss
23F2  57               push   di
23F3  b030             mov    al, 0x30
23F5  50               push   ax
23F6  a18e2b           mov    ax, word ptr [0x2b8e]
23F9  b96400           mov    cx, 0x64
23FC  f7e9             imul   cx
23FE  99               cdq    
23FF  52               push   dx
2400  50               push   ax
2401  e82be6           call   0xa2f
2404  8d7ef8           lea    di, [bp - 8]
2407  16               push   ss
2408  57               push   di
2409  b80600           mov    ax, 6
240C  50               push   ax
240D  9a4e060d06       lcall  0x60d, 0x64e   ; @StrCopy
2412  8d7ef8           lea    di, [bp - 8]
2415  16               push   ss
2416  57               push   di
2417  b80100           mov    ax, 1
241A  50               push   ax
241B  b80200           mov    ax, 2
241E  50               push   ax
241F  9ad0070d06       lcall  0x60d, 0x7d0   ; @Pos
2424  b86400           mov    ax, 0x64
2427  50               push   ax
2428  b86100           mov    ax, 0x61
242B  50               push   ax
242C  8dbeeefe         lea    di, [bp - 0x112]
2430  16               push   ss
2431  57               push   di
2432  bff122           mov    di, 0x22f1
2435  0e               push   cs
2436  57               push   di
2437  9a34060d06       lcall  0x60d, 0x634   ; @FillChar
243C  8d7ef8           lea    di, [bp - 8]
243F  16               push   ss
2440  57               push   di
2441  9ac1060d06       lcall  0x60d, 0x6c1   ; @StrCmp
2446  9a5617d302       lcall  0x2d3, 0x1756   ; OutTextXY
244B  b80b02           mov    ax, 0x20b
244E  50               push   ax
244F  9ac6027102       lcall  0x271, 0x2c6   ; Sound
2454  b86400           mov    ax, 0x64
2457  50               push   ax
2458  9a9e027102       lcall  0x271, 0x29e   ; Delay
245D  b89302           mov    ax, 0x293
2460  50               push   ax
2461  9ac6027102       lcall  0x271, 0x2c6   ; Sound
2466  b86400           mov    ax, 0x64
2469  50               push   ax
246A  9a9e027102       lcall  0x271, 0x29e   ; Delay
246F  b81003           mov    ax, 0x310
2472  50               push   ax
2473  9ac6027102       lcall  0x271, 0x2c6   ; Sound
2478  b86400           mov    ax, 0x64
247B  50               push   ax
247C  9a9e027102       lcall  0x271, 0x29e   ; Delay
2481  b81704           mov    ax, 0x417
2484  50               push   ax
2485  9ac6027102       lcall  0x271, 0x2c6   ; Sound
248A  b86400           mov    ax, 0x64
248D  50               push   ax
248E  9a9e027102       lcall  0x271, 0x29e   ; Delay
2493  9af3027102       lcall  0x271, 0x2f3   ; NoSound
2498  b87800           mov    ax, 0x78
249B  50               push   ax
249C  b88200           mov    ax, 0x82
249F  50               push   ax
24A0  bffc22           mov    di, 0x22fc
24A3  0e               push   cs
24A4  57               push   di
24A5  9a5617d302       lcall  0x2d3, 0x1756   ; OutTextXY
24AA  b8d800           mov    ax, 0xd8
24AD  50               push   ax
24AE  b87e00           mov    ax, 0x7e
24B1  50               push   ax
24B2  8b3eea2a         mov    di, word ptr [0x2aea]
24B6  d1e7             shl    di, 1
24B8  d1e7             shl    di, 1
24BA  c4bd8804         les    di, ptr [di + 0x488]
24BE  06               push   es
24BF  57               push   di
24C0  31c0             xor    ax, ax
24C2  50               push   ax
24C3  9a340ed302       lcall  0x2d3, 0xe34   ; PutImage
24C8  b8dc05           mov    ax, 0x5dc
24CB  50               push   ax
24CC  9a9e027102       lcall  0x271, 0x29e   ; Delay
24D1  e8e8fb           call   0x20bc
24D4  89ec             mov    sp, bp
24D6  5d               pop    bp
24D7  c3               ret    

;===== proc_24D8  (len 104) =====
24D8  55               push   bp
24D9  89e5             mov    bp, sp
24DB  31c0             xor    ax, ax
24DD  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
24E2  e8e6f9           call   0x1ecb
24E5  803e9f2b00       cmp    byte ptr [0x2b9f], 0
24EA  7411             je     0x24fd
24EC  833ee82a00       cmp    word ptr [0x2ae8], 0
24F1  740a             je     0x24fd
24F3  803e9e2b00       cmp    byte ptr [0x2b9e], 0
24F8  7503             jne    0x24fd
24FA  e8cefd           call   0x22cb
24FD  803e9e2b00       cmp    byte ptr [0x2b9e], 0
2502  7403             je     0x2507
2504  e802fe           call   0x2309
2507  833e8c2bff       cmp    word ptr [0x2b8c], -1
250C  7505             jne    0x2513
250E  e816f4           call   0x1927
2511  eb18             jmp    0x252b
2513  833e8c2b01       cmp    word ptr [0x2b8c], 1
2518  7505             jne    0x251f
251A  e86af5           call   0x1a87
251D  eb0c             jmp    0x252b
251F  9afa027102       lcall  0x271, 0x2fa   ; KeyPressed
2524  08c0             or     al, al
2526  7403             je     0x252b
2528  e830f9           call   0x1e5b
252B  e8e4e5           call   0xb12
252E  803e8a021b       cmp    byte ptr [0x28a], 0x1b
2533  7407             je     0x253c
2535  833ee82a00       cmp    word ptr [0x2ae8], 0
253A  75a9             jne    0x24e5
253C  89ec             mov    sp, bp
253E  5d               pop    bp
253F  c3               ret    

;===== proc_2540  (len 57) =====
2540  55               push   bp
2541  89e5             mov    bp, sp
2543  31c0             xor    ax, ax
2545  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
254A  c606a62b00       mov    byte ptr [0x2ba6], 0
254F  31c0             xor    ax, ax
2551  a38c2b           mov    word ptr [0x2b8c], ax
2554  c706ea2a1200     mov    word ptr [0x2aea], 0x12
255A  c706e42a0100     mov    word ptr [0x2ae4], 1
2560  c706e62a0100     mov    word ptr [0x2ae6], 1
2566  c606a02b00       mov    byte ptr [0x2ba0], 0
256B  89ec             mov    sp, bp
256D  5d               pop    bp
256E  c3               ret    
256F  STR 'GAME OVER'

;===== proc_2579  (len 137) =====
2579  55               push   bp
257A  89e5             mov    bp, sp
257C  31c0             xor    ax, ax
257E  9a44020d06       lcall  0x60d, 0x244   ; @StackCheck
2583  9afa027102       lcall  0x271, 0x2fa   ; KeyPressed
2588  08c0             or     al, al
258A  740a             je     0x2596
258C  9a0c037102       lcall  0x271, 0x30c   ; ReadKey
2591  a28a02           mov    byte ptr [0x28a], al
2594  ebed             jmp    0x2583
2596  b80100           mov    ax, 1
2599  50               push   ax
259A  31c0             xor    ax, ax
259C  50               push   ax
259D  9a780cd302       lcall  0x2d3, 0xc78   ; SetTextStyle?
25A2  b87800           mov    ax, 0x78
25A5  50               push   ax
25A6  b86400           mov    ax, 0x64
25A9  50               push   ax
25AA  b8c800           mov    ax, 0xc8
25AD  50               push   ax
25AE  b86e00           mov    ax, 0x6e
25B1  50               push   ax
25B2  9a1415d302       lcall  0x2d3, 0x1514   ; Bar
25B7  b80300           mov    ax, 3
25BA  50               push   ax
25BB  9a0616d302       lcall  0x2d3, 0x1606   ; SetColor
25C0  b87800           mov    ax, 0x78
25C3  50               push   ax
25C4  b86400           mov    ax, 0x64
25C7  50               push   ax
25C8  b8c800           mov    ax, 0xc8
25CB  50               push   ax
25CC  b86e00           mov    ax, 0x6e
25CF  50               push   ax
25D0  9a2f0cd302       lcall  0x2d3, 0xc2f   ; Rectangle?
25D5  b87d00           mov    ax, 0x7d
25D8  50               push   ax
25D9  b86600           mov    ax, 0x66
25DC  50               push   ax
25DD  bf6f25           mov    di, 0x256f
25E0  0e               push   cs
25E1  57               push   di
25E2  9a5617d302       lcall  0x2d3, 0x1756   ; OutTextXY
25E7  9a0c037102       lcall  0x271, 0x30c   ; ReadKey
25EC  a28a02           mov    byte ptr [0x28a], al
25EF  803e8a021b       cmp    byte ptr [0x28a], 0x1b
25F4  7505             jne    0x25fb
25F6  c606a62b01       mov    byte ptr [0x2ba6], 1
25FB  89ec             mov    sp, bp
25FD  5d               pop    bp
25FE  c3               ret    
25FF  STR 'C:'

;===== proc_2602  (len 142) =====
2602  9a00000d06       lcall  0x60d, 0   ; Sys_init
2607  9a8211d302       lcall  0x2d3, 0x1182   ; Graph_init
260C  9a00007102       lcall  0x271, 0   ; Crt_init
2611  55               push   bp
2612  89e5             mov    bp, sp
2614  c70686020100     mov    word ptr [0x286], 1
261A  c70688020100     mov    word ptr [0x288], 1
2620  bf8602           mov    di, 0x286
2623  1e               push   ds
2624  57               push   di
2625  bf8802           mov    di, 0x288
2628  1e               push   ds
2629  57               push   di
262A  bfff25           mov    di, 0x25ff
262D  0e               push   cs
262E  57               push   di
262F  9a5007d302       lcall  0x2d3, 0x750   ; InitGraph
2634  9a280bd302       lcall  0x2d3, 0xb28   ; ClearDevice
2639  c706902b3075     mov    word ptr [0x2b90], 0x7530
263F  c706922b0000     mov    word ptr [0x2b92], 0
2645  e8c2d9           call   0xa
2648  c706ea2a1200     mov    word ptr [0x2aea], 0x12
264E  e88bdb           call   0x1dc
2651  e8ecfe           call   0x2540
2654  e8a9de           call   0x500
2657  ff36e42a         push   word ptr [0x2ae4]
265B  e87de1           call   0x7db
265E  e871e3           call   0x9d2
2661  e874fe           call   0x24d8
2664  833ee82a00       cmp    word ptr [0x2ae8], 0
2669  7503             jne    0x266e
266B  e80bff           call   0x2579
266E  803ea62b00       cmp    byte ptr [0x2ba6], 0
2673  74dc             je     0x2651
2675  9a5a0ad302       lcall  0x2d3, 0xa5a   ; CloseGraph
267A  89ec             mov    sp, bp
267C  5d               pop    bp
267D  31c0             xor    ax, ax
267F  9ad8000d06       lcall  0x60d, 0xd8   ; @Halt
2684  0000             add    byte ptr [bx + si], al
2686  0000             add    byte ptr [bx + si], al
2688  0000             add    byte ptr [bx + si], al
268A  0000             add    byte ptr [bx + si], al
268C  0000             add    byte ptr [bx + si], al
268E  0000             add    byte ptr [bx + si], al