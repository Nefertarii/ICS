整体已经给出能运行的shell程序，只需完善如下几个功能  
eval              解析命令行的主要函数  
builtin_cmd       识别并分析是否为内置命令:quit,fg,bg 和 jobs    
do_bgfg           实现内置命令 bg 和 fg   
waitfg            等待前台程序完成  
sigchld_handler   处理SIGCHILD信号  
sigint_handler    处理SIGINT信号 (ctrl+c)  
sigstp_handler    处理SIGTSTP信号(ctrl+z)  

用户的输入分为第一个的name和后面的参数，之间以一个或多个空格隔开。如果name是一个tsh内置的命令，那么tsh应该马上处理这个命令然后等待下一个输入。  
否则tsh应该假设name是一个路径上的可执行文件，并在一个子进程中运行这个文件  
tsh不需要支持管道和重定向  
如果用户输入ctrl-c/ctrl-z，那么SIGINT/SIGTSTP信号应该被送给每一个在前台进程组中的进程，如果没有进程，那么这两个信号应该不起作用。  
如果一个命令以'&'结尾，那么tsh应该将它们放在后台运行，否则就放在前台运行 并等待它的结束  
每一个工作 都有一个正整数PID或者JID。JID通过'%'前缀标识符表示，例如'%5'表示JID为5的工作，而'5'代表PID为5的进程。  
tsh应该有如下内置命令：  
quit: 退出当前shell  
jobs: 列出所有后台运行的工作  
bg <job>: 这个命令将会向<job>代表的工作发送SIGCONT信号并放在后台运行，<job>可以是一个PID也可以是一个JID。  
fg <job>: 这个命令会向<job>代表的工作发送SIGCONT信号并放在前台运行，<job>可以是一个PID也可以是一个JID。  
tsh应该回收所有僵尸孩子，如果一个job是因为收到了一个它没有捕获的（没有按照信号处理函数）而终止的，那么tsh应该输出这个工作的PID和这个信号的相关描述。  
  
### void eval
分析用户刚刚输入的命令行 如果用户请求了内置命令（quit、jobs、bg 或 fg），则立即执行它。   
否则，派生子进程并在子进程的上下文中运行作业。 如果作业在前台运行，等待它终止然后返回。   
注意：每个子进程必须有一个唯一的进程组 ID，这样当在键盘上键入ctrl-c/ctrl-z时，后台子进程不会从内核收到SIGINT/SIGTSTP。   
书上525页有大致的框架，也写出了因为没有考虑竞争而导致的问题 修改一下即可使用。   
利用了linux提供的阻塞和解除阻塞信号函数，在每次使用全局变量前都使用函数sigprocmask()来阻塞信号直到操作完成。   
  
### builtin_cmd
该函数比较简单，判断输入的命令是不是内置命令。   
简单的对比后，如果是内置命令就转跳至相应的函数执行操作。  
是且完成操作后返回1，不是内置命令则返回0    

### do_bgfg
上一步的函数判断了是bg/fg,该函数只需要判断是bg,还是fg,同时检查格式并完成对应操作。   
fg/bg id和fg/bg %id的操作是不同的,不带百分号的为进程PID操作,带百分号的为作业JID操作。   
bg <job>: 要求向<job>代表的job发送SIGCONT信号并放在后台运行，<job>可以是一个PID也可以是一个JID。    
fg <job>: 要求向<job>代表的job发送SIGCONT信号并放在前台运行，<job>可以是一个PID也可以是一个JID。   
注意区分并执行相应操作即可。   
  
### waitfg
该函数也比较简单，不阻塞并等待前台程序运行完成再返回即可   
  
### Signal handlers
三个信号处理函数 errno搜索得知已改成线程局部变量不需要额外记录   
sigchld_handler   
该信号处理要考虑较多的问题，需要考虑子进程返回的原因   
需要函数waitpid并且用WNOHANG|WUNTRACED参数，WIFEXITED(status)，WIFSIGNALED(status)，等来捕获终止或者被暂停的子进程的退出状态。  
通过另外一个&status参数，判断返回的进程是由于什么原因终止或暂停的。  
WIFEXITED(status)   表示正常终止  
WIFSIGNALED(status) 表示信号终止  
其余情况为程序被暂停  
若程序被终止需要shell回收其进程 其余情况只需要修改其状态即可  
  
sigint_handler  
信号处理比较简单  
对pid判断如果存在该进程则对该进程组下所有进程发送sigint信号即可  
  
sigtstp_handler  
同sigint处理  
  
  
lab使用了大量的unix内置或系统函数/变量，很多之前没有了解也没有用过，看了要求和其他人的解答感觉后对逻辑理解能比较清楚
但真正做起来有点无从下手的感觉。
