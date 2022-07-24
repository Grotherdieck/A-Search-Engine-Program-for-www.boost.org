#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>

#define NORMAL 1
#define WARNING 2
#define DEBUG 3
#define FATAL 4// 毁灭性

// 封装成宏 方便调用
// 利用C语言给我们预设的__FILE__宏获得文件名 __LINE__获得当前行
#define LOG(LEVEL, MESSAGE) log(#LEVEL, MESSAGE, __FILE__, __LINE__)

const std::string logtxtfile = "/home/Router/Boost_Search_Engines/log.txt";

void log(std::string level, std::string message, std::string file, int line)
{

    char str[100];
    struct tm *ti;
    uint16_t year, yday;
    uint8_t month, day, week, hour, minute, second;
    time_t timestamp = time(nullptr); 
	/* 调用系统函数 */
    ti = localtime(&timestamp);
	
	year = ti->tm_year;   /* 自1900年算起 */
    month = ti->tm_mon;   /* 从1月算起，范围0-11 */
    week = ti->tm_wday;   /* 从周末算起，范围0-6 */
    yday = ti->tm_yday;  /* 从1月1日算起，范围0-365 */
    day = ti->tm_mday;    /* 日: 1-31 */
    hour = ti->tm_hour;   /* 小时:0-23点,UTC+0时间 */
    minute = ti->tm_min;  /* 分钟:0-59 */
    second = ti->tm_sec;  /* 0-60，偶尔出现的闰秒 */
    
    /* 时间校正 */
    year += 1900;
    month += 1;
    week += 1;
    std::cout << "[" << level << "]" << "["
    << "日期:" << (int)year << '-' << (int)month << '-' << (int)day << " 第" << (int)yday << "天 星期" << (int)week
    << "时间:" << (int)hour << ':' << (int)minute << ':' << (int)second
    << "][" << message << "][" << file << ' ' << line << "]" << std::endl;
}