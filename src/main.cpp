/*
 * @Author: Jnsforgit jiangnen123456@qq.com
 * @Date: 2024-09-10 16:57:37
 * @LastEditors: Jnsforgit jiangnen123456@qq.com
 * @LastEditTime: 2024-09-10 17:50:23
 * @FilePath: /backend/backend/c++/anyserver/src/main.cpp
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */

#include "CLI/CLI.hpp"
#include "common.h"
#include "core.h"

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char const *argv[])
{
    CLI::App app{"Anyserver"};                                          /* 软件描述出现在第一行打印 */
    app.footer("=====================> End <=====================");    /* 最后一行打印 */
    app.get_formatter()->column_width(40);                              /* 列的宽度 */

    app.set_version_flag("--version", std::string("24.09.05.01.00.00.00"));

    int port = 0;
    CLI::Option *opt_port = app.add_option("-p,--port", port, "Server listen port");

    // int log_level = 0;
    // CLI::Option *opt_llevel = app.add_option("-l,--log_level", log_level, "Logger output level");

    CLI11_PARSE(app, argc, argv);

    int rc = dzlog_init("test.conf", "Anyserver");
    if (rc) 
    {
        printf("zlog_intit failed\n");
        return -1;
    }

    Anyserver server((uint16_t)port, "0.0.0.0");               /* 一般设置 60s 的定时 */
    
    LOGI("Starting Anyserver at [%d]!", port);
    server.Start();
    LOGI("Stoped Anyserver at [%d]!", port);

    zlog_fini();

    return 0;
}
