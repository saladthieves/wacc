#include "source.hpp"

#include <format>

namespace wacc::front::src {
Source::Source(std::string_view source) :
    begin{source.cbegin()}, end{source.cend()} {
}

std::string Source::decorate(const token::Token& token) const {
    return decorate(token.lineStart, token.lineNo, token.offset, token.value);
}

std::string Source::decorate(Size lineStart, Size lineNo, Size offset,
                             std::string_view value) const {
    auto iter = begin + lineStart;
    while ((iter <= end) && (*iter != '\n')) ++iter;
    Size lineStop = iter - begin;
    const auto line = std::string_view{begin + lineStart, begin + lineStop};

    constexpr char c = '^';
    auto output = std::format("Line [{}]: ", lineNo);
    const auto prefixLength = output.length();

    output += std::format("{}\n", line);
    const auto totalLength = output.length();

    output += std::string(offset + prefixLength, ' ');
    output += std::string(value.length(), c);
    output += '\n';

    return output;
}
} // namespace wacc::front::src