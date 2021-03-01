#ifndef COMPUTER_H
#define COMPUTER_H

#include "ooasm.h"

class Computer {
public:
    using MemType = uint_fast64_t;

    explicit Computer(MemType size): memory(size), processor() {}

    // funkcja boot z treści zadania
    inline void boot(const std::vector<shared_ptr<Instruction>>& prog) {
        memory.reset();
        processor.executeOperations(memory, prog);
    }

    // funkcja memory dump z treści zadania
    inline void memory_dump(std::stringstream& str_stream) const {
        memory.memDump(str_stream);
    }

private:
    // komputer ma pamięć
    Memory memory;
    // .. oraz procesor
    Processor processor;
};

#endif //COMPUTER_H
