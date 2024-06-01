# Linux functions

[x] bitmap
[x] kfifo
[x] klist
[x] kthread
[x] list
[x] llist
[x] mutex
[] mm
[] of
[] radix-tree
[] rbtree
[] rtmutex
[] rwlock
[] rwsem
[x] semaphore
[] skbuff
[x] spinlock
[x] string
[] tasklet (deprecated, Please consider using threaded IRQs instead)
[] uio
[x] workqueue


## bitmap


## list


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

