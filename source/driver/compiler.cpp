#include "compiler.hpp"
#include "asm_emit.hpp"
#include "asm_gen.hpp"
#include "asm_writer.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "source.hpp"
#include "utils.hpp"

#include <filesystem>
#include <format>
#include <stdexcept>

namespace wacc::driver {
namespace {
using back::emit::AsmEmitter;
using back::gen::AsmGenerator;
using back::write::AsmWriter;
using front::lex::Lexer;
using front::parse::Parser;
using front::src::Source;
} // namespace

std::string runCompiler(const std::string& preprocessed, const DriverArgs& args) {
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

    auto source = Source{content};
    auto lexer = Lexer{source};
    auto tokens = lexer.scan();

    auto parser = Parser{std::move(tokens), source};
    auto ast = parser.parse();

    auto generator = AsmGenerator{std::move(ast)};
    auto asmAst = generator.generate();

    auto emitter = AsmEmitter{std::move(asmAst), utils::Platform{}};
    auto lines = emitter.emit();

    const auto info = utils::getFileInfo(args.path);
    auto output = std::filesystem::path{info.parent};
    output /= std::format("{}.{}", info.stem, "s");

    auto writer = AsmWriter{std::move(lines), output};
    writer.write();

    return output;
}
} // namespace wacc::driver