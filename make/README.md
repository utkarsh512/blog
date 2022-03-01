# `make` utility for building `C++` projects

Use the `Makefile` for building your `C++` projects. 

The directory structure should be as follows:

```
─┬[ Project ]
 │
 ├──● Makefile
 │
 ├──┬[ build ]
 │  │
 │  ├───[ objects ]
 │  └───[ apps ]
 │
 ├──┬[ include ]
 │  │
 │  ├──● program.hpp
 │  │
 │  ├──┬[ module1 ]
 │  │  │
 │  │  ├──● mod1c1.hpp
 │  │  └──● mod1c2.hpp
 │  │
 │  └──┬[ module2 ]
 │     │
 │     ├──● mod2c1.hpp
 │     └──● mod2c2.hpp
 │
 └──┬[ src ]
    │
    ├──● program.cpp
    │
    ├──┬[ module1 ]
    │  │
    │  ├──● mod1c1.cpp
    │  └──● mod1c2.cpp
    │
    └──┬[ module2 ]
       │
       ├──● mod2c1.cpp
       └──● mod2c2.cpp
 ```
