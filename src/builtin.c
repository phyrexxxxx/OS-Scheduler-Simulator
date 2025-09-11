#include "../include/builtin.h"
#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "../include/command.h"
#include "../include/task.h"

/*
 * Display help information
 *
 * List all available builtin commands
 */
int help(char **args)
{
    int i;
    printf("--------------------------------------------------\n");
    printf("My Little Shell!!\n");
    printf("The following are built in:\n");

    /* List all builtin commands */
    for (i = 0; i < num_builtins(); i++) {
        printf("%d: %s\n", i, builtin_str[i]);
    }
    printf("%d: replay\n", i);
    printf("--------------------------------------------------\n");
    return 1;
}

/*
 * Change current working directory
 *
 * 參數：args[1] - 目標目錄路徑
 */
int cd(char **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    } else {
        /* 使用 chdir 系統呼叫改變目錄 */
        if (chdir(args[1]) != 0)
            perror("lsh");
    }
    return 1;
}

/*
 * Output text to standard output
 *
 * 支援 -n 選項：不輸出換行符號
 */
int echo(char **args)
{
    bool newline = true;

    for (int i = 1; args[i]; ++i) {
        /* Check -n option */
        if (i == 1 && strcmp(args[i], "-n") == 0) {
            newline = false;
            continue;
        }

        printf("%s", args[i]);

        /* 在參數之間加入空格 */
        if (args[i + 1])
            printf(" ");
    }

    /* 根據 newline 標記決定是否輸出換行 */
    if (newline)
        printf("\n");

    return 1;
}

/*
 * Exit shell
 *
 * 回傳 0 表示結束 shell 主迴圈
 */
int exit_shell(char **args)
{
    return 0;
}

/*
 * Display command history
 *
 * 使用循環緩衝區儲存最近的 MAX_RECORD_NUM 筆命令
 */
int record(char **args)
{
    if (history_count < MAX_RECORD_NUM) {
        /* 歷史記錄未滿，從頭顯示到當前 */
        for (int i = 0; i < history_count; ++i) printf("%2d: %s\n", i + 1, history[i]);
    } else {
        /* 歷史記錄已滿，使用循環緩衝區 */
        for (int i = history_count % MAX_RECORD_NUM; i < history_count % MAX_RECORD_NUM + MAX_RECORD_NUM; ++i)
            printf("%2d: %s\n", i - history_count % MAX_RECORD_NUM + 1, history[i % MAX_RECORD_NUM]);
    }
    return 1;
}

/*
 * Helper function: check if a string is a pure number
 *
 * 參數：str - 要檢查的字串
 * 回傳值：true 表示是純數字，false 表示包含非數字字元
 */
bool isnum(char *str)
{
    for (int i = 0; i < strlen(str); ++i) {
        /* 檢查是否為數字字元 ('0'-'9' 的 ASCII 值為 48-57) */
        if (str[i] >= 48 && str[i] <= 57)
            continue;
        else
            return false;
    }
    return true;
}

/*
 * 顯示 process ID 相關資訊
 *
 * 支援三種模式：
 * -i: 顯示當前 process 的 PID
 * -p <pid>: 顯示指定 process 的父 process ID (PPID)
 * -c <pid>: 顯示指定 process 的所有子 process
 */
int mypid(char **args)
{
    char fname[BUF_SIZE];
    char buffer[BUF_SIZE];

    if (strcmp(args[1], "-i") == 0) {
        /* -i: Display current process's PID */
        pid_t pid = getpid();
        printf("%d\n", pid);

    } else if (strcmp(args[1], "-p") == 0) {
        if (args[2] == NULL) {
            printf("mypid -p: too few argument\n");
            return 1;
        }

        sprintf(fname, "/proc/%s/stat", args[2]);
        int fd = open(fname, O_RDONLY);
        if (fd == -1) {
            printf("mypid -p: process id not exist\n");
            return 1;
        }

        read(fd, buffer, BUF_SIZE);
        strtok(buffer, " ");
        strtok(NULL, " ");
        strtok(NULL, " ");
        char *s_ppid = strtok(NULL, " ");
        int ppid = strtol(s_ppid, NULL, 10);
        printf("%d\n", ppid);

        close(fd);

    } else if (strcmp(args[1], "-c") == 0) {
        if (args[2] == NULL) {
            printf("mypid -c: too few argument\n");
            return 1;
        }

        DIR *dirp;
        if ((dirp = opendir("/proc/")) == NULL) {
            printf("open directory error!\n");
            return 1;
        }

        struct dirent *direntp;
        while ((direntp = readdir(dirp)) != NULL) {
            if (!isnum(direntp->d_name)) {
                continue;
            } else {
                sprintf(fname, "/proc/%s/stat", direntp->d_name);
                int fd = open(fname, O_RDONLY);
                if (fd == -1) {
                    printf("mypid -p: process id not exist\n");
                    return 1;
                }

                read(fd, buffer, BUF_SIZE);
                strtok(buffer, " ");
                strtok(NULL, " ");
                strtok(NULL, " ");
                char *s_ppid = strtok(NULL, " ");
                if (strcmp(s_ppid, args[2]) == 0)
                    printf("%s\n", direntp->d_name);

                close(fd);
            }
        }
        closedir(dirp);
    } else {
        printf("wrong type! Please type again!\n");
    }

    return 1;
}

/*
 * Add a task to the system
 *
 * 參數：
 *   args[1] - task 名稱
 *   args[2] - 要執行的函數名稱
 *   args[3] - 優先權 (數值越小優先權越高)
 *
 * 使用範例：add T1 test_exit 5
 */
int add(char **args)
{
    /* 檢查參數數量 */
    if (args[1] == NULL || args[2] == NULL || args[3] == NULL) {
        printf("add: too few argument\n");
        return 1;
    }

    /* 驗證優先權參數 */
    if (!isnum(args[3]) || atoi(args[3]) < 0) {
        printf("add: priority is not a valid number\n");
        return 1;
    }

    char *task_name = args[1];
    char *function_name = args[2];
    int priority = atoi(args[3]);

    /* 建立新 task */
    Task *task = task_create(task_name, function_name, priority);
    if (task == NULL) {
        printf("Create task failed.\n");
    } else {
        /* 加入 task 到 scheduler queue */
        task_add(task);
        printf("Task %s is ready.\n", task_name);
    }
    return 1;
}

/*
 * 刪除指定的 task
 *
 * 參數：args[1] - 要刪除的 task 名稱
 *
 * 使用範例：del T1
 */
int del(char **args)
{
    /* 檢查參數 */
    if (args[1] == NULL) {
        printf("del: too few argument\n");
        return 1;
    }

    char *task_name = args[1];

    /* 呼叫 task_del 刪除 task */
    if (!task_del(task_name)) {
        printf("Cannot find task %s.\n", task_name);
    } else {
        printf("Task %s is killed.\n", task_name);
    }
    return 1;
}

/*
 * Display all task's status information
 *
 * 類似 Unix 的 ps 命令，顯示 task 列表和其狀態
 */
int ps(char **args)
{
    task_ps();
    return 1;
}

/*
 * 開始或恢復 scheduler 模擬
 *
 * 啟動 scheduler，開始執行已加入的 tasks
 * 如果之前暫停過 (Ctrl+Z)，則恢復執行
 */
int start(char **args)
{
    printf("Start simulation.\n");
    task_start();
    return 1;
}

/*
 * Builtin command name array
 *
 * 與 builtin_func 陣列一一對應
 */
const char *builtin_str[] = {
    "help",   /* 顯示幫助 */
    "cd",     /* 改變目錄 */
    "echo",   /* 輸出文字 */
    "exit",   /* 離開 shell */
    "record", /* 命令歷史 */
    "mypid",  /* PID 資訊 */
    "add",    /* 新增 task */
    "del",    /* 刪除 task */
    "ps",     /* 顯示 task 狀態 */
    "start"   /* 開始模擬 */
};

/*
 * 內建命令函數指標陣列
 *
 * 與 builtin_str 陣列一一對應
 */
const int (*builtin_func[])(char **) = {&help, &cd, &echo, &exit_shell, &record, &mypid, &add, &del, &ps, &start};

/*
 * 取得內建命令的數量
 */
int num_builtins()
{
    return sizeof(builtin_str) / sizeof(char *);
}
