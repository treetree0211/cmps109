// $Id: bigint.cpp,v 1.74 2016-03-24 19:30:57-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"

bigint::bigint (long that): uvalue (that), is_negative (that < 0) {
   DEBUGF ('~', this << " -> " << uvalue)
}

bigint::bigint (const ubigint& uvalue, bool is_negative):
                uvalue(uvalue), is_negative(is_negative) {
}

bigint::bigint (const string& that) {
   is_negative = that.size() > 0 and that[0] == '_';
   uvalue = ubigint (that.substr (is_negative ? 1 : 0));
}

bigint bigint::operator+ () const {
   return *this;
}

bigint bigint::operator- () const {
   return {uvalue, not is_negative};
}

/*----------------our code starts here---------------*/
// 5(a) check if the signs are the same of different
bigint bigint::operator+ (const bigint& that) const {
   ubigint result;
   // if the signs are different
   if (is_negative != that.is_negative) {
      // check which uvalue is bigger
      if (uvalue > that.uvalue) {
        result = uvalue - that.uvalue;
        return {result, is_negative};
      } else {
          result = that.uvalue - uvalue;
          return {result, that.is_negative};
      }
   } else {
      // the signs are same, both negative
      result = uvalue + that.uvalue;
      return {result, is_negative};
   }
}

bigint bigint::operator- (const bigint& that) const {
   ubigint result;
   // if the signs are the same
   if (is_negative == that.is_negative) {
      // check which uvalue is bigger
      if (uvalue > that.uvalue) {
        result = uvalue - that.uvalue;
        return {result, that.is_negative};
      } else {
          result = that.uvalue - uvalue;
          return {result, !is_negative};
      }
   } else {
      // if the signs are different
      result = uvalue + that.uvalue;
      return {result, is_negative};
   }
}

bigint bigint::operator* (const bigint& that) const {
   bigint result = uvalue * that.uvalue;
   // adjust the resulting sign
   // if they have the same sign, then result is positive
   if (is_negative == that.is_negative) {
      result.is_negative = false;
   } else {
      result.is_negative = true;
   }
   return result;
}

bigint bigint::operator/ (const bigint& that) const {
   bigint result = uvalue / that.uvalue;
   return result;
}

bigint bigint::operator% (const bigint& that) const {
   bigint result = uvalue % that.uvalue;
   return result;
}

bool bigint::operator== (const bigint& that) const {
   return is_negative == that.is_negative and uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const {
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? uvalue > that.uvalue
                      : uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) {
  if(that.is_negative) out << "-";
  out << that.uvalue;
   return out;
}

