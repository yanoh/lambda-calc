auto ZERO = [](auto f) {
    return [](auto x) { return x; };
};

auto INC = [](auto n) {
    return [=](auto f) {
        return [=](auto x) {
            return f(n(f)(x));
        };
    };
};
auto ADD = [](auto m) {
    return [=](auto n) {
        return n(INC)(m);
    };
};
auto MUL = [](auto m) {
    return [=](auto n) {
        return n(ADD(m))(ZERO);
    };
};

auto CONS = [](auto x) {
    return [=](auto y) {
        return [=](auto f) {
            return f(x)(y);
        };
    };
};
auto CAR = [](auto p) {
    return p([](auto x) { return [=](auto y) { return x; }; });
};
auto CDR = [](auto p) {
    return p([](auto x) { return [=](auto y) { return y; }; });
};

auto DEC = [](auto n) {
    return CAR(n([](auto p) { return CONS(CDR(p))(INC(CDR(p))); })
                (CONS(ZERO)(ZERO)));
};
auto SUB = [](auto m) {
    return [=](auto n) {
        return n(DEC)(m);
    };
};

auto ONE       = INC(ZERO);
auto TWO       = INC(ONE);
auto THREE     = INC(TWO);
auto FOUR      = INC(THREE);
auto FIVE      = INC(FOUR);

auto TEN       = MUL(FIVE)(TWO);
auto TWENTY    = MUL(TEN)(TWO);
auto FIFTY     = ADD(MUL(TWENTY)(TWO))(TEN);
auto HUNDRED   = MUL(TWENTY)(FIVE);

auto TRUE = [](auto x) {
    return [=](auto y) {
        return x;
    };
};
auto FALSE = [](auto x) {
    return [=](auto y) {
        return y;
    };
};

auto ZEROP = [](auto n) {
    return n([](auto x) { return FALSE; })(TRUE);
};
auto LT_OR_EQ = [](auto m) {
    return [=](auto n) {
        return ZEROP(SUB(m)(n));
    };
};

auto EMPTY = CONS(TRUE)(TRUE);
auto EMPTYP = CAR;

auto UNSHIFT = [](auto l) {
    return [=](auto x) {
        return CONS(FALSE)(CONS(x)(l));
    };
};

auto HEAD = [](auto l) { return CAR(CDR(l)); };
auto TAIL = [](auto l) { return CDR(CDR(l)); };

auto Z = [](auto f) {
    return [=](auto x) {
        return f([=](auto y) { return x(x)(y); });
    }([=](auto x) { return f([=](auto y) { return x(x)(y); }); });
};

auto IF = [](auto b) { return b; };

auto NOT = [](auto b) {
    return IF(b)(FALSE)(TRUE);
};

auto DIV = Z([](auto f) {
    return [=](auto m) {
        return [=](auto n) {
            return IF(LT_OR_EQ(n)(m))
                     ([=](auto x) { return INC(f(SUB(m)(n))(n))(x); })
                     (ZERO);
        };
    };
});
auto MOD = Z([](auto f) {
    return [=](auto m) {
        return [=](auto n) {
            return IF(LT_OR_EQ(n)(m))
                     ([=](auto x) { return f(SUB(m)(n))(n)(x); })
                     (m);
        };
    };
});

auto RANGE = Z([](auto f) {
    return [=](auto m) {
        return [=](auto n) {
            return IF(LT_OR_EQ(m)(n))
                     ([=](auto x) { return UNSHIFT(f(INC(m))(n))(m)(x); })
                     (EMPTY);
        };
    };
});

auto REDUCE = Z([](auto f) {
    return [=](auto l) {
        return [=](auto x) {
            return [=](auto g) {
                return IF(EMPTYP(l))
                         (x)
                         ([=](auto y) {
                             return g(f(TAIL(l))(x)(g))
                                     (HEAD(l))
                                     (y);
                         });
            };
        };
    };
});

auto PUSH = [](auto l) {
    return [=](auto x) {
        return REDUCE(l)(UNSHIFT(EMPTY)(x))(UNSHIFT);
    };
};

auto MAP = [](auto l) {
    return [=](auto f) {
        return REDUCE(l)(EMPTY)
                     ([=](auto a) {
                         return [=](auto x) {
                             return UNSHIFT(a)(f(x));
                         };
                     });
    };
};

auto SELECT = [](auto l) {
    return [=](auto p) {
        return REDUCE(l)(EMPTY)
                     ([=](auto a) {
                         return [=](auto x) {
                             return IF(p(x))
                                      (UNSHIFT(a)(x))
                                      (a);
                         };
                     });
    };
};

auto COMPLEMENT = [](auto p) {
    return [=](auto x) {
        return NOT(p(x));
    };
};

auto REJECT = [](auto l) {
    return [=](auto p) {
        return SELECT(l)(COMPLEMENT(p));
    };
};

auto SIEVE = [](auto l) {
    return [=](auto n) {
        return REJECT(l)([=](auto m) { return ZEROP(MOD(m)(n)); });
    };
};

auto SIEVE_OF_ERATOSTHENES = Z([](auto f) {
    return [=](auto l) {
        return [=](auto a) {
            return IF(EMPTYP(l))
                     (a)
                     ([=](auto x) {
                         return f(SIEVE(TAIL(l))(HEAD(l)))
                                 (PUSH(a)(HEAD(l)))
                                 (x);
                     });
        };
    };
});

auto PRIME_NUMBERS_UPTO = [](auto n) {
    return SIEVE_OF_ERATOSTHENES(RANGE(TWO)(n))(EMPTY);
};

#include <iostream>

namespace helpers
{
    auto print_number = [](auto n) {
        std::cout << n([](auto x) { return x + 1; })(0);
    };

    auto print_number_pair = [](auto p) {
        std::cout << "(";
        print_number(CAR(p));
        std::cout << " . ";
        print_number(CDR(p));
        std::cout << ")";
    };

    auto print_number_list_aux = [](auto l) {
        (IF(EMPTYP(l))
           ([=](auto) {})
           ([=](auto) {
               print_number(HEAD(l));
               (IF(EMPTYP(TAIL(l)))
                  ([]() {})
                  ([=]() { std::cout << " "; }))();
               print_number_list_aux(TAIL(l));
           })
        )(ZERO);
    };

    auto print_number_list = [](auto l) {
        std::cout << "(";
        print_number_list_aux(l);
        std::cout << ")";
    };
}

int main()
{
    helpers::print_number_list(PRIME_NUMBERS_UPTO(ADD(TWENTY)(TEN)));
    std::cout << std::endl;

    /*
    helpers::print_number_list(SIEVE(RANGE(TWO)(HUNDRED))(TWO));
    std::cout << std::endl;

    helpers::print_number(ONE);
    std::cout << std::endl;

    helpers::print_number(TWO);
    std::cout << std::endl;

    helpers::print_number(THREE);
    std::cout << std::endl;

    helpers::print_number(ADD(ONE)(THREE));
    std::cout << std::endl;

    helpers::print_number(IF(ZEROP(ZERO))(ONE)(ZERO));
    std::cout << std::endl;

    helpers::print_number(IF(ZEROP(FOUR))(ONE)(ZERO));
    std::cout << std::endl;

    helpers::print_number_pair(CONS(ONE)(TWO));
    std::cout << std::endl;

    auto list = UNSHIFT(UNSHIFT(UNSHIFT(EMPTY)(THREE))(TWO))(ONE);
    helpers::print_number_list(list);
    std::cout << std::endl;

    helpers::print_number(DEC(THREE));
    std::cout << std::endl;

    helpers::print_number(SUB(ADD(TWO)(THREE))(FOUR));
    std::cout << std::endl;

    helpers::print_number(SUB(ADD(TWO)(THREE))(ONE));
    std::cout << std::endl;

    helpers::print_number(DIV(FIVE)(TWO));
    std::cout << std::endl;

    helpers::print_number(DIV(TEN)(THREE));
    std::cout << std::endl;

    helpers::print_number(MOD(FOUR)(TWO));
    std::cout << std::endl;

    helpers::print_number(MOD(TEN)(THREE));
    std::cout << std::endl;

    helpers::print_number_list(RANGE(ZERO)(THREE));
    std::cout << std::endl;

    helpers::print_number(REDUCE(RANGE(ZERO)(THREE))(ZERO)(ADD));
    std::cout << std::endl;

    helpers::print_number_list(MAP(RANGE(ZERO)(THREE))(MUL(TWO)));
    std::cout << std::endl;

    helpers::print_number_list(
        SELECT(RANGE(ONE)(TWENTY))
              ([](auto n) { return ZEROP(MOD(n)(TWO)); })
    );
    std::cout << std::endl;

    helpers::print_number_list(
        REJECT(RANGE(ONE)(TWENTY))
              ([](auto n) { return ZEROP(MOD(n)(TWO)); })
    );
    std::cout << std::endl;
    */
}
