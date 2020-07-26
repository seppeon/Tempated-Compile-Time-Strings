A compile time single header string manipulation library.
Used for building static strings at compile time. This will be redundant in C++23.

```CPP
constexpr auto a = CreateCharList<'h', 'e', 'l', 'l', 'o'>{};
constexpr auto b = CreateCharList<'='>{};
constexpr auto c = CreateCharList<1234>{};
constexpr auto ab = a.Append(b);
constexpr auto abc = ab.Append(c);
constexpr auto abc_str = ToStringView(abc);
std::cout.write(std::data(abc_str), std::size(abc_str));
```
