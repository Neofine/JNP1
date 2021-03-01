#ifndef PROCESSOR_H
#define PROCESSOR_H

class Memory;
class Processor;

// typ opisujący klasę instrukcji jakie mogą być wywoływane w funkcji program
class Instruction {
public:
    virtual void execute(Memory &memory, Processor &proc) = 0;
    virtual bool isDeclaration() const {
        return false;
    }

    virtual ~Instruction() = default;
};

// typ opisujący vektor dzielonych wskaźników na klasę Instruction
using instr_vec = std::vector<std::shared_ptr<Instruction>>;

// typ opisujący listę inicjalizującą dzielonych wskaźników na klasę Instruction
using instr_init_list = std::initializer_list<std::shared_ptr<Instruction>>;

inline instr_vec program(instr_init_list instructions) {
    return std::vector<std::shared_ptr<Instruction>>(instructions);
}

class Processor {
public:
    // ustawia flagi na właściwe
    inline void setFlags(const WordType &x) {
        zFlag = (x == 0);
        sFlag = (x < 0);
    }

    // zwraca zero flag
    inline bool getZFlag() const {
        return zFlag;
    }

    // zwraca signed flag
    inline bool getSFlag() const {
        return sFlag;
    }

    // wykonuje operacje, na początek wszystkie deklaracje, później resztę
    inline void executeOperations(Memory &memory, const instr_vec &prog) {
        for (auto &i : prog)
            if (i->isDeclaration())
                i->execute(memory, *this);

        for (auto &i : prog)
            if (!i->isDeclaration())
                i->execute(memory, *this);

    }

private:
    bool zFlag, sFlag;
};


#endif //PROCESSOR_H
