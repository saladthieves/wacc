#include "base_test.hpp"
#include "asm_emit.hpp"
#include "asm_gen.hpp"
#include "asm_instr_fix_pass.hpp"
#include "asm_pseudo_pass.hpp"
#include "utils.hpp"

namespace wacc::test::base {
Source BaseTest::getSource(string_view code) const {
    return Source{code};
}

Lexer BaseTest::getLexer(string_view code) const {
    return Lexer{getSource(code)};
}

Parser BaseTest::getParser(string_view code) const {
    auto source = getSource(code);
    auto lexer = Lexer{source};
    return Parser{lexer.scan(), source};
}

TackyGenerator BaseTest::getTackyGenerator(string_view code) const {
    auto source = getSource(code);
    auto lexer = Lexer{source};
    auto parser = Parser{lexer.scan(), source};
    return TackyGenerator{parser.parse()};
}

AsmGenerator BaseTest::getAsmGenerator(string_view code) const {
    auto tacky = getTackyGenerator(code);
    return AsmGenerator{tacky.generate()};
}

AsmPseudoPass BaseTest::getAsmPseudoPass(string_view code) const {
    auto generator = getAsmGenerator(code);
    return AsmPseudoPass{generator.generate()};
}

AsmInstrFixPass BaseTest::getAsmInstrFixPass(string_view code) const {
    auto pass = getAsmPseudoPass(code);
    return AsmInstrFixPass{pass.run(), pass.getOffset()};
}

AsmEmitter BaseTest::getAsmEmitter(Platform platform) const {
    auto pass = getAsmInstrFixPass();
    return AsmEmitter{pass.run(), platform};
}

AsmEmitter BaseTest::getAsmEmitter(string_view code) const {
    auto pass = getAsmInstrFixPass(code);
    return AsmEmitter{pass.run(), getPlatform()};
}

AsmEmitter BaseTest::getAsmEmitter(Platform platform, string_view code) const {
    auto pass = getAsmInstrFixPass(code);
    return AsmEmitter{pass.run(), platform};
}

AsmWriter BaseTest::getAsmWriter(path path, string_view code,
                                 Platform platform) const {
    auto emitter = getAsmEmitter(platform, code);
    return AsmWriter{emitter.emit(), path};
}

Platform BaseTest::getPlatform() const {
    return Platform{};
}

Platform BaseTest::getMacOSPlatform() const {
    return Platform{PlatformType::MACOS};
}

Platform BaseTest::getLinuxPlatform() const {
    return Platform{PlatformType::LINUX};
}

Platform BaseTest::getUnknownPlatform() const {
    return Platform{PlatformType::UNKNOWN};
}
} // namespace wacc::test::base