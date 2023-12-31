#include "log/log.h"

int main(int argc, char *argv[]) {

    ht::Log::init();

    HT_CONSOLE_TRACE("This is message");
    HT_CONSOLE_DEBUG("This is message");
    HT_CONSOLE_INFO("This is message");
    HT_CONSOLE_WARN("This is message");
    HT_CONSOLE_ERROE("This is message");
    HT_CONSOLE_CRITICAL("This is message");

    HT_FILE_TRACE("This is message");
    HT_FILE_DEBUG("This is message");
    HT_FILE_INFO("This is message");
    HT_FILE_WARN("This is message");
    HT_FILE_ERROE("This is message");
    HT_FILE_CRITICAL("This is message");

    return 0;
}
