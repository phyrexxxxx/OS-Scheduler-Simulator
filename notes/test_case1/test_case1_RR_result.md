```bash
./scheduler_simulator RR

>>> $ add aaa task3 2
Task aaa is ready.
>>> $ add bbb task3 3
Task bbb is ready.
>>> $ add ccc task3 4
Task ccc is ready.
>>> $ add ddd task3 5
Task ddd is ready.
>>> $ add eee task3 6
Task eee is ready.
>>> $ add fff task3 7
Task fff is ready.
>>> $ add ggg task3 8
Task ggg is ready.
>>> $ add hhh task3 9
Task hhh is ready.
>>> $ add iii task3 10
Task iii is ready.
>>> $ add jjj task3 12
Task jjj is ready.
>>> $ add kkk task3 13
Task kkk is ready.
>>> $ add lll task3 14
Task lll is ready.
>>> $ add mmm task3 15
Task mmm is ready.
>>> $ add nnn task3 16
Task nnn is ready.
>>> $ add ooo task3 17
Task ooo is ready.
>>> $ add ppp task3 18
Task ppp is ready.
>>> $ add qqq task3 21
Task qqq is ready.
>>> $ add rrr task3 22
Task rrr is ready.
>>> $ add sss task1 23
Task sss is ready.
>>> $ add ttt task1 25
Task ttt is ready.
>>> $ add uuu task2 26
Task uuu is ready.
>>> $ add vvv task4 1
Task vvv is ready.
>>> $ add www task5 11
Task www is ready.
>>> $ add xxx task6 19
Task xxx is ready.
>>> $ add yyy task7 20
Task yyy is ready.
>>> $ add zzz task8 24
Task zzz is ready.
>>> $ start > out
>>> $ rm out
>>> $ ps
 TID|       name|      state| running| waiting| turnaround| resources| priority
--------------------------------------------------------------------------------
   1|        aaa| TERMINATED|       4|      60|         64|      none|        2
   2|        bbb| TERMINATED|       7|     107|        114|      none|        3
   3|        ccc| TERMINATED|       6|     108|        114|      none|        4
   4|        ddd| TERMINATED|       3|      67|         70|      none|        5
   5|        eee| TERMINATED|       5|      67|         72|      none|        6
   6|        fff| TERMINATED|       8|     108|        116|      none|        7
   7|        ggg| TERMINATED|       5|      72|         77|      none|        8
   8|        hhh| TERMINATED|       8|     110|        118|      none|        9
   9|        iii| TERMINATED|       6|     112|        118|      none|       10
  10|        jjj| TERMINATED|       6|     112|        118|      none|       12
  11|        kkk| TERMINATED|       5|      83|         88|      none|       13
  12|        lll| TERMINATED|       7|     112|        119|      none|       14
  13|        mmm| TERMINATED|       4|      88|         92|      none|       15
  14|        nnn| TERMINATED|       5|      89|         94|      none|       16
  15|        ooo| TERMINATED|       5|      91|         96|      none|       17
  16|        ppp| TERMINATED|       7|     113|        120|      none|       18
  17|        qqq| TERMINATED|       5|      96|        101|      none|       21
  18|        rrr| TERMINATED|       6|     114|        120|      none|       22
  19|        sss| TERMINATED|      22|     144|        166|      none|       23
  20|        ttt| TERMINATED|      22|     145|        167|      none|       25
  21|        uuu| TERMINATED|      41|     146|        187|      none|       26
  22|        vvv| TERMINATED|       0|      68|        138|      none|        1
  23|        www| TERMINATED|       0|     143|        205|      none|       11
  24|        xxx| TERMINATED|       0|     179|        265|      none|       19
  25|        yyy| TERMINATED|       0|     187|        285|      none|       20
  26|        zzz| TERMINATED|       0|     187|        305|      none|       24
>>> $ exit
```

## `out` 輸出
```
Start simulation.
Task aaa is running.
Task bbb is running.
Task ccc is running.
Task ddd is running.
Task eee is running.
Task fff is running.
Task ggg is running.
Task hhh is running.
Task iii is running.
Task jjj is running.
Task kkk is running.
Task lll is running.
Task mmm is running.
Task nnn is running.
Task ooo is running.
Task ppp is running.
Task qqq is running.
Task rrr is running.
Task sss is running.
Task ttt is running.
Task uuu is running.
Task vvv is running.
Task vvv gets resource 0
Task vvv gets resource 1
Task vvv gets resource 2
Task vvv goes to sleep.
Task aaa is running.
Task aaa has terminated.
Task bbb is running.
Task ccc is running.
Task ddd is running.
Task ddd has terminated.
Task eee is running.
Task eee has terminated.
Task fff is running.
Task ggg is running.
Task ggg has terminated.
Task hhh is running.
Task iii is running.
Task jjj is running.
Task kkk is running.
Task kkk has terminated.
Task lll is running.
Task mmm is running.
Task mmm has terminated.
Task nnn is running.
Task nnn has terminated.
Task ooo is running.
Task ooo has terminated.
Task ppp is running.
Task qqq is running.
Task qqq has terminated.
Task rrr is running.
Task sss is running.
Task ttt is running.
Task uuu is running.
Task www is running.
Task www is waiting resource.
Task bbb is running.
Task bbb has terminated.
Task ccc is running.
Task ccc has terminated.
Task fff is running.
Task fff has terminated.
Task hhh is running.
Task hhh has terminated.
Task iii is running.
Task iii has terminated.
Task jjj is running.
Task jjj has terminated.
Task lll is running.
Task lll has terminated.
Task ppp is running.
Task ppp has terminated.
Task rrr is running.
Task rrr has terminated.
Task sss is running.
Task ttt is running.
Task uuu is running.
Task www is running.
Task www is waiting resource.
Task sss is running.
Task ttt is running.
Task uuu is running.
Task vvv is running.
Task vvv releases resource 0
Task vvv releases resource 1
Task vvv releases resource 2
Task vvv has terminated.
Task www is running.
Task www gets resource 1
Task www gets resource 4
Task www goes to sleep.
Task sss is running.
Task ttt is running.
Task uuu is running.
Task xxx is running.
Task xxx is waiting resource.
Task sss is running.
Task ttt is running.
Task uuu is running.
Task xxx is running.
Task xxx is waiting resource.
Task sss is running.
Task ttt is running.
Task uuu is running.
Task www is running.
Task www gets resource 5
Task www goes to sleep.
Task sss is running.
Task sss has terminated.
Task ttt is running.
Task ttt has terminated.
Task uuu is running.
Task xxx is running.
Task xxx is waiting resource.
Task uuu is running.
Task xxx is running.
Task xxx is waiting resource.
Task uuu is running.
Task xxx is running.
Task xxx is waiting resource.
Task uuu is running.
Task xxx is running.
Task xxx is waiting resource.
Task uuu is running.
Task xxx is running.
Task xxx is waiting resource.
Task uuu is running.
Task xxx is running.
Task xxx is waiting resource.
Task uuu is running.
Task uuu has terminated.
Task xxx is running.
Task xxx is waiting resource.
Task yyy is running.
Task yyy is waiting resource.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task xxx is running.
Task xxx is waiting resource.
Task yyy is running.
Task yyy is waiting resource.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task xxx is running.
Task xxx is waiting resource.
Task yyy is running.
Task yyy is waiting resource.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task xxx is running.
Task xxx is waiting resource.
Task yyy is running.
Task yyy is waiting resource.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task xxx is running.
Task xxx is waiting resource.
Task yyy is running.
Task yyy is waiting resource.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task xxx is running.
Task xxx is waiting resource.
Task yyy is running.
Task yyy is waiting resource.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task xxx is running.
Task xxx is waiting resource.
Task yyy is running.
Task yyy is waiting resource.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task xxx is running.
Task xxx is waiting resource.
Task yyy is running.
Task yyy is waiting resource.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task xxx is running.
Task xxx is waiting resource.
Task yyy is running.
Task yyy is waiting resource.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task xxx is running.
Task xxx is waiting resource.
Task yyy is running.
Task yyy is waiting resource.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task xxx is running.
Task xxx is waiting resource.
Task yyy is running.
Task yyy is waiting resource.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task xxx is running.
Task xxx is waiting resource.
Task yyy is running.
Task yyy is waiting resource.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task xxx is running.
Task xxx is waiting resource.
Task yyy is running.
Task yyy is waiting resource.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task xxx is running.
Task xxx is waiting resource.
Task yyy is running.
Task yyy is waiting resource.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task xxx is running.
Task xxx is waiting resource.
Task yyy is running.
Task yyy is waiting resource.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task xxx is running.
Task xxx is waiting resource.
Task yyy is running.
Task yyy is waiting resource.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task xxx is running.
Task xxx is waiting resource.
Task yyy is running.
Task yyy is waiting resource.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task xxx is running.
Task xxx is waiting resource.
Task yyy is running.
Task yyy is waiting resource.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task www is running.
Task www releases resource 1
Task www releases resource 4
Task www releases resource 5
Task www has terminated.
Task xxx is running.
Task xxx gets resource 2
Task xxx gets resource 4
Task xxx goes to sleep.
Task yyy is running.
Task yyy gets resource 1
Task yyy gets resource 3
Task yyy gets resource 6
Task yyy goes to sleep.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task zzz is running.
Task zzz is waiting resource.
CPU idle.
Task xxx is running.
Task xxx releases resource 2
Task xxx releases resource 4
Task xxx has terminated.
Task zzz is running.
Task zzz gets resource 0
Task zzz gets resource 4
Task zzz gets resource 7
Task zzz goes to sleep.
CPU idle.
Task yyy is running.
Task yyy releases resource 1
Task yyy releases resource 3
Task yyy releases resource 6
Task yyy has terminated.
CPU idle.
Task zzz is running.
Task zzz releases resource 0
Task zzz releases resource 4
Task zzz releases resource 7
Task zzz has terminated.
Simulation over.
```