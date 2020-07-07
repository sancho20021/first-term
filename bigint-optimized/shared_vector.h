//
// Created by sancho20021 on 07.07.2020.
//

#ifndef BIGINT_SHARED_VECTOR_H
#define BIGINT_SHARED_VECTOR_H


#include <cstddef>
#include <cstdint>
#include <vector>

struct shared_vector {
    using iterator = std::vector<uint32_t>::iterator;
    using const_iterator = std::vector<uint32_t>::const_iterator;
private:
    struct buffer {
        size_t ref_counter;
        std::vector<uint32_t> vec;
    };
    buffer *data_;

private:
    void unshare();

    void ensure_uniqueness();

public:
    shared_vector();

    explicit shared_vector(std::vector<uint32_t> const &vector);

    shared_vector(shared_vector const &other);

    ~shared_vector();

    void swap(shared_vector &other);

    shared_vector &operator=(shared_vector rhs);

    size_t size() const;

    void push_back(uint32_t x);

    void pop_back();

    void resize(size_t new_size);

    uint32_t const &operator[](size_t i) const;

    uint32_t &operator[](size_t);

    const_iterator begin() const noexcept;

    iterator begin() noexcept;

    const_iterator end() const noexcept;

    iterator end() noexcept;

    size_t capacity() const;
};


#endif //BIGINT_SHARED_VECTOR_H
