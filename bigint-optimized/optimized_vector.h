//
// Created by sancho20021 on 07.07.2020.
//

#ifndef BIGINT_OPTIMIZED_VECTOR_H
#define BIGINT_OPTIMIZED_VECTOR_H

#include <cstdint>
#include <cstdlib>
#include "shared_vector.h"

struct optimized_vector {
private:
    static const size_t SMALL_SIZE = sizeof(shared_vector) / sizeof(uint32_t);
    union buffer {
        shared_vector *dynamic_buffer;
        uint32_t static_buffer[SMALL_SIZE];
    };
    bool is_big;
    size_t size_;
    buffer buf_;

private:
    void ensure_capacity(size_t cap);

public:
    optimized_vector();

    optimized_vector(optimized_vector const &other);

    explicit optimized_vector(std::vector<uint32_t> const &other);

    ~optimized_vector();

    optimized_vector &operator=(optimized_vector other);

    void push_back(uint32_t x);

    void pop_back();

    void resize(size_t sz);

    uint32_t const &operator[](size_t index) const;

    uint32_t &operator[](size_t index);

    size_t size() const;

    void swap(optimized_vector &other);

    size_t capacity() const;
};


#endif //BIGINT_OPTIMIZED_VECTOR_H
