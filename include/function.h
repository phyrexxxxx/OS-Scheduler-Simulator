/**
 * @file function.h
 * @brief Task 函數定義標頭檔
 *
 * 本檔案定義了可用於 Scheduler Simulator 的各種 task 函數，
 * 這些函數用於測試不同的排程情境和系統行為
 * 包含基礎測試函數和各種計算密集型 task
 */

#ifndef FUNCTION_H
#define FUNCTION_H

/* === 基礎測試函數 === */

/**
 * @brief 簡單的結束測試 task
 *
 * 此函數會立即調用 task_exit() 結束 task
 * 主要用於測試 task 的基本生命週期
 */
void test_exit();

/**
 * @brief Sleep 測試 task
 *
 * 此函數會讓 task 進入 sleep 狀態 200ms
 * 用於測試 scheduler 對於 waiting state 的處理
 */
void test_sleep();

/**
 * @brief 資源測試 task 1
 *
 * 此函數會：
 * 1. 請求資源 1, 3, 7
 * 2. Sleep 50ms（模擬使用資源）
 * 3. 釋放所有資源
 * 4. 結束 task
 *
 * 用於測試多重資源分配和 deadlock prevention
 */
void test_resource1();

/**
 * @brief 資源測試 task 2
 *
 * 此函數會：
 * 1. 請求資源 0, 3
 * 2. 立即釋放資源
 * 3. 結束 task
 *
 * 用於測試快速資源分配與釋放的情境
 */
void test_resource2();

/**
 * @brief 無窮迴圈 task（CPU 密集型）
 *
 * 此函數包含無窮迴圈，會持續消耗 CPU 資源，
 * 主要用於測試 preemptive scheduling 的行為
 */
void idle();

/* === 計算密集型 Task 函數 === */

/**
 * @brief 排序演算法 task
 *
 * 此函數會：
 * 1. 分配 12000 個整數的陣列
 * 2. 填入隨機數值
 * 3. 使用 Selection Sort 進行排序
 * 4. 釋放記憶體並結束
 *
 * 時間複雜度：O(n^2)，適合測試 CPU 密集型 task 的排程
 */
void task1();

/**
 * @brief 矩陣運算 task
 *
 * 此函數會：
 * 1. 分配 512x512 的矩陣
 * 2. 填入隨機數值
 * 3. 執行矩陣乘法運算
 * 4. 釋放記憶體並結束
 *
 * 時間複雜度：O(n^3)，適合測試高 CPU 負載的排程行為
 */
void task2();

/**
 * @brief 線性搜尋 task
 *
 * 此函數會：
 * 1. 分配 10,000,000 個整數的陣列
 * 2. 填入隨機數值
 * 3. 搜尋特定目標值
 * 4. 釋放記憶體並結束
 *
 * 時間複雜度：O(n)，適合測試記憶體密集型 task 的排程
 */
void task3();

/**
 * @brief 資源密集型 task（使用資源 0, 1, 2）
 *
 * 此函數會：
 * 1. 請求資源 0, 1, 2
 * 2. Sleep 700ms（長時間使用資源）
 * 3. 釋放所有資源
 * 4. 結束 task
 *
 * 用於測試長時間資源佔用對其他 task 的影響
 */
void task4();

/**
 * @brief 分階段資源使用 task
 *
 * 此函數會：
 * 1. 請求資源 1, 4，使用 200ms
 * 2. 額外請求資源 5，使用 400ms
 * 3. 釋放所有資源（1, 4, 5）
 * 4. 結束 task
 *
 * 用於測試動態資源請求和複雜的資源依賴關係
 */
void task5();

/**
 * @brief 中等資源使用 task（使用資源 2, 4）
 *
 * 此函數會：
 * 1. 請求資源 2, 4
 * 2. Sleep 600ms（中等時間使用資源）
 * 3. 釋放所有資源
 * 4. 結束 task
 *
 * 用於測試中等長度的資源佔用情境
 */
void task6();

/**
 * @brief 長時間資源使用 task（使用資源 1, 3, 6）
 *
 * 此函數會：
 * 1. 請求資源 1, 3, 6
 * 2. Sleep 800ms（長時間使用資源）
 * 3. 釋放所有資源
 * 4. 結束 task
 *
 * 用於測試最長時間的資源佔用對系統的影響
 */
void task7();

/**
 * @brief 混合資源使用 task（使用資源 0, 4, 7）
 *
 * 此函數會：
 * 1. 請求資源 0, 4, 7
 * 2. Sleep 400ms（中等時間使用資源）
 * 3. 釋放所有資源
 * 4. 結束 task
 *
 * 用於測試不同資源組合的分配競爭。
 */
void task8();

/**
 * @brief 複雜分階段資源使用 task
 *
 * 此函數會：
 * 1. 請求資源 5，使用 800ms
 * 2. 額外請求資源 4, 6，使用 400ms
 * 3. 釋放所有資源（4, 5, 6）
 * 4. 結束 task
 *
 * 用於測試最複雜的資源請求模式和潛在的 deadlock 情境
 */
void task9();

#endif
