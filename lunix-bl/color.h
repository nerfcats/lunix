// ansi_colors.h
#ifndef ANSI_COLORS_H
#define ANSI_COLORS_H

#include <iostream>

namespace ANSIColors {
    // Regular colors
    const std::string RESET     = "\033[0m";
    const std::string BLACK     = "\033[30m";
    const std::string RED       = "\033[31m";
    const std::string GREEN     = "\033[32m";
    const std::string YELLOW    = "\033[33m";
    const std::string BLUE      = "\033[34m";
    const std::string MAGENTA   = "\033[35m";
    const std::string CYAN      = "\033[36m";
    const std::string WHITE     = "\033[37m";

    // Bold colors
    const std::string BOLD_BLACK   = "\033[1m\033[30m";
    const std::string BOLD_RED     = "\033[1m\033[31m";
    const std::string BOLD_GREEN   = "\033[1m\033[32m";
    const std::string BOLD_YELLOW  = "\033[1m\033[33m";
    const std::string BOLD_BLUE    = "\033[1m\033[34m";
    const std::string BOLD_MAGENTA = "\033[1m\033[35m";
    const std::string BOLD_CYAN    = "\033[1m\033[36m";
    const std::string BOLD_WHITE   = "\033[1m\033[37m";

    // Underlined colors
    const std::string UNDERLINE_BLACK   = "\033[4m\033[30m";
    const std::string UNDERLINE_RED     = "\033[4m\033[31m";
    const std::string UNDERLINE_GREEN   = "\033[4m\033[32m";
    const std::string UNDERLINE_YELLOW  = "\033[4m\033[33m";
    const std::string UNDERLINE_BLUE    = "\033[4m\033[34m";
    const std::string UNDERLINE_MAGENTA = "\033[4m\033[35m";
    const std::string UNDERLINE_CYAN    = "\033[4m\033[36m";
    const std::string UNDERLINE_WHITE   = "\033[4m\033[37m";

    // Background colors
    const std::string BG_BLACK   = "\033[40m";
    const std::string BG_RED     = "\033[41m";
    const std::string BG_GREEN   = "\033[42m";
    const std::string BG_YELLOW  = "\033[43m";
    const std::string BG_BLUE    = "\033[44m";
    const std::string BG_MAGENTA = "\033[45m";
    const std::string BG_CYAN    = "\033[46m";
    const std::string BG_WHITE   = "\033[47m";

    // High intensity colors
    const std::string HI_BLACK   = "\033[90m";
    const std::string HI_RED     = "\033[91m";
    const std::string HI_GREEN   = "\033[92m";
    const std::string HI_YELLOW  = "\033[93m";
    const std::string HI_BLUE    = "\033[94m";
    const std::string HI_MAGENTA = "\033[95m";
    const std::string HI_CYAN    = "\033[96m";
    const std::string HI_WHITE   = "\033[97m";

    // Bold high intensity colors
    const std::string BOLD_HI_BLACK   = "\033[1m\033[90m";
    const std::string BOLD_HI_RED     = "\033[1m\033[91m";
    const std::string BOLD_HI_GREEN   = "\033[1m\033[92m";
    const std::string BOLD_HI_YELLOW  = "\033[1m\033[93m";
    const std::string BOLD_HI_BLUE    = "\033[1m\033[94m";
    const std::string BOLD_HI_MAGENTA = "\033[1m\033[95m";
    const std::string BOLD_HI_CYAN    = "\033[1m\033[96m";
    const std::string BOLD_HI_WHITE   = "\033[1m\033[97m";

    // High intensity background colors
    const std::string HI_BG_BLACK   = "\033[100m";
    const std::string HI_BG_RED     = "\033[101m";
    const std::string HI_BG_GREEN   = "\033[102m";
    const std::string HI_BG_YELLOW  = "\033[103m";
    const std::string HI_BG_BLUE    = "\033[104m";
    const std::string HI_BG_MAGENTA = "\033[105m";
    const std::string HI_BG_CYAN    = "\033[106m";
    const std::string HI_BG_WHITE   = "\033[107m";
}

#endif // ANSI_COLORS_H

