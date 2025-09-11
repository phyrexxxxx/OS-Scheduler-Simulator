/**
 * @file command.h
 * @brief Shell 命令解析與處理模組的標頭檔
 *
 * 本檔案定義了 Shell 命令解析所需的資料結構和函數宣告
 * 支援 pipe、重導向、背景執行等 shell 功能，並提供命令歷史記錄功能
 */

#ifndef COMMAND_H
#define COMMAND_H

/* 命令歷史記錄的最大數量 */
#define MAX_RECORD_NUM 16
/* 輸入緩衝區大小 */
#define BUF_SIZE 1024

#include <stdbool.h>

/**
 * @struct pipes
 * @brief Pipe 串列結構，用於儲存透過 pipe 連接的命令序列
 *
 * 每個 pipes 節點代表一個命令及其參數，透過 next 指標形成 linked list，
 * 支援多重 pipe 操作（如 cmd1 | cmd2 | cmd3）。
 */
struct pipes {
    char **args;        /* 指向參數字串陣列的指標 */
    int length;         /* 參數的數量 */
    struct pipes *next; /* 指向下一個 pipe 節點的指標 */
};

/**
 * @struct cmd
 * @brief 完整的命令結構，包含所有 shell 功能的相關資訊
 *
 * 此結構體封裝了一個完整的 shell 命令，包括：
 * - Pipe 命令串列
 * - 背景執行標記
 * - I/O 重導向檔案路徑
 */
struct cmd {
    struct pipes *head; /* 指向 pipe 串列的第一個節點 */
    bool background;    /* 是否為背景執行命令（'&' 符號） */
    char *in_file;      /* 輸入重導向檔案路徑（'<' 符號） */
    char *out_file;     /* 輸出重導向檔案路徑（'>' 符號） */
};

/* 命令歷史記錄陣列，儲存最近執行的命令 */
char *history[MAX_RECORD_NUM];
/* 歷史記錄計數器，記錄總共執行過的命令數量 */
int history_count;

/**
 * @brief 從標準輸入讀取一行命令
 * @return 指向包含使用者輸入命令的字串指標，失敗時返回 NULL
 *
 * 此函數會：
 * - 分配記憶體來儲存輸入
 * - 處理空白輸入
 * - 支援 replay 命令（重複執行歷史命令）
 * - 自動將有效命令加入歷史記錄
 */
char *read_line();

/**
 * @brief 將輸入的命令字串解析為結構化的命令物件
 * @param line 要解析的命令字串
 * @return 指向解析後的 cmd 結構體指標
 *
 * 此函數會解析：
 * - 命令和參數
 * - Pipe 符號（'|'）
 * - 輸入重導向（'<'）
 * - 輸出重導向（'>'）
 * - 背景執行符號（'&'）
 */
struct cmd *split_line(char *);

/**
 * @brief 測試用函數，用於顯示解析後的命令結構內容
 * @param cmd 要顯示的命令結構體指標
 *
 * 此函數會印出命令結構的詳細資訊，包括：
 * - 每個 pipe 的命令和參數
 * - 輸入/輸出重導向檔案
 * - 背景執行狀態
 */
void test_cmd_struct(struct cmd *);

#endif
