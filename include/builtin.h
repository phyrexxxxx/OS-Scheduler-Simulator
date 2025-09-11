#ifndef BUILTIN_H
#define BUILTIN_H

/*
 * Shell builtin command declaration
 *
 * 分為兩類：
 * 1. 一般 Shell 命令：help, cd, echo, exit, record, mypid
 * 2. Scheduler 控制命令：add, del, ps, start
 */

/* 一般 Shell 內建命令 */
int help(char **args);       /* 顯示幫助資訊 */
int cd(char **args);         /* 改變工作目錄 */
int echo(char **args);       /* 輸出文字 */
int exit_shell(char **args); /* 離開 shell */
int record(char **args);     /* 顯示命令歷史記錄 */
int mypid(char **args);      /* 顯示 process ID 資訊 */

/* Scheduler 控制命令 */
int add(char **args);   /* 新增 task 到系統，並設為 READY state */
int del(char **args);   /* 刪除指定 task，並設為 TERMINATED state */
int ps(char **args);    /* 顯示所有 task 狀態 */
int start(char **args); /* 開始或恢復 scheduler 執行 */

/* 內建命令名稱陣列 */
extern const char *builtin_str[];

/* 內建命令函數指標陣列 */
extern const int (*builtin_func[])(char **);

/* 取得內建命令數量 */
extern int num_builtins();

#endif
