#pragma once

#define FAST_OPERATOR(type, op, param) FORCEINLINE bool operator##op(const type& rhs) const { return param op rhs.param; }

#define FAST_OPERATOR_TWO_LESS(type, param1, param2) bool operator<(const type& rhs) const { return param1 < rhs.param1 || param1 == rhs.param1 && param2 < rhs.param2; }
#define FAST_OPERATOR_TWO_LESS_EQUAL(type, param1, param2) bool operator<=(const type& rhs) const { return param1 < rhs.param1 || param1 == rhs.param1 && param2 <= rhs.param2; }
#define FAST_OPERATOR_TWO_EQUAL(type, param1, param2) bool operator==(const type& rhs) const { return param1 == rhs.param1 && param2 == rhs.param2; }
#define FAST_OPERATOR_TWO_GREATER_EQUAL(type, param1, param2) bool operator>=(const type& rhs) const { return param1 > rhs.param1 || param1 == rhs.param1 && param2 >= rhs.param2; }
#define FAST_OPERATOR_TWO_GREATER(type, param1, param2) bool operator>(const type& rhs) const { return param1 > rhs.param1 || param1 == rhs.param1 && param2 > rhs.param2; }
#define FAST_OPERATOR_TWO_NOT_EQUAL(type, param1, param2) bool operator!=(const type& rhs) const { return param1 != rhs.param1 || param2 != rhs.param2; }