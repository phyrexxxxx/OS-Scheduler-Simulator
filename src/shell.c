#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../include/builtin.h"
#include "../include/command.h"

/*
 * 執行命令
 *
 * 參數：p - pipes 結構，包含命令參數
 * 回傳值：執行狀態
 *
 * 執行順序：
 * 1. 先檢查是否為內建命令 (add, del, ps, start, exit 等)
 * 2. 若非內建命令，則執行外部程式
 */
int execute(struct pipes *p)
{
    /* 檢查是否為內建命令 */
    for (int i = 0; i < num_builtins(); ++i) {
        if (strcmp(p->args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(p->args); /* 執行內建命令 */
        }
    }
    /* 非內建命令，執行外部程式 */
    return execvp(p->args[0], p->args);
}

/*
 * 產生 child process 執行命令
 *
 * 參數：
 *   in - 輸入 file descriptor (0 表示 stdin)
 *   out - 輸出 file descriptor (1 表示 stdout)
 *   cmd - 命令結構，包含 I/O 重導向和背景執行資訊
 *   p - pipes 結構，包含命令參數
 *
 * 功能：
 * 1. Fork child process
 * 2. 處理 I/O 重導向 (pipe, 檔案輸入/輸出)
 * 3. 執行命令
 * 4. 處理背景執行或等待 child process 結束
 */
int spawn_proc(int in, int out, struct cmd *cmd, struct pipes *p)
{
    pid_t pid;
    int status, fd;

    if ((pid = fork()) == 0) { /*  child process */
        /* 處理輸入重導向 */
        if (in != 0) {
            /* 從 pipe 讀取輸入 */
            dup2(in, 0); /* 將 in 複製到 stdin */
            close(in);
        } else {
            /* 從檔案讀取輸入 (< 重導向) */
            if (cmd->in_file) {
                fd = open(cmd->in_file, O_RDONLY);
                dup2(fd, 0);
                close(fd);
            }
        }

        /* 處理輸出重導向 */
        if (out != 1) {
            /* 輸出到 pipe */
            dup2(out, 1); /* 將 out 複製到 stdout */
            close(out);
        } else {
            /* 輸出到檔案 (> 重導向) */
            if (cmd->out_file) {
                fd = open(cmd->out_file, O_RDWR | O_CREAT, 0644);
                dup2(fd, 1);
                close(fd);
            }
        }

        /* 執行命令 */
        if (execute(p) == -1)
            perror("lsh");
        exit(EXIT_FAILURE);

    } else { /* parent process */
        if (cmd->background) {
            /* 背景執行，不等待 child process */
            if (!p->next)
                printf("[pid]: %d\n", pid);
        } else {
            /* 前景執行，等待 child process 結束 */
            do {
                waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
    }
    return 1;
}

/*
 * 處理 pipe 管線命令
 *
 * 參數：cmd - 命令結構，包含 pipe 鏈表
 * 回傳值：執行狀態 (1 表示成功)
 *
 * 處理流程：
 * 1. 如果有多個 pipe，創建 pipe 鏈
 * 2. 每個命令的輸出連接到下一個命令的輸入
 * 3. 最後一個命令輸出到 stdout
 */
int fork_pipes(struct cmd *cmd)
{
    int in = 0, fd[2];
    struct pipes *temp = cmd->head;

    /* 處理 pipe 鏈 (如果有多個命令) */
    while (temp->next != NULL) {
        pipe(fd);                         /* 創建 pipe */
        spawn_proc(in, fd[1], cmd, temp); /* 執行命令，輸出到 pipe */
        close(fd[1]);                     /* 關閉寫端 */
        in = fd[0];                       /* 下一個命令從此 pipe 讀取 */
        temp = temp->next;
    }

    /* 處理最後一個命令 */
    if (in != 0) {
        spawn_proc(in, 1, cmd, temp); /* 從 pipe 讀取，輸出到 stdout */
        return 1;
    }

    /* 只有一個命令 (沒有 pipe) */
    spawn_proc(0, 1, cmd, cmd->head);
    return 1;
}

/*
 * Shell 主迴圈
 *
 * 功能：
 * 1. 顯示 shell 提示字元
 * 2. 讀取使用者輸入
 * 3. 解析命令
 * 4. 執行命令 (內建命令或外部程式)
 * 5. 處理 I/O 重導向和 pipe
 * 6. 釋放記憶體
 * 7. 當收到 exit 命令時結束
 */
void shell()
{
    while (1) {
        /* 顯示 shell 提示字元 */
        printf(">>> $ ");

        /* 讀取使用者輸入的一行命令 */
        char *buffer = read_line();
        if (buffer == NULL)
            continue; /* 空輸入，繼續下一輪 */

        /* 解析命令行 (分割參數、處理 pipe、重導向等) */
        struct cmd *cmd = split_line(buffer);

        int status = -1;

        /* 優化：如果是單一內建命令且不是背景執行，直接在當前 process 執行 */
        if (!cmd->background && cmd->head->next == NULL) {
            /* 備份原始的 stdin/stdout */
            int fd, in = dup(0), out = dup(1);

            /* 處理輸入重導向 */
            if (cmd->in_file) {
                fd = open(cmd->in_file, O_RDONLY);
                dup2(fd, 0);
                close(fd);
            }

            /* 處理輸出重導向 */
            if (cmd->out_file) {
                fd = open(cmd->out_file, O_RDWR | O_CREAT, 0644);
                dup2(fd, 1);
                close(fd);
            }

            /* 檢查並執行內建命令 */
            for (int i = 0; i < num_builtins(); ++i)
                if (strcmp(cmd->head->args[0], builtin_str[i]) == 0)
                    status = (*builtin_func[i])(cmd->head->args);

            /* 恢復原始的 stdin/stdout */
            if (cmd->in_file)
                dup2(in, 0);
            if (cmd->out_file)
                dup2(out, 1);
            close(in);
            close(out);
        }

        /* 如果不是內建命令，或有 pipe，或是背景執行，則 fork child process */
        if (status == -1)
            status = fork_pipes(cmd);

        /* 釋放命令結構所佔用的記憶體 */
        while (cmd->head) {
            struct pipes *temp = cmd->head;
            cmd->head = cmd->head->next;
            free(temp->args);
            free(temp);
        }
        free(cmd);
        free(buffer);

        /* 如果 status 為 0，表示收到 exit 命令，結束 shell */
        if (status == 0)
            break;
    }
}
