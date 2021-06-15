压缩包中只有可运行的文件和用于提示的bomb.c  
利用objdump -d 将炸弹的asm文件保存下来即可开始分析  
  
根据给出的.c文件 可以确定程序有6个阶段 每个阶段都需要正确的输入才能进入下一阶段  
也可以通过文件读取输入(应该是按行读取)
把文件传输到了Windows上 通过vscode方便文件的查看

在第一阶段之前有函数 initialize_bomb   
一开始认为该函数里面可能会有线索 但并没有相关内容 只是单纯的初始化
简单略过后 发现了触发爆炸的函数:explode_bomb 在此设置了断点并开始分析


***phase_1:***
从main 进入phase1:
发现phase1中一个函数名已经给出了很明显的提示 "strings_not_equal"  
并且在进入该函数之前 有语句mov $0x402400,%esi  
表明了将$0x402400写入%esi 且作为第二参数传入到了函数:strings_not_equal中
(第一参数在地址400e37:mov %rax,%rdi  查找发现在地址40133c有操作mov %rdi,%rbx 又经过对栈内容的确认 知道了%rdi保存了在进入phase1之前所输入的字符串)  
根据函数名推测可能是用于对比输入与该位置保存的数据是否一致 于是使用x/s 0x402400  
发现了字符串 Border relations with Canada have never been better.(获得后立马就在调试下输入并通过了第一阶段 然后又返回来继续查看汇编文件)
函数strings_not_equal中:
401338~40133f: %rbx保存了%rdi的数据(%rdi为输入的字符串) %rbp保存了%rsi的数据(%rsi为内存中所保存密码)
401342:        进入函数:string_length 对其分析后该函数通过cmpb不断对比传入的参数%rdi中保存的字符
               若不为0 则对返回值%eax和%rdi的对比位置+1 直到对比为0 返回返回值%eax  
               可以发现函数:string_length用于返回字符串的长度
40133c~40135a: 继续进行函数:strings_not_equal的分析发现调用了两次string_length 一次调用了%rsi 一次调用了%rdi
               可以确定函数到该位置是用于对比输入与保存的字符串长度是否一致 一致则继续流程 否则转跳至返回位置并返回1(也能确定函数:string_not_equals返回1则表示失败)
               继续进行 在地址40135c进行了操作:movzbl (%rbx),%eax 并进行对比 test %al,%al
               相同转跳至地址401388处并返回0 不同则继续进行流程
40135c~401361: (这一步一没一开始想明白 为什么对比一个就能确定返回0 后来猜想应该是编译器的原因- 
               -%rbx保存了作为输入传入的%rdi 而%rdi在经过上一步判断了长度第一位不可能为0 所以只是为了下面的操作而保存至%eax)
401363~401368: 继续进行 在地址401363进行cmp 0x0(%rbp),%al 比较rbp的第一个字符和%al(%al在上一步已经被保存了%rbx的第一个字符) 相同转跳地址401372 否则转跳地址40138f 并返回1
40136a~401386: 地址401372对上述两个字符串的位置分别+1后继续对比是否为0 不为0则转跳地址40136a重复进行比对 为0则继续流程从而返回0


***phase_2***
在main进入phase2之前用%rdi保存了输入
进入phase2 简单看一遍 分配了0x28大小的空间 且依然有函数转跳 其名字同样也给出了提示
进入read_six_numbers内部查看 又分配了0x18大小的空间 并用了大量的lea操作 先没管这些操作 
继续查看发现语句mov $0x4025c3 ,%esi 并在之后调用了sscanf函数 
调用之后对比返回值和常数5 大于5则表示sscanf成功并返回 否则explode
(期间查找了一下sscanf函数的操作 返回值为成功操作的数字数量 且查看地址0x4025c3的值为"%d %d %d %d %d %d")
了解后猜测在调用该函数之前的大量lea操作作为参数的寄存器可能是为了保存值 简单整理后的地址为如下情况
%rsp = (%rsi+0x14) 
%rsp =  %rsi+0x10
%r9  =  %rsi+0x0c
%r8  =  %rsi+0x08
%rcx =  %rsi+0x04
%rdx =  %rsi
函数read_six_number操作也就确定 储存输入的6个数字在一开始phase2分配的0x18大小(24字节)的空间中以%rsi作为开始的地址
回到phase2 对剩余函数进行整理得出如下结果(0x18大小地址空间假设为数组nums[] nums[]+1表示为数组的位置+1 寄存器则省略了百分号)
if (rsp)  = 1 goto 400f30
else explode_bomb
400f17:
  nums[]+1 = rbx
  nums[]+5 = rbp
  if(rbx) = eax goto 400f25
  else explode_bomb
400f25:
  eax = nums[]
  eax+=eax (eax*2)
  nums[]+1 = eax*2 
  if(rbx!=rbp) goto 400f3c
  else goto 400f17
400f30:
  nums[]+1=rbx
  if (rbx != nums[]+5) goto400f17
  else goto 400f3c
400f3c:
  %rsp+28
  ret
返回后先对比第一个数是否为 1 是则继续进行 否则将引爆炸弹
继续往下看 每次对比nums[]的下一个数与前一个数 (num[])*2需要等于num[]+1 直到数组的最后一个数字将返回并解除炸弹
于是可以得出答案为 1,2,4,8,16,32


***phase3***
phase3感觉比较简单 且中间没有转跳其他函数
一开始进行了操作0xc(%rsp),%rcx,    0x8(%rsp),%rdx
和phase2一样 调用了sscanf 且在调用前有地址$4025cf 使用gdb发现储存的是"%d %d" 得出一开始的%rdx,%rcx为参数传入sscanf
在调用sscanf后判断了返回值 小于2则直接引爆炸弹
随后又判断%rsp+8地址的值是否小于7 大于7则引爆炸弹 即第一个输入%rdx需要小于7 并赋值给%rax
紧跟着为间接转跳语句 jmpq *0x402470(,%rax,8)  即switch语句 利用gdb x/8a 0x402470获得转跳表
转跳表关系如下
%rax = 0 goto 0x400f7c                     0x400f7c: %eax = 0xcf = 207;  goto 400fbe;
%rax = 1 goto 0x400fb9                     0x400fb9: %eax = 0x137 = 311; goto 400fbe;
%rax = 2 goto 0x400f83                     0x400f83: %eax = 0x2c3 = 707; goto 400fbe;
%rax = 3 goto 0x400f8a                     0x400f8a: %eax = 0x100 = 256; goto 400fbe;
%rax = 4 goto 0x400f91                     0x400f91: %eax = 0x185 = 389; goto 400fbe;
%rax = 5 goto 0x400f98                     0x400f98: %eax = 0xce  = 206; goto 400fbe;
%rax = 6 goto 0x400f9f                     0x400f9f: %eax = 0x2aa = 682; goto 400fbe;
%rax = 7 goto 0x400fa6                     0x400fa6: %eax = 0x147 = 327; goto 400fbe;
switch成功后全部转跳至地址400fbe 该地址之后的操作将判断第二个输入与刚刚switch赋值后的寄存器%eax 相等则成功并返回 否则将引爆炸弹
因此phase3将有8个不同的答案为
0 207, 1 311, 2 707, 3 256, 4 389, 5 206, 6 682, 7 327


***phase4***
phase4的关键在于函数func4 整理了fun4后整个就清晰了起来
phase4在调用func4之前 同样使用了地址地址$4025cf 和sscanf函数
sscanf返回要求等于2 即需要输入两个参数 在储存了输入的两个参数后 随机初始化数据进入函数func4
函数func4的转跳比较多 难度稍大 但整理后也比较好分析(对转跳的要求不是非常熟练 出错了几次)
观察汇编发现函数分成了三个判断转跳块 经过整理后的c程序如下
int func4(int x, int y, int z)
{   //x in %edi, y in %esi, z in %edx
    int k = z - y;
    k = ((int)(((unsigned)k >> 31) + k) >> 1) + y;
    if (k < x)
        return 2 * func4_(x, k + 1, z) + 1;
    else if (k > x)
        return 2 * func4_(x, y, k - 1);
    else
        return 0;
}

phase4有4个不同答案为
0 0, 1 0, 3 0, 7 0

***phase5***
