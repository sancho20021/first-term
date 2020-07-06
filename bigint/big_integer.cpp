#include "big_integer.h"

#include <algorithm>
#include "cmath"
#include <stdexcept>

using uint128_t = unsigned __int128;

big_integer::big_integer() {
    digits.push_back(0);
    sign = false;
}

big_integer::big_integer(big_integer const &other) :
        sign(other.sign),
        digits(other.digits) {}

big_integer::big_integer(int a) {
    sign = a < 0;
    digits.push_back(a == INT32_MIN ? 2147483648u : static_cast<uint32_t>(abs(a)));
}

big_integer::big_integer(std::string const &str) : big_integer() {
    auto len = str.length();
    uint32_t base = 1000000000;
    bool tmp_sign = str[0] == '-';
    size_t i = 0;
    if (tmp_sign) {
        i++;
    }
    for (; i < len; i += 9) {
        uint32_t short_number = 0;
        size_t delta = std::min(len - i, 9ul);
        for (size_t j = i; j < i + delta; j++) {
            short_number *= 10;
            short_number += static_cast<uint32_t>(str[j] - '0');
        }
        *this *= delta == 9ul ? base : static_cast<uint32_t>(std::pow(10, delta));
        *this += short_number;
    }
    sign = tmp_sign;
    remove_leading_zeros();
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
    x.sign = false;
    x += 1;
    x.sign = true;
}

big_integer big_integer::two_completement(size_t nec_length) const {
    big_integer res(*this);
    res.digits.resize(nec_length + 1);
    invert(res);
    return res;
}

big_integer big_integer::to_unsigned() const {
    big_integer res(*this);
    invert(res);
    res.remove_leading_zeros();
    return res;
}

bool big_integer::is_zero() const {
    return digits.size() == 1 && digits[0] == 0;
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
    if (!res.is_zero()) {
        res.sign = !res.sign;
    } else {
        res.sign = false;
    }
    return res;
}

big_integer big_integer::operator~() const {
    return -big_integer(*this) - 1;
}

big_integer &big_integer::operator++() {
    return *this = *this + 1;
}

big_integer big_integer::operator++(int) {
    big_integer res = *this;
    *this = *this + 1;
    return res;
}

big_integer &big_integer::operator--() {
    return *this = *this - 1;
}

big_integer big_integer::operator--(int) {
    big_integer res = *this;
    *this = *this - 1;
    return res;
}

big_integer operator+(big_integer const &a, big_integer const &b) {
    if (b.sign) {
        return a - (-b);
    } else if (a.sign) {
        return b - (-a);
    }
    big_integer res = a;
    uint64_t carry = 0;
    for (size_t i = 0; i < std::max(res.digits.size(), b.digits.size()) || carry; i++) {
        if (i == res.digits.size()) {
            res.digits.push_back(0);
        }
        uint64_t add_res = res.digits[i] + carry + (i < b.digits.size() ? b.digits[i] : 0);
        carry = add_res > static_cast<uint64_t>(UINT32_MAX) ? 1 : 0;
        add_res -= carry * (static_cast<uint64_t>(UINT32_MAX) + 1);
        res.digits[i] = static_cast<uint32_t>(add_res);
    }
    return res;
}

big_integer operator-(big_integer const &a, big_integer const &b) {
    if (a.sign) {
        return -((-a) + b);
    } else if (b.sign) {
        return a + (-b);
    }
    bool res_sign = a < b;
    auto big = res_sign ? b : a;
    auto small = res_sign ? a : b;
    int64_t carry = 0;
    for (size_t i = 0; i < small.digits.size() || carry; i++) {
        int64_t sub_res = big.digits[i] - (carry + (i < small.digits.size() ? small.digits[i] : 0));
        carry = (sub_res < 0) ? 1 : 0;
        if (carry) {
            sub_res += static_cast<uint64_t>(UINT32_MAX) + 1;
        }
        big.digits[i] = static_cast<uint32_t>(sub_res);
    }
    big.remove_leading_zeros();
    big.sign = big.is_zero() ? false : res_sign;
    return big;
}

big_integer operator*(big_integer const &a, big_integer const &b) {
    big_integer res;
    if (a.is_zero() || b.is_zero()) {
        return res;
    }
    res.digits.resize(a.digits.size() + b.digits.size());
    res.sign = a.sign ^ b.sign;
    uint64_t carry = 0;
    for (uint32_t i = 0; i < a.digits.size(); i++) {
        for (uint32_t j = 0; j < b.digits.size() || carry; j++) {
            uint128_t mul_res =
                    res.digits[i + j] + carry +
                    static_cast<uint64_t>(a.digits[i]) * (j < b.digits.size() ? b.digits[j] : 0);
            res.digits[i + j] = static_cast<uint32_t>(mul_res % (static_cast<uint64_t>(UINT32_MAX) + 1));
            carry = static_cast<uint32_t>(mul_res / (static_cast<uint64_t>(UINT32_MAX) + 1));
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
    for (int32_t i = static_cast<int32_t>(digits.size()) - 1; i >= 0; i--) {
        uint64_t cur_val = digits[i] + carry * (static_cast<uint64_t>(UINT32_MAX) + 1);
        res.digits[i] = static_cast<uint32_t>(cur_val / x);
        carry = cur_val % x;
    }
    res.sign = res.is_zero() ? false : sign;
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
    const uint128_t base = uint128_t(UINT32_MAX) + 1;
    uint64_t km = k + m;
    uint128_t r3 = (uint128_t(digits[km]) * base + uint128_t(digits[km - 1])) * base + uint128_t(digits[km - 2]);
    return uint32_t(std::min(r3 / uint128_t(d2), uint128_t(UINT32_MAX)));
}

void big_integer::difference(big_integer const &dq, uint64_t k, uint64_t m) {
    int64_t borrow = 0, diff;
    const int64_t base = int64_t(UINT32_MAX) + 1;
    for (uint64_t i = 0; i < m + 1; i++) {
        diff = int64_t(digits[i + k]) - int64_t(dq.digits[i]) - borrow + base;
        digits[i + k] = uint32_t(diff % base);
        borrow = 1 - diff / base;
    }
}

big_integer operator/(big_integer const &a, big_integer const &b) {
    if (b.is_zero()) {
        throw std::runtime_error("Division by zero");
    } else if (a.digits.size() < b.digits.size()) {
        big_integer res;
        return res;
    } else if (b.digits.size() == 1) {
        return !b.sign ? a.div_long_short(b.digits[0]).first : -(a.div_long_short(b.digits[0]).first);
    }
    bool res_sign = a.sign ^b.sign;
    auto n = a.digits.size(), m = b.digits.size();
    auto f = (1L << 32) / (uint64_t(b.digits[m - 1]) + 1);
    big_integer r = a * f;
    big_integer d = b * f;
    r.sign = false;
    d.sign = false;
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
    big_integer trimmed_a = a.two_completement(max_size);
    big_integer trimmed_b = b.two_completement(max_size);
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
    res.digits.pop_back();
    for (uint32_t i = 0; i < digits_shift_amount; i++) {
        res.digits.push_back(0);
    }
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
    res.sign = res.is_zero() ? false : a.sign;
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
        num = num.two_completement(num.digits.size() + digits_shift_amount);
    }
    big_integer res;
    if (digits_shift_amount >= num.digits.size()) {
        return res;
    }
    res.digits.pop_back();
    for (uint32_t i = digits_shift_amount; i < num.digits.size(); i++) {
        res.digits.push_back(num.digits[i]);
    }
    for (uint32_t i = 0; i < res.digits.size() - 1; i++) {
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
    res.sign = res.is_zero() ? false : res.sign;
    return res;
}

bool operator==(big_integer const &a, big_integer const &b) {
    return !(a != b);
}

bool operator!=(big_integer const &a, big_integer const &b) {
    if (a.digits.size() != b.digits.size() || a.sign != b.sign) {
        return true;
    }
    for (uint32_t i = 0; i < a.digits.size(); i++) {
        if (a.digits[i] != b.digits[i]) {
            return true;
        }
    }
    return false;
}

int positive_equal_size_compare(big_integer const &a, big_integer const &b) {
    for (auto i = static_cast<uint32_t>(a.digits.size()); i > 0; i--) {
        if (a.digits[i - 1] > b.digits[i - 1]) {
            return 1;
        } else if (a.digits[i - 1] < b.digits[i - 1]) {
            return -1;
        }
    }
    return 0;
}

bool a_smaller_b(big_integer const &a, big_integer const &b) {
    if (a.sign != b.sign) {
        return a.sign;
    } else if (a.digits.size() < b.digits.size()) {
        return !a.sign;
    } else if (a.digits.size() > b.digits.size()) {
        return a.sign;
    } else {
        return (-a) > (-b);
    }
}

bool operator<(big_integer const &a, big_integer const &b) {
    if (a.sign == b.sign && !a.sign && a.digits.size() == b.digits.size()) {
        return positive_equal_size_compare(a, b) == -1;
    } else {
        return a_smaller_b(a, b);
    }
}

bool operator>(big_integer const &a, big_integer const &b) {
    if (a.sign == b.sign && !a.sign && a.digits.size() == b.digits.size()) {
        return positive_equal_size_compare(a, b) == 1;
    } else {
        return !a_smaller_b(a, b);
    }
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return !(a > b);
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return !(a < b);
}

std::string to_string(big_integer const &a) {
    if (a.is_zero()) {
        return "0";
    }
    std::string str;
    big_integer rest = a;
    while (!rest.is_zero()) {
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

std::ostream &operator<<(std::ostream &stream, big_integer const &number) {
    stream << to_string(number);
    return stream;
}