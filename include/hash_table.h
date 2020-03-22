#ifndef LOX_HASH_TABLE_H
#define LOX_HASH_TABLE_H

#include "contract.h"

namespace lox {

template <typename Key, typename Value>
struct hash_table {
  hash_table() noexcept : entries_{nullptr}, capacity_mask_{-1}, count_{0} {}
  ~hash_table() noexcept {
    if (entries_) {
      delete entries_;
    }
  }

  int size() const noexcept { return count_; }

  bool insert(const Key* key, Value value) noexcept {
    constexpr auto max_load = 0.75;
    if (count_ + 1 > capacity_of(capacity_mask_) * max_load) {
      adjust_capacity();
    }
    entry& dest = find_entry(entries_, capacity_mask_, key);
    if (dest.key == nullptr) {
      dest.key = key;
      dest.value = value;
      ++count_;
      return true;
    }
    return false;
  }

  bool contains(const Key* key) const noexcept {
    entry& dest = find_entry(entries_, capacity_mask_, key);
    return dest.key != nullptr;
  }

  Value& operator[](const Key* key) noexcept {
    entry& dest = find_entry(entries_, capacity_mask_, key);
    ENSURES(dest.key != nullptr);
    return dest.value;
  }

 private:
  struct entry {
    const Key* key = nullptr;
    Value value;
  };

  entry& find_entry(entry* entries, int capacity_mask, const Key* key) const
      noexcept {
    int index = key->hash() & capacity_mask;
    while (true) {
      entry& current = entries[index];
      if (current.key == nullptr || current.key == key) {
        return current;
      }
      index = (index + 1) & capacity_mask;
    }
  }

  void adjust_capacity() noexcept {
    constexpr int initial_capacity = 8;
    const auto capacity = capacity_of(capacity_mask_);
    const auto new_capacity_mask = capacity_mask_of(
        capacity < initial_capacity ? initial_capacity : capacity * 2);
    const auto new_capacity = capacity_of(new_capacity_mask);
    entry* new_entries = new entry[new_capacity];
    for (int i = 0; i <= capacity_mask_; ++i) {
      entry& current = entries_[i];
      if (current.key != nullptr) {
        entry& dest = find_entry(new_entries, new_capacity_mask, current.key);
        dest = current;
      }
    }
    delete entries_;
    entries_ = new_entries;
    capacity_mask_ = new_capacity_mask;
  }

  int capacity_of(int capacity_mask) const noexcept {
    return capacity_mask + 1;
  }
  int capacity_mask_of(int capacity) const noexcept { return capacity - 1; }

  entry* entries_;
  int capacity_mask_;
  int count_;
};

}  // namespace lox

#endif
