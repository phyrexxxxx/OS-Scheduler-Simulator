/**
 * @file resource.c
 * @brief 系統資源管理模組的實作檔
 *
 * 本檔案實作了 OS Scheduler 模擬器中的資源管理系統
 * 提供了完整的資源分配、釋放和等待機制，
 * 確保系統的穩定性和正確性。
 *
 * 核心功能：
 * - 原子性多重資源分配
 * - 資源佔用狀態追蹤
 * - Task 等待與喚醒機制
 * - Context switching 整合
 */

#include "../include/resource.h"
#include <stdbool.h>
#include <stdio.h>
#include "../include/task.h"

/**
 * @brief 全域資源狀態陣列
 *
 * 此陣列追蹤系統中所有 8 個資源的佔用狀態
 * - true: 資源已被某個 task 佔用
 * - false: 資源可用
 *
 * 初始化為全部可用狀態（false）
 */
bool all_resource[RESOURCE_SIZE] = {false};

/**
 * @brief 請求系統資源
 * @param count 要請求的資源數量
 * @param resources 指向資源 ID 陣列的指標
 *
 * 此函數實作了原子性的多重資源分配機制
 * 使用 "all-or-nothing" 策略來避免部分分配導致的死鎖
 *
 * 實作細節：
 * 1. 參數驗證：確保 count 在合理範圍內
 * 2. Context 保存：使用 getcontext 保存當前執行狀態
 * 3. 可用性檢查：檢查所有資源是否都可用
 * 4. 原子性分配：全部成功或全部失敗
 * 5. 等待處理：若失敗則進入 WAITING 狀態
 */
void get_resources(int count, int *resources)
{
    int i;

    /* 參數驗證：確保請求的資源數量在合理範圍 */
    if (count < 0 || count >= RESOURCE_SIZE) {
        return; /* 無效參數，直接返回 */
    }

    /**
     * 保存當前 task 的 context
     * 若資源不可用，後續會透過 setcontext 跳轉回這裡，
     * 在資源釋放後重新嘗試分配。
     */
    getcontext(&(get_current_task()->context));

    /* 第一階段：檢查所有要求的資源是否都可用 */
    bool available = true;
    for (i = 0; i < count; i++) {
        if (all_resource[resources[i]] == true) {
            available = false; /* 發現至少一個資源被佔用 */
            break;
        }
    }

    if (available) {
        /* 所有資源都可用：執行原子性分配 */
        for (i = 0; i < count; i++) {
            all_resource[resources[i]] = true;                 /* 標記全域資源為佔用 */
            get_current_task()->resource[resources[i]] = true; /* 記錄 task 持有此資源 */
            printf("Task %s gets resource %d\n", get_current_task()->task_name, resources[i]);
        }
    } else {
        /* 有資源不可用：task 進入等待狀態 */
        printf("Task %s is waiting resource.\n", get_current_task()->task_name);
        get_current_task()->state = WAITING; /* 設定 task 狀態為 WAITING */

        /**
         * 跳轉到 scheduler 的主迴圈 context
         * 這會讓 scheduler 選擇下一個可執行的 task，
         * 當前 task 會等待直到資源釋放
         */
        setcontext(get_current_context());
    }
}

/**
 * @brief 釋放系統資源
 * @param count 要釋放的資源數量
 * @param resources 指向資源 ID 陣列的指標
 *
 * 此函數釋放當前 task 持有的指定資源，
 * 並更新系統資源狀態。釋放後，等待這些資源的
 * 其他 task 可能會被 scheduler 喚醒並重新調度。
 *
 * 實作細節：
 * 1. 參數驗證：確保 count 在合理範圍
 * 2. 資源狀態更新：同時更新全域和 task 局部狀態
 * 3. 日誌輸出：為除錯提供資訊
 *
 * 重要特性：
 * - 立即生效：釋放後其他 task 可立即獲得資源
 * - 線程安全：此函數本身不處理 context switching
 * - 配合 scheduler：釋放後由 scheduler 負責重新調度
 */
void release_resources(int count, int *resources)
{
    int i;

    /* 參數驗證：確保要釋放的資源數量在合理範圍 */
    if (count < 0 || count >= RESOURCE_SIZE) {
        return; /* 無效參數，直接返回 */
    }

    /* 釋放所有指定的資源 */
    for (i = 0; i < count; i++) {
        all_resource[resources[i]] = false;                 /* 標記全域資源為可用 */
        get_current_task()->resource[resources[i]] = false; /* 清除 task 的資源持有記錄 */

        /* 輸出釋放資訊（用於除錯和監控） */
        printf("Task %s releases resource %d\n", get_current_task()->task_name, resources[i]);
    }

    /**
     * 注意：此函數不直接處理 task 的喚醒，
     * 由 scheduler 在下一次調度時檢查 WAITING task 並喚醒它們
     * 這種設計避免了複雜的同步問題。
     */
}
