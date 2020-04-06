#ifndef LOX_HASH_TABLE_H
#define LOX_HASH_TABLE_H

#include "contract.h"
#include "object.h"
#include "value.h"

namespace lox {

struct hash_table {
 public:
  hash_table() noexcept : entries_{nullptr}, capacity_mask_{-1}, count_{0} {}

  ~hash_table() noexcept {
    if (entries_) {
      delete[] entries_;
    }
  }

  int size() const noexcept { return count_; }

  bool insert(string* key, value v) noexcept {
    adjust_capacity();
    auto dest = find_entry(entries_, capacity_mask_, key);
    if (dest->key_ == nullptr) {
      dest->key_ = key;
      dest->value_ = v;
      ++count_;
      return true;
    }
    return false;
  }

  bool contains(const string* key) const noexcept {
    auto dest = find_entry(entries_, capacity_mask_, key);
    return dest->key_ != nullptr;
  }

  value* get_if(const string* key) const noexcept {
    auto dest = find_entry(entries_, capacity_mask_, key);
    return dest->key_ ? &dest->value_ : nullptr;
  }

  bool set(const string* key, value v) const noexcept {
    auto dest = find_entry(entries_, capacity_mask_, key);
    if (dest->key_) {
      dest->value_ = v;
      return true;
    }
    return false;
  }

  bool erase(const string* key) noexcept {
    if (count_ == 0) {
      return false;
    }
    auto dest = find_entry(entries_, capacity_mask_, key);
    if (dest->key_ != nullptr) {
      dest->key_ = nullptr;
      --count_;
      return true;
    }
    return false;
  }

  string* find_string(const std::string& str) noexcept {
    if (count_ == 0) {
      return nullptr;
    }
    int index = string::hash(str) & capacity_mask_;
    while (true) {
      auto& current = entries_[index];
      if (current.key_ != nullptr && *current.key_ == str) {
        return current.key_;
      } else if (current.value_.is_nil()) {
        return nullptr;
      }
      index = (index + 1) & capacity_mask_;
    }
  }

 private:
  struct entry {
    string* key_ = nullptr;
    value value_;
  };

  constexpr static int capacity_of(int capacity_mask) noexcept {
    return capacity_mask + 1;
  }
  constexpr static int capacity_mask_of(int capacity) noexcept {
    return capacity - 1;
  }

  entry* find_entry(entry* entries, int capacity_mask, const string* key) const
      noexcept {
    int index = key->hash() & capacity_mask;
    entry* tobmstone = nullptr;
    while (true) {
      auto current = &entries[index];
      if (current->key_ == nullptr) {
        if (current->value_.is_nil()) {
          return tobmstone != nullptr ? tobmstone : current;
        } else {
          if (tobmstone == nullptr) {
            tobmstone = current;
          }
        }
      } else if (current->key_ == key) {
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
      auto new_entries = new entry[new_capacity];
      for (int index = 0; index <= capacity_mask_; ++index) {
        auto current = &entries_[index];
        if (current->key_ != nullptr) {
          auto dest = find_entry(new_entries, new_capacity_mask, current->key_);
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

  entry* entries_;
  int capacity_mask_;
  int count_;
};

}  // namespace lox

#endif
