#ifndef OOASM_H
#define OOASM_H

#include <memory>
#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include <map>
#include "memory.h"
#include "processor.h"

using std::shared_ptr;
using std::make_shared;
using std::string;

// wyjątek zwracany jak zostanie podany argument nie właściwej wielkości
// w funkcji data
class Invalid_id : public std::exception {
public:
    inline const char *what() const noexcept override {
        return "Invalid identificator";
    }
};

// klasa pomocnicza na identyfikator
class ID {
public:
    explicit ID(const char *what) : name(what) {
        if (name.size() > 10 || name.empty())
            throw Invalid_id();
    }

    // zwraca wartość identyfikatora
    inline string getName() const {
        return name;
    }

private:
    const string name;
};

// ------------------------------- RVALUES -------------------------------------
class RValue {
public:
    virtual WordType evaluate(Memory &memory) const = 0;
    virtual ~RValue() = default;
};

class Num : public RValue {
public:
    explicit Num(WordType number) : value(number) {}

    inline WordType evaluate([[maybe_unused]] Memory &mem) const override {
        return value;
    }

private:
    const WordType value;
};

inline shared_ptr<Num> num(WordType what) {
    return make_shared<Num>(Num(what));
}

class Mem : public RValue {
public:
    explicit Mem(shared_ptr<RValue> what) : inside(std::move(what)) {}

    inline MemType getAddr(Memory &mem) const {
        return static_cast<MemType>(inside->evaluate(mem));
    }

    inline WordType evaluate(Memory &mem) const override {
        return mem.valueAt(getAddr(mem));
    }

private:
    shared_ptr<RValue> inside;
};

inline shared_ptr<Mem> mem(const shared_ptr<RValue>& what) {
    return make_shared<Mem>(Mem(what));
}

class Lea : public RValue {
public:
    explicit Lea(const shared_ptr<ID>& identificator) : id(identificator) {}

    inline WordType evaluate(Memory &mem) const override {
        return mem.getVarAddr(id->getName());
    }

private:
    shared_ptr<ID> id;
};

inline shared_ptr<Lea> lea(const char *what) {
    return make_shared<Lea>(Lea(make_shared<ID>(ID(what))));
}
// --------------------------- Instructions ------------------------------------

class Data : public Instruction {
public:
    explicit Data(const shared_ptr<ID>& identificator,
                  const shared_ptr<Num>&  amount)
            : id(identificator), value(amount) {}

    inline bool isDeclaration() const override {
        return true;
    }

    inline void execute(Memory &memory, [
                        [maybe_unused]] Processor &proc) override {
        memory.addVariable(id->getName(), value->evaluate(memory));
    }

private:
    shared_ptr<ID> id;
    shared_ptr<Num> value;
};

inline shared_ptr<Data> data(const char *what,
                           const shared_ptr<Num>& amount) {
    return make_shared<Data>(Data(make_shared<ID>(ID(what)), amount));
}


class Add : public Instruction {
public:
    Add(shared_ptr<Mem> mem, shared_ptr<RValue> amount) :
            place(std::move(mem)), amount(std::move(amount)) {}

    inline void execute(Memory &mem, Processor &proc) override {
        WordType ans = place->evaluate(mem) + amount->evaluate(mem);
        mem.setValue(place->getAddr(mem), ans);
        proc.setFlags(ans);
    }

private:
    shared_ptr<Mem> place;
    shared_ptr<RValue> amount;
};

inline shared_ptr<Add> add(const shared_ptr<Mem>& mem,
                         const shared_ptr<RValue>& amount) {
    return make_shared<Add>(Add(mem, amount));
}

class Sub : public Instruction {
public:
    Sub(const shared_ptr<Mem>& mem, const shared_ptr<RValue>& amount) :
            place(mem), amount(amount) {}

    inline void execute(Memory &mem, Processor &proc) override {
        WordType ans = place->evaluate(mem) - amount->evaluate(mem);
        mem.setValue(place->getAddr(mem), ans);
        proc.setFlags(ans);
    }

private:
    shared_ptr<Mem> place;
    shared_ptr<RValue> amount;
};

shared_ptr<Sub> sub(const shared_ptr<Mem>& mem,
                         const shared_ptr<RValue>& amount) {
    return make_shared<Sub>(Sub(mem, amount));
}

class Inc : public Instruction {
public:
    explicit Inc(const shared_ptr<Mem>& mem) : place(mem) {}

    inline void execute(Memory &mem, Processor &proc) override {
        WordType ans = place->evaluate(mem) + 1;
        mem.setValue(place->getAddr(mem), ans);
        proc.setFlags(ans);
    }

private:
    shared_ptr<Mem> place;
};

inline shared_ptr<Inc> inc(const shared_ptr<Mem>& mem) {
    return make_shared<Inc>(Inc(mem));
}
class Dec : public Instruction {
public:
    explicit Dec(const shared_ptr<Mem>& mem) : place(mem) {}

    inline void execute(Memory &mem, Processor &proc) override {
        WordType ans = place->evaluate(mem) - 1;
        mem.setValue(place->getAddr(mem), ans);
        proc.setFlags(ans);
    }

private:
    shared_ptr<Mem> place;
};

inline shared_ptr<Dec> dec(const shared_ptr<Mem>& mem) {
    return make_shared<Dec>(Dec(mem));
}


class Mov : public Instruction {
public:
    Mov(const shared_ptr<Mem>& mem, const shared_ptr<RValue>& amount) :
            place(mem), amount(amount) {}

    inline void execute(Memory &mem, Processor &proc) override {
        WordType ans = amount->evaluate(mem);
        mem.setValue(place->getAddr(mem), ans);
        proc.setFlags(ans);
    }

private:
    shared_ptr<Mem> place;
    shared_ptr<RValue> amount;
};

inline shared_ptr<Mov> mov(const shared_ptr<Mem>& mem,
                         const shared_ptr<RValue>& amount) {
    return make_shared<Mov>(Mov(mem, amount));
}

class One : public Instruction {
public:
    explicit One(const shared_ptr<Mem>& mem) : place(mem) {}

    inline void execute(Memory &mem,
                        [[maybe_unused]] Processor &proc) override {
        mem.setValue(place->getAddr(mem), 1);
    }

private:
    shared_ptr<Mem> place;
};

inline shared_ptr<One> one(const shared_ptr<Mem>& mem) {
    return make_shared<One>(One(mem));
}

class Ones : public Instruction {
public:
    explicit Ones(const shared_ptr<Mem>& mem) : place(mem) {}

    inline void execute(Memory &mem, Processor &proc) override {
        if (proc.getSFlag()) {
            mem.setValue(place->getAddr(mem), 1);
        }
    }

private:
    shared_ptr<Mem> place;
};

inline shared_ptr<Ones> ones(const shared_ptr<Mem>& mem) {
    return make_shared<Ones>(Ones(mem));
}

class Onez : public Instruction {
public:
    explicit Onez(const shared_ptr<Mem>& mem) : place(mem) {}

    inline void execute(Memory &mem, Processor &proc) override {
        if (proc.getZFlag()) {
            mem.setValue(place->getAddr(mem), 1);
        }
    }

private:
    shared_ptr<Mem> place;
};

inline shared_ptr<Onez> onez(const shared_ptr<Mem>& mem) {
    return make_shared<Onez>(Onez(mem));
}

#endif //OOASM_H
