#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/command.h"
#include "include/shell.h"
#include "include/task.h"

/*
 * Main Program Entry Point
 *
 * 功能：
 * 1. 解析命令列參數，決定使用哪種 scheduling algorithm
 * 2. 初始化 shell 的歷史記錄緩衝區
 * 3. 設定選定的排程演算法
 * 4. 啟動互動式 shell
 * 5. 清理記憶體並結束程式
 */
int main(int argc, char *argv[])
{
    /* 初始化 shell 歷史記錄計數器 */
    history_count = 0;

    /* Allocate memory for command history */
    for (int i = 0; i < MAX_RECORD_NUM; ++i) {
        history[i] = (char *) malloc(BUF_SIZE * sizeof(char));
    }

    /* 檢查命令列參數數量是否正確 */
    if (argc < 2) {
        printf("Usage: %s {algorithm}\n", argv[0]);
        printf("  Valid algorithm: FCFS / RR / PP\n");
        return 0;
    }

    /* Set scheduling algorithm based on user input */
    if (strcmp(argv[1], "FCFS") == 0) {
        set_algorithm(FCFS);
    } else if (strcmp(argv[1], "RR") == 0) {
        set_algorithm(RR);
    } else if (strcmp(argv[1], "PP") == 0) {
        set_algorithm(PP);
    } else {
        /* Invalid algorithm parameter, display usage instructions */
        printf("Usage: %s {algorithm}\n", argv[0]);
        printf("  Valid algorithm: FCFS / RR / PP\n");
        return 0;
    }

    /* 啟動互動式 shell，進入主要的命令處理迴圈 */
    shell();

    /* Free allocated memory for history */
    for (int i = 0; i < MAX_RECORD_NUM; ++i) {
        free(history[i]);
    }

    return 0;
}
