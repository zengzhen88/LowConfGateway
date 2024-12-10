#include <iostream>
#include <regex>
#include <string>

extern "C" int cpp_regexFunction(const char *inStr, const char *matchStr) {
    // C++函数实现
    // std::string text(inStr);
    static const std::regex regexs(inStr);

    // 示例1: 匹配"Hello"
    if (std::regex_match(inStr, regexs)) {
        return 0;
    }

    return -1;
}
