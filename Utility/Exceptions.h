#include <exception>
#include <string>
#include <utility>
#include <sstream>
#include <cstring>

class Exception : public std::exception {
    std::string _msg;
    std::string _file;
    int _line{};
    std::string _func;

public:
    Exception(std::string msg, std::string file, int line, std::string func) : _msg(
            std::move(msg)),
                                                                               _file(std::move(
                                                                                       file)),
                                                                               _line(line),
                                                                               _func(std::move(
                                                                                       func)) {}

    [[nodiscard]] std::string get_file() const { return _file; }

    [[nodiscard]] int get_line() const { return _line; }

    [[nodiscard]] std::string get_func() const { return _func; }

    [[nodiscard]] const char *what() const noexcept override {
        std::stringstream str{};
        str << "Msg: " << _msg << ": " << " | Arquivo: " << _file << " | Linha: " << _line << " | Func: " << _func;
        char *msg = new char[512];
        strcpy(msg, str.str().c_str());
        return msg;
    }
};