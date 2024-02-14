#include <cassert>
#include <sstream>
#include <iomanip>
#include <iostream>
#include "bigint.h"

BigInt::BigInt()
{
  nums.push_back(0);
  this->negative = false;
}

BigInt::BigInt(uint64_t val, bool negative)
{
  nums.push_back(val);
  this->negative = negative;
}

BigInt::BigInt(std::initializer_list<uint64_t> vals, bool negative)
{
  for (auto i = vals.begin(); i != vals.end(); ++i) {
    nums.push_back(*i);
  }
  this->negative = negative;
}

BigInt::BigInt(const BigInt &other)
{
  nums = other.get_bit_vector();
  this->negative = other.is_negative();
}

BigInt::~BigInt()
{
}

BigInt &BigInt::operator=(const BigInt &rhs)
{
  this->nums = rhs.get_bit_vector();
  this->negative = rhs.is_negative();
  return *this;
}

bool BigInt::is_negative() const
{
  return negative;
}

const std::vector<uint64_t> &BigInt::get_bit_vector() const {
  return nums;
}

uint64_t BigInt::get_bits(unsigned index) const
{
  try {
    return nums.at(index);
  } catch (...) {
    return 0;
  }

}

BigInt BigInt::operator+(const BigInt &rhs) const
{
  BigInt res;
  if (this->negative != rhs.negative) {
    if (this->compare_magnitudes(rhs) >= 0) {
        res = this->subtract_magnitudes(rhs);
        res.negative = this->negative; 
    } else {
        res = rhs.subtract_magnitudes(*this);
        res.negative = rhs.negative; 
    }
      return res;
  } else {
    res = this->add_magnitudes(rhs); 
    res.negative = this->negative; 
  }
  return res; 
}

BigInt BigInt::operator-(const BigInt &rhs) const
{
  return *this + -rhs;
}

BigInt BigInt::operator-() const
{
  BigInt to_return = *(new BigInt(*this));
  if (to_return.to_hex() != "0") {
    to_return.negative = !to_return.is_negative();
  }
  return to_return;
}

bool BigInt::is_bit_set(unsigned n) const
{
  uint64_t one = 1;
  unsigned val = (n / 64);
  if (!get_bits(val)) {
    return false;
  } else {
    return ((get_bits(val) & (one << (n % 64))) > 0);
  }
}

BigInt BigInt::operator<<(unsigned n) const
{
  BigInt res;
  unsigned shift_chunks = n / 64;
  unsigned shift_bits = n % 64;
  uint64_t carry = 0;

  res.magnitude.resize(magnitude.size() + shift_chunks, 0);

  for (size_t i = 0; i < magnitude.size(); ++i) {
    uint64_t shifted_chunk = magnitude[i] << shift_bits;

    res.magnitude[i + shift_chunks] |= (shifted_chunk | carry);

    carry = (magnitude[i] >> (64 - shift_bits));
    }

  if (carry != 0) {
      res.magnitude.push_back(carry);
  }

  return res; 
}

BigInt BigInt::operator*(const BigInt &rhs) const
{
  BigInt product = BigInt(); 
  product.negative = this->negative != rhs.negative;

  const BigInt& pos_lhs = this->negative ? -*this : *this;
  const BigInt& pos_rhs = rhs.negative ? -rhs : rhs;

    // Perform multiplication using the standard algorithm
  for (int i = 0; i < pos_rhs.magnitude.size() * 64; i++)
  {
    if (pos_rhs.is_bit_set(i)) {
        product = product + (pos_lhs << i);
    }
  }

  return product;
}

BigInt BigInt::operator/(const BigInt &rhs) const
{
  if (rhs.compare_magnitudes(*this) > 0) {
    return BigInt();
  }
  
  BigInt pos_this = *this;
  pos_this.negative = false;
  BigInt pos_rhs = rhs;
  pos_rhs.negative = false;

  BigInt low(0);
  BigInt high(*this);

  while (low <= high) {
    BigInt mid = (low + high) / 2;
    BigInt product = mid * pos_rhs;

    int cmp = product.compare_magnitudes(pos_this);

    if (cmp == 0) {
      if (this->negative != rhs.negative) {
        mid.negative = true;
      }
      return mid;
    } else if (cmp < 0) {
      low = mid + 1;
    } else if (cmp > 0) {
      high = mid - 1;
    }
  }

  if (this->negative != rhs.negative) {
    low.negative = true;
  }
  
  return low;
}

int BigInt::compare(const BigInt &rhs) const
{
  bool reverse = this->negative != rhs.negative;
  int magnitude_comparison = this->compare_magnitudes(rhs);

  return reverse ? -magnitude_comparison : magnitude_comparison;
}

std::string BigInt::to_hex() const
{
  std::stringstream val_stream;
  if (negative) {
    val_stream << "-";
  }
  for (auto i = nums.end(); i != nums.begin() - 1; --i) {
    if (val_stream.str() != "" && val_stream.str() != "-") {
      val_stream << std::setfill('0') << std::setw(16);
    }
    if (*i != 0) {
      val_stream << std::hex << *i;
    } else if (i != nums.end() && val_stream.str() != "") {
      val_stream << 0;
    }
  }
  if (val_stream.str() == "") {
    val_stream << 0;
  }
  return val_stream.str();
}

std::string BigInt::to_dec() const
{
  BigInt current = *this; 
  current.negative = false; 

  std::string res;
    
  while (!current.is_zero()) {
      uint64_t quotient = 0;
      for (int i = current.magnitude.size() - 1; i >= 0; --i) {
          uint64_t combined = quotient * (UINT64_MAX + 1ULL) + current.magnitude[i];
          current.magnitude[i] = combined / 10;
          quotient = combined % 10;
      }
      res = std::to_string(quotient) + res;
        
      while (!current.magnitude.empty() && current.magnitude.back() == 0) {
          current.magnitude.pop_back();
      }
  }

  if (negative) {
      res = "-" + res;
  }

  return res;
}

int BigInt::compare_magnitude(const BigInt &rhs) const
{
  if (this->magnitude.size() < rhs.magnitude.size()) {
      return -1;
  } else if (this->magnitude.size() > rhs.magnitude.size()) {
      return 1;
  } else {
      for (size_t i = this->magnitude.size(); i > 0; --i) {
          if (this->magnitude[i - 1] < rhs.magnitude[i - 1]) {
              return -1;
          } else if (this->magnitude[i - 1] > rhs.magnitude[i - 1]) {
              return 1;
          }
      }
      return 0;
  }
}

BigInt BigInt::add_magnitudes(const BigInt &rhs) const
{
  BigInt res;
  uint64_t carry = 0;
  size_t max_size = std::max(this->magnitude.size(), rhs.magnitude.size());

  for (size_t i = 0; i < max_size; ++i) {
      uint64_t sum_result = carry;
      if (i < this->magnitude.size()) {
          sum_result += this->magnitude[i];
      }
      if (i < rhs.magnitude.size()) {
          sum_result += rhs.magnitude[i];
      }

      carry = sum_result >> 64;

      res.magnitude.push_back(sum_result & UINT64_MAX);
  }

  if (carry != 0) {
      res.magnitude.push_back(carry);
  }

  return res;
}

BigInt BigInt::subtract_magnitudes(const BigInt &rhs) const
{
  BigInt res;
  uint64_t borrow = 0;

  for (size_t i = 0; i < this->magnitude.size(); ++i) {
      uint64_t sub_result = this->magnitude[i] - borrow;
      if (i < rhs.magnitude.size()) {
          sub_result -= rhs.magnitude[i];
      }

      borrow = (sub_result > this->magnitude[i]) ? 1 : 0;

      res.magnitude.push_back(sub_result);
  }

  while (!res.magnitude.empty() && res.magnitude.back() == 0) {
      res.magnitude.pop_back();
  }

  return res;
}

bool BigInt::is_zero() const
{
    return nums.size() == 1 && nums[0] == 0;
}
