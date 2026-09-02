0x08079870  push    ebp
0x08079871  mov     ebp, esp
0x08079873  push    edi
0x08079874  push    esi
0x08079875  push    ebx
0x08079876  sub     esp, 0x4c
0x08079879  mov     eax, dword ptr [ebp + 0x18]
0x0807987c  mov     edi, dword ptr [ebp + 8]
0x0807987f  mov     dword ptr [ebp - 0x34], 0
0x08079886  mov     ebx, dword ptr [eax + 0x20]
0x08079889  test    ebx, ebx
0x0807988b  jne     0x80798a2
0x0807988d  jmp     0x8079a68
0x08079892  lea     esi, [esi]
0x08079898  test    esi, esi
0x0807989a  mov     ebx, esi
0x0807989c  je      0x8079a68
0x080798a2  imul    eax, dword ptr [ebx + 0x10], 0x188
0x080798a9  add     eax, dword ptr [0x836ea50]
0x080798af  mov     edx, dword ptr [ebp + 0xc]
0x080798b2  mov     esi, dword ptr [ebx + 4]
0x080798b5  cmp     dword ptr [eax + 0xf8], edx
0x080798bb  jne     0x8079898
0x080798bd  imul    eax, dword ptr [eax + 0xdc], 0x1a4c
0x080798c7  add     eax, dword ptr [0x836eae0]
0x080798cd  je      0x80798f8
0x080798cf  mov     eax, dword ptr [eax + 4]
0x080798d2  mov     dword ptr [esp + 8], eax
0x080798d6  mov     eax, dword ptr [edi + 4]
0x080798d9  mov     dword ptr [esp], 0x8bacac0
0x080798e0  mov     dword ptr [esp + 4], eax
0x080798e4  call    0x809ee50
0x080798e9  mov     edx, dword ptr [ebp + 0x14]
0x080798ec  test    edx, edx
0x080798ee  jne     0x8079a78
0x080798f4  test    al, 6
0x080798f6  je      0x8079898
0x080798f8  lea     edx, [ebp - 0x1c]
0x080798fb  lea     eax, [ebp - 0x20]
0x080798fe  mov     dword ptr [esp + 0x18], edx
0x08079902  mov     dword ptr [esp + 0x1c], eax
0x08079906  mov     eax, dword ptr [edi + 0x149c]
0x0807990c  mov     dword ptr [ebp - 0x1c], 0
0x08079913  mov     dword ptr [ebp - 0x20], 0
0x0807991a  mov     dword ptr [ebp - 0x24], 0
0x08079921  mov     dword ptr [esp + 0x14], eax
0x08079925  mov     eax, dword ptr [edi + 0x1498]
0x0807992b  mov     dword ptr [ebp - 0x28], 0
0x08079932  mov     dword ptr [esp + 0x10], eax
0x08079936  mov     eax, dword ptr [edi + 0x1490]
0x0807993c  mov     dword ptr [esp + 0xc], eax
0x08079940  mov     eax, dword ptr [edi + 0x148c]
0x08079946  mov     dword ptr [esp + 8], eax
0x0807994a  mov     eax, dword ptr [edi + 0x1184]
0x08079950  mov     dword ptr [esp + 4], eax
0x08079954  imul    eax, dword ptr [edi + 0x1180], 0x63fc8
0x0807995e  add     eax, dword ptr [0x8fc81e0]
0x08079964  mov     dword ptr [esp], eax
0x08079967  call    0x80ef710
0x0807996c  lea     edx, [ebp - 0x28]
0x0807996f  imul    eax, dword ptr [ebx + 0x10], 0x188
0x08079976  add     eax, dword ptr [0x836ea50]
0x0807997c  mov     dword ptr [esp + 0x1c], edx
0x08079980  lea     edx, [ebp - 0x24]
0x08079983  mov     dword ptr [esp + 0x18], edx
0x08079987  mov     edx, dword ptr [eax + 0x7c]
0x0807998a  mov     dword ptr [esp + 0x14], edx
0x0807998e  mov     edx, dword ptr [eax + 0x78]
0x08079991  mov     dword ptr [esp + 0x10], edx
0x08079995  mov     edx, dword ptr [eax + 0x70]
0x08079998  mov     dword ptr [esp + 0xc], edx
0x0807999c  mov     edx, dword ptr [eax + 0x6c]
0x0807999f  mov     dword ptr [esp + 8], edx
0x080799a3  mov     eax, dword ptr [eax + 0x100]
0x080799a9  mov     dword ptr [esp + 4], eax
0x080799ad  imul    eax, dword ptr [edi + 0x1180], 0x63fc8
0x080799b7  add     eax, dword ptr [0x8fc81e0]
0x080799bd  mov     dword ptr [esp], eax
0x080799c0  call    0x80ef710
0x080799c5  mov     eax, dword ptr [ebp - 0x1c]
0x080799c8  mov     edx, dword ptr [ebp - 0x20]
0x080799cb  sub     eax, dword ptr [ebp - 0x24]
0x080799ce  sub     edx, dword ptr [ebp - 0x28]
0x080799d1  imul    eax, eax
0x080799d4  imul    edx, edx
0x080799d7  lea     eax, [edx + eax]
0x080799da  mov     dword ptr [ebp - 0x30], eax
0x080799dd  fild    dword ptr [ebp - 0x30]
0x080799e0  fld     st(0)
0x080799e2  fsqrt   
0x080799e4  fucomi  st(0)
0x080799e6  jp      0x80799ee
0x080799e8  je      0x8079a00
0x080799ea  fstp    st(0)
0x080799ec  jmp     0x80799f0
0x080799ee  fstp    st(0)
0x080799f0  fstp    qword ptr [esp]
0x080799f3  call    0x804b2ac
0x080799f8  jmp     0x8079a02
0x080799fa  lea     esi, [esi]
0x08079a00  fstp    st(1)
0x08079a02  fnstcw  word ptr [ebp - 0x2a]
0x08079a05  movzx   eax, word ptr [ebp - 0x2a]
0x08079a09  mov     ah, 0xc
0x08079a0b  mov     word ptr [ebp - 0x2c], ax
0x08079a0f  fldcw   word ptr [ebp - 0x2c]
0x08079a12  fistp   dword ptr [ebp - 0x30]
0x08079a15  fldcw   word ptr [ebp - 0x2a]
0x08079a18  mov     eax, dword ptr [ebp - 0x30]
0x08079a1b  cmp     dword ptr [ebp + 0x10], eax
0x08079a1e  jl      0x8079898
0x08079a24  test    eax, eax
0x08079a26  js      0x8079898
0x08079a2c  imul    eax, dword ptr [ebx + 0x10], 0x188
0x08079a33  add     eax, dword ptr [0x836ea50]
0x08079a39  mov     ebx, dword ptr [eax + 0xf0]
0x08079a3f  test    ebx, ebx
0x08079a41  je      0x8079898
0x08079a47  mov     dword ptr [esp + 4], 1
0x08079a4f  mov     ebx, esi
0x08079a51  mov     dword ptr [esp], eax
0x08079a54  call    0x8075210
0x08079a59  test    esi, esi
0x08079a5b  mov     dword ptr [ebp - 0x34], 1
0x08079a62  jne     0x80798a2
0x08079a68  mov     eax, dword ptr [ebp - 0x34]
0x08079a6b  add     esp, 0x4c
0x08079a6e  pop     ebx
0x08079a6f  pop     esi
0x08079a70  pop     edi