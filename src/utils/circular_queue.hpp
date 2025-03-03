#pragma once

#include <array>
#include <memory>
#include <utility>

template <typename T, size_t Capacity> class CircularQueue {
public:
  void push(const T &item) {
    std::construct_at(&m_data[m_tail], item);
    m_tail = (m_tail + 1) % Capacity;
    if (m_size < Capacity) {
      ++m_size;
    } else {
      m_head = (m_head + 1) % Capacity;
    }
  };

  T dequeue() {
    T item = std::move(m_data[m_head]);
    m_head = (m_head + 1) % Capacity;
    --m_size;
    return item;
  };

  T const &head() const { return m_data[m_head]; }

  [[nodiscard]] bool empty() const { return m_size == 0; };

  [[nodiscard]] bool full() const { return m_size == Capacity; };

  [[nodiscard]] size_t size() const { return m_size; };

private:
  std::array<T, Capacity> m_data;
  size_t m_head = 0, m_tail = 0, m_size = 0;
};
