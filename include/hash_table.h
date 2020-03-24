#ifndef LOX_HASH_TABLE_H
#define LOX_HASH_TABLE_H

#include "contract.h"

namespace lox {

template <typename Key, typename Value = void>
struct hash_table {
 private:
  struct entry {
    const Key* key = nullptr;
    std::enable_if_t<!std::is_same_v<Value, void>, Value> value;
  };

 public:
  hash_table() noexcept : entries_{nullptr}, capacity_mask_{-1}, count_{0} {}

  ~hash_table() noexcept {
    if (entries_) {
      delete[] entries_;
    }
  }

  int size() const noexcept { return count_; }

  bool insert(entry e) noexcept {
    constexpr auto max_load = 0.75;
    if (count_ + 1 > capacity_of(capacity_mask_) * max_load) {
      adjust_capacity();
    }
    auto& dest = find_entry(entries_, capacity_mask_, e.key);
    if (dest.key == nullptr) {
      dest.key = e.key;
      if constexpr (!std::is_same_v<Value, void>) {
        dest.value = e.value;
      }
      ++count_;
      return true;
    }
    return false;
  }

  bool contains(const Key* key) const noexcept {
    auto& dest = find_entry(entries_, capacity_mask_, key);
    return dest.key != nullptr;
  }

  typename std::enable_if_t<!std::is_same_v<Value, void>, Value>& operator[](
      const Key* key) noexcept {
    auto& dest = find_entry(entries_, capacity_mask_, key);
    ENSURES(dest.key != nullptr);
    return dest.value;
  }

 private:
  entry& find_entry(entry* entries, int capacity_mask, const Key* key) const
      noexcept {
    int index = key->hash() & capacity_mask;
    while (true) {
      auto& current = entries[index];
      if (current.key == nullptr || *current.key == *key) {
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
    auto* new_entries = new entry[new_capacity];
    for (int i = 0; i <= capacity_mask_; ++i) {
      auto& current = entries_[i];
      if (current.key != nullptr) {
        auto& dest = find_entry(new_entries, new_capacity_mask, current.key);
        dest = current;
      }
    }
    if (entries_) {
      delete[] entries_;
    }
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

template <typename Key>
using set = hash_table<Key>;

}  // namespace lox

#endif
