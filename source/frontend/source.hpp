#pragma once

#include "token.hpp"

#include <string>
#include <string_view>

// TODO: Add tests
namespace wacc {
namespace front {
namespace src {
class Source {
public:
    using Iter = std::string_view::iterator;
    using ConstIter = std::string_view::const_iterator;
    using Size = unsigned int;

    Source(std::string_view source);

    const ConstIter& cbegin() const { return begin; }

    const ConstIter& cend() const { return end; }

    std::string decorate(const token::Token& token) const;

    std::string decorate(Size lineStart, Size lineNo, Size offset,
                         std::string_view value) const;

private:
    ConstIter begin{nullptr};
    ConstIter end{nullptr};
};
} // namespace src
} // namespace front
} // namespace wacc