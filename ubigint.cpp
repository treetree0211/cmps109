// $Id: ubigint.cpp,v 1.12 2016-04-04 13:07:41-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "ubigint.h"
#include "debug.h"

ubigint::ubigint (unsigned long that): uvalue (that) {
   DEBUGF ('~', this << " -> " << uvalue)
}
/*----------------our code starts here---------------*/
ubigint::ubigint (const string& that): uvalue(0) {
   for (char digit: that) {
      if (not isdigit (digit)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
      uvalue = uvalue * 10 + digit - '0';
   }

   for(auto i = that.rbegin(); i != that.rend(); i++) {
      ubig_value.push_back(*i);
   }
}


ubigint ubigint::operator+ (const ubigint& that) const {
   ubigint result;
   int carry = 0;
   int digit = 0;

   // get the longer digits of two number
   size_t digits;
   if (ubig_value.size() > that.ubig_value.size()) {
      digits = ubig_value.size() + 1;
   } else {
      digits = that.ubig_value.size() + 1;
   }
   // proceed from low order end to the high order end
   for (size_t i = 0; i < digits; i++) {
      // for any sum >= 10 take the remainder and add carry
      digit = carry;
      if (i < ubig_value.size()) {
         digit += ubig_value.at(i) - '0';
      }
      if (i < that.ubig_value.size()) {
         digit += ubig_value.at(i) - '0';
      }
      // use push_back to append new digits
      result.ubig_value.push_back((digit % 10) + '0');
      carry = digit / 10;
   }

   // when run out of shorter number, matching longer vector with zeros
   while (result.ubig_value.size() > 0 and
      result.ubig_value.back() == 0) {
      result.ubig_value.pop_back();
   }
   return result;
}

ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   ubigint result;
   size_t digits = ubig_value.size();
   int borrow = 0;
   int digit = 0;

   // if left digit is smaller than right digit,
   // add 10 to digit and set borrow to next digit to -1
   for (size_t i = 0; i < digits; i++) {
      digit = ubig_value.at(i) - '0' - borrow + 10;
      if (i < that.ubig_value.size()) {
         digit -= that.ubig_value.at(i) - '0';
      }
      // use push_back to append new digits
      result.ubig_value.push_back((digit % 10) + '0');
      borrow = 1 - digit / 10;
   }

   // when run out of shorter number, matching longer vector with zeros
   while (result.ubig_value.size() > 0 and
      result.ubig_value.back() == 0) {
      result.ubig_value.pop_back();
   }
   return result;
}

ubigint ubigint::operator* (const ubigint& that) const {
   ubigint result;
   // a new vector, size is equal to the sum of the sizes of the other two
   int size = ubig_value.size() + that.ubig_value.size();

   for (int i = 0; i < size + 1; i++) {
      result.ubig_value.push_back('0' + 0);
   }
   // outer loop: for 0 <= i < m, c <- 0
   for (size_t i = 0; i < ubig_value.size(); i++) {
      int carry = 0;
      // inner loop: d <- p(i+j) + uiVj + c
      for (size_t j = 0; j < that.ubig_value.size(); j++) {
         int digit = 0;
         // p(i+j)
         digit = result.ubig_value.at(i + j) - '0';
         // uiVj
         digit += (ubig_value.at(i) - '0') * (that.ubig_value.at(j) - '0');
         // carry
         digit += carry;
         // p(i+j) <- d % 10
         result.ubig_value.at(i + j) = ((digit % 10) + '0');
         // c <- d / 10
         carry = digit / 10;
      }
      // p(i+n) <- c
      result.ubig_value.at(i + that.ubig_value.size()) = carry + '0';
   }
   // when run out of shorter number, matching longer vector with zeros
   while (result.ubig_value.size() > 0 and
      result.ubig_value.back() == 0+'0') {
      result.ubig_value.pop_back();
   }
   return result;
}


//

ubigint::quo_rem ubigint::udivide (const ubigint& that) const{
   // Note: divisor is modified so pass by value (copy).
   static const ubigint zero {0};

   if (that == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint divisor = that;
   ubigint quotient {0};
   ubigint remainder = (*this); // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   return {quotient, remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return (udivide (that)).first;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return (udivide (that)).second;
}

bool ubigint::operator== (const ubigint& that) const {
   // check of the lengths of the vectors are the same
   if (ubig_value.size() != that.ubig_value.size()) return false;
   //  run down both vectors
   // return false as soon a difference is found.
   for (int i = ubig_value.size() - 1; i >= 0; i--) {
      if (ubig_value[i] != that.ubig_value[i]) return false;
   }
   return true;
}

bool ubigint::operator< (const ubigint& that) const {
   if (ubig_value.size() != that.ubig_value.size()) {
      return ubig_value.size() < that.ubig_value.size();
   }
   if ((*this) == that) return false;
   // If the signs are the same, for positive numbers,
   // the shorter one is less,
   // and for negative nubmers, the longer one is less
   for (int i = ubig_value.size() - 1; i >= 0; i--) {
      if(ubig_value[i] < that.ubig_value[i]) return true;
      else if (ubig_value[i] < that.ubig_value[i]) return false;
   }

   return false;
}
// print numbers in the same way as dc(1) does.
ostream& operator<< (ostream& out, const ubigint& that) {
   if (that.ubig_value.size() == 0) out << "0";
   int count = 0;
   for (auto i = that.ubig_value.rbegin(); i != that.ubig_value.rend();
      i++){
      out << (*i - '0');
      // make sure the checksource does not complain
      if (count == 71) {
         out << "\\" << endl;
         count = 0;
      }

      count++;
   }
   return out;
}

