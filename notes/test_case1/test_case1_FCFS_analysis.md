## 測試案例總覽
這個測試使用了 26 個 task，執行不同的函數：
- task1: 排序演算法（CPU 密集型）
- task2: 矩陣運算（CPU + Memory 密集型）
- task3: 線性搜尋（Memory 密集型）
- task4-8: 資源管理測試（需要不同資源組合）

## 詳細執行分析
### 第一階段：前 21 個 Task（aaa 到 uuu）順利執行
Task aaa-rrr (18個 task3)：
- 這些都執行 task3（線性搜尋），是 Memory 密集型任務
- FCFS 特點：嚴格按照加入順序執行
- 每個 task 的 running time 約 5-7 個單位（50-70ms）
- waiting time 遞增：因為要等前面的 task 完成
  - ⭐ waiting time: 只計算在 ready queue 的時間
  - aaa: waiting = 0（第一個，不用等）
  - bbb: waiting = 5（等 aaa 執行完）
  - ccc: waiting = 10（等 aaa + bbb）
  - 以此類推...

Task sss-ttt (2個 task1)：
- 執行排序演算法，CPU 密集型
- running time 較長：約 21-22 個單位，如下表所示:
  ```
    19|        sss| TERMINATED|      22|     103|        125|      none|       23
    20|        ttt| TERMINATED|      21|     125|        146|      none|       25
  ```

- waiting time 遞增：因為要等前面的 task 完成

Task uuu (1個 task2)：
- 矩陣運算，最耗時
- running = 42 個單位（420ms），如下表所示:
  ```
    21|        uuu| TERMINATED|      42|     146|        188|      none|       26
  ```

### 第二階段：資源競爭與死鎖情況
```c
// src/function.c
void task4()
{
    int resource_list[3] = {0, 1, 2};           /* 定義需要的資源清單 */
    get_resources(3, resource_list);            /* 請求資源（可能被阻塞） */
    task_sleep(70);                             /* 使用資源 700ms */
    release_resources(3, resource_list);        /* 釋放所有資源 */
    task_exit();                                /* 正常結束 task */
    while (1);                                  /* 防護性無窮迴圈 */
}
```
關鍵轉折點 - Task vvv (task4)：
```
Task vvv gets resource 0
Task vvv gets resource 1
Task vvv gets resource 2
Task vvv goes to sleep.  // sleep 700ms
```
vvv 佔用了資源 0, 1, 2 並 sleep 70 個單位時間

連鎖反應：
1. www (task5) 需要資源 1, 4 → 資源 1 被 vvv 佔用 → 等待
2. xxx (task6) 需要資源 2, 4 → 資源 2 被 vvv 佔用 → 等待
3. yyy (task7) 需要資源 1, 3, 6 → 資源 1 被 vvv 佔用 → 等待
4. zzz (task8) 需要資源 0, 4, 7 → 資源 0 被 vvv 佔用 → 等待

CPU idle 循環：
- 因為所有 READY 的 task 都在等資源
- 系統進入 idle 狀態，持續 70 個單位時間
- 每次檢查：www → xxx → yyy → zzz → 都在等待 → CPU idle
- 也就是說，在 70 個單位時間內，CPU 都是 idle 的，並且一直輸出以下訊息:
  ```
  Task www is running.
  Task www is waiting resource.
  Task xxx is running.
  Task xxx is waiting resource.
  Task yyy is running.
  Task yyy is waiting resource.
  Task zzz is running.
  Task zzz is waiting resource.
  CPU idle.
  ```

### 第三階段：資源釋放與連鎖執行
vvv 醒來並結束：
```
Task vvv is running.
Task vvv releases resource 0
Task vvv releases resource 1
Task vvv releases resource 2
Task vvv has terminated.
```
連鎖執行順序：
1. www 立即獲得資源 1, 4 → sleep 200ms → 獲得資源 5 → sleep 400ms → 結束
2. xxx 等 www 釋放資源 4 後，獲得資源 2, 4 → sleep 600ms → 結束
3. yyy 在 www 釋放資源 1 後，獲得資源 1, 3, 6 → sleep 800ms → 結束
4. zzz 最後獲得資源 0, 4, 7 → sleep 400ms → 結束

## 最終 ps 結果分析
關鍵指標解釋：

Running Time（執行時間）：
- 前 18 個 task (18個 task3)：5-7 單位（實際 CPU 執行時間）
- sss, ttt (2個 task1)：21-22 單位（排序演算法較耗時）
- uuu (1個 task2)：42 單位（矩陣運算最耗時）
- vvv-zzz (5個 task5-8)：0 單位！為什麼？因為它們只是請求資源後立即 sleep，沒有實際計算

Waiting Time（等待時間）：
- 遞增模式：每個 task 要等前面所有 task 完成
- vvv-zzz 都是 188：它們同時在等待（都被 vvv 的資源佔用阻塞）

Turnaround Time（周轉時間）：
- = 從 task 建立到結束的總時間
- = running + waiting + sleep 時間
- 最長的是 zzz：418 單位（等了很久才拿到資源）

## FCFS 特性總結
1. 無搶佔：一旦 task 開始執行，會執行到完成或主動放棄 CPU
2. 公平但低效：嚴格按順序，但可能導致短任務等待長任務
3. 資源飢餓：後面的 task 可能等很久才能獲得資源
4. Convoy Effect：長任務（如 uuu）會讓後面所有 task 等待

這個測試展示了 FCFS 的優缺點，特別是在資源競爭情況下的表現