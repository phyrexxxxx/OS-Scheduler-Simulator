## 實作規劃
### 第一階段：基礎架構建立
1. **Task Management System**
   - 使用 ucontext API 建立 task context
   - 設計 task 資料結構，包含 TID、name、state、priority、時間統計等
   - 實作 task state transition 機制
   - 實作 `task_sleep()` 和 `task_exit()` API

2. **Queue System**
   - Ready Queue: 存放可執行的 task
   - Waiting Queue: 存放等待資源或 sleep 的 task
   - 根據排程演算法選擇適當的資料結構（FCFS/RR 用 queue，Priority 用 priority queue）

### 第二階段：排程器實作
1. **Context Switching**
   - task 切換:
      - 使用 `setcontext()` 和 `getcontext()` 組合進行 task 切換
      - 或是使用 `swapcontext()` 進行 task 切換
   - 保存並恢復 task 執行狀態

2. **排程演算法**
   - **FCFS**: 先到先服務，simple queue 實作
   - **RR**: 時間片輪轉（30ms，3個 10ms tick），需要額外追蹤 time slice
   - **Priority Preemptive**: 優先權搶占式，需要在每次 timer interrupt 檢查是否需要 preempt

### 第三階段：資源管理
1. **Resource System**
   - 使用 boolean array 追蹤 8 個資源的可用性
   - 實作 `get_resource()` 和 `release_resource()` API
   - 處理資源競爭和 deadlock prevention

2. **Resource Allocation Logic**
   - 檢查所有需求資源是否可用
   - Atomic 的資源分配/釋放操作
      - Atomic 分配，也就是實作 "all-or-nothing" 策略
   - 等待機制：task 進入 WAITING state 直到資源可用
      - 當資源可用時喚醒正在 WAITING state 的 task

### 第四階段：Timer 與 Signal 處理
1. **Timer Setup**
   - 使用 `setitimer()` 設定 `SIGVTALRM` 每 10ms 觸發
   - 在 shell mode 暫停 timer，simulation mode 恢復

2. **Signal Handler**
   - 更新所有 task 的時間統計（running time, waiting time, turnaround time）
   - 檢查 sleep task 是否該喚醒
   - 檢查是否需要重新排程（特別是 RR 和 Priority）

### 第五階段：Shell 介面
1. **Command Implementation**
   - `add`: 建立新 task 並設為 READY state
   - `del`: 將指定 task 設為 TERMINATED state
   - `ps`: 顯示所有 task 資訊
   - `start`: 開始或恢復模擬

2. **Mode Switching**
   - Ctrl+Z 暫停模擬，切換到 shell mode
   - 適當處理 signal masking 避免 timer 干擾 shell

## 測試策略
### 單元測試
1. **Task Management 測試**
   - Context creation/switching 正確性
   - State transition 邏輯
   - 時間統計準確性

2. **Scheduler 測試**
   - 各排程演算法正確性
   - Edge case 處理（empty queue, single task）
   - Preemption 時機正確性

3. **Resource Management 測試**
   - 資源分配/釋放正確性
   - Blocking/unblocking 邏輯
   - 多重資源請求處理

### 整合測試
1. **使用提供的測試案例**
   - `test/general.txt`: 基本功能測試
   - `test/test_case1.txt`, `test/test_case2.txt`: 複雜情境測試
   - `test/judge_shell.py`: Shell 功能測試

2. **自訂測試案例**
   - 高負載情境
   - 資源競爭情境
   - 各種排程演算法比較

### 效能測試
- 大量 task 情況下的效能表現
- Memory usage 監控
- Signal handling 延遲測試

## 關鍵實作細節
### 1. ucontext 使用注意事項
- Stack size 設定要足夠（實際實作使用 128KB，`STACK_SIZE = 1024 * 128`）
- `makecontext()` 參數傳遞限制
- Context 切換時的 register 保存

### 2. Signal Handler 設計
- **Reentrancy**: signal handler 必須是 reentrant
- **Async-safe**: 只能使用 async-signal-safe 函數
- **Critical Section**: 適當使用 signal masking 保護關鍵區間

### 3. 時間管理
- 所有時間以 10ms 為單位
- Waiting time 只計算在 ready queue 的時間
- Turnaround time 從 task 建立到終止的總時間

### 4. 資源管理
- `get_resource()` 的重新檢查機制
- 避免 race condition 在資源分配過程中
- 資源釋放時及時喚醒等待的 task

### 5. 記憶體管理
- Task stack 的分配與釋放
- 避免 memory leak
- Context 資料結構的正確清理

## 潛在風險與對策
### 技術風險
1. **Signal handling complexity**: 詳細研讀 signal safety，使用適當的同步機制
2. **Context switching overhead**: 最佳化 context switch 路徑
3. **Timer accuracy**: 考慮系統 timer resolution 限制

### 實作風險
1. **Race conditions**: 仔細設計 critical section 保護
2. **Deadlock**: 在資源分配時實作 deadlock prevention
3. **Memory management**: 使用 valgrind 檢查 memory leak
