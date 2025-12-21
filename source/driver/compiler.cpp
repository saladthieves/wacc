#include "compiler.hpp"
#include "asm_ast.hpp"
#include "asm_emit.hpp"
#include "asm_gen.hpp"
#include "asm_instr_fix_pass.hpp"
#include "asm_pseudo_pass.hpp"
#include "asm_writer.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "source.hpp"
#include "tacky_gen.hpp"
#include "utils.hpp"

#include <filesystem>
#include <format>
#include <stdexcept>

namespace wacc::driver {
namespace {
using back::emit::AsmEmitter;
using back::gen::AsmGenerator;
using back::pass::AsmInstrFixPass;
using back::pass::AsmInstrPtrs;
using back::pass::AsmPseudoPass;
using back::write::AsmWriter;
using front::lex::Lexer;
using front::parse::Parser;
using front::src::Source;
using tacky::gen::TackyGenerator;
} // namespace

CompilerResult runCompiler(const std::string& preprocessed,
                           const DriverArgs& args) {
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

    if (args.lex) return {false};

    auto parser = Parser{std::move(tokens), source};
    auto ast = parser.parse();

    if (args.parse) return {false};

    auto tackyGenerator = TackyGenerator{std::move(ast)};
    auto tackyAst = tackyGenerator.generate();

    if (args.tacky) return {false};

    auto asmGenerator = AsmGenerator{std::move(tackyAst)};
    auto asmAst = asmGenerator.generate();

    auto pseudo = AsmPseudoPass{std::move(asmAst)};
    auto pseudoAst = pseudo.run();

    auto fixPass =
        AsmInstrFixPass{std::move(pseudoAst), pseudo.getAbsoluteOffset()};
    auto fixAst = fixPass.run();

    if (args.codegen) return {false};

    auto emitter = AsmEmitter{std::move(fixAst), utils::Platform{}};
    auto lines = emitter.emit();

    const auto info = utils::getFileInfo(args.path);
    auto output = std::filesystem::path{info.parent};
    output /= std::format("{}.{}", info.stem, "s");

    auto writer = AsmWriter{std::move(lines), output};
    writer.write();

    return {true, output};
}
} // namespace wacc::driver