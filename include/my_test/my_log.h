#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>

#include <linux/kernel.h>
// #include <linux/time.h>
// #include <linux/timekeeping.h>

// 日志记录到文件的函数
void log_to_file(const char *filename, int line_number, const char* file_name, const char *format, ...) {
    // 打开文件以追加模式
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Unable to open log file");
        return;
    }

    // 获取当前时间
    time_t rawtime;
    struct tm timeinfo;  // 使用一个本地的结构体来存储时间
    time(&rawtime);
    localtime_r(&rawtime, &timeinfo);  // 使用线程安全的 localtime_r

    // 获取当前文件名
    // const char *file_name = __FILE__;

    // 打印时间、文件名、行号到文件
    fprintf(file, "[%04d-%02d-%02d %02d:%02d:%02d] %s:%d - ", 
            timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
            timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, file_name, line_number);

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
void redirect_stdout_to_file(const char *log_file, const char *log_message, int line_number, const char* file_name) {
    // 将标准输出重定向到文件（以追加模式）
    FILE *stdout_backup = freopen(log_file, "a", stdout);
    if (stdout_backup == NULL) {
        perror("Failed to redirect stdout to log file");
        return;
    }

    // 使用 log_to_file 输出内容到文件
    log_to_file(log_file, line_number, file_name, log_message);

    // 恢复标准输出到终端
    fclose(stdout);
    // 将 stdout 恢复为终端输出（可选）
    // freopen("/dev/tty", "a", stdout);  // 在 Unix/Linux 下恢复标准输出到终端
}




// 内核日志记录函数
//  kernel_log_message(__LINE__, __FILE__, "ovs_vport_receive begin - vport: %s, skb len: %d", vport->dev->name, skb->len);
void kernel_log_message(int line_number, const char* file_name, const char *format, ...) {
    
    time_t now;
    struct tm tm_info;
    char time_buf[26];
    char log_buffer[512];  // 用于存储格式化后的消息
    va_list args;
    int len;

    // 获取当前时间
    time(&now);
    localtime_r(&now, &tm_info);
    
    // 格式化时间和基本信息
    len = snprintf(log_buffer, sizeof(log_buffer),
            "[%04d-%02d-%02d %02d:%02d:%02d] %s:%d - ",
            tm_info.tm_year + 1900, tm_info.tm_mon + 1, tm_info.tm_mday,
            tm_info.tm_hour, tm_info.tm_min, tm_info.tm_sec,
            file_name, line_number);

    // 格式化用户消息
    va_start(args, format);
    vsnprintf(log_buffer + len, sizeof(log_buffer) - len, format, args);
    va_end(args);

    // 使用 printk 输出完整消息
    printk(KERN_INFO "%s\n", log_buffer);
}

// 简化版本的日志记录函数
// kernel_log("ovs_vport_receive begin", __LINE__, __FILE__);
void kernel_log(const char *log_message, int line_number, const char* file_name) {
    time_t now;
    struct tm tm_info;
    char time_buf[26];
    char log_buffer[512];

    // 获取当前时间
    time(&now);
    localtime_r(&now, &tm_info);
    
    // 格式化完整消息
    snprintf(log_buffer, sizeof(log_buffer),
            "[%04d-%02d-%02d %02d:%02d:%02d] %s:%d - %s",
            tm_info.tm_year + 1900, tm_info.tm_mon + 1, tm_info.tm_mday,
            tm_info.tm_hour, tm_info.tm_min, tm_info.tm_sec,
            file_name, line_number, log_message);

    // 直接使用 printk 输出
    printk(KERN_INFO "%s\n", log_buffer);
}