0x08097110  push    ebp
0x08097111  mov     ebp, esp
0x08097113  push    edi
0x08097114  push    esi
0x08097115  push    ebx
0x08097116  sub     esp, 0x6c
0x08097119  mov     edx, dword ptr [ebp + 0x14]
0x0809711c  mov     esi, dword ptr [ebp + 0xc]
0x0809711f  mov     eax, dword ptr [edx + 4]
0x08097122  test    eax, eax
0x08097124  jle     0x80971ed
0x0809712a  mov     ecx, dword ptr [edx + 0xc]
0x0809712d  test    esi, esi
0x0809712f  mov     dword ptr [ebp - 0x3c], ecx
0x08097132  je      0x80971ed
0x08097138  mov     edx, dword ptr [esi + 0x1184]
0x0809713e  test    edx, edx
0x08097140  js      0x80971ed
0x08097146  mov     ebx, dword ptr [0x8fc81e0]
0x0809714c  imul    edx, edx, 0xcc4
0x08097152  imul    ecx, dword ptr [esi + 0x1180], 0x63fc8
0x0809715c  add     edx, dword ptr [ecx + ebx + 0x14]
0x08097160  je      0x80971ed
0x08097166  mov     ecx, eax
0x08097168  and     eax, 0xff
0x0809716d  mov     dword ptr [ebp - 0x40], eax
0x08097170  sar     ecx, 8
0x08097173  mov     dword ptr [esp + 0xc], eax
0x08097177  mov     eax, dword ptr [ebp - 0x3c]
0x0809717a  mov     dword ptr [esp + 0x10], edx
0x0809717e  mov     dword ptr [esp], esi
0x08097181  mov     dword ptr [ebp - 0x4c], edx
0x08097184  mov     dword ptr [ebp - 0x44], ecx
0x08097187  mov     dword ptr [esp + 8], eax
0x0809718b  mov     dword ptr [esp + 4], ecx
0x0809718f  call    0x8079870
0x08097194  mov     edx, dword ptr [ebp - 0x4c]
0x08097197  mov     dword ptr [ebp - 0x48], esi
0x0809719a  mov     esi, edx
0x0809719c  test    eax, eax
0x0809719e  setne   al
0x080971a1  xor     ebx, ebx
0x080971a3  movzx   edi, al
0x080971a6  nop     
0x080971a8  mov     eax, dword ptr [esi + ebx*4 + 0x78]
0x080971ac  test    eax, eax
0x080971ae  je      0x80971de
0x080971b0  mov     edx, dword ptr [ebp - 0x40]
0x080971b3  mov     ecx, dword ptr [ebp - 0x3c]
0x080971b6  mov     dword ptr [esp + 0x10], eax
0x080971ba  mov     eax, dword ptr [ebp - 0x44]
0x080971bd  mov     dword ptr [esp + 0xc], edx
0x080971c1  mov     edx, dword ptr [ebp - 0x48]
0x080971c4  mov     dword ptr [esp + 8], ecx
0x080971c8  mov     dword ptr [esp + 4], eax
0x080971cc  mov     dword ptr [esp], edx
0x080971cf  call    0x8079870
0x080971d4  mov     ecx, 1
0x080971d9  test    eax, eax
0x080971db  cmovne  edi, ecx
0x080971de  add     ebx, 1
0x080971e1  cmp     ebx, 8
0x080971e4  jne     0x80971a8
0x080971e6  test    edi, edi
0x080971e8  mov     esi, dword ptr [ebp - 0x48]
0x080971eb  jne     0x80971f8
0x080971ed  add     esp, 0x6c
0x080971f0  pop     ebx
0x080971f1  pop     esi
0x080971f2  pop     edi
0x080971f3  pop     ebp
0x080971f4  ret     
0x080971f5  lea     esi, [esi]
0x080971f8  mov     edx, dword ptr [ebp + 0x14]
0x080971fb  mov     byte ptr [ebp - 0x2d], 0xdd
0x080971ff  mov     eax, dword ptr [edx]
0x08097201  mov     dword ptr [ebp - 0x28], eax
0x08097204  mov     eax, dword ptr [edx + 4]
0x08097207  mov     dword ptr [ebp - 0x24], eax
0x0809720a  mov     eax, dword ptr [edx + 8]
0x0809720d  mov     dword ptr [ebp - 0x20], eax
0x08097210  mov     eax, dword ptr [edx + 0xc]
0x08097213  mov     dword ptr [ebp - 0x1c], eax
0x08097216  mov     eax, dword ptr [esi]
0x08097218  mov     dword ptr [esp], esi
0x0809721b  mov     dword ptr [esp + 0x10], 0
0x08097223  mov     dword ptr [esp + 0xc], 0x64
0x0809722b  mov     dword ptr [ebp - 0x2c], eax
0x0809722e  lea     eax, [ebp - 0x2d]
0x08097231  mov     dword ptr [esp + 8], 0x15
0x08097239  mov     dword ptr [esp + 4], eax
0x0809723d  call    0x807a870
0x08097242  add     esp, 0x6c
0x08097245  pop     ebx
0x08097246  pop     esi
0x08097247  pop     edi
0x08097248  pop     ebp
0x08097249  ret     
0x0809724a  lea     esi, [esi]
0x08097250  push    ebp
0x08097251  mov     ebp, esp
0x08097253  sub     esp, 0x48
0x08097256  mov     dword ptr [ebp - 8], ebx
0x08097259  mov     ebx, dword ptr [ebp + 0x14]
0x0809725c  mov     dword ptr [ebp - 4], esi
0x0809725f  mov     esi, dword ptr [ebp + 0xc]
0x08097262  mov     eax, dword ptr [ebx + 4]
0x08097265  test    eax, eax
0x08097267  js      0x80972d0
0x08097269  mov     edx, dword ptr [ebx + 0xc]
0x0809726c  mov     dword ptr [esp + 4], eax
0x08097270  lea     eax, [esi + 0x248]
0x08097276  mov     dword ptr [esp], eax
0x08097279  mov     dword ptr [esp + 8], edx
0x0809727d  call    0x80e4740
0x08097282  mov     eax, dword ptr [ebx + 4]
0x08097285  test    eax, eax
0x08097287  jle     0x80972d0
0x08097289  mov     byte ptr [ebp - 0x1d], 0xdd
0x0809728d  mov     eax, dword ptr [ebx]
0x0809728f  mov     dword ptr [ebp - 0x18], eax
0x08097292  mov     eax, dword ptr [ebx + 4]
0x08097295  mov     dword ptr [ebp - 0x14], eax
0x08097298  mov     eax, dword ptr [ebx + 8]
0x0809729b  mov     dword ptr [ebp - 0x10], eax
0x0809729e  mov     eax, dword ptr [ebx + 0xc]
0x080972a1  mov     dword ptr [ebp - 0xc], eax
0x080972a4  mov     eax, dword ptr [esi]
0x080972a6  mov     dword ptr [esp + 0x10], 0
0x080972ae  mov     dword ptr [esp + 0xc], 0x64
0x080972b6  mov     dword ptr [esp + 8], 0x15
0x080972be  mov     dword ptr [ebp - 0x1c], eax
0x080972c1  lea     eax, [ebp - 0x1d]
0x080972c4  mov     dword ptr [esp + 4], eax
0x080972c8  mov     dword ptr [esp], esi
0x080972cb  call    0x807a870
0x080972d0  mov     ebx, dword ptr [ebp - 8]
0x080972d3  mov     esi, dword ptr [ebp - 4]
0x080972d6  mov     esp, ebp
0x080972d8  pop     ebp
0x080972d9  ret     
0x080972da  lea     esi, [esi]
0x080972e0  push    ebp
0x080972e1  mov     ebp, esp
0x080972e3  sub     esp, 0x28
0x080972e6  mov     edx, dword ptr [ebp + 0x14]
0x080972e9  mov     dword ptr [esp + 0x10], 0
0x080972f1  mov     dword ptr [esp + 0xc], 0
0x080972f9  mov     eax, dword ptr [edx + 0xc]
0x080972fc  mov     dword ptr [esp + 8], eax