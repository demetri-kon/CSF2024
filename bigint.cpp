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
  // TODO: implement
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
  // TODO: implement
}

BigInt BigInt::operator-(const BigInt &rhs) const
{
  // TODO: implement
  // Hint: a - b could be computed as a + -b
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
  // TODO: implement
}

BigInt BigInt::operator<<(unsigned n) const
{
  // TODO: implement
}

BigInt BigInt::operator*(const BigInt &rhs) const
{
  // TODO: implement
}

BigInt BigInt::operator/(const BigInt &rhs) const
{
  // TODO: implement
}

int BigInt::compare(const BigInt &rhs) const
{
  // TODO: implement
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
  // TODO: implement
}
