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

### ***1***
汇编代码中 函数touch1 在地址0x4017c0
且getbuf有 rsp-0x28 即40bytes
于是在输入的第40字节后添加地址 4017c0 以替换栈中的ret值
答案如下
输入:  00 00 ... 00 c0 17 40
位数:   0  1 ... 40 41 42 43
