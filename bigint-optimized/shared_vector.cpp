//
// Created by sancho20021 on 07.07.2020.
//

#include "shared_vector.h"
#include <cassert>

shared_vector::shared_vector() {
    data_ = new buffer();
    data_->ref_counter = 1;
}

shared_vector::shared_vector(std::vector<uint32_t> const &vector) : shared_vector() {
    data_->vec = vector;
}

shared_vector::shared_vector(shared_vector const &other) {
    data_ = other.data_;
    ++data_->ref_counter;
}

shared_vector::~shared_vector() {
    --data_->ref_counter;
    if (data_->ref_counter == 0)
        delete data_;
}

void shared_vector::swap(shared_vector &other) {
    std::swap(data_, other.data_);
}

shared_vector &shared_vector::operator=(shared_vector rhs) {
    swap(rhs);
    return *this;
}

size_t shared_vector::size() const { return data_->vec.size(); }

uint32_t const &shared_vector::operator[](size_t i) const {
    return data_->vec[i];
}

uint32_t &shared_vector::operator[](size_t index) {
    ensure_uniqueness();
    return data_->vec[index];
}

void shared_vector::unshare() {
    auto *new_data = new buffer();
    new_data->vec = data_->vec;
    --data_->ref_counter;
    assert(data_->ref_counter != 0);
    data_ = new_data;
    data_->ref_counter = 1;
}

void shared_vector::ensure_uniqueness() {
    if (data_->ref_counter != 1) {
        unshare();
    }
}

void shared_vector::push_back(uint32_t x) {
    ensure_uniqueness();
    data_->vec.push_back(x);
}

void shared_vector::pop_back() {
    ensure_uniqueness();
    data_->vec.pop_back();
}

void shared_vector::resize(size_t new_size) {
    ensure_uniqueness();
    data_->vec.resize(new_size);
}

shared_vector::iterator shared_vector::begin() noexcept {
    return data_->vec.begin();
}

shared_vector::iterator shared_vector::end() noexcept {
    return data_->vec.end();
}

shared_vector::const_iterator shared_vector::begin() const noexcept {
    return data_->vec.begin();
}

shared_vector::const_iterator shared_vector::end() const noexcept {
    return data_->vec.end();
}

size_t shared_vector::capacity() const {
    return data_->vec.capacity();
}

