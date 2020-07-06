#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <vector>

struct big_integer {
    big_integer();

    big_integer(big_integer const &other);

    big_integer(int a);

    explicit big_integer(std::string const &str);

    ~big_integer();

    big_integer &operator=(big_integer const &other);

    big_integer &operator+=(big_integer const &rhs);

    big_integer &operator-=(big_integer const &rhs);

    big_integer &operator*=(big_integer const &rhs);

    big_integer &operator/=(big_integer const &rhs);

    big_integer &operator%=(big_integer const &rhs);

    big_integer &operator&=(big_integer const &rhs);

    big_integer &operator|=(big_integer const &rhs);

    big_integer &operator^=(big_integer const &rhs);

    big_integer &operator<<=(int rhs);

    big_integer &operator>>=(int rhs);

    big_integer operator+() const;

    big_integer operator-() const;

    big_integer operator~() const;

    big_integer &operator++();

    big_integer operator++(int);

    big_integer &operator--();

    big_integer operator--(int);

    friend bool operator==(big_integer const &a, big_integer const &b);

    friend bool operator!=(big_integer const &a, big_integer const &b);

    friend bool operator<(big_integer const &a, big_integer const &b);

    friend bool operator>(big_integer const &a, big_integer const &b);

    friend bool operator<=(big_integer const &a, big_integer const &b);

    friend bool operator>=(big_integer const &a, big_integer const &b);

    friend big_integer operator+(big_integer const &a, big_integer const &b);

    friend big_integer operator-(big_integer const &a, big_integer const &b);

    friend big_integer operator*(big_integer const &a, big_integer const &b);

    friend big_integer operator/(big_integer const &a, big_integer const &b);

    friend big_integer operator%(big_integer const &a, big_integer const &b);

    friend big_integer operator&(big_integer const &a, big_integer const &b);

    friend big_integer operator|(big_integer const &a, big_integer const &b);

    friend big_integer operator^(big_integer const &a, big_integer const &b);

    friend big_integer operator<<(big_integer const &a, int b);

    friend big_integer operator>>(big_integer const &a, int b);

    friend std::string to_string(big_integer const &a);

private:
    void remove_leading_zeros();

    friend void invert(big_integer &x);

    big_integer two_completement(size_t) const; 

    big_integer to_unsigned() const;

    std::pair<big_integer, uint32_t> div_long_short(uint32_t x) const;

    friend big_integer logical_op(big_integer const &a, big_integer const &b,
                           uint32_t (*op)(uint32_t a, uint32_t b));

    bool smaller(big_integer const &dq, uint64_t k, uint64_t m) const;

    uint32_t trial(uint64_t k, uint64_t m, uint64_t const d2) const;

    void difference(big_integer const &dq, uint64_t k, uint64_t m);

    bool is_zero() const;

    friend int positive_equal_size_compare(big_integer const &a, big_integer const &b);

    friend bool a_smaller_b(big_integer const &a, big_integer const &b);

private:
    bool sign;
    std::vector<uint32_t> digits;
};

big_integer operator+(big_integer const &a, big_integer const &b);

big_integer operator-(big_integer const &a, big_integer const &b);

big_integer operator*(big_integer const &a, big_integer const &b);

big_integer operator/(big_integer const &a, big_integer const &b);

big_integer operator%(big_integer const &a, big_integer const &b);

big_integer operator&(big_integer const &a, big_integer const &b);

big_integer operator|(big_integer const &a, big_integer const &b);

big_integer operator^(big_integer const &a, big_integer const &b);

big_integer operator<<(big_integer const &a, int b);

big_integer operator>>(big_integer const &a, int b);

bool operator==(big_integer const &a, big_integer const &b);

bool operator!=(big_integer const &a, big_integer const &b);

bool operator<(big_integer const &a, big_integer const &b);

bool operator>(big_integer const &a, big_integer const &b);

bool operator<=(big_integer const &a, big_integer const &b);

bool operator>=(big_integer const &a, big_integer const &b);

std::string to_string(big_integer const &a);

std::ostream &operator<<(std::ostream &stream, big_integer const &number);

#endif