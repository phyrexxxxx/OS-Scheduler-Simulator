/**
 * @file function.c
 * @brief Task 函數實作檔
 *
 * 本檔案實作了各種用於 Scheduler Simulator 的 task 函數，
 * 包含基礎測試函數和各種計算密集型 task
 * 這些函數展示了不同的系統行為，如：
 * - Task 生命週期管理
 * - 資源分配與競爭
 * - CPU 密集型運算
 * - Memory 密集型操作
 * - Sleep 和 I/O 模擬
 */

#include "../include/function.h"
#include <stdio.h>
#include <stdlib.h>
#include "../include/resource.h"
#include "../include/task.h"

/**
 * @brief 簡單的結束測試 task
 *
 * 此函數立即調用 task_exit() 來結束 task 執行
 * 主要用於測試基本的 task 生命週期和 scheduler 對
 * task 終止的處理。
 *
 * 執行流程：
 * 1. 調用 task_exit() 通知 scheduler task 已完成
 * 2. 進入無窮迴圈（理論上不會執行到）
 */
void test_exit()
{
    task_exit(); /* 通知 scheduler 此 task 已完成 */
    while (1);   /* 防護性無窮迴圈，理論上不會執行到此處 */
}

/**
 * @brief Sleep 測試 task
 *
 * 此函數測試 task 的 sleep 機制，讓 task 暫時放棄 CPU
 * 並進入 waiting state，等待 scheduler 在指定時間後
 * 重新喚醒
 *
 * 執行流程：
 * 1. 調用 task_sleep(20) 進入 sleep 狀態 200ms
 * 2. 被 scheduler 喚醒後調用 task_exit()
 * 3. 進入防護性無窮迴圈
 */
void test_sleep()
{
    task_sleep(20); /* Sleep 200ms (20 * 10ms timer intervals) */
    task_exit();    /* Sleep 結束後正常結束 task */
    while (1);      /* 防護性無窮迴圈 */
}

/**
 * @brief 資源測試 task 1 - 多重資源分配測試
 *
 * 此函數測試系統的多重資源分配機制，包括：
 * - 同時請求多個資源
 * - 資源使用期間的 sleep
 * - 正確的資源釋放
 *
 * 執行流程：
 * 1. 請求資源 1, 3, 7（可能需要等待其他 task 釋放）
 * 2. 使用資源期間 sleep 50ms
 * 3. 釋放所有已分配的資源
 * 4. 正常結束 task
 */
void test_resource1()
{
    int resource_list[3] = {1, 3, 7};    /* 定義需要的資源清單 */
    get_resources(3, resource_list);     /* 請求資源（可能阻塞等待） */
    task_sleep(5);                       /* 模擬使用資源 50ms */
    release_resources(3, resource_list); /* 釋放所有資源 */
    task_exit();                         /* 正常結束 task */
    while (1);                           /* 防護性無窮迴圈 */
}

/**
 * @brief 資源測試 task 2 - 快速資源分配測試
 *
 * 此函數測試快速的資源請求和釋放，不包含 sleep，
 * 主要用於測試資源管理系統的效率和正確性。
 *
 * 執行流程：
 * 1. 請求資源 0, 3（可能需要等待）
 * 2. 立即釋放資源（不使用 sleep）
 * 3. 正常結束 task
 *
 * 此測試有助於發現資源管理中的 race condition 問題。
 */
void test_resource2()
{
    int resource_list[2] = {0, 3};       /* 定義需要的資源清單 */
    get_resources(2, resource_list);     /* 請求資源 */
    release_resources(2, resource_list); /* 立即釋放資源 */
    task_exit();                         /* 正常結束 task */
    while (1);                           /* 防護性無窮迴圈 */
}

/**
 * @brief 無窮迴圈 task（CPU 密集型）
 *
 * 此函數包含無窮迴圈，會持續消耗 CPU 資源直到被
 * scheduler 強制 preempt。主要用於測試：
 * - Preemptive scheduling 的行為
 * - Time slice 的正確實作
 * - CPU 使用率統計
 * - 系統在高 CPU 負載下的表現
 *
 * 注意：此 task 永遠不會自行結束，只能被 scheduler 強制終止
 */
void idle()
{
    while (1); /* 無窮迴圈，持續消耗 CPU 直到被 preempt */
}

/**
 * @brief 排序演算法 task（CPU 密集型）
 *
 * 此函數實作了 Selection Sort 演算法，
 * 用於測試 CPU 密集型 task 的排程行為
 *
 * 演算法特性：
 * - 時間複雜度：O(n^2)
 * - 空間複雜度：O(1)
 * - 使用 XOR swap 進行元素交換
 *
 * 執行流程：
 * 1. 分配 12000 個整數的陣列
 * 2. 使用隨機數填入陣列
 * 3. 執行選擇排序演算法
 * 4. 釋放記憶體並結束 task
 */
void task1()
{
    int len = 12000;                              /* 陣列大小 */
    int *arr = (int *) malloc(len * sizeof(int)); /* 分配陣列記憶體 */

    /* 使用隨機數填入陣列 */
    for (int i = 0; i < len; ++i) arr[i] = rand();

    /* 選擇排序演算法：對每個位置找到最小元素 */
    for (int i = 0; i < len; ++i) {
        for (int j = i; j < len; ++j) {
            if (arr[i] > arr[j]) {
                /* 使用 XOR swap 交換元素 */
                arr[i] ^= arr[j];
                arr[j] ^= arr[i];
                arr[i] ^= arr[j];
            }
        }
    }

    free(arr);   /* 釋放陣列記憶體 */
    task_exit(); /* 正常結束 task */
    while (1);   /* 防護性無窮迴圈 */
}

/**
 * @brief 矩陣運算 task（CPU + Memory 密集型）
 *
 * 此函數執行 512x512 矩陣的自乘運算，
 * 用於測試高 CPU 和記憶體負載的 task 排程
 *
 * 演算法特性：
 * - 時間複雜度：O(n^3)
 * - 空間複雜度：O(n^2)
 * - 大量 cache miss 可能導致效能下降
 *
 * 執行流程：
 * 1. 分配兩個 512x512 矩陣的記憶體
 * 2. 使用隨機數填入輸入矩陣
 * 3. 執行矩陣自乘運算（matrix * matrix）
 * 4. 釋放所有記憶體並結束 task
 */
void task2()
{
    int size = 512;                   /* 矩陣大小 */
    int *matrix[size], *result[size]; /* 宣告矩陣指標陣列 */

    /* 分配矩陣的每一列記憶體 */
    for (int i = 0; i < size; ++i) {
        matrix[i] = (int *) malloc(size * sizeof(int));
        result[i] = (int *) malloc(size * sizeof(int));
    }

    /* 使用 0-99 的隨機數填入輸入矩陣 */
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j) matrix[i][j] = rand() % 100;

    /* 執行矩陣乘法：result = matrix * matrix */
    for (int row = 0; row < size; ++row) {
        for (int col = 0; col < size; ++col) {
            result[row][col] = 0; /* 初始化結果元素 */
            /* 計算 dot product */
            for (int i = 0; i < size; ++i) {
                result[row][col] += matrix[row][i] * matrix[i][col];
            }
        }
    }

    /* 釋放所有分配的記憶體 */
    for (int i = 0; i < size; ++i) {
        free(matrix[i]);
        free(result[i]);
    }
    task_exit(); /* 正常結束 task */
    while (1);   /* 防護性無窮迴圈 */
}

/**
 * @brief 線性搜尋 task（Memory 密集型）
 *
 * 此函數在大型陣列中執行線性搜尋，
 * 主要測試記憶體密集型 task 的排程行為
 *
 * 演算法特性：
 * - 時間複雜度：O(n)
 * - 空間複雜度：O(n)
 * - 大量順序記憶體存取
 * - 可能觸發 page fault
 *
 * 執行流程：
 * 1. 分配 10,000,000 個整數的大型陣列
 * 2. 使用隨機數填入陣列
 * 3. 搜尋特定目標值 65409
 * 4. 釋放記憶體並結束 task
 */
void task3()
{
    int find_target = 65409; /* 搜尋目標 */

    int len = 10000000;                            /* 陣列大小：1000萬個元素 */
    int *list = (int *) malloc(len * sizeof(int)); /* 分配大型陣列 */

    /* 使用 0 到 find_target 的隨機數填入陣列 */
    for (int i = 0; i < len; ++i) list[i] = rand() % find_target;

    /* 線性搜尋目標值 */
    for (int i = 0; i < len; ++i)
        if (list[i] == find_target)
            break; /* 找到目標後結束搜尋 */

    free(list);  /* 釋放大型陣列記憶體 */
    task_exit(); /* 正常結束 task */
    while (1);   /* 防護性無窮迴圈 */
}

/**
 * @brief 資源密集型 task（使用資源 0, 1, 2）
 *
 * 此函數測試長時間的資源佔用對其他 task 的影響
 * 特別適用於測試資源競爭和 deadlock prevention
 *
 * 資源使用模式：
 * - 佔用資源：0, 1, 2
 * - 使用時間：700ms（系統中最長的單次資源使用）
 *
 * 執行流程：
 * 1. 請求資源 0, 1, 2（可能需要等待其他 task）
 * 2. 長時間使用資源（700ms）
 * 3. 釋放所有資源
 * 4. 正常結束 task
 */
void task4()
{
    int resource_list[3] = {0, 1, 2};    /* 定義需要的資源清單 */
    get_resources(3, resource_list);     /* 請求資源（可能被阻塞） */
    task_sleep(70);                      /* 使用資源 700ms */
    release_resources(3, resource_list); /* 釋放所有資源 */
    task_exit();                         /* 正常結束 task */
    while (1);                           /* 防護性無窮迴圈 */
}

/**
 * @brief 分階段資源使用 task
 *
 * 此函數展示了複雜的資源管理模式，包括：
 * - 動態資源請求（分次請求不同資源）
 * - 資源累積使用
 * - 一次性釋放所有資源
 *
 * 資源使用時間表：
 * - 階段 1：資源 1, 4 使用 200ms
 * - 階段 2：資源 1, 4, 5 使用 400ms
 * - 總計：600ms
 *
 * 執行流程：
 * 1. 請求資源 1, 4
 * 2. 使用資源 200ms
 * 3. 額外請求資源 5（現在擁有 1, 4, 5）
 * 4. 使用所有資源 400ms
 * 5. 一次性釋放所有資源
 * 6. 正常結束 task
 */
void task5()
{
    /* 第一階段：請求基礎資源 */
    int resource_list_1[2] = {1, 4};
    get_resources(2, resource_list_1); /* 請求資源 1, 4 */
    task_sleep(20);                    /* 使用資源 200ms */

    /* 第二階段：擴展資源使用 */
    int resource_list_2[1] = {5};
    get_resources(1, resource_list_2); /* 額外請求資源 5 */
    task_sleep(40);                    /* 使用所有資源 400ms */

    /* 結束階段：一次性釋放所有資源 */
    int resource_list_all[3] = {1, 4, 5};
    release_resources(3, resource_list_all); /* 釋放所有資源 */
    task_exit();                             /* 正常結束 task */
    while (1);                               /* 防護性無窮迴圈 */
}

/**
 * @brief 中等資源使用 task（使用資源 2, 4）
 *
 * 此函數代表中等長度的資源佔用，用於測試：
 * - 中等時間的資源競爭
 * - 與其他 task 的資源交互作用
 * - 資源等待佇列的行為
 *
 * 資源使用模式：
 * - 佔用資源：2, 4
 * - 使用時間：600ms（中等長度）
 *
 * 執行流程：
 * 1. 請求資源 2, 4
 * 2. 使用資源 600ms
 * 3. 釋放所有資源
 * 4. 正常結束 task
 */
void task6()
{
    int resource_list[2] = {2, 4};       /* 定義需要的資源清單 */
    get_resources(2, resource_list);     /* 請求資源 2, 4 */
    task_sleep(60);                      /* 使用資源 600ms */
    release_resources(2, resource_list); /* 釋放所有資源 */
    task_exit();                         /* 正常結束 task */
    while (1);                           /* 防護性無窮迴圈 */
}

/**
 * @brief 長時間資源使用 task（使用資源 1, 3, 6）
 *
 * 此函數代表最長時間的資源佔用，用於測試：
 * - 長時間資源佔用對系統效能的影響
 * - 資源飢餓（Resource Starvation）情境
 * - Task 等待時間的累積
 *
 * 資源使用模式：
 * - 佔用資源：1, 3, 6
 * - 使用時間：800ms（系統中最長的單次資源使用）
 *
 * 執行流程：
 * 1. 請求資源 1, 3, 6
 * 2. 長時間使用資源 800ms
 * 3. 釋放所有資源
 * 4. 正常結束 task
 */
void task7()
{
    int resource_list[3] = {1, 3, 6};    /* 定義需要的資源清單 */
    get_resources(3, resource_list);     /* 請求資源 1, 3, 6 */
    task_sleep(80);                      /* 使用資源 800ms */
    release_resources(3, resource_list); /* 釋放所有資源 */
    task_exit();                         /* 正常結束 task */
    while (1);                           /* 防護性無窮迴圈 */
}

/**
 * @brief 混合資源使用 task（使用資源 0, 4, 7）
 *
 * 此函數使用不同的資源組合，用於測試：
 * - 資源分配的競爭情況
 * - 不同資源組合的交互影響
 * - Deadlock detection 機制
 *
 * 資源使用模式：
 * - 佔用資源：0, 4, 7
 * - 使用時間：400ms（中等長度）
 *
 * 執行流程：
 * 1. 請求資源 0, 4, 7
 * 2. 使用資源 400ms
 * 3. 釋放所有資源
 * 4. 正常結束 task
 */
void task8()
{
    int resource_list[3] = {0, 4, 7};    /* 定義需要的資源清單 */
    get_resources(3, resource_list);     /* 請求資源 0, 4, 7 */
    task_sleep(40);                      /* 使用資源 400ms */
    release_resources(3, resource_list); /* 釋放所有資源 */
    task_exit();                         /* 正常結束 task */
    while (1);                           /* 防護性無窮迴圈 */
}

/**
 * @brief 複雜分階段資源使用 task
 *
 * 此函數展示了最複雜的資源管理模式，包括：
 * - 長時間的單一資源使用
 * - 後續的多重資源擴展
 * - 複雜的資源依賴關係
 * - 潛在的 deadlock 情境測試
 *
 * 資源使用時間表：
 * - 階段 1：資源 5 使用 800ms
 * - 階段 2：資源 4, 5, 6 使用 400ms
 * - 總計：1200ms（最長的 task）
 *
 * 執行流程：
 * 1. 請求資源 5
 * 2. 長時間使用資源 5（800ms）
 * 3. 額外請求資源 4, 6（現在擁有 4, 5, 6）
 * 4. 使用所有資源 400ms
 * 5. 一次性釋放所有資源
 * 6. 正常結束 task
 */
void task9()
{
    /* 第一階段：長時間單一資源使用 */
    int resource_list_1[1] = {5};
    get_resources(1, resource_list_1); /* 請求資源 5 */
    task_sleep(80);                    /* 長時間使用資源 5（800ms） */

    /* 第二階段：擴展資源使用 */
    int resource_list_2[2] = {4, 6};
    get_resources(2, resource_list_2); /* 額外請求資源 4, 6 */
    task_sleep(40);                    /* 使用所有資源 400ms */

    /* 結束階段：一次性釋放所有資源 */
    int resource_list_all[3] = {4, 5, 6};
    release_resources(3, resource_list_all); /* 釋放所有資源 */
    task_exit();                             /* 正常結束 task */
    while (1);                               /* 防護性無窮迴圈 */
}
