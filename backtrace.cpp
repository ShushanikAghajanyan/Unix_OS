#include <execinfo.h>
#include <cxxabi.h>
#include <iostream>
#include <sstream>
#include <vector>

void stack_dump(int depth, std::ostream& os) {
    std::vector<void*> buffer(depth + 1);
    int size = backtrace(buffer.data(), depth + 1);
    char** symbols = backtrace_symbols(buffer.data(), size);
    if (symbols == nullptr) {
        os << "backtrace_symbols() failed\n";
        return;
    }

    for (int i = 1; i < size; ++i) {
        std::string symbol(symbols[i]);
        size_t begin = symbol.find('(');
        size_t end = symbol.find('+', begin);
        if (begin != std::string::npos && end != std::string::npos) {
            std::string func_name = symbol.substr(begin + 1, end - begin - 1);
            int status = 0;
            char* demangled_name = abi::__cxa_demangle(func_name.c_str(), nullptr, nullptr, &status);
            if (status == 0 && demangled_name != nullptr) {
                os << demangled_name << '\n';
                free(demangled_name);
            } else {
                os << func_name << '\n';
            }
        } 
    }

    free(symbols);
}


void new_function() {
    int depth;
    std::cout << " Enter depth of stack for backtracking: ";
    std::cin >> depth;
    stack_dump(depth, std::cerr);
}

void new_function_2(){
    new_function();
}

int main() {
    new_function_2();
    return 0;
}
