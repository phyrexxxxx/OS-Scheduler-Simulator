/**
 * @file resource.h
 * @brief 系統資源管理模組的標頭檔
 *
 * 本檔案定義了 OS Scheduler 模擬器中資源管理的核心介面
 * 系統總共提供 8 個資源（ID: 0-7），支援：
 * - 原子性的多重資源分配
 * - 資源等待機制
 * - 死鎖預防（Deadlock Prevention）
 * - 資源釋放與喚醒機制
 *
 * 資源管理特性：
 * - 每個資源只能同時被一個 task 持有
 * - 支援 task 同時請求多個資源
 * - 若資源不可用，task 會進入 WAITING State
 * - 當資源釋放時，等待的 task 會被重新調度
 */

#ifndef RESOURCE_H
#define RESOURCE_H

/**
 * @brief 請求系統資源
 * @param count 要請求的資源數量
 * @param resources 指向資源 ID 陣列的指標
 *
 * 此函數會嘗試原子性地分配指定的資源給當前 task
 *
 * 行為描述：
 * - 檢查所有指定的資源是否都可用
 * - 如果全部可用：分配所有資源給當前 task
 * - 如果任一資源被佔用：task 進入 WAITING State，等待資源釋放
 * - 使用 getcontext/setcontext 機制進行 context switching
 *
 * 重要特性：
 * - 原子性操作：要麼全部分配成功，要麼全部失敗
 * - 避免部分分配導致的死鎖問題
 * - 自動處理 task 狀態轉換
 *
 * 使用範例：
 * ```c
 * int resources[] = {1, 3, 5};
 * get_resources(3, resources);  // 請求資源 1, 3, 5
 * ```
 */
void get_resources(int, int *);

/**
 * @brief 釋放系統資源
 * @param count 要釋放的資源數量
 * @param resources 指向資源 ID 陣列的指標
 *
 * 此函數會釋放當前 task 持有的指定資源，並更新系統資源狀態
 *
 * 行為描述：
 * - 將指定的資源標記為可用
 * - 清除 task 的資源持有記錄
 * - 可能喚醒等待這些資源的其他 task
 *
 * 重要特性：
 * - 只能釋放當前 task 實際持有的資源
 * - 釋放後立即生效，其他 task 可以獲得資源
 * - 配合 scheduler 進行 task 重新調度
 *
 * 使用範例：
 * ```c
 * int resources[] = {1, 3, 5};
 * release_resources(3, resources);  // 釋放資源 1, 3, 5
 * ```
 *
 * 注意事項：
 * - 應確保 count 和 resources 陣列的對應關係正確
 * - 釋放不持有的資源可能導致未定義行為
 */
void release_resources(int, int *);

#endif
