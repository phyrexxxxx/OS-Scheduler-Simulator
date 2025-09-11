#ifndef TASK_H
#define TASK_H

#include <stdbool.h>
#include <time.h>
#include <ucontext.h>

/* Task State Definitions */
#define READY 0      /* READY State：在 ready queue 中等待執行 */
#define RUNNING 1    /* RUNNING State：正在 CPU 上執行 */
#define WAITING 2    /* WAITING State：等待資源或 sleep */
#define TERMINATED 3 /* TERMINATED State：task 已經結束 */

/* Scheduling Algorithm Definitions */
#define FCFS 0 /* First Come First Serve */
#define RR 1   /* Round Robin */
#define PP 2   /* Priority Preemptive */

/* System Constants */
#define RESOURCE_SIZE 8         /* 系統資源總數 (8 個資源，ID: 0-7) */
#define STACK_SIZE (1024 * 128) /* 每個 task 的 stack 大小 (128KB) */

/*
 * Task Control Block (TCB) 結構
 *
 * 用於儲存 task 的所有相關資訊，包括：
 * - Context: CPU 暫存器狀態 (使用 ucontext API)
 * - Stack: task 專屬的執行堆疊
 * - Scheduling 相關資訊：優先權、狀態、時間統計
 * - Resource 管理：持有的資源陣列
 */
typedef struct Task {
    ucontext_t context;           /* task 的 context (CPU 暫存器狀態) */
    char stack[STACK_SIZE];       /* task 專屬的 stack 空間 */
    char *task_name;              /* task 名稱 (唯一識別符) */
    char *function_name;          /* 要執行的函數名稱 */
    int priority;                 /* 優先權 (數值越小優先權越高) */
    int state;                    /* 當前狀態 (READY/RUNNING/WAITING/TERMINATED) */
    int tid;                      /* Task ID (唯一編號) */
    int running;                  /* 累計執行時間 (單位: 10ms) */
    int waiting;                  /* 累計等待時間 (在 ready queue 中的時間) */
    struct Task *next;            /* 指向下一個 task 的指標 (用於 linked list) */
    int sleep_time;               /* 剩餘 sleep 時間 (單位: 10ms) */
    bool resource[RESOURCE_SIZE]; /* 資源持有狀態陣列 (true: 持有, false: 未持有) */
    int time_quantum;             /* Round Robin 的剩餘時間片 (單位: 10ms) */
    int turnaround;               /* Turnaround time (從建立到結束的總時間) */
} Task;

/* Task Management Functions */
Task *get_current_task();               /* 取得當前執行中的 task */
ucontext_t *get_current_context();      /* 取得當前的 context */
void set_algorithm(int algo);           /* 設定排程演算法 */
Task *task_create(char *, char *, int); /* 建立新的 task */

/* Task Operation Functions */
void task_add(Task *); /* 將 task 加入系統，設為 READY State */
bool task_del(char *); /* 刪除指定名稱的 task，設為 TERMINATED State */
void task_ps();        /* 顯示所有 task 的狀態 (類似 Unix ps 命令) */
void task_start();     /* 開始或恢復排程器執行 */
void task_sleep(int);  /* 讓當前 task sleep 指定時間 */
void task_exit();      /* 結束當前 task */

#endif
