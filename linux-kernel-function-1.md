# Linux kernel functions 1

[x] bitmap
[x] kfifo
[x] klist
[x] kthread
[x] list
[x] llist
[x] mutex
[x] memory allocator
[x] of
[x] radix-tree
[x] rbtree
[x] rtmutex
[x] rwlock
[x] rwsem
[x] semaphore
[x] skbuff
[x] spinlock
[x] string
[x] tasklet (deprecated, Please consider using threaded IRQs instead)
[x] uio
[x] workqueue

## 内存分配接口

### slab allocator

使用 slab（slub）分配器的分配常用接口有：

- kmalloc

  ```c
  /**
   * kmalloc - allocate memory
   * @size: how many bytes of memory are required.
   * @flags: the type of memory to allocate.
   *
   * kmalloc is the normal method of allocating memory
   * for objects smaller than page size in the kernel.
   *
   * The allocated object address is aligned to at least ARCH_KMALLOC_MINALIGN
   * bytes. For @size of power of two bytes, the alignment is also guaranteed
   * to be at least to the size.
   *
   * The @flags argument may be one of the GFP flags defined at
   * include/linux/gfp.h and described at
   * :ref:`Documentation/core-api/mm-api.rst <mm-api-gfp-flags>`
   *
   * The recommended usage of the @flags is described at
   * :ref:`Documentation/core-api/memory-allocation.rst <memory_allocation>`
   *
   * Below is a brief outline of the most useful GFP flags
   *
   * %GFP_KERNEL
   *	Allocate normal kernel ram. May sleep.
   *
   * %GFP_NOWAIT
   *	Allocation will not sleep.
   *
   * %GFP_ATOMIC
   *	Allocation will not sleep.  May use emergency pools.
   *
   * %GFP_HIGHUSER
   *	Allocate memory from high memory on behalf of user.
   *
   * Also it is possible to set different flags by OR'ing
   * in one or more of the following additional @flags:
   *
   * %__GFP_HIGH
   *	This allocation has high priority and may use emergency pools.
   *
   * %__GFP_NOFAIL
   *	Indicate that this allocation is in no way allowed to fail
   *	(think twice before using).
   *
   * %__GFP_NORETRY
   *	If memory is not immediately available,
   *	then give up at once.
   *
   * %__GFP_NOWARN
   *	If allocation fails, don't issue any warnings.
   *
   * %__GFP_RETRY_MAYFAIL
   *	Try really hard to succeed the allocation but fail
   *	eventually.
   */
  static __always_inline void *kmalloc(size_t size, gfp_t flags);
  ```

- kzalloc

  ```c
  /**
   * kzalloc - allocate memory. The memory is set to zero.
   * @size: how many bytes of memory are required.
   * @flags: the type of memory to allocate (see kmalloc).
   */
  static inline void *kzalloc(size_t size, gfp_t flags);
  ```

- krealloc

  ```c
  /**
   * krealloc - reallocate memory. The contents will remain unchanged.
   * @p: object to reallocate memory for.
   * @new_size: how many bytes of memory are required.
   * @flags: the type of memory to allocate.
   *
   * The contents of the object pointed to are preserved up to the
   * lesser of the new and old sizes.  If @p is %NULL, krealloc()
   * behaves exactly like kmalloc().  If @new_size is 0 and @p is not a
   * %NULL pointer, the object pointed to is freed.
   *
   * Return: pointer to the allocated memory or %NULL in case of error
   */
  void *krealloc(const void *p, size_t new_size, gfp_t flags);
  ```

- kmalloc_array

  ```c
  /**
   * kmalloc_array - allocate memory for an array.
   * @n: number of elements.
   * @size: element size.
   * @flags: the type of memory to allocate (see kmalloc).
   */
  static inline void *kmalloc_array(size_t n, size_t size, gfp_t flags);
  ```

- kcalloc

  ```c
  /**
   * kcalloc - allocate memory for an array. The memory is set to zero.
   * @n: number of elements.
   * @size: element size.
   * @flags: the type of memory to allocate (see kmalloc).
   */
  static inline void *kcalloc(size_t n, size_t size, gfp_t flags);
  ```

- kfree

  ```c
  void kfree(const void *);
  ```

- kmem_cache_create

  ```c
  /*
   * Flags to pass to kmem_cache_create().
   * The ones marked DEBUG are only valid if CONFIG_DEBUG_SLAB is set.
   */
  /* DEBUG: Perform (expensive) checks on alloc/free */
  #define SLAB_CONSISTENCY_CHECKS	((slab_flags_t __force)0x00000100U)
  /* DEBUG: Red zone objs in a cache */
  #define SLAB_RED_ZONE		((slab_flags_t __force)0x00000400U)
  /* DEBUG: Poison objects */
  #define SLAB_POISON		((slab_flags_t __force)0x00000800U)
  /* Align objs on cache lines */
  #define SLAB_HWCACHE_ALIGN	((slab_flags_t __force)0x00002000U)
  /* Use GFP_DMA memory */
  #define SLAB_CACHE_DMA		((slab_flags_t __force)0x00004000U)
  /* Use GFP_DMA32 memory */
  #define SLAB_CACHE_DMA32	((slab_flags_t __force)0x00008000U)
  /* DEBUG: Store the last owner for bug hunting */
  #define SLAB_STORE_USER		((slab_flags_t __force)0x00010000U)
  /* Panic if kmem_cache_create() fails */
  #define SLAB_PANIC		((slab_flags_t __force)0x00040000U)
  /*
   * SLAB_TYPESAFE_BY_RCU - **WARNING** READ THIS!
   *
   * This delays freeing the SLAB page by a grace period, it does _NOT_
   * delay object freeing. This means that if you do kmem_cache_free()
   * that memory location is free to be reused at any time. Thus it may
   * be possible to see another object there in the same RCU grace period.
   *
   * This feature only ensures the memory location backing the object
   * stays valid, the trick to using this is relying on an independent
   * object validation pass. Something like:
   *
   *  rcu_read_lock()
   * again:
   *  obj = lockless_lookup(key);
   *  if (obj) {
   *    if (!try_get_ref(obj)) // might fail for free objects
   *      goto again;
   *
   *    if (obj->key != key) { // not the object we expected
   *      put_ref(obj);
   *      goto again;
   *    }
   *  }
   *  rcu_read_unlock();
   *
   * This is useful if we need to approach a kernel structure obliquely,
   * from its address obtained without the usual locking. We can lock
   * the structure to stabilize it and check it's still at the given address,
   * only if we can be sure that the memory has not been meanwhile reused
   * for some other kind of object (which our subsystem's lock might corrupt).
   *
   * rcu_read_lock before reading the address, then rcu_read_unlock after
   * taking the spinlock within the structure expected at that address.
   *
   * Note that SLAB_TYPESAFE_BY_RCU was originally named SLAB_DESTROY_BY_RCU.
   */
  /* Defer freeing slabs to RCU */
  #define SLAB_TYPESAFE_BY_RCU	((slab_flags_t __force)0x00080000U)
  /* Spread some memory over cpuset */
  #define SLAB_MEM_SPREAD		((slab_flags_t __force)0x00100000U)
  /* Trace allocations and frees */
  #define SLAB_TRACE		((slab_flags_t __force)0x00200000U)
  
  /* Flag to prevent checks on free */
  #ifdef CONFIG_DEBUG_OBJECTS
  # define SLAB_DEBUG_OBJECTS	((slab_flags_t __force)0x00400000U)
  #else
  # define SLAB_DEBUG_OBJECTS	0
  #endif
  
  /* Avoid kmemleak tracing */
  #define SLAB_NOLEAKTRACE	((slab_flags_t __force)0x00800000U)
  
  /* Fault injection mark */
  #ifdef CONFIG_FAILSLAB
  # define SLAB_FAILSLAB		((slab_flags_t __force)0x02000000U)
  #else
  # define SLAB_FAILSLAB		0
  #endif
  /* Account to memcg */
  #ifdef CONFIG_MEMCG_KMEM
  # define SLAB_ACCOUNT		((slab_flags_t __force)0x04000000U)
  #else
  # define SLAB_ACCOUNT		0
  #endif
  
  #ifdef CONFIG_KASAN
  #define SLAB_KASAN		((slab_flags_t __force)0x08000000U)
  #else
  #define SLAB_KASAN		0
  #endif
  
  /* The following flags affect the page allocator grouping pages by mobility */
  /* Objects are reclaimable */
  #define SLAB_RECLAIM_ACCOUNT	((slab_flags_t __force)0x00020000U)
  #define SLAB_TEMPORARY		SLAB_RECLAIM_ACCOUNT	/* Objects are short-lived */
  
  /* Slab deactivation flag */
  #define SLAB_DEACTIVATED	((slab_flags_t __force)0x10000000U)

  /**
   * kmem_cache_create - Create a cache.
   * @name: A string which is used in /proc/slabinfo to identify this cache.
   * @size: The size of objects to be created in this cache.
   * @align: The required alignment for the objects.
   * @flags: SLAB flags
   * @ctor: A constructor for the objects.
   *
   * Cannot be called within a interrupt, but can be interrupted.
   * The @ctor is run when new pages are allocated by the cache.
   *
   * The flags are
   *
   * %SLAB_POISON - Poison the slab with a known test pattern (a5a5a5a5)
   * to catch references to uninitialised memory.
   *
   * %SLAB_RED_ZONE - Insert `Red` zones around the allocated memory to check
   * for buffer overruns.
   *
   * %SLAB_HWCACHE_ALIGN - Align the objects in this cache to a hardware
   * cacheline.  This can be beneficial if you're counting cycles as closely
   * as davem.
   *
   * Return: a pointer to the cache on success, NULL on failure.
   */
  struct kmem_cache *
  kmem_cache_create(const char *name, unsigned int size, unsigned int align,
      slab_flags_t flags, void (*ctor)(void *));

  ```

- KMEM_CACHE

  ```c
  /*
   * Please use this macro to create slab caches. Simply specify the
   * name of the structure and maybe some flags that are listed above.
   *
   * The alignment of the struct determines object alignment. If you
   * f.e. add ____cacheline_aligned_in_smp to the struct declaration
   * then the objects will be properly aligned in SMP configurations.
   */
  #define KMEM_CACHE(__struct, __flags)					\
  		kmem_cache_create(#__struct, sizeof(struct __struct),	\
  			__alignof__(struct __struct), (__flags), NULL)
  ```

- kmem_cache_create_usercopy

  ```c
  /**
   * kmem_cache_create_usercopy - Create a cache with a region suitable
   * for copying to userspace
   * @name: A string which is used in /proc/slabinfo to identify this cache.
   * @size: The size of objects to be created in this cache.
   * @align: The required alignment for the objects.
   * @flags: SLAB flags
   * @useroffset: Usercopy region offset
   * @usersize: Usercopy region size
   * @ctor: A constructor for the objects.
   *
   * Cannot be called within a interrupt, but can be interrupted.
   * The @ctor is run when new pages are allocated by the cache.
   *
   * The flags are
   *
   * %SLAB_POISON - Poison the slab with a known test pattern (a5a5a5a5)
   * to catch references to uninitialised memory.
   *
   * %SLAB_RED_ZONE - Insert `Red` zones around the allocated memory to check
   * for buffer overruns.
   *
   * %SLAB_HWCACHE_ALIGN - Align the objects in this cache to a hardware
   * cacheline.  This can be beneficial if you're counting cycles as closely
   * as davem.
   *
   * Return: a pointer to the cache on success, NULL on failure.
   */
  struct kmem_cache *
  kmem_cache_create_usercopy(const char *name,
        unsigned int size, unsigned int align,
        slab_flags_t flags,
        unsigned int useroffset, unsigned int usersize,
        void (*ctor)(void *));
  ```

- KMEM_CACHE_USERCOPY

  ```c
  /*
   * To whitelist a single field for copying to/from usercopy, use this
   * macro instead for KMEM_CACHE() above.
   */
  #define KMEM_CACHE_USERCOPY(__struct, __flags, __field)			\
  		kmem_cache_create_usercopy(#__struct,			\
  			sizeof(struct __struct),			\
  			__alignof__(struct __struct), (__flags),	\
  			offsetof(struct __struct, __field),		\
  			sizeof_field(struct __struct, __field), NULL)
  
  ```

- kmem_cache_shrink

  ```c
  /**
   * kmem_cache_shrink - Shrink a cache.
   * @cachep: The cache to shrink.
   *
   * Releases as many slabs as possible for a cache.
   * To help debugging, a zero exit status indicates all slabs were released.
   *
   * Return: %0 if all slabs were released, non-zero otherwise
   */
  int kmem_cache_shrink(struct kmem_cache *cachep);
  ```

- kmem_cache_destroy

  ```c
  void kmem_cache_destroy(struct kmem_cache *s);
  ```

### mempool

一种应用于紧急情况下的内存缓存池，提前申请一个缓存池可以确保在一些不能出现申请内存出错的情况下使用。

一般情况下，使用 mempool_create 创建缓存池。

```c
/**
 * mempool_create - create a memory pool
 * @min_nr:    the minimum number of elements guaranteed to be
 *             allocated for this pool.
 * @alloc_fn:  user-defined element-allocation function.
 * @free_fn:   user-defined element-freeing function.
 * @pool_data: optional private data available to the user-defined functions.
 *
 * this function creates and allocates a guaranteed size, preallocated
 * memory pool. The pool can be used from the mempool_alloc() and mempool_free()
 * functions. This function might sleep. Both the alloc_fn() and the free_fn()
 * functions might sleep - as long as the mempool_alloc() function is not called
 * from IRQ contexts.
 *
 * Return: pointer to the created memory pool object or %NULL on error.
 */
mempool_t *mempool_create(int min_nr, mempool_alloc_t *alloc_fn,
				mempool_free_t *free_fn, void *pool_data);
```

其中，

- min_nr 指定该缓存池可以确保的最小的元素数量。
- alloc_fn 为用户自定义的元素分配函数。
- free_fn 为用于自定义的元素释放函数。
- pool_data 为可选入参，用于上述用户定义的函数。

大多数情况下，我们只想要系统的 slab 分配器来帮我们分配和释放元素，那么此时，你可以使用以下代码：

```c
// 注意这是个伪代码例子，不能实用。
mempool_t *pool;
struct kmem_cache *cache;

#define MY_POOL_MINIMUM 10

srtuct memcache {
  int a;
  int b;
  void *data;
};
cache = KMEM_CACHE(struct memcache, SLAB_POISON);
pool = mempool_create(MY_POOL_MINIMUM,
 mempool_alloc_slab, mempool_free_slab,
 cache);
```

一旦上面的缓冲池建立后，可以使用以下接口分配内存：

```c
void *mempool_alloc(mempool_t *pool, gfp_t gfp_mask) __malloc;
void mempool_free(void *element, mempool_t *pool);
```

另外可以使用 `int mempool_resize(mempool_t *pool, int new_min_nr);` 重置缓冲池的大小。

不再需要使用内存池后，通过 `void mempool_destroy(mempool_t *pool);` 释放。

### 申请以页为单位的大块内存

有些情况需要使用大量内存，那么可以使用以页为单位的分配接口。

```c
/*
 * Common helper functions. Never use with __GFP_HIGHMEM because the returned
 * address cannot represent highmem pages. Use alloc_pages and then kmap if
 * you need to access high mem.
 */
unsigned long __get_free_pages(gfp_t gfp_mask, unsigned int order);

#define __get_free_page(gfp_mask) \
		__get_free_pages((gfp_mask), 0)

unsigned long get_zeroed_page(gfp_t gfp_mask);

void free_pages(unsigned long addr, unsigned int order);

#define free_page(addr) free_pages((addr), 0)
```

常用的 GFP 标志，kmalloc 和 __get_free_pages 类接口共用。
并且注意这些接口会出现内存申请失败的情况，特别是使用 GFP_ATOMIC 的情况下，所以必须要处理失败情况。

```c
/**
 * DOC: Useful GFP flag combinations
 *
 * Useful GFP flag combinations
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Useful GFP flag combinations that are commonly used. It is recommended
 * that subsystems start with one of these combinations and then set/clear
 * %__GFP_FOO flags as necessary.
 *
 * %GFP_ATOMIC users can not sleep and need the allocation to succeed. A lower
 * watermark is applied to allow access to "atomic reserves".
 * The current implementation doesn't support NMI and few other strict
 * non-preemptive contexts (e.g. raw_spin_lock). The same applies to %GFP_NOWAIT.
 *
 * %GFP_KERNEL is typical for kernel-internal allocations. The caller requires
 * %ZONE_NORMAL or a lower zone for direct access but can direct reclaim.
 *
 * %GFP_KERNEL_ACCOUNT is the same as GFP_KERNEL, except the allocation is
 * accounted to kmemcg.
 *
 * %GFP_NOWAIT is for kernel allocations that should not stall for direct
 * reclaim, start physical IO or use any filesystem callback.
 *
 * %GFP_NOIO will use direct reclaim to discard clean pages or slab pages
 * that do not require the starting of any physical IO.
 * Please try to avoid using this flag directly and instead use
 * memalloc_noio_{save,restore} to mark the whole scope which cannot
 * perform any IO with a short explanation why. All allocation requests
 * will inherit GFP_NOIO implicitly.
 *
 * %GFP_NOFS will use direct reclaim but will not use any filesystem interfaces.
 * Please try to avoid using this flag directly and instead use
 * memalloc_nofs_{save,restore} to mark the whole scope which cannot/shouldn't
 * recurse into the FS layer with a short explanation why. All allocation
 * requests will inherit GFP_NOFS implicitly.
 *
 * %GFP_USER is for userspace allocations that also need to be directly
 * accessibly by the kernel or hardware. It is typically used by hardware
 * for buffers that are mapped to userspace (e.g. graphics) that hardware
 * still must DMA to. cpuset limits are enforced for these allocations.
 *
 * %GFP_DMA exists for historical reasons and should be avoided where possible.
 * The flags indicates that the caller requires that the lowest zone be
 * used (%ZONE_DMA or 16M on x86-64). Ideally, this would be removed but
 * it would require careful auditing as some users really require it and
 * others use the flag to avoid lowmem reserves in %ZONE_DMA and treat the
 * lowest zone as a type of emergency reserve.
 *
 * %GFP_DMA32 is similar to %GFP_DMA except that the caller requires a 32-bit
 * address.
 *
 * %GFP_HIGHUSER is for userspace allocations that may be mapped to userspace,
 * do not need to be directly accessible by the kernel but that cannot
 * move once in use. An example may be a hardware allocation that maps
 * data directly into userspace but has no addressing limitations.
 *
 * %GFP_HIGHUSER_MOVABLE is for userspace allocations that the kernel does not
 * need direct access to but can use kmap() when access is required. They
 * are expected to be movable via page reclaim or page migration. Typically,
 * pages on the LRU would also be allocated with %GFP_HIGHUSER_MOVABLE.
 *
 * %GFP_TRANSHUGE and %GFP_TRANSHUGE_LIGHT are used for THP allocations. They
 * are compound allocations that will generally fail quickly if memory is not
 * available and will not wake kswapd/kcompactd on failure. The _LIGHT
 * version does not attempt reclaim/compaction at all and is by default used
 * in page fault path, while the non-light is used by khugepaged.
 */
#define GFP_ATOMIC	(__GFP_HIGH|__GFP_ATOMIC|__GFP_KSWAPD_RECLAIM)
#define GFP_KERNEL	(__GFP_RECLAIM | __GFP_IO | __GFP_FS)
#define GFP_KERNEL_ACCOUNT (GFP_KERNEL | __GFP_ACCOUNT)
#define GFP_NOWAIT	(__GFP_KSWAPD_RECLAIM)
#define GFP_NOIO	(__GFP_RECLAIM)
#define GFP_NOFS	(__GFP_RECLAIM | __GFP_IO)
#define GFP_USER	(__GFP_RECLAIM | __GFP_IO | __GFP_FS | __GFP_HARDWALL)
#define GFP_DMA		__GFP_DMA
#define GFP_DMA32	__GFP_DMA32
#define GFP_HIGHUSER	(GFP_USER | __GFP_HIGHMEM)
#define GFP_HIGHUSER_MOVABLE	(GFP_HIGHUSER | __GFP_MOVABLE)
#define GFP_TRANSHUGE_LIGHT	((GFP_HIGHUSER_MOVABLE | __GFP_COMP | \
			 __GFP_NOMEMALLOC | __GFP_NOWARN) & ~__GFP_RECLAIM)
#define GFP_TRANSHUGE	(GFP_TRANSHUGE_LIGHT | __GFP_DIRECT_RECLAIM)
```

### alloc_pages

alloc_pages 类的接口适用于需要使用 struct page 结构内存的场合，特别是在需要使用高地址内存时。

```c
/*
 * Allocate pages, preferring the node given as nid. When nid == NUMA_NO_NODE,
 * prefer the current CPU's closest node. Otherwise node must be valid and
 * online.
 */
static inline struct page *alloc_pages_node(int nid, gfp_t gfp_mask,
						unsigned int order);


static inline struct page *
alloc_pages(gfp_t gfp_mask, unsigned int order);

#define alloc_page(gfp_mask) alloc_pages(gfp_mask, 0)

void free_pages(unsigned long addr, unsigned int order);
#define free_page(addr) free_pages((addr), 0)
```

### vmalloc 以及 ioremap

```c
#include <linux/vmalloc.h>

void *vmalloc(unsigned long size);
void vfree(void * addr);
```

注意 vmalloc 申请的内存与 kmalloc 的区别在于物理地址和虚拟地址的连续性。
kmalloc 申请的内存，其物理地址和虚拟地址都是连续的，物理地址和虚拟地址是一对一映射的；
然而，vmalloc 申请的内存，只保证虚拟地址是连续的，它可能修改页表将多个物理地址映射为一个连续的虚拟地址。
使用 vmalloc 比较困难，且不被内核社区认可。
由于 vmalloc 的物理地址不连续，所以它申请的内存不能用于 DMA 传输。

```c
void *ioremap(unsigned long offset, unsigned long size);
void iounmap(void * addr);
```

注意，ioremap 不会申请物理内存，它的作用是通过页表将 MMIO 映射为虚拟地址，以供驱动代码操作 MMIO 型的硬件寄存器。
使用 ioremap 映射后的虚拟地址可以直接通过 writel、readl 等接口操作硬件寄存器。
需要非常注意的是，__pa 是无法从 ioremap 后的虚拟地址转换称物理地址的。
这是因为 ioremap 的虚拟地址区间是一块预留的空间，每次 ioremap 映射都是从这块空间中取出相应大小的虚拟地址块来使用。
并不遵循一定的偏移，所以没法使用 __pa 这种通过减去偏移来获取物理地址的方式。
可行的方式为从 resource 结构中获取（resource->start）。

## bitmap

使用例子参见 bmtest.c。

## list

使用例子参见 list-test.c。

## kthread

```c
/**
 * kthread_create - create a kthread on the current node
 * @threadfn: the function to run in the thread, which prototype is 
 * int (*threadfn)(void *data).
 * @data: data pointer for @threadfn()
 * @namefmt: printf-style format string for the thread name
 * @arg...: arguments for @namefmt.
 *
 * This macro will create a kthread on the current node, leaving it in
 * the stopped state.  This is just a helper for kthread_create_on_node();
 * see the documentation there for more details.
 */

#define kthread_create(threadfn, data, namefmt, arg...) \
	kthread_create_on_node(threadfn, data, NUMA_NO_NODE, namefmt, ##arg)
```

```c
/**
 * kthread_run - create and wake a thread.
 * @threadfn: the function to run until signal_pending(current).
 * @data: data ptr for @threadfn.
 * @namefmt: printf-style name for the thread.
 *
 * Description: Convenient wrapper for kthread_create() followed by
 * wake_up_process().  Returns the kthread or ERR_PTR(-ENOMEM).
 */
#define kthread_run(threadfn, data, namefmt, ...)			   \
({									   \
	struct task_struct *__k						   \
		= kthread_create(threadfn, data, namefmt, ## __VA_ARGS__); \
	if (!IS_ERR(__k))						   \
		wake_up_process(__k);					   \
	__k;								   \
})
```

## mutex

```c
struct mutex lock;
mutex_init(&lock);

// static DEFINE_MUTEX(read_lock);

mutex_lock(&lock);

mutex_unlock(&lock);

mutex_destory(&lock);
```

## spinlock

```c
/* spinlock data type */
spinlock_t spinlock;

spin_lock_init(&spinlock);

spin_lock(&spinlock);

spin_unlock(&spinlock);

```

## semaphore

```c
struct semaphore sem;

sema_init(&sem, 1);
up(&sem);
down(&sem);
```
## string

下面列举了一些常用的字符串处理接口。

```c
/**
 * strcpy - Copy a %NUL terminated string
 * @dest: Where to copy the string to
 * @src: Where to copy the string from
 */
char *strcpy(char *dest, const char *src);

/**
 * strncpy - Copy a length-limited, C-string
 * @dest: Where to copy the string to
 * @src: Where to copy the string from
 * @count: The maximum number of bytes to copy
 *
 * The result is not %NUL-terminated if the source exceeds
 * @count bytes.
 *
 * In the case where the length of @src is less than  that  of
 * count, the remainder of @dest will be padded with %NUL.
 *
 */
char *strncpy(char *dest, const char *src, size_t count)

/**
 * strlcpy - Copy a C-string into a sized buffer
 * @dest: Where to copy the string to
 * @src: Where to copy the string from
 * @size: size of destination buffer
 *
 * Compatible with ``*BSD``: the result is always a valid
 * NUL-terminated string that fits in the buffer (unless,
 * of course, the buffer size is zero). It does not pad
 * out the result like strncpy() does.
 */
size_t strlcpy(char *dest, const char *src, size_t size);

/**
 * strscpy - Copy a C-string into a sized buffer
 * @dest: Where to copy the string to
 * @src: Where to copy the string from
 * @count: Size of destination buffer
 *
 * Copy the string, or as much of it as fits, into the dest buffer.  The
 * behavior is undefined if the string buffers overlap.  The destination
 * buffer is always NUL terminated, unless it's zero-sized.
 *
 * Preferred to strlcpy() since the API doesn't require reading memory
 * from the src string beyond the specified "count" bytes, and since
 * the return value is easier to error-check than strlcpy()'s.
 * In addition, the implementation is robust to the string changing out
 * from underneath it, unlike the current strlcpy() implementation.
 *
 * Preferred to strncpy() since it always returns a valid string, and
 * doesn't unnecessarily force the tail of the destination buffer to be
 * zeroed.  If zeroing is desired please use strscpy_pad().
 *
 * Returns:
 * * The number of characters copied (not including the trailing %NUL)
 * * -E2BIG if count is 0 or @src was truncated.
 */
ssize_t strscpy(char *dest, const char *src, size_t count);

/**
 * strscpy_pad() - Copy a C-string into a sized buffer
 * @dest: Where to copy the string to
 * @src: Where to copy the string from
 * @count: Size of destination buffer
 *
 * Copy the string, or as much of it as fits, into the dest buffer.  The
 * behavior is undefined if the string buffers overlap.  The destination
 * buffer is always %NUL terminated, unless it's zero-sized.
 *
 * If the source string is shorter than the destination buffer, zeros
 * the tail of the destination buffer.
 *
 * For full explanation of why you may want to consider using the
 * 'strscpy' functions please see the function docstring for strscpy().
 *
 * Returns:
 * * The number of characters copied (not including the trailing %NUL)
 * * -E2BIG if count is 0 or @src was truncated.
 */
ssize_t strscpy_pad(char *dest, const char *src, size_t count);

/**
 * strcat - Append one %NUL-terminated string to another
 * @dest: The string to be appended to
 * @src: The string to append to it
 */
char *strcat(char *dest, const char *src);

/**
 * strncat - Append a length-limited, C-string to another
 * @dest: The string to be appended to
 * @src: The string to append to it
 * @count: The maximum numbers of bytes to copy
 *
 * Note that in contrast to strncpy(), strncat() ensures the result is
 * terminated.
 */
char *strncat(char *dest, const char *src, size_t count);

/**
 * strlcat - Append a length-limited, C-string to another
 * @dest: The string to be appended to
 * @src: The string to append to it
 * @count: The size of the destination buffer.
 */
size_t strlcat(char *dest, const char *src, size_t count);

/**
 * strcmp - Compare two strings
 * @cs: One string
 * @ct: Another string
 */
int strcmp(const char *cs, const char *ct);

/**
 * strncmp - Compare two length-limited strings
 * @cs: One string
 * @ct: Another string
 * @count: The maximum number of bytes to compare
 */
int strncmp(const char *cs, const char *ct, size_t count);

int strcasecmp(const char *s1, const char *s2);

/**
 * strncasecmp - Case insensitive, length-limited string comparison
 * @s1: One string
 * @s2: The other string
 * @len: the maximum number of characters to compare
 */
int strncasecmp(const char *s1, const char *s2, size_t len);

/**
 * strchr - Find the first occurrence of a character in a string
 * @s: The string to be searched
 * @c: The character to search for
 *
 * Note that the %NUL-terminator is considered part of the string, and can
 * be searched for.
 */
char *strchr(const char *s, int c);

/**
 * strchrnul - Find and return a character in a string, or end of string
 * @s: The string to be searched
 * @c: The character to search for
 *
 * Returns pointer to first occurrence of 'c' in s. If c is not found, then
 * return a pointer to the null byte at the end of s.
 */
char *strchrnul(const char *s, int c);

/**
 * strnchrnul - Find and return a character in a length limited string,
 * or end of string
 * @s: The string to be searched
 * @count: The number of characters to be searched
 * @c: The character to search for
 *
 * Returns pointer to the first occurrence of 'c' in s. If c is not found,
 * then return a pointer to the last character of the string.
 */
char *strnchrnul(const char *s, size_t count, int c);

/**
 * strnchr - Find a character in a length limited string
 * @s: The string to be searched
 * @count: The number of characters to be searched
 * @c: The character to search for
 *
 * Note that the %NUL-terminator is considered part of the string, and can
 * be searched for.
 */
char *strnchr(const char *s, size_t count, int c);

/**
 * strrchr - Find the last occurrence of a character in a string
 * @s: The string to be searched
 * @c: The character to search for
 */
char *strrchr(const char *s, int c);

/**
 * strstr - Find the first substring in a %NUL terminated string
 * @s1: The string to be searched
 * @s2: The string to search for
 */
char *strstr(const char *s1, const char *s2);

/**
 * strnstr - Find the first substring in a length-limited string
 * @s1: The string to be searched
 * @s2: The string to search for
 * @len: the maximum number of characters to search
 */
char *strnstr(const char *s1, const char *s2, size_t len);

/**
 * strlen - Find the length of a string
 * @s: The string to be sized
 */
size_t strlen(const char *s);

/**
 * strpbrk - Find the first occurrence of a set of characters
 * @cs: The string to be searched
 * @ct: The characters to search for
 */
char *strpbrk(const char *cs, const char *ct);

/**
 * strsep - Split a string into tokens
 * @s: The string to be searched
 * @ct: The characters to search for
 *
 * strsep() updates @s to point after the token, ready for the next call.
 *
 * It returns empty tokens, too, behaving exactly like the libc function
 * of that name. In fact, it was stolen from glibc2 and de-fancy-fied.
 * Same semantics, slimmer shape. ;)
 */
char *strsep(char **s, const char *ct);

/**
 * strspn - Calculate the length of the initial substring of @s which only contain letters in @accept
 * @s: The string to be searched
 * @accept: The string to search for
 */
size_t strspn(const char *s, const char *accept);

/**
 * strcspn - Calculate the length of the initial substring of @s which does not contain letters in @reject
 * @s: The string to be searched
 * @reject: The string to avoid
 */
size_t strcspn(const char *s, const char *reject);

/**
 * memset - Fill a region of memory with the given value
 * @s: Pointer to the start of the area.
 * @c: The byte to fill the area with
 * @count: The size of the area.
 *
 * Do not use memset() to access IO space, use memset_io() instead.
 */
void *memset(void *s, int c, size_t count);
```

## kfifo

kfifo 的使用可以参考内核代码仓库中 samples/kfifo/inttype-example.c。

## klist

klist 在内核 list 之上提供了一层封装，提供了一些辅助函数。

```c
/**
 * klist_init - Initialize a klist structure.
 * @k: The klist we're initializing.
 * @get: The get function for the embedding object (NULL if none)
 * @put: The put function for the embedding object (NULL if none)
 *
 * Initialises the klist structure.  If the klist_node structures are
 * going to be embedded in refcounted objects (necessary for safe
 * deletion) then the get/put arguments are used to initialise
 * functions that take and release references on the embedding
 * objects.
 */
void klist_init(struct klist *k, void (*get)(struct klist_node *),
		void (*put)(struct klist_node *));

/**
 * klist_add_tail - Initialize a klist_node and add it to back.
 * @n: node we're adding.
 * @k: klist it's going on.
 */
void klist_add_tail(struct klist_node *n, struct klist *k);

/**
 * klist_add_head - Initialize a klist_node and add it to front.
 * @n: node we're adding.
 * @k: klist it's going on.
 */
void klist_add_head(struct klist_node *n, struct klist *k);

/**
 * klist_add_behind - Init a klist_node and add it after an existing node
 * @n: node we're adding.
 * @pos: node to put @n after
 */
void klist_add_behind(struct klist_node *n, struct klist_node *pos);

/**
 * klist_add_before - Init a klist_node and add it before an existing node
 * @n: node we're adding.
 * @pos: node to put @n after
 */
void klist_add_before(struct klist_node *n, struct klist_node *pos);

/**
 * klist_del - Decrement the reference count of node and try to remove.
 * @n: node we're deleting.
 */
void klist_del(struct klist_node *n);

/**
 * klist_remove - Decrement the refcount of node and wait for it to go away.
 * @n: node we're removing.
 */
void klist_remove(struct klist_node *n);
```

## llist（NULL 结尾的无锁的单链表）

**不需要锁的情况**

如果有多个生产者和多个消耗者，在无锁的情况下，llist_add 可以用于所有的生产者并且同时 llist_del_all 可以用于所有的消耗者。
另外，单个消耗者可以使用 llist_del_first 同时多个生产者可以在无锁的情况下使用 llist_add。

**需要加锁的情况**

如果有多个消耗者，其中有一个使用 llist_del_first，并且其它消耗者使用 llist_del_first 或 llist_del_all，那么此时需要加锁。
因为 llist_del_first 依赖的 list->first->next 没有改变，但是没有锁的保护，不能够确保在删除操作中发生了抢占，并且在被抢占后，list->first 和在 llist_del_first 引发 cmpxchg 成功之前一样。
例如，当一个 llist_del_first 操作正在一个消耗者中进行时，一个 llist_del_first、llist_add、llist_add（或 llist_del_all、llist_add、llist_add）操作序列在另外一个消耗者中进行则产生破坏。

可以总结如下：

```
          |   add    | del_first |  del_all
add       |    -     |     -     |     -
del_first |          |     L     |     L
del_all   |          |           |     -
```

一个特殊的行中的操作可以和一个列中的操作同时发生，“-” 表明无需锁，“L” 表明需要锁。

通过 llist_del_all 删除链表条目可以由遍历函数来遍历，比如 llist_for_each。
但是链表条目不能在其从链表中删除之前安全的遍历。
删除条目的顺序是从最新添加到最旧添加。
如果你想要从旧的条目到新条目的顺序删除，那么你必须反转链表。

这些链表的基础原子操作是 long 数据类型的 cmpxchg。
在一些没有 NMI 安全的 cmpxchg 实现的架构下，链表**不能**在 NMI 处理函数中使用。
所以 NMI 处理函数中使用链表的代码应该用 CONFIG_ARCH_HAVE_NMI_SAFE_CMPXCHG 区分。

```c
struct my_data {
  void *data;
  struct llist_node n;
};

LLIST_HEAD(my_llist);

// add one entry
struct my_data d, d1;
d.data = kmalloc(16, GFP_KERNEL);
if (!d.data) {
  pr_err("can not allocate data\n");
  return -ENOMEM;
}
strcpy(d.data, "entry1");
llist_add(&d.n, &my_llist);

// traverse
struct my_data *dp;
llist_for_each_entry(dp, &my_llist, n) {
  pr_info("%s\n", dp->data);
}

// add another entry
d1.data = kmalloc(32, GFP_KERNEL);
if (!d1.data) {
  pr_err("can not allocate data\n");
  return -ENOMEM;
}
strcpy(d1.data, "entry1");
llist_add(&d1.n, &my_llist);

// reverse the llist
struct llist_node *my_llist_r;
my_llist_r = llist_reverse_order(&my_llist);
llist_for_each_entry(dp, &my_llist, n) {
  pr_info("%s\n", dp->data);
}

// traverse safely and free allocated data
struct my_data *dpt;
llist_for_each_entry_safe(dp, dpt, &my_llist_r, n) {
  kfree(dp);
}

// delete entries
llist_del_first(&my_llist_r);

llist_del_all(&my_llist_r);

if (llist_empty(&my_llist)) {
  pr_info("my_llist is empty now\n");
}
```

## workqueue

参见 wq-test.c 和 wqd-test.c。

## tasklet (deprecated, Please consider using threaded IRQs instead since 5.10 (longterm version)) 

```c
struct task_data {
  struct tasklet_struct task;
  int a;
};

struct task_data td;

static void task_func(struct tasklet_struct *t)
{
  struct task_data *tdp = from_tasklet(struct task_data, t, task);
  
  // do Something ...
  pr_info("data = %d\n", tdp->a);
}

static void task_func_i(unsigned long data)
{
  struct task_data *tdp = (struct task_data *)data;

  pr_info("data = %d\n", tdp->a);
}

// setup
tasklet_setup(&td.task, task_func);
// or init
tasklet_init(&td.task, task_func_i, (ulong)&td);

// tasklet schedule
tasklet_schedule(&td.task);

// kill
tasklet_kill(&td.task);
```

## radix-tree

```c
struct my_data {
  int a;
};

static RADIX_TREE(my_tree, GFP_KERNEL);
static struct my_data d1, d2;
d1.a = 1;
d2.a = 2;

radix_tree_insert(&my_tree, 0, &d1);
radix_tree_insert(&my_tree, 4, &d1);

struct my_data *dt = radix_tree_lookup(&my_tree, 4);
pr_info("4th data = %d\n", dt->a);

radix_tree_delete(&my_tree, 4);
```

## rbtree 

```c
struct my_data {
  int size;
  void *data;
  struct rb_node node;
};

struct my_data d;
struct rb_root mytree;

mytree = RB_ROOT;

struct rb_node **link = &mytree->rb_root.rb_node, *rb = NULL;
while (*link) {
  rb = *link;
  if (x > rb_entry(rb, struct my_data, node)->size) {
    link = &rb->rb_left;
  } else {
    link = &rb->rb_right;
    first = false;
  }
}
rb_link_node(&d.node, rb, link);
rb_insert_color(&d.node, &mytree);

rb_erase(&d.node, &mytree);
```

## rtmutex

```c
static DEFINE_RT_MUTEX(lock);
rt_mutex_lock(&lock);
rt_mutex_unlock(&lock);
```

## rwlock

```c
static DEFINE_RWLOCK(lock);
// or
// rwlock_t lock;
// rwlock_init(&lock);

unsigned long flags;
write_lock(&lock);
write_trylock(&lock);
write_unlock(&lock);
write_lock_irqsave(&lock, flags); // 上锁并获取 irq flags
write_unlock_irqrestore(&lock, flags);

read_lock(&lock);
read_trylock(&lock);
read_unlock(&lock);
read_lock_irqsave(&lock, flags);
read_unlock_irqrestore(&lock, flags);
```

## rwsem

```c
static DECLARE_RWSEM(sem);
// or
// struct rw_semaphore sem;
// init_rwsem(&sem);

down_read(&sem);
up_read(&sem);
down_write(&sem);
up_write(&sem);
```

## of

设备树相关的 API 可以移步查阅 https://docs.kernel.org/devicetree/kernel-api.html。

内核会对不同的驱动子系统封装不同的调用接口，比如时钟驱动子系统的 API 封装在
include/linux/of_clk.h 中。
具体使用哪些接口可根据需求或参考现有内核代码确定。

## uio 

UIO 的使用可以参考内核文档，
https://www.kernel.org/doc/html/v4.14/driver-api/uio-howto.html。

## skbuff

struct sk_buff 数据结构体是代表网络包的主要数据结构。

sk_buff 仅仅是一个元数据结构，它本身不会保存任何网络包数据。
所有的数据都保存在关联的 buffer 中。

sk_buff.head 指向主要的 “head” buffer。
head buffer 分成两部分：

- 数据 buffer，带有 header 或有 payload，skb 的部分操作会对其进行操作，比如 skb_put() 或 skb_pull()。
- 共享信息（struct skb_shared_info），保存有指向这种格式的（page、offset 和 length）只读数据的指针数组。

skb_shared_info.frag_list 也可能指向其它 skb。

基本的框图如下：

```
                                ---------------
                               | sk_buff       |
                                ---------------
   ,---------------------------  + head
  /          ,-----------------  + data
 /          /      ,-----------  + tail
|          |      |            , + end
|          |      |           |
v          v      v           v
 -----------------------------------------------
| headroom | data |  tailroom | skb_shared_info |
 -----------------------------------------------
                               + [page frag]
                               + [page frag]
                               + [page frag]
                               + [page frag]       ---------
                               + frag_list    --> | sk_buff |
                                                   ---------
```

更加详细的介绍可参考 https://docs.kernel.org/networking/skbuff.html。
