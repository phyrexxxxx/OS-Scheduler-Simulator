#include "../include/task.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "../include/function.h"

/*
 * Global Variables for Task Management
 */
static int tid = 1;          /* Task ID 計數器，從 1 開始遞增 */
static Task *queue = NULL;   /* Task queue 的頭指標 (linked list) */
static int algorithm = 0;    /* 當前使用的排程演算法 (FCFS/RR/PP) */
static bool is_idle = false; /* CPU 是否處於 idle 狀態的標記 */
static bool pause = false;   /* 模擬是否暫停的標記 (Ctrl+Z) */

/* Context 相關變數 */
static ucontext_t current_context; /* 主迴圈的 context (scheduler context) */
static ucontext_t pause_context;   /* 暫停時儲存的 context */
static Task *current_task = NULL;  /* 當前正在執行的 task 指標 */

/*
 * 取得當前執行中的 task
 * 回傳值：當前 task 的指標，若無則回傳 NULL
 */
Task *get_current_task()
{
    return current_task;
}

/*
 * 取得當前的 scheduler context
 * 用於 task 切換時回到 scheduler 的主迴圈
 */
ucontext_t *get_current_context()
{
    return &current_context;
}

/*
 * 設定排程演算法
 * 參數：algo - 演算法類型 (FCFS=0, RR=1, PP=2)
 */
void set_algorithm(int algo)
{
    algorithm = algo;
}

/*
 * 建立新的 task
 *
 * 參數：
 *   task_name - task 的名稱 (唯一識別符)
 *   function_name - 要執行的函數名稱
 *   priority - 優先權 (用於 PP 演算法，數值越小優先權越高)
 *
 * 回傳值：成功回傳 task 指標，失敗回傳 NULL
 */
Task *task_create(char *task_name, char *function_name, int priority)
{
    int i;
    /* 動態分配 Task Control Block (TCB) 的記憶體 */
    Task *task = (Task *) malloc(sizeof(Task));
    if (task == NULL) {
        return NULL;
    }

    /* 初始化 task 的基本資訊 */
    task->task_name = strdup(task_name);         /* 複製 task 名稱 */
    task->function_name = strdup(function_name); /* 複製函數名稱 */
    task->priority = priority;                   /* 設定優先權 */
    task->state = READY;                         /* 初始狀態為 READY */
    task->tid = tid++;                           /* 分配唯一的 Task ID */
    task->running = 0;                           /* 執行時間初始化為 0 */
    task->waiting = 0;                           /* 等待時間初始化為 0 */
    task->time_quantum = 0;                      /* RR 時間片初始化為 0 */
    task->turnaround = 0;                        /* Turnaround time 初始化為 0 */
    task->next = NULL;                           /* linked list 指標初始化 */

    /* 初始化資源陣列，所有資源都未持有 */
    for (i = 0; i < RESOURCE_SIZE; i++) {
        task->resource[i] = false;
    }

    /* 設定 task 的 context (使用 ucontext API) */
    getcontext(&(task->context));                               /* 取得當前 context 作為基礎 */
    task->context.uc_stack.ss_sp = task->stack;                 /* 設定 stack 指標 */
    task->context.uc_stack.ss_size = sizeof(char) * STACK_SIZE; /* 設定 stack 大小 (128KB) */
    task->context.uc_link = &current_context;                   /* 設定返回的 context (scheduler) */
    if (strcmp(task->function_name, "task1") == 0) {
        makecontext(&(task->context), task1, 0);
    } else if (strcmp(task->function_name, "task2") == 0) {
        makecontext(&(task->context), task2, 0);
    } else if (strcmp(task->function_name, "task3") == 0) {
        makecontext(&(task->context), task3, 0);
    } else if (strcmp(task->function_name, "task4") == 0) {
        makecontext(&(task->context), task4, 0);
    } else if (strcmp(task->function_name, "task5") == 0) {
        makecontext(&(task->context), task5, 0);
    } else if (strcmp(task->function_name, "task6") == 0) {
        makecontext(&(task->context), task6, 0);
    } else if (strcmp(task->function_name, "task7") == 0) {
        makecontext(&(task->context), task7, 0);
    } else if (strcmp(task->function_name, "task8") == 0) {
        makecontext(&(task->context), task8, 0);
    } else if (strcmp(task->function_name, "task9") == 0) {
        makecontext(&(task->context), task9, 0);
    } else if (strcmp(task->function_name, "test_exit") == 0) {
        makecontext(&(task->context), test_exit, 0);
    } else if (strcmp(task->function_name, "test_sleep") == 0) {
        makecontext(&(task->context), test_sleep, 0);
    } else if (strcmp(task->function_name, "test_resource1") == 0) {
        makecontext(&(task->context), test_resource1, 0);
    } else if (strcmp(task->function_name, "test_resource2") == 0) {
        makecontext(&(task->context), test_resource2, 0);
    } else if (strcmp(task->function_name, "idle") == 0) {
        makecontext(&(task->context), idle, 0);
    } else {
        printf("Invalid function name: %s\n", task->function_name);
        return NULL;
    }
    return task;
}

/*
 * 將 task 加入 task queue
 *
 * 根據不同的排程演算法，task 的插入位置不同：
 * - FCFS/RR: 插入到 queue 尾端 (FIFO)
 * - PP: 根據優先權插入到適當位置 (優先權越小越前面)
 */
void task_add(Task *task)
{
    if (algorithm != PP) { /* FCFS 或 RR 演算法 */
        /* 將 task 加到 queue 尾端 (FIFO 順序) */
        if (queue == NULL) {
            queue = task; /* queue 為空，直接設為第一個 */
        } else {
            /* 找到 queue 尾端並插入 */
            Task *ptr = queue;
            while (ptr->next != NULL) {
                ptr = ptr->next;
            }
            ptr->next = task;
        }
    } else { /* PP 演算法 */
        /* 根據優先權插入到適當位置 (數值越小優先權越高) */
        if (queue == NULL) {
            queue = task; /* queue 為空，直接設為第一個 */
        } else {
            /* 檢查是否應該插入到 queue 最前面 */
            if (task->priority < queue->priority) {
                task->next = queue;
                queue = task;
            } else {
                /* 找到適當的插入位置 (保持優先權順序) */
                Task *ptr = queue;
                while (ptr->next != NULL) {
                    if (task->priority < ptr->next->priority) {
                        /* 插入到 ptr 和 ptr->next 之間 */
                        task->next = ptr->next;
                        ptr->next = task;
                        return;
                    }
                    ptr = ptr->next;
                }
                /* 優先權最低，插入到 queue 尾端 */
                ptr->next = task;
            }
        }
    }
}

/*
 * 刪除指定名稱的 task
 *
 * 參數：task_name - 要刪除的 task 名稱
 * 回傳值：成功回傳 true，找不到 task 回傳 false
 *
 * 注意：不是真的從 queue 中移除，而是將狀態設為 TERMINATED
 */
bool task_del(char *task_name)
{
    Task *ptr = queue;
    /* 遍歷 queue 尋找符合名稱的 task */
    while (ptr != NULL) {
        if (strcmp(ptr->task_name, task_name) == 0) {
            ptr->state = TERMINATED; /* 標記為終止狀態 */
            return true;
        }
        ptr = ptr->next;
    }
    return false; /* 找不到指定的 task */
}

/*
 * 顯示所有 task 的狀態資訊 (類似 Unix ps 命令)
 *
 * 顯示內容包括：
 * - TID: Task ID
 * - name: Task 名稱
 * - state: 當前狀態 (READY/RUNNING/WAITING/TERMINATED)
 * - running: 累計執行時間
 * - waiting: 累計等待時間
 * - turnaround: Turnaround time
 * - resources: 持有的資源列表
 * - priority: 優先權
 */
void task_ps()
{
    printf("%4s|%11s|%11s|%8s|%8s|%11s|%10s|%9s\n", "TID", "name", "state", "running", "waiting", "turnaround",
           "resources", "priority");
    printf("--------------------------------------------------------------------------------\n");

    Task *ptr = queue;
    char *state[4] = {"READY", "RUNNING", "WAITING", "TERMINATED"}; /* 狀態名稱陣列 */
    char resource[20] = {'\0'};                                     /* 資源列表字串緩衝區 */
    char turnaround[20] = {'\0'};                                   /* Turnaround time 字串緩衝區 */
    while (ptr != NULL) {
        int i = 0;
        for (i = 0; i < RESOURCE_SIZE; i++) {
            if (ptr->resource[i] != false) {
                sprintf(resource + strlen(resource), "%d ", i);
            }
            resource[strlen(resource) - 1] = '\0';
        }
        if (strlen(resource) == 0) {
            sprintf(resource, "none");
        }

        if (ptr->turnaround == 0) {
            sprintf(turnaround, "none");
        } else {
            sprintf(turnaround, "%d", ptr->turnaround);
        }

        printf("%4d|%11s|%11s|%8d|%8d|%11s|%10s|%9d\n", ptr->tid, ptr->task_name, state[ptr->state], ptr->running,
               ptr->waiting, turnaround, resource, ptr->priority);
        ptr = ptr->next;
    }
}

/*
 * 設定 timer，每 10ms 觸發一次 SIGVTALRM signal
 *
 * 使用 ITIMER_VIRTUAL：只計算 process 在 user mode 執行的時間
 * 這確保了時間統計的準確性
 */
void set_timer()
{
    struct itimerval value;
    value.it_value.tv_sec = 0;             /* 初始延遲：0 s */
    value.it_value.tv_usec = 10 * 1000;    /* 初始延遲：10 ms */
    value.it_interval.tv_sec = 0;          /* 間隔時間：0 s */
    value.it_interval.tv_usec = 10 * 1000; /* 間隔時間：10 ms */
    setitimer(ITIMER_VIRTUAL, &value, NULL);
}

/*
 * 關閉 timer，停止產生 SIGVTALRM signal
 */
void close_timer()
{
    struct itimerval value;
    value.it_value.tv_sec = 0; /* 設定為 0 表示停止 timer */
    value.it_value.tv_usec = 0;
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_usec = 0;
    setitimer(ITIMER_VIRTUAL, &value, NULL);
}

/*
 * 找出下一個 READY 狀態的 task (用於 Round Robin)
 *
 * 參數：p - 當前 task 的指標
 * 回傳值：下一個 READY 狀態的 task，若無則回傳 NULL
 *
 * 搜尋順序：先從 p 的下一個開始找到 queue 尾端，
 *         再從 queue 頭開始找 (實現循環)
 */
Task *set_next_ready(Task *p)
{
    /* 從當前 task 的下一個開始搜尋到 queue 尾端 */
    Task *ptr = p->next;
    while (ptr != NULL) {
        if (ptr->state == READY) {
            return ptr;
        }
        ptr = ptr->next;
    }
    /* 從 queue 頭開始搜尋 (實現循環 Round Robin) */
    ptr = queue;
    while (ptr != NULL) {
        if (ptr->state == READY) {
            return ptr;
        }
        ptr = ptr->next;
    }
    return NULL; /* 沒有找到 READY 狀態的 task */
}

/*
 * SIGVTALRM signal handler
 *
 * 每 10ms 觸發一次，負責：
 * 1. 更新所有 task 的時間統計 (running/waiting/turnaround)
 * 2. 處理 sleep 中的 task (減少 sleep_time)
 * 3. Round Robin 的時間片管理
 * 4. 觸發 context switch (如果需要)
 */
void signal_handler()
{
    Task *ptr = queue, *next_task = NULL;
    bool running = false; /* 是否有 task 在執行 */
    bool ready = false;   /* 是否有 task 從 WAITING 變為 READY */

    /* 遍歷所有 task，更新狀態和時間 */
    while (ptr != NULL) {
        if (ptr->state == WAITING) {
            /* 更新 sleep 時間 */
            if (ptr->sleep_time > 0) {
                ptr->sleep_time -= 10; /* 每次減少 10ms */
            }
            /* Sleep 時間結束，設為 READY 狀態 */
            if (ptr->sleep_time <= 0) {
                ptr->state = READY;
                ready = true;
            }
        } else if (ptr->state == RUNNING) {
            ptr->running++; /* 增加執行時間 */
            running = true;
        } else if (ptr->state == READY) {
            ptr->waiting++; /* 增加等待時間 (在 ready queue 中) */
        }

        /* Round Robin 時間片管理 */
        if (algorithm == RR) {
            if (ptr->state == RUNNING && ptr->time_quantum > 0) {
                ptr->time_quantum -= 10; /* 減少剩餘時間片 */
                /* 時間片用完，設為 READY 狀態 */
                if (ptr->time_quantum <= 0) {
                    ptr->state = READY;
                }
            }
        }

        /* 更新 turnaround time (除了已終止的 task) */
        if (ptr->state != TERMINATED) {
            ptr->turnaround++;
        }
        ptr = ptr->next;
    }
    /* Round Robin: 檢查當前 task 的時間片是否用完 */
    if (algorithm == RR && current_task != NULL && current_task->time_quantum <= 0) {
        next_task = set_next_ready(current_task); /* 找下一個 READY 的 task */
    }

    /* Round Robin: 執行 context switch */
    if (next_task != NULL) {
        getcontext(&(current_task->context)); /* 儲存當前 task 的 context */
        if (current_task->time_quantum <= 0) {
            if (current_task != next_task) {
                printf("Task %s is running.\n", next_task->task_name);
            }
            current_task = next_task;
            next_task->state = RUNNING;
            next_task->time_quantum = 30;      /* 重設時間片為 30ms (3個 tick) */
            setcontext(&(next_task->context)); /* 切換到下一個 task */
        }
    }

    /* 如果 CPU idle 但有 task 變為 READY，回到 scheduler 主迴圈 */
    if (is_idle && !running && ready) {
        setcontext(&current_context);
    }
}

/*
 * SIGTSTP (Signal Terminal Stop) signal handler (Ctrl+Z)
 *
 * 處理模擬暫停：
 * 1. 儲存當前 context
 * 2. 關閉 timer
 * 3. 回到 shell 模式
 */
void pause_handler()
{
    pause = true;
    /* 儲存暫停時的 context，以便之後恢復 */
    getcontext(&pause_context);
    if (pause) {
        close_timer();                /* 停止 timer */
        setcontext(&current_context); /* 回到 scheduler 主迴圈 */
    } else {
        set_timer(); /* 恢復 timer (當從暫停恢復時) */
    }
}

/*
 * 開始或恢復 scheduler 執行
 *
 * 這是 scheduler 的主函數，負責：
 * 1. 設定 signal handlers
 * 2. 啟動 timer
 * 3. 執行 scheduling 主迴圈
 * 4. 處理 task 的執行和切換
 */
void task_start()
{
    /* 如果是從暫停狀態恢復，回到暫停時的 context */
    if (pause) {
        pause = false;
        setcontext(&pause_context);
    }

    /* 註冊 signal handlers */
    signal(SIGVTALRM, signal_handler); /* Timer signal */
    signal(SIGTSTP, pause_handler);    /* Ctrl+Z signal */

    set_timer(); /* 啟動 timer */

    /* Scheduler 主迴圈 */
    while (true) {
        /* 設定返回點：當呼叫 setcontext(&current_context) 時會跳到這裡 */
        getcontext(&current_context);

        /* 檢查是否按了 Ctrl+Z */
        if (pause) {
            pause = false;
            break; /* 返回 shell */
        }
        /* Round Robin: 處理 task 終止的情況 */
        if (algorithm == RR && current_task != NULL && current_task->state == TERMINATED) {
            Task *next_task = set_next_ready(current_task); /* 找下一個 READY 的 task */
            /* 切換到下一個 task */
            if (next_task != NULL) {
                current_task = next_task;
                next_task->state = RUNNING;
                next_task->time_quantum = 30; /* 設定時間片 */
                printf("Task %s is running.\n", next_task->task_name);
                setcontext(&(next_task->context)); /* 執行 context switch */
            }
        }
        /* 遍歷 task queue，尋找可執行的 task */
        Task *ptr = queue;
        is_idle = false;
        bool all_task_finish = true;

        while (ptr != NULL) {
            /* 檢查是否還有未完成的 task */
            if (ptr->state != TERMINATED) {
                all_task_finish = false;
            }

            if (ptr->state == READY) {
                /* 找到 READY 的 task，開始執行 */
                printf("Task %s is running.\n", ptr->task_name);
                ptr->state = RUNNING;

                /* Round Robin: 設定時間片 */
                if (algorithm == RR) {
                    ptr->time_quantum = 30;
                }

                current_task = ptr;
                setcontext(&(ptr->context)); /* 切換到 task context */

            } else if (ptr->state == WAITING) {
                is_idle = true; /* 有 task 在等待，CPU 可能需要 idle */

            } else if (ptr->state == RUNNING) {
                /* task 仍在執行中，繼續執行 */
                current_task = ptr;
                setcontext(&(ptr->context));
            }
            ptr = ptr->next;
        }
        /* 所有 task 都已完成，結束模擬 */
        if (all_task_finish) {
            printf("Simulation over.\n");
            close_timer(); /* 關閉 timer */
            return;
        }

        /* 沒有可執行的 task，CPU 進入 idle 狀態 */
        if (is_idle) {
            printf("CPU idle.\n");
            idle(); /* 執行 idle 函數 (無窮迴圈) */
        }
    }
}

/*
 * 讓當前 task 進入 sleep 狀態
 *
 * 參數：ms - sleep 的時間 (ms)
 *
 * 執行流程：
 * 1. 將 task 狀態設為 WAITING
 * 2. 設定 sleep_time
 * 3. 儲存當前 context
 * 4. 切換回 scheduler
 */
void task_sleep(int ms)
{
    if (current_task != NULL) {
        printf("Task %s goes to sleep.\n", current_task->task_name);
        current_task->state = WAITING;      /* 設為等待狀態 */
        current_task->sleep_time = 10 * ms; /* 轉換為 10ms 的倍數 */

        /* 儲存當前 context (當 sleep 結束後會從這裡繼續) */
        getcontext(&(current_task->context));

        if (current_task->state == WAITING) {
            /* 回到 scheduler 主迴圈 */
            setcontext(&current_context);
        }
    }
}

/*
 * 結束當前 task
 *
 * 將 task 狀態設為 TERMINATED，並切換回 scheduler
 */
void task_exit()
{
    if (current_task != NULL) {
        printf("Task %s has terminated.\n", current_task->task_name);
        current_task->state = TERMINATED; /* 標記為終止狀態 */
        setcontext(&current_context);     /* 回到 scheduler 主迴圈 */
    }
}
