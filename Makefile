# 目標執行檔名稱 (Target executable name)
TARGET 	= scheduler_simulator

# 編譯器設定
# gcc -g: 使用 GCC 編譯器並包含除錯資訊 (debug information)
CC     	= gcc -g

# 編譯器選項 (Compiler flags)
# -Wall: 啟用所有警告訊息 (enable all warnings)
# -lpthread: 連結 pthread 函式庫 (link pthread library for multithreading)
FLAGS  	= -Wall -lpthread

# 目標檔案清單 (Object files list)
# 包含所有需要編譯的 .c 檔案對應的 .o 目標檔案
OBJ    	= builtin.o command.o shell.o function.o resource.o task.o

# 標頭檔目錄
INCLUDE = ./include/

# 原始碼目錄
SRC		= ./src/

# ==============================================================================
# 建置規則 (Build Rules)
# ==============================================================================

# 預設目標：建置整個專案 (Default target: build entire project)
all: $(TARGET)

# 主要目標建置規則 (Main target build rule)
# 依賴 main.c 和所有目標檔案，將它們連結成最終執行檔
$(TARGET): main.c $(OBJ)
	$(CC) $(FLAGS) -o $(TARGET) $(OBJ) $<

# 通用目標檔案建置規則 (Generic object file build rule)
# 自動規則：將 src/ 目錄下的 .c 檔案編譯成對應的 .o 目標檔案
# 同時檢查對應的標頭檔是否存在於 include/ 目錄中
%.o: ${SRC}%.c ${INCLUDE}%.h
	$(CC) $(FLAGS) -c $<

# ==============================================================================
# 清理規則 (Clean Rules)
# ==============================================================================

# 宣告 clean 為偽目標 (declare clean as phony target)
# 偽目標不會檢查檔案是否存在，總是執行對應的命令
.PHONY: clean

# 完全清理：刪除執行檔、所有目標檔案和輸出檔案 (Complete cleanup)
clean:
	rm -f ${TARGET} *.o out*

# 僅清理目標檔案 (Clean only object files)
clean_obj:
	rm -f *.o
