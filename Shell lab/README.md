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
tsh应该回收所有僵尸孩子，如果一个工作是因为收到了一个它没有捕获的（没有按照信号处理函数）而终止的，那么tsh应该输出这个工作的PID和这个信号的相关描述。  
  
### 函数 void eval(char* cmdline)  
分析用户刚刚输入的命令行 如果用户请求了内置命令（quit、jobs、bg 或 fg），则立即执行它。   
否则，派生子进程并在子进程的上下文中运行作业。 如果作业在前台运行，等待它终止然后返回。  
注意：每个子进程必须有一个唯一的进程组 ID，这样当我们在键盘上键入 ctrl-c (ctrl-z) 时，我们的后台子进程不会从内核收到 SIGINT (SIGTSTP)。 
  
