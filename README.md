# Scope Guard

[![Build Status](https://travis-ci.org/offa/scope-guard.svg?branch=master)](https://travis-ci.org/offa/scope-guard)
[![GitHub release](https://img.shields.io/github/release/offa/scope-guard.svg)](https://github.com/offa/scope-guard/releases)
[![License](https://img.shields.io/badge/license-MIT-yellow.svg)](LICENSE)
![C++](https://img.shields.io/badge/c++-17-green.svg)

Implementation of *Scope Guards* (`scope_exit`, `scope_success`, `scope_fail`) and `unique_resource` as proposed in [**P0052R10**][1].


## Compatibility

This implementation conforms to [P0052][1], except:

###### Namespace
The namespace `sr` is used instead of `std`.

###### Filenames
The filenames contain a `.h` extension. To enable the compatible header as specified in the document the CMake option `ENABLE_COMPAT_HEADER` can be used. This will generate and install an additional header named `scope` (without file extension).


## Standardisation progress

[P0052][1] has been adopted (2019-03) and is in the [*Library Fundamentals v3*][2] now.


## Documentation

- [*P0052R10 – Generic Scope Guard and RAII Wrapper for the Standard Library*][1] (P. Sommerlad, A. L. Sandoval, E. Niebler, D. Krügler)
- [*N4853 – C++ Extensions for Library Fundamentals, Version 3*][2] (T. Köppe)


## License

**MIT License**

    Copyright (c) 2017-2020 offa

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.


[1]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0052r10.pdf
[2]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/n4853.html
