# Prex Kernel Debugging Tips and Tricks

*For Prex version 0.8.2, 2009/04/11*

### Table of Contents

- [Building a Debugging Kernel](#building-a-debugging-kernel)
- [Kernel Print](#kernel-print)
- [Kernel Panic](#kernel-panic)
- [Assertion Check](#assertion-check)
- [Function Trace](#function-trace)
- [Kernel Dump](#kernel-dump)
- [Remote debugging with GDB](#remote-debugging-with-gdb)
- [Using GDB with QEMU](#using-gdb-with-qemu)





## Building a Debugging Kernel

The Prex kernel is compiled with debugging version by default. The debugging version will have the following extra functions compared with the release version.

- printf() to output the debug message.
- panic() log for the fatal error.
- ASSERT() for the assertion check.
- Kernel function trace.
- Dump of the kernel objects.

However, you should care about the following two points.

- The size of executable image will become larger than      the release version.
- The execution speed becomes slower than the release version.

In order to build a release version, you must set the shell variable named NDEBUG to 1 before compiling the kernel.

```
$ export NDEBUG=1
```

## Kernel Print

The most powerful tool for kernel debugging is printf(). You can check the code path, or get the data value by putting the printf() anywhere in  the kernel code.

This is the subset of printf() in standard C library. You can use only following identifiers for the output string.

- %d - Decimal signed integer
- %x - Hex integer
- %u - Unsigned integer
- %c - Character
- %s - String

## Kernel Panic

### What is Kernel Panic?

Panic is used to detect unrecoverable error in the kernel. The kernel code can call panic() routine anytime if it encounters a fatal error. panic() function will stop the system and display the message if output device is available. If the kernel is not compiled with debug mode, panic() will just reset the system without any message. When GDB is connect to the kernel, the control will be dropped to the debugger in panic().

### Panic Screen

The following is a sample screen of panic() on i386.

```
=====================================================
Kernel panic!
Failed to create interrupt thread
=====================================================
============================
Trap 3: Breakpoint
============================
Trap frame 80002f28 error 0
 eax 00000001 ebx 800110ac ecx ffffffff edx 800192b6 esi 00000000 edi 00000000
 eip 80014289 esp 80002f28 ebp 80002f78 eflags 00000002
 cs  00000010 ss  00000018 ds  00000018 es  00000018 esp0 00002fff
 >> interrupt is disabled
Stack trace:
 80011103
 8001130c
 80010115
```

## Assertion Check

### What is Assertion Check?

The assert macro puts error messages into the kernel. If the specified expression in ASSERT() is false, it displays the message to the output device and stops the system.

For example, the following ASSERT() macro is put to the entry of the irq_attach() routine. This means the caller of irq_attach() must provide a valid function pointer for ISR (interrupt service routine).

```
int irq_attach(int vector, int prio, int shared, int (*isr)(int), void (*ist)(int))
{
	ASSERT(isr != NULL);
	...
```

If the kernel detect the invalid argument for isr, it will cause a panic() with the following message.

```
Assertion failed: irq.c line:120 in irq_attach() 'isr != NULL'
```

The important point is ASSERT() macro is only enabled with the kernel debug version. If the assertion is caused by an application bug or h/w bug, it should not be covered by ASSERT(). Such fault should be handled as an "error" properly.

### IRQ Assertion

There are some kernel routines that can not be called during an interrupt context. For example, the following functions can not be called by ISR.

- IRQ control code. (irq_attach() or irq_detach).
- The system call code that requires an user mode context.
- The kernel code that assumes it can synchronize by scheduling lock.      (kmem).

In order to detect the invalid call of these functions, the kernel checks the variable named irq_level. The kernel developer can use this check in the head of the kernel routine which can not be called by ISR. When ISR call such routine, the kernel will report the problem with panic() message.

```
void
irq_detach(irq_t irq)
{
        ASSERT(irq_level == 0);
        ...
```

## Function Trace

**IMPORTANT: Function trace was dropped in the formal source tree.**

The kernel trace is used to log the entry/exit of all functions at  runtime. It is useful to debug the timing critical issue related to  an interrupt handler.

### Installing the kernel trace

You must set the shell variable named KTRACE to 1 before compiling the kernel.

```
$ export KTRACE=1
```

The kernel function trace is disabled by default. If you get the error from gcc, you should check the function with "extern inline" in the header file. Such function must be replaced as "static inline" routine to enable the function trace.

### Using the kernel trace

The following functions are available for the kernel trace.

- trace_on: Enable a function trace.
- trace_off: Disable a function trace.
- trace_dump: Dump the latest function log.

The trace code is using a ring buffer to store the log data. So, older information will be disposed when newer log is filled in the buffer.

## Kernel Dump

### Kernel Dump

The kernel dump is used to dump the current status of each kernel objects.

If you assign magic keys for the kernel dump, you can get the  kernel dump as far as the keyboard IRQ is enabled. The key assignment of the kernel dump depends on each platform.

### Dump Sample

The following dump screen was got with Prex-i386pc.

```
Kernel dump usage:
F1=help F2=thread F3=task F4=object F5=timer F6=irq F7=dev F8=mem

thread_dump:
 mod thread   task     stat pol  prio base lock qntm total    susp sleep event
 --- -------- -------- ---- ---- ---- ---- ---- ---- -------- ---- ------------
 Knl 8002a014 800191a0 SLP  FIFO  022  022    1   50        1    0 interrupt
 Knl 80029a54 800191a0 SLP  FIFO  020  020    1   50        0    0 interrupt
 Knl 80029464 800191a0 SLP  RR    015  015    1   50        0    0 timer
 Knl 800190c0 800191a0 RUN* FIFO  255  255    1    0    44719    0 -
 Usr 8002a6e4 8002a574 SLP  RR    200  200    1   50        3    0 kbd

task_dump:
 mod task      nr_obj nr_thr map      susp exc hdlr name
 --- --------- ------ ------ -------- ---- -------- ------------
 Knl 800191a0*      1      4 8001ac20    0 00000000 kernel
 Usr 8002a574       1      1 8002a5e4    0 00000000 kmon

IRQ dump:
 vector isr      ist      ist-thr  ist-prio count
 ------ -------- -------- -------- -------- --------
 00     800110c8 00000000 00000000        0    54124
 01     80020784 800209f8 80029a54       20       66
 06     800212d0 80021360 8002a014       22        1
 12     80021aec 00000000 00000000        0        0

device_dump:
 device   open     close    read     write    ioctl    event    name
 -------- -------- -------- -------- -------- -------- -------- ------------
 8002a544 00000000 00000000 00000000 80020ecc 00000000 00000000 console
 80029fb4 80021b80 80021bb0 80021be0 00000000 00000000 00000000 mouse
 80029f44 8002161c 8002163c 80021714 80021800 800218dc 00000000 fd0
 800299e4 80020a30 80020a50 80020a70 00000000 00000000 00000000 kbd
 800299b4 00000000 00000000 80020584 00000000 00000000 00000000 rtc
 80029984 80022808 80022828 00000000 00000000 80022848 00000000 pm
 80029954 00000000 00000000 00000000 00000000 8002212c 00000000 cpu

page_dump:
 free pages:
 start      end      size
 --------   -------- --------
 00025000 - 00027000     2000
 00034000 - 0009f000    6b000
 00100000 - 040ff000  3fff000
 used=200K free=65971K total=66171K

kmem_dump: page=2 (8Kbyte) alloc=7056byte unused=1136byte
Allocated block:
 Size   48 bytes => 11
 Size   64 bytes => 5
 Size  112 bytes => 1
 Size  224 bytes => 4
 Size 1040 bytes => 5
Free block:
 Size   32 bytes => 1
 Size 1072 bytes => 1

vm_dump:
task=8002a574 map=8002a5e4 name=kmon
 region   virtual  physical size     flags
 -------- -------- -------- -------- -----
 8002a624 08048000 0002c000     1000 R----
 8002a654 08049000 0002e000     1000 RW---
 8002a6b4 7ffff000 0002f000     1000 RW---
 *total=12K bytes
```

## Remote Debugging with GDB

**IMPORTANT: The GDB support was dropped in the formal source tree.**

### Installing the GDB remote stub

To install GDB stub into the Prex kernel, you must uncomment the following line in config.h.

```
#define CONFIG_GDB 1
```

The GDB support is disabled by default.

### Connecting to the remote machine

You should specify the path to the symbol file.

```
(gdb) symbol-file /usr/src/prex/sys/prex.sym
(gdb) set remotebaud 115200
(gdb) target remote /dev/ttyS0
```

### Debugging Sample

```
$ i386-elf-gdb
GNU gdb 6.3
Copyright 2004 Free Software Foundation, Inc.
GDB is free software, covered by the GNU General Public License, and you are
welcome to change it and/or distribute copies of it under certain conditions.
Type "show copying" to see the conditions.
There is absolutely no warranty for GDB.  Type "show warranty" for details.
This GDB was configured as "--host=i686-pc-cygwin --target=i386-elf".
(gdb) symbol-file /usr/src/prex-0.1.2/sys/prex.sym
Reading symbols from /usr/src/prex-0.1.2/sys/prex.sym...done.
(gdb) set remotebaud 115200
(gdb) target remote /dev/ttyS0
Remote debugging using /dev/ttyS0
gdb_init () at gdb_glue.c:98
98      }
(gdb) b context_init
Breakpoint 1 at 0x80010d27: file context.c, line 72.
(gdb) info break
Num Type           Disp Enb Address    What
1   breakpoint     keep y   0x80010d27 in context_init at context.c:72
(gdb) c
Continuing.

Breakpoint 1, context_init (ctx=0x8001a16c, kstack=0x8002c414) at context.c:72
72      {
(gdb) bt 3
#0  context_init (ctx=0x8001a16c, kstack=0x8002c414) at context.c:72
#1  0x800137fe in thread_init () at thread.c:469
#2  0x800125e0 in kernel_main () at main.c:76
(More stack frames follow...)
(gdb) list context_init
67       *
68       * @ctx: context id (pointer)
69       * @kstack: kernel stack for the context
70       */
71      void context_init(context_t ctx, void *kstack)
72      {
73              struct kern_regs *k;
74              struct cpu_regs *u;
75
76              ctx->uregs = (struct cpu_regs *)(kstack - sizeof(struct cpu_regs
));
(gdb) up
#1  0x800137fe in thread_init () at thread.c:469
469             context_init(&idle_thread.context, stk + KSTACK_SIZE);
(gdb) list
464                     panic("Failed to allocate idle stack");
465             memset(stk, 0, KSTACK_SIZE);
466             idle_thread.kstack = stk;
467             printf("kernel stack size=%d\n", KSTACK_SIZE);
468
469             context_init(&idle_thread.context, stk + KSTACK_SIZE);
470             list_insert(&kern_task.threads, &idle_thread.task_link);
471     }
(gdb) b 465
Breakpoint 2 at 0x800137c6: file thread.c, line 465.
(gdb) c
Continuing.
```

## Using GDB with QEMU

QEMU has capability to connect to GDB on the same local machine.

- Preparing gdbinit

  You have to prepare the following "gdbqemu" for gdb.

  ```
  symbol-file /usr/src/prex/sys/prex.sym
  target remote localhost:1234
  ```

- Run QEMU

  Execute the following command:

  ```
  $ qemu -s -S -fda (your directory)/prex-X.X.X.i386-pc.img -localtime
  ```

  Then, the following message will be appered.

  ```
  Waiting gdb connection on port 1234
  _
  ```

- Run GDB

  ```
  $ gdb -x gdbqemu
  ```



Copyright© 2005-2009 Kohsuke Ohtani