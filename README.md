# Scope Guard

[![Build Status](https://travis-ci.org/offa/scope-guard.svg?branch=master)](https://travis-ci.org/offa/scope-guard)
[![GitHub release](https://img.shields.io/github/release/offa/scope-guard.svg)](https://github.com/offa/scope-guard/releases)
[![License](https://img.shields.io/badge/license-GPLv3-yellow.svg)](LICENSE)
![C++](https://img.shields.io/badge/c++-17-green.svg)

Implementation of *Scope Guards* (`scope_exit`, `scope_success`, `scope_fail`) and `unique_resource` as proposed in [**P0052R5**][2] (former [N3949][1]).


## Compatibility

This implementation conforms to [P0052][2], except:

###### Namespace
The namespace `sr` is used instead of `std`.

###### Filenames
The filenames contain a `.h` extension. To enable the compatible header as specified in the document the CMake option `ENABLE_COMPAT_HEADER` can be used. This will generate and install an additional header named `scope` (without file extension).

###### std::uncaught_exceptions()
The function `std::uncaught_exceptions()` is not available in C++14 and is therefore replaced with `std::uncaught_exception()`. This has some limitations, but is the only solution until C++17 is used.


## Implementation decisions

###### Factory functions
The Factory functions for both types are kept for the C++14 implementations as *deduction guides* aren't available.


## Documentation

- [*P0052R5 - Generic Scope Guard and RAII Wrapper for the Standard Library*][2] (P. Sommerlad, A. L. Sandoval, E. Niebler, D. Krügler)
- [*N3949 - Scoped Resource - Generic RAII Wrapper for the Standard Library*][1] (P. Sommerlad, A. L. Sandoval)



## License

**GNU General Public License (GPL)**

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.


[1]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3949.pdf
[2]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0052r5.pdf
