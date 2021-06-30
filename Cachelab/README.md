lab要求必须在X86-64的机器上运行  
lab有两个部分  
第一部分需要用一个C程序模拟高速缓存的操作  
第二部分需要优化一个矩阵转置函数 使其缓存不命中最小化  

### ***PART A***  
该部分需要先安装软件 valgrind(内存泄露检测工具)  
系统自带了valgrind 省去了该步骤  
valgrind 得出的形式如下  
\[ ]操作符 地址,大小  
```  
 I 0400d7d4,8
 M 0421c7f0,4
 L 04f6b868,8
 S 7ff0005c8,8
```
I 表示指令加载(instruction) 由于只对数据缓存性能做检测 因此编写的程序中可以忽略I  
L 表示数据加载(load)  
S 表示数据存储(store)  
M 表示数据修改(modify)  

该部分需要在 csim.c 里编写一个使用LRU策略的 cache 模拟器  
lab提供了一个程序csim-ref 需要写出一个和它功能一样的程序  
```
Example 
Usage: ./csim-ref \[-hv] -s <s> -E <E> -b <b> -t <tracefile>
-h: Optional help flag that prints usage info
-v: Optional verbose flag that displays trace info
-s <s>: Number of set index bits (S = 2^s is the number of sets)
-E <E>: Associativity (number of lines per set)
-b <b>: Number of block bits (B = 2^b is the block size)
-t <tracefile>: Name of the valgrind trace to replay
 ```
第一部分理解后相对简单 将需求拆分后一步步进行实现  
1.读取输入的参数argv 并进行判断和处理  
2.将参数存入设置好的结构中  
3.利用参数计算出高速缓存的结构 并进行初始化  
4.读取文件并保存  
5.处理文件 将其保存至一个结构组中  
6.模拟高速缓存的运行 该部分为难点 需要整合处理文件以及操作高速缓存    
7.输出运行结果  

高速缓存的实现需要了解其构造和如何组成   
代码中实现的高速缓存实现    
由结构 cacheblock 实现高速缓存的最小结构    
包含了 有效位 标记位 和用于lru替换的lru计数   
再利用该结构组成一个二维数组形式的高速缓存   

高速缓存的作用则是三步    
1.组选择：利用块大小(B) 计算出该地址应该被分配的组下标(index)     
2.行匹配：利用地址(memory) 计算出该地址标记位(tag)      
3.字查找：用得出所在的组(index)和行(tag) 判断是否存在于该高速缓存块中 或者该块是否为空   
若存在(标记位为1)   则判断是否命中 命中则成功返回 否则进行替换   
若不存在(标记位为0) 则可以直接插入该空块   

个人在代码实现过程中遇到的难点    
需要提前计划对高速缓存的结构方式是如何 代码中的高速缓存相当于三维数组了 不记录很容易混乱   
对LRU的实现 LRU为最近最久未使用的块淘汰 LRU需要在每次执行高速缓存操作完成后再进行计数增加   


### ***PART B***
转置矩阵
把A的横行写为AT的纵列
把A的纵列写为AT的横行

该部分需要再trans.c文件中 编写一个经过优化的转置矩阵函数 名字为transpose_submit
要求的最小miss数
32 × 32: 8 points if m < 300, 0 points if m > 600
64 × 64: 8 points if m < 1, 300, 0 points if m > 2, 000
61 × 67: 10 points if m < 2, 000, 0 points if m > 3, 000
函数可以只针对这三个大小的矩阵优化 不必应付所有大小情况
高速缓存的大小为(s = 5, E = 1, b = 5)
不能使用超过12个变量
不能使用递归
不能定义新的数组也不能开辟新的malloc空间
不能修改矩阵A 但可以修改矩阵B









