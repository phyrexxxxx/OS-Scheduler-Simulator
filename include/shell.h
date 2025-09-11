#ifndef SHELL_H
#define SHELL_H

#include "command.h"

/*
 * Shell 介面相關函數宣告
 * 
 * 本模組提供互動式 shell 功能，支援：
 * - 內建命令執行 (add, del, ps, start, exit 等)
 * - 外部命令執行
 * - Pipe 管線處理
 * - I/O 重導向
 * - 背景執行
 */

/* 執行命令 (內建命令或外部程式) */
int execute(struct pipes*);

/* 產生子 process 執行命令，處理 pipe 和 I/O 重導向 */
int spawn_proc(int, int, struct cmd*, struct pipes*);

/* 處理 pipe 管線命令 */
int fork_pipes(struct cmd*);

/* Shell 主迴圈，處理使用者輸入和命令執行 */
void shell();

#endif
