#include "big_integer.h"

#include <algorithm>
#include "cmath"
#include <stdexcept>
#include <cassert>

using uint128_t = unsigned __int128;
const big_integer big_integer::ZERO(0);
const big_integer big_integer::ONE(1);
const uint64_t UINT32_MAX_L = static_cast<uint64_t>(UINT32_MAX);

big_integer::big_integer() {
    digits.push_back(0);
    sign = false;
}

big_integer::big_integer(big_integer const &other) = default;

big_integer::big_integer(int a) {
    sign = a < 0;
    digits.push_back(a == INT32_MIN ? INT32_MIN_POSITIVE : static_cast<uint32_t>(std::abs(a)));
}

const size_t DIGITS_IN_UINT32_T = 9;

big_integer::big_integer(std::string const &str) : big_integer() {
    auto len = str.length();
    uint32_t const BASE = 1000000000;
    bool tmp_sign = str[0] == '-';
    size_t i = tmp_sign;
    for (; i < len; i += DIGITS_IN_UINT32_T) {
        uint32_t short_number = 0;
        size_t delta = std::min(len - i, DIGITS_IN_UINT32_T);
        for (size_t j = i; j < i + delta; j++) {
            short_number *= 10;
            short_number += static_cast<uint32_t>(str[j] - '0');
        }
        *this *= delta == DIGITS_IN_UINT32_T ? BASE : static_cast<uint32_t>(std::pow(10, delta));
        *this += short_number;
    }
    remove_leading_zeros();
    if (*this != ZERO) {
        sign = tmp_sign;
    }
}

big_integer::~big_integer() = default;

void big_integer::remove_leading_zeros() {
    while (digits.size() > 1 && digits[digits.size() - 1] == 0) {
        digits.pop_back();
    }
}

void invert(big_integer &x) {
    if (!x.sign) {
        return;
    }
    for (auto &i : x.digits) {
        i = ~i;
    }
    x = add_abs(x, big_integer::ONE, true);
}

big_integer big_integer::two_completement(size_t nec_length) const {
    big_integer res(*this);
    while (res.digits.size() < nec_length) {
        res.digits.push_back(0);
    }
    invert(res);
    return res;
}

big_integer big_integer::to_unsigned() const {
    big_integer res(*this);
    invert(res);
    res.remove_leading_zeros();
    return res;
}

big_integer &big_integer::operator=(big_integer const &other) = default;

big_integer &big_integer::operator+=(big_integer const &b) {
    return (*this = *this + b);
}

big_integer &big_integer::operator-=(big_integer const &b) {
    return (*this = *this - b);
}

big_integer &big_integer::operator*=(big_integer const &b) {
    return (*this = *this * b);
}

big_integer &big_integer::operator/=(big_integer const &b) {
    return (*this = *this / b);
}

big_integer &big_integer::operator%=(big_integer const &b) {
    return (*this = *this % b);
}

big_integer &big_integer::operator&=(big_integer const &b) {
    return (*this = *this & b);
}

big_integer &big_integer::operator|=(big_integer const &b) {
    return (*this = *this | b);
}

big_integer &big_integer::operator^=(big_integer const &b) {
    return (*this = *this ^ b);
}

big_integer &big_integer::operator<<=(int b) {
    return (*this = *this << b);
}

big_integer &big_integer::operator>>=(int b) {
    return (*this = *this >> b);
}

big_integer big_integer::operator+() const { return *this; }

big_integer big_integer::operator-() const {
    big_integer res(*this);
    res.sign = res == ZERO ? false : !res.sign;
    return res;
}

big_integer big_integer::operator~() const {
    return -big_integer(*this) - ONE;
}

big_integer &big_integer::operator++() {
    return *this = *this + ONE;
}

big_integer big_integer::operator++(int) {
    big_integer res = *this;
    *this = *this + ONE;
    return res;
}

big_integer &big_integer::operator--() {
    return *this = *this - ONE;
}

big_integer big_integer::operator--(int) {
    big_integer res = *this;
    *this = *this - ONE;
    return res;
}

big_integer add_abs(big_integer const &a, big_integer const &b, bool res_sign) {
    big_integer res = big_integer::abs(a);
    uint64_t carry = 0;
    for (size_t i = 0; i < std::max(res.digits.size(), b.digits.size()) || carry; i++) {
        if (i == res.digits.size()) {
            res.digits.push_back(0);
        }
        uint64_t add_res = res.digits[i] + carry + (i < b.digits.size() ? b.digits[i] : 0);
        carry = add_res > UINT32_MAX_L ? 1 : 0;
        add_res -= carry * (UINT32_MAX_L + 1);
        res.digits[i] = static_cast<uint32_t>(add_res);
    }
    res.sign = res == big_integer::ZERO ? false : res_sign;
    return res;
}

big_integer sub_abs(big_integer const &a, big_integer const &b, bool res_sign) {
    bool less = abs_compare(a, b) == -1;
    auto big = big_integer::abs(less ? b : a);
    auto small = less ? a : b;
    int64_t carry = 0;
    small.digits.resize(big.digits.size());
    for (size_t i = 0; i < small.digits.size(); i++) {
        int64_t sub_res = big.digits[i] - carry - small.digits[i];
        carry = (sub_res < 0) ? 1 : 0;
        if (carry) {
            sub_res += UINT32_MAX_L + 1;
        }
        big.digits[i] = static_cast<uint32_t>(sub_res);
    }
    big.remove_leading_zeros();
    big.sign = big == big_integer::ZERO ? false : res_sign;
    return big;
}

big_integer add_or_sub(big_integer const &a, big_integer const &b, bool different_signs) {
    if (different_signs) {
        return sub_abs(a, b, a.sign != (abs_compare(a, b) == -1));
    } else {
        return add_abs(a, b, a.sign);
    }
}

big_integer operator+(big_integer const &a, big_integer const &b) {
    return add_or_sub(a, b, a.sign != b.sign);
}

big_integer operator-(big_integer const &a, big_integer const &b) {
    return add_or_sub(a, b, a.sign == b.sign);
}

big_integer operator*(big_integer const &a, big_integer const &b) {
    big_integer res;
    if (a == big_integer::ZERO || b == big_integer::ZERO) {
        return res;
    }
    res.digits.resize(a.digits.size() + b.digits.size());
    res.sign = a.sign ^ b.sign;
    uint64_t carry = 0;
    for (size_t i = 0; i < a.digits.size(); i++) {
        for (uint32_t j = 0; j < b.digits.size() || carry; j++) {
            uint128_t mul_res =
                    res.digits[i + j] + carry +
                    static_cast<uint64_t>(a.digits[i]) * (j < b.digits.size() ? b.digits[j] : 0);
            res.digits[i + j] = static_cast<uint32_t>(mul_res % (UINT32_MAX_L + 1));
            carry = static_cast<uint32_t>(mul_res / (UINT32_MAX_L + 1));
        }
    }
    res.remove_leading_zeros();
    return res;
}

std::pair<big_integer, uint32_t> big_integer::div_long_short(uint32_t x) const {
    if (x == 0) {
        throw std::runtime_error("Division by zero");
    }
    uint64_t carry = 0;
    big_integer res;
    res.digits.pop_back();
    res.digits.resize(digits.size());
    for (size_t i = digits.size(); i > 0; i--) {
        uint64_t cur_val = digits[i - 1] + carry * (UINT32_MAX_L + 1);
        res.digits[i - 1] = static_cast<uint32_t>(cur_val / x);
        carry = cur_val % x;
    }
    res.sign = res == ZERO ? false : sign;
    res.remove_leading_zeros();
    return {res, carry};
}

bool big_integer::smaller(big_integer const &dq, uint64_t k, uint64_t m) const {
    uint64_t i = m, j = 0;
    while (i != j) {
        if (digits[i + k] != dq.digits[i]) {
            j = i;
        } else {
            i--;
        }
    }
    return digits[i + k] < dq.digits[i];
}

uint32_t big_integer::trial(uint64_t k, uint64_t m, uint64_t const d2) const {
    const uint128_t BASE = uint128_t(UINT32_MAX) + 1;
    uint64_t km = k + m;
    uint128_t r3 = (uint128_t(digits[km]) * BASE + uint128_t(digits[km - 1])) * BASE + uint128_t(digits[km - 2]);
    return uint32_t(std::min(r3 / uint128_t(d2), uint128_t(UINT32_MAX)));
}

void big_integer::difference(big_integer const &dq, uint64_t k, uint64_t m) {
    int64_t borrow = 0, diff;
    const int64_t BASE = int64_t(UINT32_MAX) + 1;
    for (uint64_t i = 0; i < m + 1; i++) {
        diff = int64_t(digits[i + k]) - int64_t(dq.digits[i]) - borrow + BASE;
        digits[i + k] = uint32_t(diff % BASE);
        borrow = 1 - diff / BASE;
    }
}

big_integer operator/(big_integer const &a, big_integer const &b) {
    if (b == big_integer::ZERO) {
        throw std::runtime_error("Division by zero");
    } else if (a.digits.size() < b.digits.size()) {
        return big_integer::ZERO;
    } else if (b.digits.size() == 1) {
        return !b.sign ? a.div_long_short(b.digits[0]).first : -(a.div_long_short(b.digits[0]).first);
    }
    bool res_sign = a.sign ^b.sign;
    auto n = a.digits.size(), m = b.digits.size();
    auto f = (1L << 32) / (uint64_t(b.digits[m - 1]) + 1);
    big_integer r = big_integer::abs(a * f);
    big_integer d = big_integer::abs(b * f);
    big_integer q;
    q.digits.resize(n - m + 1);
    r.digits.push_back(0);
    const uint64_t d2 = (static_cast<uint64_t>(d.digits[m - 1]) << 32) + uint64_t(d.digits[m - 2]);
    for (auto k = int32_t(n - m); k >= 0; k--) {
        auto qt = r.trial(uint64_t(k), m, d2);
        big_integer qt_mul;
        qt_mul.digits[0] = qt;
        big_integer dq = qt_mul * d;
        dq.digits.resize(m + 1);
        if (r.smaller(dq, uint64_t(k), m)) {
            qt--;
            dq = d * qt;
        }
        q.digits[k] = qt;
        r.difference(dq, uint64_t(k), m);
    }
    q.sign = res_sign;
    q.remove_leading_zeros();
    return q;
}

big_integer operator%(big_integer const &a, big_integer const &b) {
    return a - (a / b) * b;
}

big_integer logical_op(big_integer const &a, big_integer const &b, uint32_t (*op)(uint32_t, uint32_t)) {
    size_t max_size = std::max(a.digits.size(), b.digits.size());
    big_integer trimmed_a = a.two_completement(max_size + 1);
    big_integer trimmed_b = b.two_completement(max_size + 1);
    for (size_t i = 0; i < trimmed_b.digits.size(); i++) {
        trimmed_a.digits[i] = (*op)(trimmed_a.digits[i], trimmed_b.digits[i]);
    }
    trimmed_a.sign = (*op)(a.sign, b.sign);
    trimmed_a.remove_leading_zeros();
    return trimmed_a.to_unsigned();
}

big_integer operator&(big_integer const &a, big_integer const &b) {
    return logical_op(a, b, [](uint32_t x, uint32_t y) { return x & y; });
}

big_integer operator|(big_integer const &a, big_integer const &b) {
    return logical_op(a, b, [](uint32_t x, uint32_t y) { return x | y; });
}

big_integer operator^(big_integer const &a, big_integer const &b) {
    return logical_op(a, b, [](uint32_t x, uint32_t y) { return x ^ y; });
}

const uint32_t BITS_IN_DIGIT = 32;

big_integer operator<<(big_integer const &a, int b) {
    if (b < 0) {
        return a >> abs(b);
    } else if (b == 0) {
        return big_integer(a);
    }
    auto digits_shift_amount = static_cast<uint32_t>(b);
    auto bits_shift_amount = digits_shift_amount % BITS_IN_DIGIT;
    digits_shift_amount /= BITS_IN_DIGIT;
    big_integer res;
    res.digits.resize(digits_shift_amount);
    for (auto i : a.digits) {
        res.digits.push_back(i);
    }
    uint32_t remainder = 0;
    for (size_t i = digits_shift_amount; i < res.digits.size(); i++) {
        uint32_t tmp = (res.digits[i] << bits_shift_amount);
        tmp += remainder;
        remainder = (res.digits[i] >> (BITS_IN_DIGIT - bits_shift_amount));
        res.digits[i] = tmp;
    }
    res.digits.push_back(remainder);
    res.remove_leading_zeros();
    res.sign = res == big_integer::ZERO ? false : a.sign;
    return res;
}

big_integer operator>>(big_integer const &a, int b) {
    if (b < 0) {
        return a << abs(b);
    } else if (b == 0) {
        return big_integer(a);
    }
    big_integer num = a;
    auto digits_shift_amount = static_cast<uint32_t>(b);
    auto bits_shift_amount = digits_shift_amount % BITS_IN_DIGIT;
    digits_shift_amount /= BITS_IN_DIGIT;
    if (num.sign) {
        num = num.two_completement(num.digits.size() + digits_shift_amount + 1);
    }
    big_integer res;
    if (digits_shift_amount >= num.digits.size()) {
        return res;
    }
    res.digits.pop_back();
    for (size_t i = digits_shift_amount; i < num.digits.size(); i++) {
        res.digits.push_back(num.digits[i]);
    }
    for (size_t i = 0; i < res.digits.size() - 1; i++) {
        res.digits[i] >>= bits_shift_amount;
        uint32_t small_shift = res.digits[i + 1] << (BITS_IN_DIGIT - bits_shift_amount);
        res.digits[i] += small_shift;
    }
    res.digits[res.digits.size() - 1] >>= bits_shift_amount;
    res.remove_leading_zeros();
    res.sign = num.sign;
    if (res.sign) {
        res.digits.pop_back();
    }
    res = res.to_unsigned();
    res.sign = res == big_integer::ZERO ? false : res.sign;
    return res;
}

bool operator==(big_integer const &a, big_integer const &b) {
    return !(a != b);
}

bool operator!=(big_integer const &a, big_integer const &b) {
    if (a.digits.size() != b.digits.size() || a.sign != b.sign) {
        return true;
    }
    for (size_t i = 0; i < a.digits.size(); i++) {
        if (a.digits[i] != b.digits[i]) {
            return true;
        }
    }
    return false;
}

int abs_compare(big_integer const &a, big_integer const &b) {
    if (a.digits.size() < b.digits.size()) {
        return -1;
    } else if (a.digits.size() > b.digits.size()) {
        return 1;
    }
    for (size_t i = a.digits.size(); i > 0; i--) {
        if (a.digits[i - 1] > b.digits[i - 1]) {
            return 1;
        } else if (a.digits[i - 1] < b.digits[i - 1]) {
            return -1;
        }
    }
    return 0;
}

bool less_x_greater(big_integer const &a, big_integer const &b, bool sign_flag) {
    if (a.sign != b.sign) {
        return sign_flag;
    } else return sign_flag ? abs_compare(a, b) == 1 : abs_compare(a, b) == -1;
}

bool operator<(big_integer const &a, big_integer const &b) {
    return less_x_greater(a, b, a.sign);
}

bool operator>(big_integer const &a, big_integer const &b) {
    return less_x_greater(a, b, !a.sign);
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return !(a > b);
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return !(a < b);
}

std::string to_string(big_integer const &a) {
    if (a == big_integer::ZERO) {
        return "0";
    }
    std::string str;
    big_integer rest = a;
    while (rest != big_integer::ZERO) {
        auto div_res = rest.div_long_short(10);
        rest = div_res.first;
        str.push_back('0' + char(div_res.second));
    }
    bool is_negative = a.sign;
    if (is_negative) {
        str.push_back('-');
    }
    std::reverse(str.begin(), str.end());
    return str;
}

big_integer big_integer::abs(big_integer const &a) {
    big_integer res(a);
    res.sign = false;
    return res;
}

std::ostream &operator<<(std::ostream &stream, big_integer const &number) {
    stream << to_string(number);
    return stream;
}