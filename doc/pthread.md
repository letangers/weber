## pthread 多线程基础

### 1. 线程

#### a. 创建线程

```c
#include <pthread.h>

int pthread_create(pthread_t *tid, const pthread_attr_t *attr,
                      void *(*start_routine) (void *), void *arg);
/*
1. 功能
	创建新线程
2. 参数
    pthread_t *tid, 线程id，用来标识线程
    pthread_attr_t *attr, 线程属性(下文详述)
    一个接收 void * 参数的函数指针
    传递给 start_routine 的参数
3. 返回
	成功返回0，失败返回错误码
*/
```

#### b.线程属性

```c
#include <pthread.h>

pthread_attr_t attr;
// 初始化 pthread_attr_t 指针，成功返回0
int pthread_attr_init(pthread_attr_t *attr);
// 销毁 pthread_attr_t，成功返回0
int pthread_attr_destroy(pthread_attr_t *attr);

/*
线程属性包括：
  1. detachstate,是否joinable
  	可选值为PTHREAD_CREATE_DETACHED和PTHREAD_CREATE_JOINABLE
  2. scope,线程竞争cpu的范围
  	PTHREAD_SCOPE_SYSTEM(与系统中所有线程竞争) 
  	PTHREAD_SCOPE_PROCESS(与本进程的线程竞争)
  3. inheritsched,是否继承调度策略和调度参数
  	PTHREAD_EXPLICIT_SCHED(使用显示制定的调度策略和参数)
    PTHREAD_INHERIT_SCHED(继承调用者线程的值)
  4. schedpolicy,新线程的调度策略
  	SCHED_OTHER(正常、非实时)
  	SCHED_RR(实时、轮转法)
  	SCHED_FIFO(实时、先入先出)
  5. schedpriority,调度优先级(调度策略为实时时有效)
  6. guardsize,线程栈末尾的警戒缓冲区大小
  7. stackaddress,线程栈的最低地址
  8. stacksize,线程栈大小
  
当 pthread_create 传入的属性参数为 NULL时，各属性的默认值为
	Detach state        = PTHREAD_CREATE_JOINABLE
    Scope               = PTHREAD_SCOPE_SYSTEM
    Inherit scheduler   = PTHREAD_INHERIT_SCHED
    Scheduling policy   = SCHED_OTHER
    Scheduling priority = 0
    Guard size          = 4096 bytes
    Stack address       = 0x40196000
    Stack size          = 0x201000 bytes
*/

// 每个线程属性均对应 set 和 get 两个方法，具体可见man pthread_attr_init
int pthread_attr_setxxx(pthread_attr_t *attr,type value);
int pthread_attr_getxxx(pthread_attr_t *attr,type *value);

```



### 2. 互斥量

#### a. 基本操作

```c
#include <pthread.h>

pthread_mutex_t mutex;
// 初始化互斥锁
int pthread_mutex_init(pthread_mutex_t *mutex,const pthread_mutexattr_t* attr);
// 销毁互斥锁
int pthread_mutex_destroy(pthread_mutex_t *mutex);

// 加锁
// 成功返回 0，失败返回错误码。如果已经被锁，则阻塞
int pthread_mutex_lock(pthread_mutex_t *mutex);
// 不阻塞，若加锁成功返回0
int pthread_mutex_trylock(pthread_mutex_t *mutex);
// 超时或成功加锁返回，超时返回 ETIMEDOUT
int pthread_mutex_timedlock(pthread_mutex_t *mutex);

// 释放锁
int pthread_mutex_unlock(pthread_mutex_t *mutex);
```

#### b. 互斥量属性

```c
include <pthread.h>
pthread_mutexattr_t attr;
// 初始化
int pthread_mutexattr_init (pthread_mutexattr_t *__attr);
// 销毁
int pthread_mutexattr_destroy (pthread_mutexattr_t *__attr);
    
/* 
互斥量属性包括：
  1. shared, 进程共享属性
  	PTHREAD_PROCESS_PRIVATE，默认
  	PTHREAD_PROCESS_SHARED，表示互斥量可用于多进程的线程
  2. type, 类型属性
  	PTHREAD_MUTEX_NORMAL      标准互斥量类型，不做任何特殊的错误检查或死锁检测
  	PTHREAD_MUTEX_ERRORCHECK  提供错误检查
  	PTHREAD_MUTEX_RECURSIVE   允许同一线程在互斥量解锁前多次加锁（需要相同解锁次数才能释放）
  	PTHREAD_MUTEX_DEFAULT     提供默认特性和行为，操作系统可把此类型映射到其他的某一种互斥量
  3. protocol, 互斥锁协议属性
  	PTHREAD_PRIO_NONE
  	PTHREAD_PRIO_INHERIT
  	PTHREAD_PRIO_PROTECT
  4. prioceiling, 优先级上限属性
  5. robust, 健壮属性
  	PTHREAD_MUTEX_STALLED
  	PTHREAD_MUTEX_ROBUST
*/
```



### 3. 条件变量

#### a. 基本操作

```c
#include <pthread.h>

pthread_cond_t;

// 创建 条件变量
int pthread_cond_init(pthread_cond_t *cond,const pthread_condattr_t *attr);
// 销毁 条件变量
int pthread_cond_destroy(pthread_cond_t * cond);

// 等待条件变量被唤醒，mutex将会加锁
int pthread_cond_wait(pthread_cond_t *cond,pthread_mutex_t *mutex);
int pthread_cond_timedwait (pthread_cond_t *cond,
                            pthread_mutex_t *mutex,const struct timespec *abstime);

// 唤醒所有满足条件的线程
int pthread_cond_broadcast(pthread_cond_t *cond);
// 唤醒一个等待返回条件的线程
int pthread_cond_signal()
```

#### b. 条件变量属性

```c
#include <pthread.h>
pthread_conattr_t attr;
// 初始化
int pthread_condattr_init (pthread_condattr_t *__attr);
// 销毁
int pthread_condattr_destroy (pthread_condattr_t *__attr);

/* 
条件变量属性包括:
  1. shared, 进程共享属性，控制条件变量是否可用于多进程的线程
  2. clock, 时钟属性，控制 pthread_cond_timedwait 的超时参数采用哪个时钟
*/
```



### 4. 其他常用函数

```c
#include <pthread.h>

// 退出线程
int pthread_exit(void *rval_ptr);

// rval 保存 pthread_exit 退出时的退出码
int pthread_join(pthread_t tid,void *rval);

// 分离线程
int pthread_detach(pthread_t tid);

// 获取当前线程id
pthread_t pthread_self();

// 比较两个线程 id
int pthread_equal(pthread_t thread_1,pthread_t thread_2);
```



### 5. 上文未提到的pthread库提供的其他内容

```c
#include <pthread.h>
/*
  1. pthread_barrier_t   计数锁
  2. pthread_spinlock_t  自旋锁
  3. pthread_rwlock_t    读写锁
  4. pthread_once_t    线程的一次性初始化 
  5. pthread_kill() 发送信号
  6. pthread_sigmask() 信号屏蔽处理
  7. pthread_cleanup_push  pthread_cleanup_pop  线程清理函数
  8. 线程私有数据 pthread_key_t
    - pthread_key_create()
    - pthread_key_delete()
    - pthread_setspecific()
    - pthread_getspecific()
 9. pthread_cancel() pthread_setcancelstate() pthread_setcanceltype()
10. pthread_atfork()
*/
```

