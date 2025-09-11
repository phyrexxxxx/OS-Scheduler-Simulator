## 測試案例總覽
這個測試使用了 26 個 task，執行不同的函數：
- task1: 排序演算法（CPU 密集型）
- task2: 矩陣運算（CPU + Memory 密集型）
- task3: 線性搜尋（Memory 密集型）
- task4-8: 資源管理測試（需要不同資源組合）

Round Robin 排程演算法:
- 時間片 (Time Quantum)：30ms（3個 10ms tick）
- 每個 task 最多執行 30ms 就要讓出 CPU

## Round Robin 執行特點
### 第一輪執行（初始階段）
關鍵觀察：看輸出的前面部分
```
Task aaa is running.
Task bbb is running.
Task ccc is running.
...
Task vvv is running.
```

每個 task 輪流執行 30ms！這是 RR 的核心特色：
- 不像 FCFS 讓一個 task 執行到完成
- 而是每個 task 執行一小段時間就換下一個

特殊案例 - vvv 的資源佔用
```
Task vvv gets resource 0
Task vvv gets resource 1
Task vvv gets resource 2
Task vvv goes to sleep.
```

vvv 在第一輪就拿到資源並 sleep，這會影響後續的 www, xxx, yyy, zzz

### 第二輪執行（資源競爭階段）
短任務快速完成：
- aaa (task3) 只需要約 40-50ms 就能完成
  - 所以只需要 2 個 30ms time slice 就能完成
- 在第二輪就結束了：`Task aaa has terminated.`
- ddd, eee, ggg 等也很快完成

長任務分段執行：
- sss, ttt (task1 排序) 需要 220ms
- uuu (task2 矩陣) 需要 410ms
- 它們會經過多輪才能完成

## 最終 ps 結果深度分析
### 1. Running Time（實際執行時間）
對比 FCFS 和 RR：
- FCFS：aaa-rrr 約 5-7 單位
- RR：aaa-rrr 約 3-8 單位（更分散）

為什麼？因為 RR 的 context switch 開銷！每 30ms 就要切換

### 2. Waiting Time（等待時間）- 最大差異!
⭐ waiting time: 計算在 ready queue 的時間

FCFS vs RR 對比：

| Task | FCFS Waiting | RR Waiting |
| --- | --- | --- |
| aaa | 0 | 60 |
| bbb | 5 | 107 |
| ccc | 10 | 108 |
| ... | ... | ... |
| uuu | 146 | 146 |

RR 的 waiting time 為什麼這麼高？

以 aaa 為例：
1. 執行 30ms → 換人
2. 等其他 25 個 task 各執行 30ms
3. 再輪到 aaa 執行
4. 這個等待過程累積成 waiting time

⭐ 公式：waiting ≈ (其他 task 數量) × (時間片長度) × (需要的輪數)

### 3. Turnaround Time（周轉時間）
短任務的周轉時間：
- FCFS：aaa = 5（很快完成）
- RR：aaa = 64（要等很多輪）

長任務的周轉時間：
- FCFS：uuu = 188
- RR：uuu = 187（差不多）

⭐ 結論：RR 對短任務不利，但對長任務較公平！

### 4. 資源相關 task (vvv-zzz)
執行順序差異：

FCFS：
1. vvv 第 22 個執行（要等前 21 個完成）
2. vvv sleep 期間，後面的都被阻塞

RR：
1. vvv 第一輪就執行（公平輪轉）
2. vvv sleep 期間，其他 task 繼續輪轉
3. 資源競爭的處理更靈活

Turnaround 對比:

| Task |   FCFS    |    RR    |
| --- | --- | --- |
| vvv | 258 | 138  (RR 更快！) |
| www | 318 | 205 |
| xxx | 378 | 265 |
| yyy | 398 | 285 |
| zzz | 418 | 305 |

## Round Robin 特性總結
優點：

1. 響應時間好：每個 task 很快就能獲得 CPU
2. 公平性高：不會有 task 等太久才第一次執行
3. 適合交互式系統：用戶感覺所有程序都在「同時」運行

缺點：

1. Context Switch 開銷：頻繁切換消耗資源
2. 短任務反而慢：原本 50ms 能完成的，可能要等好幾輪
3. 平均等待時間可能更長：特別是任務長度差異不大時

## RR vs FCFS 選擇指南
- 選 FCFS：任務長度已知且差異大，批次處理系統
- 選 RR：交互式系統，需要快速響應，公平性重要

這個測試完美展示了 RR 的「時間片輪轉」特性和其對不同類型任務的影響！
