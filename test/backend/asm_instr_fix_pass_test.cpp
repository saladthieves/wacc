#include "asm_ast.hpp"
#include "asm_instr_fix_pass.hpp"
#include "asm_matchers.hpp"
#include "base_test.hpp"

#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>
#include <string>

using namespace wacc::back::ast;
using namespace wacc::test::match;

using wacc::back::pass::AsmInstrFixPass;

using std::make_unique;
using std::string;

class AsmInstrFixPassTest :
    public testing::Test,
    public wacc::test::base::BaseTest {
protected:
    void SetUp() override { instructions = AsmInstrPtrs{}; }

    void addInstr(AsmInstrPtr instr) {
        instructions.push_back(std::move(instr));
    }

    AsmProgPtr getProgram() {
        auto fun = make_unique<AsmFun>("main", std::move(instructions));
        return make_unique<AsmProg>(std::move(fun));
    }

    AsmInstrPtrs instructions;
};

TEST_F(AsmInstrFixPassTest, throwOnNull) {
    // ARRANGE
    auto pass = AsmInstrFixPass{nullptr, 0};
    string error{};

    // ACT
    try {
        pass.run();
    } catch (const std::runtime_error& ex) {
        error = ex.what();
    }

    // ASSERT
    ASSERT_TRUE(error.contains("AsmInstrFixPassError"));
    ASSERT_TRUE(error.contains("root is null"));
}

TEST_F(AsmInstrFixPassTest, genAsmAllocStack) {
    // ARRANGE
    addInstr(make_unique<AsmMov>(std::make_unique<AsmStack>(-4),
                                 std::make_unique<AsmReg>(AsmReg::Type::AX)));
    auto pass = AsmInstrFixPass{getProgram(), 4};

    // ACT
    auto ast = pass.run();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    ASSERT_EQ(body.size(), 2);

    matchAsmAllocStack(body[0], [](auto& value) {
        ASSERT_EQ(value, 4);
    });

    matchAsmMov(body[1], [](auto& src, auto& dest) {
        matchAsmStack(src, -4);
        matchAsmReg(dest, AsmReg::Type::AX);
    });
}

TEST_F(AsmInstrFixPassTest, fixAsmMov) {
    // ARRANGE
    addInstr(make_unique<AsmMov>(make_unique<AsmStack>(-4),
                                 make_unique<AsmStack>(-4)));
    auto pass = AsmInstrFixPass{getProgram(), 4};

    // ACT
    auto ast = pass.run();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    ASSERT_EQ(body.size(), 3);

    matchAsmAllocStack(body[0], [](auto& value) {
        ASSERT_EQ(value, 4);
    });

    matchAsmMov(body[1], [](auto& src, auto& dest) {
        matchAsmStack(src, -4);
        matchAsmReg(dest, AsmReg::Type::R10);
    });

    matchAsmMov(body[2], [](auto& src, auto& dest) {
        matchAsmReg(src, AsmReg::Type::R10);
        matchAsmStack(dest, -4);
    });
}

TEST_F(AsmInstrFixPassTest, noFixAsmMov) {
    // ARRANGE
    addInstr(make_unique<AsmMov>(make_unique<AsmStack>(-4),
                                 make_unique<AsmReg>(AsmReg::Type::R10)));
    auto pass = AsmInstrFixPass{getProgram(), 4};

    // ACT
    auto ast = pass.run();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    ASSERT_EQ(body.size(), 2);

    matchAsmAllocStack(body[0], [](auto& value) {
        ASSERT_EQ(value, 4);
    });

    matchAsmMov(body[1], [](auto& src, auto& dest) {
        matchAsmStack(src, -4);
        matchAsmReg(dest, AsmReg::Type::R10);
    });
}

TEST_F(AsmInstrFixPassTest, fixAsmIdiv) {
    // ARRANGE
    addInstr(make_unique<AsmIdiv>(make_unique<AsmImm>(15)));

    auto pass = AsmInstrFixPass{getProgram(), 4};

    // ACT
    auto ast = pass.run();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    ASSERT_EQ(body.size(), 3);

    matchAsmAllocStack(body[0], [](auto& value) {
        ASSERT_EQ(value, 4);
    });

    matchAsmMov(body[1], [](auto& src, auto& dest) {
        matchAsmImm(src, 15);
        matchAsmReg(dest, AsmReg::Type::R10);
    });

    matchAsmIdiv(body[2], [](auto& op) {
        matchAsmReg(op, AsmReg::Type::R10);
    });
}

TEST_F(AsmInstrFixPassTest, noFixAsmIdiv) {
    // ARRANGE
    addInstr(make_unique<AsmIdiv>(make_unique<AsmReg>(AsmReg::Type::R11)));

    auto pass = AsmInstrFixPass{getProgram(), 4};

    // ACT
    auto ast = pass.run();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    ASSERT_EQ(body.size(), 2);

    matchAsmAllocStack(body[0], [](auto& value) {
        ASSERT_EQ(value, 4);
    });

    matchAsmIdiv(body[1], [](auto& op) {
        matchAsmReg(op, AsmReg::Type::R11); //
    });
}

TEST_F(AsmInstrFixPassTest, fixAsmBinaryMult) {
    // ARRANGE
    addInstr(make_unique<AsmBinary>(AsmBinary::Type::BINARY_MULT,
                                    make_unique<AsmImm>(28),
                                    make_unique<AsmStack>(-4)));
    auto pass = AsmInstrFixPass{getProgram(), 4};

    // ACT
    auto ast = pass.run();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    ASSERT_EQ(body.size(), 4);

    matchAsmAllocStack(body[0], [](auto& value) {
        ASSERT_EQ(value, 4);
    });

    matchAsmMov(body[1], [](auto& src, auto& dest) {
        matchAsmStack(src, -4);
        matchAsmReg(dest, AsmReg::Type::R11);
    });

    matchAsmBinary(body[2], [](auto& op, auto& src, auto& dest) {
        ASSERT_EQ(op, AsmBinary::Type::BINARY_MULT);
        matchAsmImm(src, 28);
        matchAsmReg(dest, AsmReg::Type::R11);
    });

    matchAsmMov(body[3], [](auto& src, auto& dest) {
        matchAsmReg(src, AsmReg::Type::R11);
        matchAsmStack(dest, -4);
    });
}

TEST_F(AsmInstrFixPassTest, noFixAsmBinaryMult) {
    // ARRANGE
    addInstr(make_unique<AsmBinary>(AsmBinary::Type::BINARY_MULT,
                                    make_unique<AsmImm>(28),
                                    make_unique<AsmReg>(AsmReg::Type::R10)));
    auto pass = AsmInstrFixPass{getProgram(), 4};

    // ACT
    auto ast = pass.run();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    ASSERT_EQ(body.size(), 2);

    matchAsmAllocStack(body[0], [](auto& value) {
        ASSERT_EQ(value, 4);
    });

    matchAsmBinary(body[1], [](auto& op, auto& src, auto& dest) {
        ASSERT_EQ(op, AsmBinary::Type::BINARY_MULT);
        matchAsmImm(src, 28);
        matchAsmReg(dest, AsmReg::Type::R10);
    });
}

TEST_F(AsmInstrFixPassTest, fixAsmBinaryAddSub) {
    // ARRANGE
    addInstr(make_unique<AsmBinary>(AsmBinary::Type::BINARY_ADD,
                                    make_unique<AsmStack>(-4),
                                    make_unique<AsmStack>(-4)));
    auto pass = AsmInstrFixPass{getProgram(), 4};

    // ACT
    auto ast = pass.run();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    ASSERT_EQ(body.size(), 3);

    matchAsmAllocStack(body[0], [](auto& value) {
        ASSERT_EQ(value, 4);
    });

    matchAsmMov(body[1], [](auto& src, auto& dest) {
        matchAsmStack(src, -4);
        matchAsmReg(dest, AsmReg::Type::R10);
    });

    matchAsmBinary(body[2], [](auto& op, auto& src, auto& dest) {
        ASSERT_EQ(op, AsmBinary::Type::BINARY_ADD);
        matchAsmReg(src, AsmReg::Type::R10);
        matchAsmStack(dest, -4);
    });
}

TEST_F(AsmInstrFixPassTest, noFixAsmBinaryAddSub) {
    // ARRANGE
    addInstr(make_unique<AsmBinary>(AsmBinary::Type::BINARY_SUB,
                                    make_unique<AsmStack>(-4),
                                    make_unique<AsmReg>(AsmReg::Type::R10)));
    auto pass = AsmInstrFixPass{getProgram(), 4};

    // ACT
    auto ast = pass.run();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    ASSERT_EQ(body.size(), 2);

    matchAsmAllocStack(body[0], [](auto& value) {
        ASSERT_EQ(value, 4);
    });

    matchAsmBinary(body[1], [](auto& op, auto& src, auto& dest) {
        ASSERT_EQ(op, AsmBinary::Type::BINARY_SUB);
        matchAsmStack(src, -4);
        matchAsmReg(dest, AsmReg::Type::R10);
    });
}

TEST_F(AsmInstrFixPassTest, fixAsmBinaryShiftLR) {
    // ARRANGE
    const auto binOp = AsmBinary::Type::BINARY_BIT_LSH;
    addInstr(make_unique<AsmBinary>(binOp, make_unique<AsmStack>(-4),
                                    make_unique<AsmStack>(-8)));
    auto pass = AsmInstrFixPass{getProgram(), 4};

    // ACT
    auto ast = pass.run();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    ASSERT_EQ(body.size(), 3);

    matchAsmAllocStack(body[0], [](auto& value) {
        ASSERT_EQ(value, 4);
    });

    matchAsmMov(body[1], [](auto& src, auto& dest) {
        matchAsmStack(src, -4);
        matchAsmReg(dest, AsmReg::Type::CX);
    });

    matchAsmBinary(body[2], [&](auto& op, auto& src, auto& dest) {
        ASSERT_EQ(op, binOp);
        matchAsmReg(src, AsmReg::Type::CX, AsmReg::Size::BYTE);
        matchAsmStack(dest, -8);
    });
}

TEST_F(AsmInstrFixPassTest, noFixAsmBinaryShiftLR) {
    // ARRANGE
    const auto binOp = AsmBinary::Type::BINARY_BIT_RSH;
    const auto reg = AsmReg::Type::R10;
    addInstr(make_unique<AsmBinary>(binOp, make_unique<AsmImm>(2),
                                    make_unique<AsmReg>(reg)));
    auto pass = AsmInstrFixPass{getProgram(), 4};

    // ACT
    auto ast = pass.run();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    ASSERT_EQ(body.size(), 2);

    matchAsmAllocStack(body[0], [](auto& value) {
        ASSERT_EQ(value, 4);
    });

    matchAsmBinary(body[1], [&](auto& op, auto& src, auto& dest) {
        ASSERT_EQ(op, binOp);
        matchAsmImm(src, 2);
        matchAsmReg(dest, reg, AsmReg::Size::DOUBLE_WORD);
    });
}

TEST_F(AsmInstrFixPassTest, fixAsmBinaryAndXorOr) {
    // ARRANGE
    const auto binOp = AsmBinary::Type::BINARY_BIT_AND;
    addInstr(make_unique<AsmBinary>(binOp, make_unique<AsmStack>(-4),
                                    make_unique<AsmStack>(-8)));
    auto pass = AsmInstrFixPass{getProgram(), 4};

    // ACT
    auto ast = pass.run();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    ASSERT_EQ(body.size(), 3);

    matchAsmAllocStack(body[0], [](auto& value) {
        ASSERT_EQ(value, 4);
    });

    matchAsmMov(body[1], [](auto& src, auto& dest) {
        matchAsmStack(src, -4);
        matchAsmReg(dest, AsmReg::Type::R10);
    });

    matchAsmBinary(body[2], [](auto& op, auto& src, auto& dest) {
        ASSERT_EQ(op, AsmBinary::Type::BINARY_BIT_AND);
        matchAsmReg(src, AsmReg::Type::R10);
        matchAsmStack(dest, -8);
    });
}

TEST_F(AsmInstrFixPassTest, noFixAsmBinaryAndXorOr) {
    // ARRANGE
    const auto binOp = AsmBinary::Type::BINARY_BIT_OR;
    addInstr(make_unique<AsmBinary>(binOp, make_unique<AsmStack>(-4),
                                    make_unique<AsmReg>(AsmReg::Type::R10)));
    auto pass = AsmInstrFixPass{getProgram(), 4};

    // ACT
    auto ast = pass.run();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    ASSERT_EQ(body.size(), 2);

    matchAsmAllocStack(body[0], [](auto& value) {
        ASSERT_EQ(value, 4);
    });

    matchAsmBinary(body[1], [&](auto& op, auto& src, auto& dest) {
        ASSERT_EQ(op, binOp);
        matchAsmStack(src, -4);
        matchAsmReg(dest, AsmReg::Type::R10);
    });
}

TEST_F(AsmInstrFixPassTest, fixAsmBinaryCmpBothStack) {
    // ARRANGE
    addInstr(make_unique<AsmCmp>(make_unique<AsmStack>(-4),
                                 make_unique<AsmStack>(-8)));

    auto pass = AsmInstrFixPass{getProgram(), 4};

    // ACT
    auto ast = pass.run();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    ASSERT_EQ(body.size(), 3);

    matchAsmAllocStack(body[0], [](auto& value) {
        ASSERT_EQ(value, 4);
    });

    matchAsmMov(body[1], [](auto& src, auto& dest) {
        matchAsmStack(src, -4);
        matchAsmReg(dest, AsmReg::Type::R10);
    });

    matchAsmCmp(body[2], [](auto& left, auto& right) {
        matchAsmReg(left, AsmReg::Type::R10);
        matchAsmStack(right, -8);
    });
}

TEST_F(AsmInstrFixPassTest, fixAsmBinaryCmpRightImm) {
    // ARRANGE
    addInstr(make_unique<AsmCmp>(make_unique<AsmStack>(-8),
                                 make_unique<AsmImm>(29)));

    auto pass = AsmInstrFixPass{getProgram(), 8};

    // ACT
    auto ast = pass.run();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    ASSERT_EQ(body.size(), 3);

    matchAsmAllocStack(body[0], [](auto& value) {
        ASSERT_EQ(value, 8);
    });

    matchAsmMov(body[1], [](auto& src, auto& dest) {
        matchAsmImm(src, 29);
        matchAsmReg(dest, AsmReg::Type::R11);
    });

    matchAsmCmp(body[2], [](auto& left, auto& right) {
        matchAsmStack(left, -8);
        matchAsmReg(right, AsmReg::Type::R11);
    });
}

TEST_F(AsmInstrFixPassTest, noFixAsmBinaryCmp) {
    // ARRANGE
    addInstr(make_unique<AsmCmp>(make_unique<AsmReg>(AsmReg::Type::R10),
                                 make_unique<AsmReg>(AsmReg::Type::R11)));
    auto pass = AsmInstrFixPass{getProgram(), 8};
    
    // ACT
    auto ast = pass.run();

    // ASSERT
    const auto& body = matchAsmProg(ast);
    ASSERT_EQ(body.size(), 2);
    
    matchAsmCmp(body[1], [](auto& left, auto& right) {
        matchAsmReg(left, AsmReg::Type::R10);
        matchAsmReg(right, AsmReg::Type::R11);
    });
}