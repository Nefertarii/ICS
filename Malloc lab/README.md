看了其他人的讲解才发现网站提供的Self-Study Handout没有Traces文件夹 只有两个很小的测试文件

在 mm.h 中声明了以下四个函数组成,并在 mm.c 中定义。  
int mm_init(void);  
void \*mm_malloc(size_t size);  
void mm_free(void \*ptr);  
void \*mm_realloc(void \*ptr, size_t size);  
动态内存分配器将由这四个函数构成,修改这四个函数以满足要求  
mm_init：在调用mm_malloc、mm_realloc 或 mm_free 之前，需要调用mm_init 来执行任何必要的初始化，例如分配初始堆区域。  
返回值如果在执行初始化时出现问题，则为-1，否则为0。  

mm_malloc：mm_malloc 返回一个指针，指向至少为 size 大小的字节的区域。整个分配的块应位于堆区域内，不能与其他片重叠   
需要像标准库的malloc一致，返回需要进行8字节对齐  

mm_free：释放ptr所指向的块 什么也不返回   
所指的块必须是mm_malloc 或是 mm_realloc所分配的块，才能释放  

mm_realloc：函数将ptr所指的块重新分配为 size 大小的块     
如果 ptr 为 NULL，调用等于 mm_malloc(size)；  
如果 size 等于 0，调用等于 mm_free(ptr)；  
如果 ptr 不为 NULL，ptr必须是mm_malloc 或是 mm_realloc所分配的块。

memlib.c 模拟了内存系统，可以调用下面的方法来得到响应的信息：  
void \*mem_sbrk(int incr)：让堆扩展 incr 个字节，并返回新分配的地址的头指针  
void \*mem_heap_lo(void)：返回指向堆的第一个字节的指针  
void \*mem_heap_hi(void)：返回指向堆的最后一个字节的指针  
size_t mem_heapsize(void)：返回当前的堆大小  
size_t mem_pagesize(void)：返回系统的 page size  
 
不能更改任何mm.c的接口  
不能调用任何内存管理相关的库来处理  
不能定义任何全局 array, tree, list，struct 但是可以使用诸如 int, float 和 pointer 等变量  
需要与malloc 包的语义一致 
要求返回的指针必须是8字节对齐的    

实现一个动态内存分配器 但其接口的功能与malloc要保持一致  
难点在于如何在吞吐量和空间利用率上进行取舍  
free操作会产生空闲的块，如何维护这些空闲的块  
最好的情况是使用分离的多个链表 如何实现是个问题  


个人认为该lab难点在于给出的内容太多容易让人混乱，和debug比较困难  
两天在摸清楚了书上的基础实现后 准备开始在该模板上转换方式实现更高效的动态内存分配器  
对于书上的模板 有许多地方并没有实现 且没有合并操作/清除无效块 导致堆内存会超出限制 

### 反复尝试后 对该lab毫无头绪 时间不够 暂时搁置该lab   
