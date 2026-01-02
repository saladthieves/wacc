#pragma once

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
#include <string_view>

namespace wacc {
namespace test {
namespace base {
namespace {
using back::emit::AsmEmitter;
using back::gen::AsmGenerator;
using back::pass::AsmInstrFixPass;
using back::pass::AsmPseudoPass;
using back::write::AsmWriter;
using front::lex::Lexer;
using front::parse::Parser;
using front::src::Source;
using tacky::gen::TackyGenerator;
using wacc::utils::Platform;
using wacc::utils::PlatformType;

using std::string_view;
using std::filesystem::path;
} // namespace

class BaseTest {
protected:
    Source getSource(string_view code) const;

    Lexer getLexer(string_view code) const;

    Parser getParser(string_view code) const;

    TackyGenerator getTackyGenerator(string_view code) const;

    AsmGenerator getAsmGenerator(string_view code) const;

    AsmPseudoPass getAsmPseudoPass(string_view code) const;

    AsmInstrFixPass getAsmInstrFixPass(string_view code) const;

    AsmEmitter getAsmEmitter(Platform platform, string_view code) const;

    AsmWriter getAsmWriter(path path, string_view code,
                           Platform platform = Platform{}) const;

    Platform getPlatform() const;

    Platform getMacOSPlatform() const;

    Platform getLinuxPlatform() const;

    Platform getUnknownPlatform() const;
};
} // namespace base
} // namespace test
} // namespace wacc