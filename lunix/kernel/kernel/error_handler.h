#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <iostream>
using namespace std;

class error_handler
{
public:
    error_handler();

    void panic(string reason);
    void oops(string reason);

    void setup_signal_handlers();
private:
    void print_stack_trace();
    static void handle_signal(int signal);

    void log_to_file(const string& log_message);
    void dump_registers(stringstream& panic_info);
};

#endif // ERROR_HANDLER_H
