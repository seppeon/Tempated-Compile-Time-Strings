A compile time single header string manipulation library.
Used for building static strings at compile time.
Designed where template char arguments are more convient than the C++20 constexpr string.

Define your symbols as static constexpr variables. 

Example:
```CPP
constexpr auto a = CreateCharList<'h', 'e', 'l', 'l', 'o'>{};
constexpr auto b = CreateCharList<'='>{};
constexpr auto c = CreateCharList<1234>{};
constexpr auto d = CreateCharList<',', 5678>{};
constexpr auto ab = a.Append(b);
constexpr auto abc = ab.Append(c);
constexpr auto abc_str = ToStringView(abc.Append(d));
std::cout.write(std::data(abc_str), std::size(abc_str));
```
Results in:
```BASH
hello=1234,5678
```

Extracted from a dimensional analysis library for building static strings of the units.
