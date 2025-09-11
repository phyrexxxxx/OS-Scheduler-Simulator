## 測試案例總覽
這個測試使用了 26 個 task，執行不同的函數：
- task1: 排序演算法（CPU 密集型）
- task2: 矩陣運算（CPU + Memory 密集型）
- task3: 線性搜尋（Memory 密集型）
- task4-8: 資源管理測試（需要不同資源組合）

## Priority Preemptive 核心概念
優先權規則：
- 數字越小，優先權越高（1 最高，26 最低）
- 搶佔式：高優先權 task 可以立即搶佔低優先權 task 的 CPU

## 關鍵觀察：執行順序完全改變
看 ps 輸出的 TID 順序（按優先權排列）

| TID  | name  | priority |
| --- | --- | --- |
| 22   | vvv  |    1 | ← 最高優先權，第一個執行
| 1    | aaa  |    2 |
| 2    | bbb  |    3 |
| 3    | ccc  |    4 |
| ... | ... | ... |
| 21   | uuu  |   26 | ← 最低優先權，最後執行

這與加入順序完全不同！ vvv 雖然第 22 個加入，但因為優先權是 1，所以第一個執行

## 詳細執行流程分析
### 第一階段：vvv 立即執行（優先權 1）
```
Task vvv is running.
Task vvv gets resource 0
Task vvv gets resource 1
Task vvv gets resource 2
Task vvv goes to sleep.
```

關鍵點：vvv 一加入就立即執行，因為它優先權最高！

### 第二階段：按優先權順序執行（2-10）
```
Task aaa is running. (優先權 2)
Task bbb is running. (優先權 3)
Task ccc is running. (優先權 4)
...
Task iii is running. (優先權 10)
```
完美的優先權順序！每個 task 都執行到完成才換下一個。

### 第三階段：資源競爭的優先權處理
當執行到 www（優先權 11）時：
```
Task www is running.
Task www is waiting resource.
Task jjj is running. (優先權 12)
Task jjj has terminated. 
```

⭐ 重要: 即使 www 在等資源，系統仍按優先權執行下一個可執行的 task（jjj）

### 第四階段：vvv 醒來的搶佔
```
Task mmm is running.
Task mmm has terminated.
Task vvv is running. ← vvv 醒來立即搶佔！
Task vvv releases resource 0
Task vvv releases resource 1
Task vvv releases resource 2
Task vvv has terminated.
```
搶佔發生！vvv sleep 結束後，因為優先權最高（1），立即搶佔 CPU

## 最終 ps 結果深度分析
### 1. Running Time 分析
與 FCFS 幾乎相同：
- 短任務（task3）：4-7 單位
- 中任務（task1）：21-22 單位
- 長任務（task2）：44 單位

為什麼？ 因為 PP 不像 RR 有頻繁的 context switch

### 2. Waiting Time - 優先權的影響
對比三種演算法的 waiting time：

| Task  | Priority  | FCFS   | RR    | PP |
| --- | --- | --- | --- |
| vvv | 1 | 188 | 68 | 5 | ← 優先權最高，幾乎不等待
| aaa | 2 | 0 | 60 | 0 | ← 優先權高，立即執行
| www | 11 | 188 | 143 | 82 | ← 中等優先權
| uuu | 26 | 146 | 146 | 146 | ← 最低優先權，等最久

規律：
- 高優先權：waiting time 極短
- 低優先權：可能等很久（飢餓問題）

### 3. Turnaround Time 對比

| 演算法 | vvv | aaa | uuu | zzz |
| --- | --- | --- | --- | --- |
| FCFS | 258 | 5 | 188 | 418 |
| RR | 138 | 64 | 187 | 305 |
| PP | 75 | 5 | 190 | 246 |

PP 的優勢：
- 高優先權 task（vvv）：turnaround 最短（75）
- 低優先權但無資源競爭的（uuu）：與其他演算法差不多

### 4. 資源競爭的優先權影響
執行順序：
1. vvv（優先權 1）→ 立即執行，佔用資源 0, 1, 2
2. www（優先權 11）→ 等待資源 1
3. xxx（優先權 19）→ 等待資源 2
4. yyy（優先權 20）→ 等待資源 1
5. zzz（優先權 24）→ 等待資源 0

資源釋放後的執行順序：
- www 先執行（優先權 11）
- 然後 xxx（優先權 19）
- 然後 yyy（優先權 20）
- 最後 zzz（優先權 24）

## Priority Preemptive 特性總結
優點：
1. 重要任務優先：關鍵任務能立即獲得 CPU
2. 響應時間可控：透過調整優先權控制響應時間
3. 適合即時系統：保證高優先權任務的執行

缺點：
1. 飢餓問題：低優先權任務可能永遠得不到執行
2. 優先權反轉 (Priority Inversion)：低優先權佔用資源時，高優先權也要等
3. 需要合理設定優先權：設定不當會導致系統效能差

## 三種演算法總比較
| 特性 |  FCFS |        RR         | PP |
| --- | --- | --- | --- |
| 公平性 | 中等 | 高 | 低 |
| 響應時間 | 差 | 好 | 依優先權 |
| 適用場景 | 批次處理 | 分時系統 | 即時系統 |
| Context開銷 | 低 | 高 | 中 |
| 飢餓問題 | 無 | 無 | 有 |

這個測試完美展示了優先權搶佔式排程的特點：優先權決定一切！高優先權任務享有特權，而低優先權任務可能要等很久
