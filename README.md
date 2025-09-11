# OS Scheduler Simulator

這是一個用 C 語言實作的作業系統 Scheduler 模擬器，旨在模擬 user-level thread scheduling，使用 Linux signal 機制與 ucontext API (`#include <ucontext.h>`) 來實作三種排程演算法

## 專案概述
本專案實作了一個完整的 user-level thread scheduler，包含:

- **Task Management System**：使用 ucontext API 建立與管理 task
- **Scheduling Algorithms**：FCFS、Round Robin、Priority-based Preemptive
- **Resource Management System**：8 個系統資源 (ID: 0-7) 的分配與釋放
- **Timer 與 Signal 機制**：每 10ms 觸發 `SIGVTALRM` 進行排程決策
- **互動式 Shell 介面**：提供命令來建立、刪除、查看 task 狀態

## 系統架構
### Task State Diagram
- **READY**: task 準備執行，在 ready queue 中等待 CPU
- **RUNNING**: task 正在執行
- **WAITING**: task 等待資源或 sleep
- **TERMINATED**: task 已結束

### 核心元件
1. **Task Manager** (`task.h/.c`)
   - Task Control Block (TCB) 管理
   - Task 狀態轉換 (READY → RUNNING → WAITING → TERMINATED)
   - Context switching 使用 ucontext API
   - 時間統計 (running time, waiting time, turnaround time)

2. **Scheduler** (`scheduler.h/.c`)
   - FCFS (First Come First Serve)
   - RR (Round Robin, 時間片 = 30ms)
   - PP (Priority Preemptive, 數值越小優先權越高)
   - Timer-based scheduling with `SIGVTALRM`

3. **Resource Manager** (`resource.h/.c`)
   - 8 個系統資源的分配與釋放
   - Deadlock prevention 機制
   - Resource waiting queue 管理

4. **Shell Interface** (`builtin.c`)
   - `add`: 建立新 task 並設為 READY state
   - `del`: 將指定 task 設為 TERMINATED state 並刪除 task
   - `ps`: 顯示所有 task 資訊
   - `start`: 開始或恢復模擬

## 編譯與執行
### 編譯
```bash
make clean
make
```

### 執行
```bash
# 執行單一排程演算法
./scheduler_simulator FCFS    # First Come First Serve
./scheduler_simulator RR      # Round Robin
./scheduler_simulator PP      # Priority Preemptive

# 執行所有排程演算法比較
./scheduler_simulator all
```

### 使用方法
1. **啟動程式**後會進入互動式 shell 模式
2. **建立 task**：`add {task_name} {function_name} {priority}`
3. **查看 task**：`ps`
4. **開始模擬**：`start`
5. **暫停模擬**：按 `Ctrl+Z`
6. **刪除 task**：`del {task_name}`
7. **離開程式**：`exit`

### 可用的 Task 函數
- `test_exit`: 簡單的結束測試
- `test_sleep`: Sleep 測試 (sleep 200ms)
- `test_resource1`: 資源測試 1 (使用資源 1, 3, 7)
- `test_resource2`: 資源測試 2 (使用資源 0, 3)
- `idle`: 無窮迴圈 (CPU 密集)
- `task1-task9`: 不同的計算密集型 task

## 使用範例
```bash
./scheduler_simulator FCFS

>>> $ add T1 test_exit 1
Task T1 is ready.

>>> $ add T2 test_sleep 2  
Task T2 is ready.

>>> $ ps
 TID|       name|      state| running| waiting| turnaround| resources| priority
--------------------------------------------------------------------------------
   1|         T1|      READY|       0|       0|       none|      none|        1
   2|         T2|      READY|       0|       0|       none|      none|        2

>>> $ start
Start simulation.
Task T1 is running.
Task T1 has terminated.
Task T2 is running.
Task T2 goes to sleep.
CPU idle.
Task T2 is running.
Task T2 has terminated.
Simulation over.

>>> $ exit
```

T1 執行後就結束的原因：

T1 執行的是 `test_exit()` 函數:
```c
void test_exit()
{
   task_exit();     /* 通知 scheduler 此 task 已完成 */
   while (1);       /* 防護性無窮迴圈，理論上不會執行到此處 */
}
```
這個函數的設計就是立即調用 `task_exit()`，所以 T1 一開始執行就會立即結束。

T2 從 Sleep 狀態變回 Running 的原因：

T2 執行的是 `test_sleep()` 函數：
```c
void test_sleep()
{
   task_sleep(20);  /* Sleep 200ms (20 * 10ms timer intervals) */
   task_exit();     /* Sleep 結束後正常結束 task  */
   while (1);       /* 防護性無窮迴圈 */
}
```
執行流程是：
1. T2 調用 `task_sleep(20)` 進入 WAITING 狀態，sleep 200ms
2. 在 `signal_handler()` 中（`task.c:313-378`），每 10ms 會檢查所有 task 的狀態
3. 當 sleep 時間到了，會將 T2 的狀態從 WAITING 改回 READY（`task.c:327-330`）
4. Scheduler 在主迴圈中發現 T2 變成 READY，就會讓它繼續執行
5. T2 從 `task_sleep()` 返回後，執行 `task_exit()` 結束

這是正常的 sleep 機制：task 進入 sleep → 等待指定時間 → 被 scheduler 喚醒 → 繼續執行剩餘代碼。

## 測試
### 自動測試
測試腳本:

```bash
# 測試 shell 功能
python3 test/judge_shell.py
```
預期輸出:
```
The shell works properly.
```

```bash
# 自動執行模擬器
python3 test/auto_run.py FCFS test/general.txt
python3 test/auto_run.py RR test/test_case1.txt
python3 test/auto_run.py PP test/test_case2.txt
python3 test/auto_run.py all test/general.txt
```

### 測試檔案
- `test/general.txt`: 基本功能測試（不包括暫停功能）
- `test/test_case1.txt`: 複雜測試案例 1
- `test/test_case2.txt`: 複雜測試案例 2

## 實作特色
### 技術特點
1. **完整的 Context Switching**
   - 使用 POSIX ucontext API
   - 每個 task 有獨立的 128KB stack
   - 支援 task 間的無縫切換

2. **精確的時間管理**
   - 10ms 精度的 timer
   - 準確的 waiting time 統計（只計算在 ready queue 的時間）
   - Turnaround time 統計

3. **資源管理機制**
   - Atomic 資源分配/釋放
   - 支援多重資源請求
   - 資源等待隊列管理

4. **強健的排程演算法**
   - FCFS: 依照到達順序排程
   - RR: 30ms 時間片輪轉
   - PP: 支援 preemption 的優先權排程

### Signal Handling

- **SIGVTALRM**: 每 10ms 觸發排程決策
- **SIGTSTP (Ctrl+Z)**: 暫停模擬並回到 shell 模式
- Async-signal-safe 的 signal handler 設計

## 已知限制
1. **Context Switching 複雜性**: ucontext API 的使用較為複雜，在某些情況下可能需要額外的除錯
2. **Signal Handling**: Linux signal 機制的複雜性可能導致時序問題
3. **Memory Management**: 需要謹慎處理 task stack 的記憶體管理

## 檔案結構

```
.
├── include/              # 標頭檔
│   ├── task.h           # Task 管理系統
│   ├── scheduler.h      # Scheduler 核心
│   ├── resource.h       # 資源管理系統
│   ├── builtin.h        # Shell 內建命令
│   ├── command.h        # 命令解析
│   ├── shell.h          # Shell 介面
│   └── function.h       # Task 函數定義
├── src/                 # 原始碼
│   ├── task.c          # Task 管理實作
│   ├── scheduler.c     # Scheduler 實作
│   ├── resource.c      # 資源管理實作
│   ├── builtin.c       # Shell 命令實作
│   ├── command.c       # 命令解析實作
│   ├── shell.c         # Shell 介面實作
│   └── function.c      # Task 函數實作（不可修改）
├── test/               # 測試檔案
│   ├── auto_run.py     # 自動執行腳本
│   ├── judge_shell.py  # Shell 測試腳本
│   ├── general.txt     # 基本測試案例
│   ├── test_case1.txt  # 測試案例 1
│   └── test_case2.txt  # 測試案例 2
├── main.c              # 主程式進入點
├── makefile            # 編譯設定
└── README.md           # 本檔案
```

## 開發技術
- **程式語言**: C (C99 標準)
- **編譯器**: GCC
- **系統呼叫**: ucontext, signal, timer
- **平台**: Linux (POSIX)
