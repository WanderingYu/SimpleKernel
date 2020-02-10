
// This file is a part of MRNIU/SimpleKernel (https://github.com/MRNIU/SimpleKernel).
//
// sched.c for MRNIU/SimpleKernel.

#ifdef __cplusplus
extern "C" {
#endif

#include "assert.h"
#include "cpu.hpp"
#include "include/linkedlist.h"
#include "intr/include/intr.h"
#include "sched/sched.h"

void clock_handler(pt_regs_t * regs UNUSED) {
	schedule();
}

// 当前任务指针
static task_pcb_t * curr_task = NULL;

void sched_init() {
	cpu_cli();
	// 获取内核进程控制结构体信息
	curr_task = (task_pcb_t *)(kernel_stack);
	// 注册时间相关的处理函数
	register_interrupt_handler(IRQ0, &clock_handler);
	enable_irq(IRQ0);
	printk_info("sched init\n");
	return;
}

void sched_switch(task_context_t * curr UNUSED, task_context_t * next UNUSED) {
	return;
}

// 比较方法
int vs_med(void * v1, void * v2) {
	return v1 == v2;
}

void schedule() {
	// 首先从链表中找到正在执行的进程，结果不为空
	ListEntry * tmp = list_find_data(runnable_list, vs_med, curr_task);
	assert( (tmp != NULL), "Error at sched.c, tmp == NULL!\n");

	// printk_debug("(task_pcb_t *)list_data(tmp): 0x%08X\n", (task_pcb_t *)list_data(tmp) );
	task_pcb_t * next = (task_pcb_t *)(list_data(list_next(tmp) ) );
	// printk_debug("next: 0x%08X\n", (next) );
	if(next != NULL) {
		change_task_to(next);
	}
	// 如果下一个进程为 NULL，则从第一个任务开始
	else {
		change_task_to( (task_pcb_t *)(list_nth_data(runnable_list, 0) ) );
	}
	return;
}

void change_task_to(task_pcb_t * next) {
	if(curr_task != next) {
		// printk_debug("current pid %d\n", current->pid);
		task_pcb_t * prev = curr_task;
		curr_task = next;
		// printk_debug("prev: 0x%08X\n", (prev) );
		// printk_debug("prev->context->esp: 0x%08X\n", (prev->context->esp) );
		// printk_debug("curr_task: 0x%08X\n", (curr_task) );
		// printk_debug("curr_task->context->esp: 0x%08X\n", (curr_task->context->esp) );
		switch_to( (prev->context), (curr_task->context) );
		// printk_debug("333333333\n");
	}
}

#ifdef __cplusplus
}
#endif