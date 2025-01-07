#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>

// 日志记录到文件的函数
void log_to_file(const char *filename, int line_number, const char *format, ...) {
    // 打开文件以追加模式
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Unable to open log file");
        return;
    }

    // 获取当前时间
    time_t rawtime;
    struct tm *timeinfo;
    char time_str[20]; // 用于存储时间字符串
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);

    // 获取当前文件名
    const char *file_name = __FILE__;

    // 打印时间、文件名、行号到文件
    fprintf(file, "[%s] %s:%d - ", time_str, file_name, line_number);

    // 使用变参打印用户提供的信息
    va_list args;
    va_start(args, format);
    vfprintf(file, format, args);
    va_end(args);

    // 添加换行符
    fprintf(file, "\n");

    // 关闭文件
    fclose(file);
}

// 重定向标准输出到文件，记录日志，并恢复标准输出
void redirect_stdout_to_file(const char *log_file, const char *log_message, int line_number) {
    // 将标准输出重定向到文件（以追加模式）
    FILE *stdout_backup = freopen(log_file, "a", stdout);
    if (stdout_backup == NULL) {
        perror("Failed to redirect stdout to log file");
        return;
    }

    // 使用 log_to_file 输出内容到文件
    log_to_file(log_file, line_number, log_message);

    // 恢复标准输出到终端
    fclose(stdout);
    // 将 stdout 恢复为终端输出（可选）
    // freopen("/dev/tty", "a", stdout);  // 在 Unix/Linux 下恢复标准输出到终端
}
