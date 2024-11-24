#include <limits>
#include <stdexcept>
#include <type_traits>

template <typename T> void set_bit(T &number, unsigned int bit, bool value) {
  static_assert(std::is_integral<T>::value, "T must be an integral type");
  if (bit >= std::numeric_limits<T>::digits) {
    throw std::out_of_range("But index out of range");
  }

  if (value) {
    number |= 1 << bit;
  } else {
    number &= ~(1 << bit);
  }
}

template <typename T> void set_bit_with_mask(T &number, T mask, bool value) {
  static_assert(std::is_integral<T>::value, "T must be an integral type");

  if (value) {
    number |= mask;
  } else {
    number &= ~mask;
  }
}

template <typename T> bool get_bit(const T &number, unsigned int bit) {
  static_assert(std::is_integral<T>::value, "T must be an integral type");
  if (bit >= std::numeric_limits<T>::digits) {
    throw std::out_of_range("But index out of range");
  }

  return (number & (static_cast<T>(1) << bit)) != 0;
}

template <typename T> bool get_bit_with_mask(const T &number, T mask) {
  static_assert(std::is_integral<T>::value, "T must be an integral type");

  return (number & mask) != 0;
}
