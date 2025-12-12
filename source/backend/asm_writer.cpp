#include "asm_writer.hpp"
#include <fstream>
#include <sstream>

namespace wacc::back::write {
AsmWriter::AsmWriter(LinesPtr ptr, std::filesystem::path assembly) :
    lines{std::move(ptr)}, assembly{assembly} {
}

void AsmWriter::write() {
    if (!lines) {
        fail("Writing assembly failed: LinesPtr is null");
    }

    if (lines->empty()) {
        fail("Writing assembly failed: LinesPtr is empty");
    }

    if (assembly.empty()) {
        fail("Writing assembly failed: No output file provided");
    }

    std::stringstream buffer{};

    for (const auto& line : *lines) {
        buffer << line << '\n';
    }

    std::ofstream file{assembly};
    file << buffer.rdbuf();

    file.close();
}
} // namespace wacc::back::write