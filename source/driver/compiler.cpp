#include "compiler.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "generator.hpp"
#include "utils.hpp"

#include <filesystem>
#include <format>
#include <stdexcept>

namespace wacc::driver {
void runCompiler(const std::string& preprocessed, const DriverArgs& args) {
    if (!std::filesystem::exists(preprocessed)) {
        auto message =
            std::format("The provided preprocessed file does not exist: [{}]",
                        preprocessed);
        throw std::runtime_error(message);
    }

    if (!preprocessed.ends_with(".i")) {
        throw std::runtime_error("The preprocessed file must end in .i");
    }

    const auto content = utils::readFile(preprocessed);

    const auto info = utils::getFileInfo(args.path);
    auto sourcePath = std::filesystem::path{args.path};
    auto directory = std::filesystem::directory_iterator{info.parent};
    for (auto& file : directory) {
        const auto& filePath = file.path();
        if (filePath != sourcePath) {
            std::filesystem::remove(filePath);
        }
    }

    auto lexer = core::lex::Lexer{content};
    auto tokens = lexer.scan();
    
    auto parser = core::parse::Parser{std::move(tokens)};
    auto ast = parser.parse();

    auto generator = core::gen::Generator{std::move(ast)};
    generator.generate();
}
} // namespace wacc::driver