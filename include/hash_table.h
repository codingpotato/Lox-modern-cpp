#ifndef LOX_HASH_TABLE_H
#define LOX_HASH_TABLE_H

#include "contract.h"
#include "object.h"
#include "value.h"

namespace lox {

struct Hash_table {
 public:
  Hash_table() noexcept : entries{nullptr}, capacity_mask{-1}, count{0} {}

  ~Hash_table() noexcept {
    if (entries) {
      delete[] entries;
    }
  }

  int size() const noexcept { return count; }

  bool insert(String* key, Value v) noexcept {
    adjust_capacity();
    auto dest = find_entry(entries, capacity_mask, key);
    if (dest->key == nullptr) {
      dest->key = key;
      dest->value = v;
      ++count;
      return true;
    }
    return false;
  }

  bool contains(const String* key) const noexcept {
    auto dest = find_entry(entries, capacity_mask, key);
    return dest->key != nullptr;
  }

  Value* get_if(const String* key) const noexcept {
    auto dest = find_entry(entries, capacity_mask, key);
    return dest->key ? &dest->value : nullptr;
  }

  bool set(const String* key, Value v) const noexcept {
    auto dest = find_entry(entries, capacity_mask, key);
    if (dest->key) {
      dest->value = v;
      return true;
    }
    return false;
  }

  bool erase(const String* key) noexcept {
    if (count == 0) {
      return false;
    }
    auto dest = find_entry(entries, capacity_mask, key);
    if (dest->key != nullptr) {
      dest->key = nullptr;
      --count;
      return true;
    }
    return false;
  }

  String* find_string(const std::string& string) noexcept {
    if (count == 0) {
      return nullptr;
    }
    int index = String::hash_from(string) & capacity_mask;
    while (true) {
      auto& current = entries[index];
      if (current.key != nullptr && *current.key == string) {
        return current.key;
      } else if (current.value.is_nil()) {
        return nullptr;
      }
      index = (index + 1) & capacity_mask;
    }
  }

  template <typename Visitor>
  void for_each(Visitor&& visitor) const noexcept {
    for (auto i = 0; i <= capacity_mask; ++i) {
      if (entries[i].key) {
        visitor(entries[i].key, entries[i].value);
      }
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
    int index = key->get_hash() & capacity_mask;
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
    if (count + 1 > capacity_of(capacity_mask) * max_load) {
      constexpr int initial_capacity = 8;
      const auto capacity = capacity_of(capacity_mask);
      const auto new_capacity_mask = capacity_mask_of(
          capacity < initial_capacity ? initial_capacity : capacity * 2);
      const auto new_capacity = capacity_of(new_capacity_mask);
      auto new_entries = new Entry[new_capacity];
      for (int index = 0; index <= capacity_mask; ++index) {
        auto current = &entries[index];
        if (current->key != nullptr) {
          auto dest = find_entry(new_entries, new_capacity_mask, current->key);
          *dest = *current;
        }
      }
      if (entries) {
        delete[] entries;
      }
      entries = new_entries;
      capacity_mask = new_capacity_mask;
    }
  }

  Entry* entries;
  int capacity_mask;
  int count;
};

}  // namespace lox

#endif
