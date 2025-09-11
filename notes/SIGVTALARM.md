# 什麼是 `SIGVTALRM`?

`SIGVTALRM` 是 Linux/Unix 系統中的一個 Signal，就像是系統的「鬧鐘」功能

## Library
```c
#include <signal.h>    // Signal 相關函數
#include <sys/time.h>  // Timer 相關函數
```

🔍 名稱解析

- SIG = Signal（信號）
- V = Virtual（虛擬）
- T = Time（時間）
- ALRM = Alarm（鬧鐘）

所以 SIGVTALRM = 虛擬時間鬧鐘信號

💡 簡單比喻

想像你在做功課時設定手機鬧鐘：
- 普通鬧鐘 (`SIGALRM`)：不管你有沒有在讀書，時間到就響
- 虛擬鬧鐘 (`SIGVTALRM`)：只有在你「真正讀書」的時間才計時，睡覺時不算

## Example
### 在程式中的意思
```c
/*
   * @brief 設定虛擬時間定時器，每 10ms 觸發一次 SIGVTALRM signal
   * 
   * 此程式碼設定了一個 virtual timer，用於實現 preemptive scheduling。
   * Virtual timer 只在「process 實際執行時」計時，不包括 sleep 或等待 I/O 時間。
   */

  // 設定每 10ms 觸發一次 SIGVTALRM
  struct itimerval timer;                    /* interval timer 結構體，用於設定定時器參數 */

  /* 設定初始觸發時間 */
  timer.it_value.tv_sec = 0;                /* 初始延遲：秒數部分設為 0 */
  timer.it_value.tv_usec = 10000;           /* 初始延遲：10ms = 10000 us */

  /* 設定重複觸發間隔 */
  timer.it_interval.tv_sec = 0;             /* 重複間隔：秒數部分設為 0 */
  timer.it_interval.tv_usec = 10000;        /* 重複間隔：每次間隔 10ms */

  /* 啟動 virtual timer，開始定期發送 SIGVTALRM signal */
  setitimer(ITIMER_VIRTUAL, &timer, NULL);  /* 設定 virtual timer，NULL 表示不需要取得舊設定 */
```

### 「初始觸發時間」的意思
🔔 鬧鐘的比喻

想像你要設定一個重複響鈴的鬧鐘：

1. 初始觸發時間 (`it_value`)：
- 「現在開始，等多久後第一次響鈴？」
- 就像設定「10 分鐘後開始響」
2. 重複間隔時間 (`it_interval`)：
- 「第一次響了之後，每隔多久再響一次？」
- 就像設定「之後每 5 分鐘響一次」

📊 時間軸圖解
```
現在    10ms後     20ms後     30ms後     40ms後
|       |         |         |         |
|   第一次響    第二次響    第三次響    第四次響
|   (初始)     (重複)     (重複)     (重複)
|       ↑         ↑         ↑         ↑
啟動   it_value  it_interval it_interval it_interval
timer
```
💻 在程式碼中
```c
timer.it_value.tv_usec = 10000;    // 10ms 後「第一次」觸發 SIGVTALRM
timer.it_interval.tv_usec = 10000; // 之後「每隔 10ms」觸發一次
```
🤔 為什麼需要分開設定？

情況 1：想要立即開始
```c
timer.it_value.tv_usec = 0;        // 立即觸發第一次
timer.it_interval.tv_usec = 10000; // 之後每 10ms 一次
```
情況 2：想要延遲開始
```c
timer.it_value.tv_usec = 50000;    // 等 50ms 後才開始第一次
timer.it_interval.tv_usec = 10000; // 之後每 10ms 一次
```
情況 3：只響一次（不重複）
```c
timer.it_value.tv_usec = 10000;    // 10ms 後響一次
timer.it_interval.tv_usec = 0;     // 之後不再重複
```

🎯 在這個專案中
```c
timer.it_value.tv_usec = 10000;    // 程式啟動後 10ms 開始第一次排程
timer.it_interval.tv_usec = 10000; // 之後每 10ms 進行一次排程檢查
```
所以「初始觸發時間」就是「第一次鬧鐘響鈴的延遲時間」！

## SIGVTALRM 在這個專案中的作用
1. 定時觸發排程：每 10ms 叫醒 scheduler
2. 檢查是否需要切換 task：
    - Round Robin：檢查時間片 (time slice) 是否用完
    - Priority Preemptive：檢查是否有更高優先權的 task
3. 只計算「實際執行時間」：程式暫停時不會觸發

🔄 實際執行流程
```
程式執行 → 10ms → SIGVTALRM 觸發 → Signal Handler 執行
        ↑                                    ↓
    繼續執行 ← 排程決策（可能切換 task）← 檢查排程條件
```

## SIGVTALRM 與其他 Timer 的差異
| Timer 類型  | 計時方式       | 使用場景       |
|-----------|------------|------------|
| `SIGALRM`   | 真實時間（牆上時鐘） | 絕對時間控制     |
| `SIGVTALRM` | 程式執行時間     | CPU 使用時間控制 |
| `SIGPROF`   | 程式+系統時間    | 效能分析       |

## 為什麼選擇 `SIGVTALRM`？
在 OS Scheduler 中，我們關心的是：
- ✅ 程式實際使用 CPU 的時間
- ❌ 不是程式等待 I/O 或 sleep 的時間

所以用 `SIGVTALRM` 最適合，因為它只在程式真正執行時才計時！

這就是為什麼這個模擬器能夠精確控制每個 task 的 CPU 使用時間，實現真正的 time-sharing 效果