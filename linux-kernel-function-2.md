# Linux kernel functions 2

longterm:	6.6.32
longterm:	6.1.92
longterm:	5.15.160
longterm:	5.10.218
longterm:	5.4.277
longterm:	4.19.315

[ ] bsearch
[ ] btree
[ ] circ_buf
[x] completion
[ ] container_of
[ ] futex
[ ] generic-radix-tree
[ ] hashtable
[ ] kobject
[ ] kref
[ ] kernel_read_file_from_path
[ ] math
[ ] mailbox
[ ] plist
[ ] rculist
[ ] rcupdate
[ ] rcuref
[ ] rcutree
[ ] resource
[ ] rhashtable
[ ] ring_buffer
[ ] seqlock
[ ] softirq
[x] timer
[x] waitqueue
[ ] watchqueue

asm specified

[x] atomic
[x] barrier
[ ] bitops
[x] cache
[ ] mmu 与 tlb


网上的教程文档

https://embetronicx.com/tutorials/linux/device-drivers/waitqueue-in-linux-device-driver-tutorial/

## Linux 任务状态

Linux 的任务状态的定义在 include/linux/sched.h，理解任务的状态有助于理解 Linux 是如何调度任务的。
并且，有些与任务调度相关的内核功能也需要理解了任务状态后才能灵活应用。

下面是 include/linux/sched.h 中所定义的任务状态：

```c
/* Used in tsk->state: */
#define TASK_RUNNING			0x0000
#define TASK_INTERRUPTIBLE		0x0001
#define TASK_UNINTERRUPTIBLE		0x0002
#define __TASK_STOPPED			0x0004
#define __TASK_TRACED			0x0008
/* Used in tsk->exit_state: */
#define EXIT_DEAD			0x0010
#define EXIT_ZOMBIE			0x0020
#define EXIT_TRACE			(EXIT_ZOMBIE | EXIT_DEAD)
/* Used in tsk->state again: */
#define TASK_PARKED			0x0040
#define TASK_DEAD			0x0080
#define TASK_WAKEKILL			0x0100
#define TASK_WAKING			0x0200
#define TASK_NOLOAD			0x0400
#define TASK_NEW			0x0800
#define TASK_STATE_MAX			0x1000

/* Convenience macros for the sake of set_current_state: */
#define TASK_KILLABLE			(TASK_WAKEKILL | TASK_UNINTERRUPTIBLE)
#define TASK_STOPPED			(TASK_WAKEKILL | __TASK_STOPPED)
#define TASK_TRACED			(TASK_WAKEKILL | __TASK_TRACED)

#define TASK_IDLE			(TASK_UNINTERRUPTIBLE | TASK_NOLOAD)

/* Convenience macros for the sake of wake_up(): */
#define TASK_NORMAL			(TASK_INTERRUPTIBLE | TASK_UNINTERRUPTIBLE)

/* get_task_state(): */
#define TASK_REPORT			(TASK_RUNNING | TASK_INTERRUPTIBLE | \
					 TASK_UNINTERRUPTIBLE | __TASK_STOPPED | \
					 __TASK_TRACED | EXIT_DEAD | EXIT_ZOMBIE | \
					 TASK_PARKED)
```

1. **TASK_RUNNING** 表明该 task “应该”处在运行队列（run queue）中。

   任务可能尚未出现在运行队列中，原因是将任务标记为 TASK_RUNNING 并将其放置在运行队列上的操作不是原子的。
   所以需要持有 runqueue_lock 这个读写自旋锁才能进行读取查看运行队列。
   如果这样操作，就能看到运行队列上的每个任务都处于 TASK_RUNNING 状态。
   但是，如果不这样操作那么不一定了。
   类似地，驱动程序可以将自己（或者更确切地说是它们运行的进程上下文）标记为 TASK_INTERRUPTIBLE（或 TASK_UNINTERRUPTIBLE）。
   在调用 schedule() 后，任务将被从运行队列中移出（除非存在挂起信号，在这种情况下，它会保留在运行队列中）。
   
2. **TASK_INTERRUPTIBLE** 表明任务正在休眠但是可以被信号或者到期的定时器事件唤醒。
3. **TASK_UNINTERRUPTIBLE** 与 TASK_INTERRUPTIBLE 类似，但是不能被唤醒，只有等到条件满足才可能切回运行状态。
4. **__TASK_TRACED** 表示任务被 trace，此时任务处于停止状态，被另外进程 trace 中。
5. **__TASK_STOPPED** 表示任务暂停中，由于收到控制信号引起。
6. **EXIT_ZOMBIE** 表示任务已经终止了，但是父任务没有收集到它的状态（wait()）。
7. **EXIT_DEAD** 表示任务彻底消亡。

## bsearch

## btree

## circ_buf

## completion

completion 的初始化与内核中其它数据结构一样，有两种方式。
一种是静态初始化，一种是动态初始化。

```c
/* 静态 */
static DECLARE_COMPLETION(c);

/* 动态 */
static struct completion dc;
init_completion(&dc);

reinit_completion(&dc);

/* 使用下列接口进行完成前的等待 */
void wait_for_completion(struct completion *);
void wait_for_completion_io(struct completion *);
int wait_for_completion_interruptible(struct completion *x);
int wait_for_completion_killable(struct completion *x);
unsigned long wait_for_completion_timeout(struct completion *x,
						   unsigned long timeout);
unsigned long wait_for_completion_io_timeout(struct completion *x,
						    unsigned long timeout);
long wait_for_completion_interruptible_timeout(
	struct completion *x, unsigned long timeout);
long wait_for_completion_killable_timeout(
	struct completion *x, unsigned long timeout);
bool try_wait_for_completion(struct completion *x);
bool completion_done(struct completion *x);

/* 设置已完成 */
/* 向一个线程发送完成信号 */
void complete(struct completion *);
/* 向所有线程发送完成信号 */
void complete_all(struct completion *);
```

注意，带有 _interruptible 的接口除了能被当前等待的完成量唤醒外，也可以被信号等事件唤醒。
其它接口则都是只有等到完成量被设置后才被唤醒。

## waitqueue

```c
static int waitq_flag = 0;
static DECLARE_WAIT_QUEUE_HEAD(waitq);
/* dynamic */
// static wait_queue_head_t waitq_d;

/* initialize if dynamic */
// init_waitqueue_head(&waitq_d);

/* wait for the condition */
wait_event_interruptible(waitq, waitq_flag != 0);

/* wake up the waiting task */
wake_up_interruptible(&waitq);
```

注意这里使用的接口或者宏都是高度封装过的。
我们只需要简单的定义一个 wait_queue_head_t 结构实例，而 wait_event* 的封装中给我们定义并向 head 中添加了 struct wait_queue_entry 实例 entry。
然而，如果这种封装不满足您的使用场景，您也可以自行设计 wait_event* 函数。
具体可参考 include/linux/wait.h。

## timer

```c
static struct timer_list timer;

timer_setup(&timer, timer_callback, 0);
mod_timer(&timer, jiffies + msecs_to_jiffies(100));

del_timer(&timer);
```

## 架构相关

这里所谓的架构相关是指这些功能与底层架构结合的较为紧密。
但是不一定这些功能接口会因架构而不统一，Linux 本身将这些功能都封装成了比较统一的接口。
如果有不统一会下文会做特别说明。

### atomic

原子操作，保证操作的原子性。

```c
static atomic_t a;

atomic_set(&a, 0);

atomic_inc(&a);

atomic_add(5, &a);

atomic_dec(&a);

atomic_read(&a);
```

### barrier

```c
mb();
rmb();
wmb();

smp_mb();
smp_rmb();
smp_wmb();
```

### cache

cache 相关的操作主要是确保数据一致性的刷新操作。
cache 刷新接口有：

```c
static inline void flush_cache_all(void);

static inline void flush_cache_mm(struct mm_struct *mm);
static inline void flush_cache_range(struct vm_area_struct *vma,
				     unsigned long start,
				     unsigned long end);
static inline void flush_cache_page(struct vm_area_struct *vma,
				    unsigned long vmaddr,
				    unsigned long pfn);

static inline void flush_dcache_page(struct page *page);
static inline void flush_dcache_mmap_lock(struct address_space *mapping);
static inline void flush_dcache_mmap_unlock(struct address_space *mapping);

static inline void flush_icache_range(unsigned long start, unsigned long end);
static inline void flush_icache_page(struct vm_area_struct *vma,
				     struct page *page);
static inline void flush_icache_user_page(struct vm_area_struct *vma,
					   struct page *page,
					   unsigned long addr, int len);

static inline void flush_cache_vmap(unsigned long start, unsigned long end);
static inline void flush_cache_vunmap(unsigned long start, unsigned long end);
```


