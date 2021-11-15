

# 1 WAITPID

系列函数有

```c
wait(int* stat_loc);

waitpid(pid_t pid, int* stat_loc, int options);
```



**描述**

* wait() function suspends execution of its calling process unitl 两种情况
    * a terminated child process
    * a signal is received

当成功返回的时候, stac_loc 会包含 退出进程的信息



* pid 这个参数， 表明我们应该等待的是哪些孩子进程
    * if pid == -1, the call waits for any child process
    * if pid == 0, the call waits for any child process in the process group of the caller
    * if pid > 0, the call waits for the process with process id **pid**
    * if pid < - 1, the call waits for any process whose process group id equals the absolute value of pid



* 对于 stat_loc 这个参数来说， 有几点要注意



* options 这个参数 contains the bitwise OR of any of the folllowing options
    * **WNOHANG** 表明 the call should not block if there are no processes that wish to report status
    * **WUNTRACED**, 表明 the children of the current process that are stopped due to a SIGTTIN, SIGTTOU, SIGTSTP, STGSTOP signal should reprot their statuss





函数返回值

* If wait() returns due to a stopped or terminated child process, the process ID of the child is returned to the calling process





**如何得知child process 的具体情况呢**

采用 下面的Macro Function

```c
WIFEXITED(status); True if the process terminated normally by a call

WIFSIGNALED(status); True if the process terminated due to recepit of a signal
  
WIFSTOOED(status);
	True if thr process has not terminated, but has stopped and can be restarted
```





# 2 SIGPROCMASK



Name -- Manipulate current signal mask

```c
int sigprocmask(int how, const sigset_t* restrict set, sigset_t* restrict oset);
```



**函数功能描述**

* the sigprocmask function examines and / or changes the current signal mask (those signasls that are blocked from delivery)
* Signals are blocked if ther are members of the current signal mask set



函数的功能取决于 how

* SIG_BLOCK: the new mask is the union of the current mask and the specified set
* SIG_UNBLOCK: the new mask is the intersection of the current mask and the complement of the specified set
* SIG_SETMASK: the current mask is replaced by the specifed set





当然信号 system quitely disallows **SIGKILL** or **SIGSTOP** to be blocked







书533 的地方提到 

* sigprocmask 函数改变当前阻塞信号的集合
* 具体的行为依赖于 how
    * SIG_BLOCK: 把 set 中的信号 加入到 blocked 中  blocked = blocked | set
    * SIG_UNBLOCK: 把 blocked 中删除 set 的信号 blocked = blocked & (~set)
    * SIG_SETMASK: block = set

```c
/*
代码示例
Page 533
*/

sigset_t mask, prev_mask;

Sigemptyset(&mask);
Sigaddset(&mask, SIGINT);

//这个可以block SIGINT 信号
Sigprocmask(SIG_BLOCK, &mask, &prev_mask);
...
  ..
  ... 里面的代码不会被block掉
Sigprocmask(SIG_SETMASK, &prev_mask, NULL);
```













# Reference



*  `man waitpid`