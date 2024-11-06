#include <iostream>
#include <regex>
#include <string>

extern "C" int cpp_regexFunction(const char *inStr, const char *matchStr) {
    // C++函数实现
    std::string text(inStr);

    // 示例1: 匹配"Hello"
    std::regex regexStr(matchStr);
    std::smatch match;
    if (std::regex_search(text, regexStr)) {
        return 0;
    }

    return -1;
}
