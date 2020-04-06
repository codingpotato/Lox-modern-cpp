#ifndef LOX_HASH_TABLE_H
#define LOX_HASH_TABLE_H

#include "contract.h"
#include "object.h"
#include "value.h"

namespace lox {

struct Hash_table {
 public:
  Hash_table() noexcept : entries_{nullptr}, capacity_mask_{-1}, count_{0} {}

  ~Hash_table() noexcept {
    if (entries_) {
      delete[] entries_;
    }
  }

  int size() const noexcept { return count_; }

  bool insert(String* key, Value v) noexcept {
    adjust_capacity();
    auto dest = find_entry(entries_, capacity_mask_, key);
    if (dest->key == nullptr) {
      dest->key = key;
      dest->value = v;
      ++count_;
      return true;
    }
    return false;
  }

  bool contains(const String* key) const noexcept {
    auto dest = find_entry(entries_, capacity_mask_, key);
    return dest->key != nullptr;
  }

  Value* get_if(const String* key) const noexcept {
    auto dest = find_entry(entries_, capacity_mask_, key);
    return dest->key ? &dest->value : nullptr;
  }

  bool set(const String* key, Value v) const noexcept {
    auto dest = find_entry(entries_, capacity_mask_, key);
    if (dest->key) {
      dest->value = v;
      return true;
    }
    return false;
  }

  bool erase(const String* key) noexcept {
    if (count_ == 0) {
      return false;
    }
    auto dest = find_entry(entries_, capacity_mask_, key);
    if (dest->key != nullptr) {
      dest->key = nullptr;
      --count_;
      return true;
    }
    return false;
  }

  String* find_string(const std::string& str) noexcept {
    if (count_ == 0) {
      return nullptr;
    }
    int index = String::hash(str) & capacity_mask_;
    while (true) {
      auto& current = entries_[index];
      if (current.key != nullptr && *current.key == str) {
        return current.key;
      } else if (current.value.is_nil()) {
        return nullptr;
      }
      index = (index + 1) & capacity_mask_;
    }
  }

 private:
  struct Entry {
    String* key = nullptr;
    Value value;
  };

  constexpr static int capacity_of(int capacity_mask) noexcept {
    return capacity_mask + 1;
  }
  constexpr static int capacity_mask_of(int capacity) noexcept {
    return capacity - 1;
  }

  Entry* find_entry(Entry* entries, int capacity_mask, const String* key) const
      noexcept {
    int index = key->hash() & capacity_mask;
    Entry* tobmstone = nullptr;
    while (true) {
      auto current = &entries[index];
      if (current->key == nullptr) {
        if (current->value.is_nil()) {
          return tobmstone != nullptr ? tobmstone : current;
        } else {
          if (tobmstone == nullptr) {
            tobmstone = current;
          }
        }
      } else if (current->key == key) {
        return current;
      }

      index = (index + 1) & capacity_mask;
    }
  }

  void adjust_capacity() noexcept {
    constexpr auto max_load = 0.75;
    if (count_ + 1 > capacity_of(capacity_mask_) * max_load) {
      constexpr int initial_capacity = 8;
      const auto capacity = capacity_of(capacity_mask_);
      const auto new_capacity_mask = capacity_mask_of(
          capacity < initial_capacity ? initial_capacity : capacity * 2);
      const auto new_capacity = capacity_of(new_capacity_mask);
      auto new_entries = new Entry[new_capacity];
      for (int index = 0; index <= capacity_mask_; ++index) {
        auto current = &entries_[index];
        if (current->key != nullptr) {
          auto dest = find_entry(new_entries, new_capacity_mask, current->key);
          *dest = *current;
        }
      }
      if (entries_) {
        delete[] entries_;
      }
      entries_ = new_entries;
      capacity_mask_ = new_capacity_mask;
    }
  }

  Entry* entries_;
  int capacity_mask_;
  int count_;
};

}  // namespace lox

#endif
