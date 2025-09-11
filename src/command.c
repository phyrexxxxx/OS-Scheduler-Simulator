/**
 * @file command.c
 * @brief Shell 命令解析與處理模組的實作檔
 *
 * 本檔案實作了 Shell 命令解析的核心功能，包括：
 * - 從標準輸入讀取命令列
 * - 解析命令字串為結構化物件
 * - 支援 pipe、重導向、背景執行等功能
 * - 命令歷史記錄管理
 */

#include "../include/command.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 從標準輸入讀取一行命令
 * @return 指向包含使用者輸入命令的字串指標，失敗或空輸入時返回 NULL
 *
 * 此函數負責：
 * 1. 分配記憶體緩衝區來儲存使用者輸入
 * 2. 從 stdin 讀取一行文字
 * 3. 處理特殊命令（如 replay）
 * 4. 管理命令歷史記錄
 * 5. 過濾空白或無效輸入
 */
char *read_line()
{
    /* 分配輸入緩衝區記憶體 */
    char *buffer = (char *) malloc(BUF_SIZE * sizeof(char));
    if (buffer == NULL) {
        perror("Unable to allocate buffer");
        exit(1);
    }

    /* 從標準輸入讀取一行 */
    if (fgets(buffer, BUF_SIZE, stdin) != NULL) {
        /* 檢查是否為空白輸入（換行、空格、Tab） */
        if (buffer[0] == '\n' || buffer[0] == ' ' || buffer[0] == '\t') {
            free(buffer);
            buffer = NULL;
        } else {
            /* 處理 replay 命令：重複執行歷史命令 */
            if (strncmp(buffer, "replay", 6) == 0) {
                char *token = strtok(buffer, " "); /* 獲取 "replay" 關鍵字 */
                token = strtok(NULL, " ");         /* 獲取要重複的命令索引 */
                int index = strtol(token, NULL, 10);

                /* 檢查索引是否有效 */
                if (index > MAX_RECORD_NUM || index > history_count) {
                    free(buffer);
                    buffer = NULL;
                } else {
                    /* 分配臨時緩衝區來組合命令 */
                    char *temp = (char *) malloc(BUF_SIZE * sizeof(char));
                    int head = 0;

                    /* 計算歷史記錄的起始位置（處理環形緩衝區） */
                    if (history_count > MAX_RECORD_NUM) {
                        head += history_count % MAX_RECORD_NUM;
                    }

                    /* 複製歷史命令到臨時緩衝區 */
                    strncpy(temp, history[(head + index - 1) % MAX_RECORD_NUM], BUF_SIZE);

                    /* 附加額外的參數（如果有的話） */
                    token = strtok(NULL, " ");
                    while (token) {
                        strcat(temp, " ");
                        strcat(temp, token);
                        token = strtok(NULL, " ");
                    }

                    /* 將組合後的命令複製回 buffer */
                    strncpy(buffer, temp, BUF_SIZE);
                    free(temp);

                    /* 移除換行符並加入歷史記錄 */
                    buffer[strcspn(buffer, "\n")] = 0;
                    strncpy(history[history_count % MAX_RECORD_NUM], buffer, BUF_SIZE);
                    ++history_count;
                }
            } else {
                /* 一般命令：移除換行符並加入歷史記錄 */
                buffer[strcspn(buffer, "\n")] = 0;
                strncpy(history[history_count % MAX_RECORD_NUM], buffer, BUF_SIZE);
                ++history_count;
            }
        }
    }

    return buffer;
}

/**
 * @brief 將輸入的命令字串解析為結構化的命令物件
 * @param line 要解析的命令字串
 * @return 指向解析後的 cmd 結構體指標
 *
 * 此函數會將單一命令字串解析成包含以下資訊的結構化物件：
 * - 命令和參數列表
 * - Pipe 連接的命令序列
 * - I/O 重導向設定
 * - 背景執行標記
 *
 * 支援的語法：
 * - cmd arg1 arg2        : 基本命令
 * - cmd1 | cmd2          : Pipe
 * - cmd < input.txt      : 輸入重導向
 * - cmd > output.txt     : 輸出重導向
 * - cmd &                : 背景執行
 */
struct cmd *split_line(char *line)
{
    int args_length = 10; /* 初始參數陣列大小 */

    /* 分配並初始化主命令結構 */
    struct cmd *new_cmd = (struct cmd *) malloc(sizeof(struct cmd));
    new_cmd->head = (struct pipes *) malloc(sizeof(struct pipes));
    new_cmd->head->args = (char **) malloc(args_length * sizeof(char *));

    /* 初始化參數陣列為 NULL */
    for (int i = 0; i < args_length; ++i) {
        new_cmd->head->args[i] = NULL;
    }

    /* 初始化 pipe 節點屬性 */
    new_cmd->head->length = 0;
    new_cmd->head->next = NULL;

    /* 初始化命令屬性 */
    new_cmd->background = false;
    new_cmd->in_file = NULL;
    new_cmd->out_file = NULL;

    /* 指向當前正在處理的 pipe 節點 */
    struct pipes *temp = new_cmd->head;

    /* 使用 strtok 分割命令字串 */
    char *token = strtok(line, " ");
    while (token != NULL) {
        if (token[0] == '|') {
            /* 遇到 pipe 符號：建立新的 pipe 節點 */
            struct pipes *new_pipe = (struct pipes *) malloc(sizeof(struct pipes));
            new_pipe->args = (char **) malloc(args_length * sizeof(char *));

            /* 初始化新 pipe 節點的參數陣列 */
            for (int i = 0; i < args_length; ++i) {
                new_pipe->args[i] = NULL;
            }

            new_pipe->length = 0;
            new_pipe->next = NULL;

            /* 將新節點連接到 pipe 串列 */
            temp->next = new_pipe;
            temp = new_pipe;
        } else if (token[0] == '<') {
            /* 輸入重導向：讀取檔名 */
            token = strtok(NULL, " ");
            new_cmd->in_file = token;
        } else if (token[0] == '>') {
            /* 輸出重導向：讀取檔名 */
            token = strtok(NULL, " ");
            new_cmd->out_file = token;
        } else if (token[0] == '&') {
            /* 背景執行標記 */
            new_cmd->background = true;
        } else {
            /* 一般參數：加入當前 pipe 節點的參數列表 */
            temp->args[temp->length] = token;
            temp->length++;
        }

        /* 獲取下一個 token */
        token = strtok(NULL, " ");
    }

    return new_cmd;
}

/**
 * @brief 測試用函數，用於顯示解析後的命令結構內容
 * @param cmd 要顯示的命令結構體指標
 *
 * 此函數主要用於除錯目的，會詳細印出命令結構的所有資訊：
 * - 每個 pipe 階段的命令和參數
 * - 輸入重導向檔案路徑
 * - 輸出重導向檔案路徑
 * - 背景執行狀態
 */
void test_cmd_struct(struct cmd *cmd)
{
    struct pipes *temp = cmd->head;
    int pipe_count = 0;

    /* 遍歷所有 pipe 節點並顯示其內容 */
    while (temp != NULL) {
        printf("pipe %d: ", pipe_count);

        /* 顯示當前 pipe 的所有參數 */
        for (int i = 0; i < temp->length; ++i) {
            printf("%s ", temp->args[i]);
        }
        printf("\n");

        /* 移動到下一個 pipe 節點 */
        temp = temp->next;
        ++pipe_count;
    }

    /* 顯示 I/O 重導向資訊 */
    printf(" in: %s\n", cmd->in_file ? cmd->in_file : "none");
    printf("out: %s\n", cmd->out_file ? cmd->out_file : "none");

    /* 顯示背景執行狀態 */
    printf("background: %s\n", cmd->background ? "true" : "false");
}
