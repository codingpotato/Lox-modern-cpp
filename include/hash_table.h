#ifndef LOX_HASH_TABLE_H
#define LOX_HASH_TABLE_H

#include "contract.h"
#include "gc.h"
#include "object.h"
#include "value.h"

namespace lox {

struct Hash_table {
 public:
  Hash_table() noexcept : entries{nullptr}, capacity_mask{-1}, count{0} {}
  ~Hash_table() noexcept { free_current_entries(); }

  Hash_table(const Hash_table&) = delete;
  Hash_table(Hash_table&&) = delete;
  Hash_table& operator=(const Hash_table&) = delete;
  Hash_table& operator=(Hash_table&&) = delete;

  bool contains(const String* key) const noexcept {
    const auto& dest = find_entry(entries, capacity_mask, key);
    return dest.key != nullptr;
  }

  void insert(String* key, Value value) noexcept {
    adjust_capacity();
    auto& dest = find_entry(entries, capacity_mask, key);
    if (dest.key == nullptr && dest.value.is_nil()) {
      ++count;
    }
    dest.key = key;
    dest.value = value;
  }

  bool set(const String* key, Value value) noexcept {
    if (auto& dest = find_entry(entries, capacity_mask, key); dest.key) {
      dest.value = value;
      return true;
    }
    return false;
  }

  Value* get_if(const String* key) const noexcept {
    auto& dest = find_entry(entries, capacity_mask, key);
    return dest.key ? &dest.value : nullptr;
  }

  bool erase(const String* key) noexcept {
    if (count == 0) {
      return false;
    }
    if (auto& dest = find_entry(entries, capacity_mask, key); dest.key) {
      dest.key = nullptr;
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
      if (auto& current = entries[index];
          current.key && *current.key == string) {
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

  template <typename Pred>
  void erase_if(Pred&& pred) noexcept {
    for (auto i = 0; i <= capacity_mask; ++i) {
      if (auto& dest = entries[i]; dest.key && pred(dest.key, dest.value)) {
        dest.key = nullptr;
        dest.value = true;
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

  Entry& find_entry(Entry* entries, int capacity_mask, const String* key) const
      noexcept {
    int index = key->get_hash() & capacity_mask;
    Entry* tobmstone = nullptr;
    while (true) {
      if (auto& current = entries[index]; !current.key) {
        if (current.value.is_nil()) {
          return tobmstone ? *tobmstone : current;
        } else if (!tobmstone) {
          tobmstone = &current;
        }
      } else if (current.key == key) {
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
      auto new_entries = allocate_new_entries(capacity_of(new_capacity_mask));
      count = 0;
      for (int index = 0; index <= capacity_mask; ++index) {
        if (const auto& current = entries[index]; current.key) {
          auto& dest = find_entry(new_entries, new_capacity_mask, current.key);
          dest = current;
          ++count;
        }
      }
      free_current_entries();
      entries = new_entries;
      capacity_mask = new_capacity_mask;
    }
  }

  Entry* allocate_new_entries(size_t count) const noexcept {
    if (auto tracker = Memory_tracker::current(); tracker) {
      tracker->allocate(sizeof(Entry) * count);
    }
    return new Entry[count];
  }

  void free_current_entries() noexcept {
    if (entries) {
      if (auto tracker = Memory_tracker::current(); tracker) {
        tracker->free(sizeof(Entry) * capacity_of(capacity_mask));
      }
      delete[] entries;
    }
  }

  Entry* entries;
  int capacity_mask;
  int count;
};

}  // namespace lox

#endif
