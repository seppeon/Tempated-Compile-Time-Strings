#pragma once
#include <string_view>
#include <array>

template <char ... a>
struct CharList;
using EmptyString = CharList<>;

template <typename T>
struct PopFrontImpl
{
	using type = EmptyString;
};
template <char f, char...a>
struct PopFrontImpl<CharList<f, a...>>
{
	using type = CharList<a...>;
};

template <typename T, typename O = EmptyString>
struct ReverseImpl
{
	using type = O;
};
template <char f, char ... a, char ... o>
struct ReverseImpl<CharList<f, a...>, CharList<o...>>
{
	using type = typename ReverseImpl<CharList<a...>, CharList<f, o...>>::type;
};

template <typename T>
struct PopBackImpl
{
	using type = typename ReverseImpl<typename PopFrontImpl<typename ReverseImpl<T>::type>::type>::type;
};

template <typename T>
struct SizeImpl;
template <char ... a>
struct SizeImpl<CharList<a...>>
{
	static constexpr size_t value = sizeof...(a);
};

template <typename T, size_t i, size_t desired_length, bool smaller_or_equal = (desired_length >= SizeImpl<T>::value)>
struct SubstringImpl
{
	using type = typename SubstringImpl<typename PopFrontImpl<T>::type, i - 1, desired_length>::type;
};
template <typename T, size_t desired_length>
struct SubstringImpl<T, 0, desired_length, true>
{
	using type = T;
};
template <typename T, size_t desired_length, bool smaller_or_equal>
struct SubstringImpl<T, 0, desired_length, smaller_or_equal>
{
	using type = typename SubstringImpl<typename PopBackImpl<T>::type, 0, desired_length>::type;
};

template <typename T>
struct FrontImpl
{
	static constexpr char value = '\0';
};
template <char f, char ... a>
struct FrontImpl<CharList<f, a...>>
{
	static constexpr char value = f;
};

template <typename T>
struct BackImpl : FrontImpl<typename ReverseImpl<T>::type> {};

template <typename T, size_t i>
struct AtImpl
{
	static constexpr char value = AtImpl<typename PopFrontImpl<T>::type, i - 1>::value;
};

template <typename T>
struct AtImpl<T, 0>
{
	static constexpr char value = '\0';
};

template <char f, char ... a>
struct AtImpl<CharList<f, a...>, 0>
{
	static constexpr char value = f;
};

template <char ... a>
struct CharList
{
	template <size_t i>
	static constexpr char At() noexcept
	{
		return AtImpl<CharList, i>::value;
	}
	static constexpr char Front() noexcept
	{
		return FrontImpl<CharList>::value;
	}
	static constexpr char Back() noexcept
	{
		return BackImpl<CharList>::value;
	}
	static constexpr size_t Size() noexcept
	{
		return SizeImpl<CharList>::value;
	}

	template <size_t i, size_t len>
	static constexpr auto Substring() noexcept -> typename SubstringImpl<CharList, i, len>::type
	{
		return {};
	}
	static constexpr auto Reverse() noexcept -> typename ReverseImpl<CharList>::type
	{
		return {};
	}
	static constexpr auto PopFront() noexcept -> typename PopFrontImpl<CharList>::type
	{
		return {};
	}
	static constexpr auto PopBack() noexcept -> typename PopBackImpl<CharList>::type
	{
		return {};
	}

	template <char ... b>
	static constexpr auto Append(CharList<b...>) noexcept -> CharList<a..., b...>
	{
		return {};
	}
	template <char ... b, char ... c, typename ... Args>
	static constexpr auto Append(CharList<b...>, CharList<c...>, Args...args)
	{
		return Append(CharList<b..., c...>{}, args...);
	}

	static constexpr size_t length = sizeof...(a);
	static constexpr size_t array_size = (length == 0) ? 1 : length;
	static constexpr char value[ array_size ]{ a ... };
};

template <auto V>
struct ToCharListImpl
{
private:
	template <auto Value>
	class Int
	{
	private:
		using T = decltype(Value);
		using uint = std::make_unsigned_t<T>;

		static constexpr uint abs = static_cast<uint>(Value >= 0 ? Value : -Value);
		static constexpr uint Digits() noexcept
		{
			uint value = abs;
			uint digits = 1;
			while (value /= 10) ++digits;
			return digits;
		}
		static constexpr bool negative = Value < 0;
		static constexpr size_t digits = Digits();
		static constexpr size_t length = (negative ? 1 : 0) + digits;
	private:
		using array_t = std::array<char, length>;

		static constexpr array_t ToString() noexcept
		{
			auto reverse = [](auto first, auto last) constexpr
			{
				while ((first != last) && (first != --last))
				{
					auto& a = *first++;
					auto& b = *last;
					auto temp = b;
					b = std::move(a);
					a = std::move(temp);
				}
			};
			array_t output{};
			auto value = abs;
			size_t index = 0;
			do output[index++] = "0123456789"[value % 10];
			while (value /= 10);
			if constexpr (negative) output[index++] = '-';
			reverse(output.begin(), output.begin() + length);
			return output;
		}
		static constexpr array_t value = ToString();

		template <typename L>
		struct IntToLabel;
		template <size_t ... I>
		struct IntToLabel<std::index_sequence<I...>>
		{
			using type = CharList<value[I]...>;
		};
	public:
		using type = typename IntToLabel<std::make_index_sequence<length>>::type;
	};

	template <auto V>
	using Vt = decltype(V);

	template <auto V, typename T = Vt<V>>
	struct Dispatch;

	template <auto Value> struct Dispatch<Value, char>	   { using type = CharList<Value>; };
	template <auto Value> struct Dispatch<Value, int8_t>   { using type = typename Int<Value>::type; };
	template <auto Value> struct Dispatch<Value, int16_t>  { using type = typename Int<Value>::type; };
	template <auto Value> struct Dispatch<Value, int32_t>  { using type = typename Int<Value>::type; };
	template <auto Value> struct Dispatch<Value, int64_t>  { using type = typename Int<Value>::type; };
	template <auto Value> struct Dispatch<Value, uint8_t>  { using type = typename Int<Value>::type; };
	template <auto Value> struct Dispatch<Value, uint16_t> { using type = typename Int<Value>::type; };
	template <auto Value> struct Dispatch<Value, uint32_t> { using type = typename Int<Value>::type; };
	template <auto Value> struct Dispatch<Value, uint64_t> { using type = typename Int<Value>::type; };
public:
	using type = typename Dispatch<V>::type;
};

template <>
struct ToCharListImpl<true>
{
	using type = CharList<'t', 'r', 'u', 'e'>;
};

template <>
struct ToCharListImpl<false>
	{
		using type = CharList<'f', 'a', 'l', 's', 'e'>;
	};

	template <auto ... Vs>
	struct CharListImpl
	{
		using type = decltype(CharList<>::Append((typename ToCharListImpl<Vs>::type{})...));
	};

	template <auto ... Vs>
	using CreateCharList = typename CharListImpl<Vs...>::type;

	template <char ... c>
	constexpr auto ToStringView(CharList<c...>) noexcept -> std::string_view
	{
		return { std::data(CharList<c...>::value), CharList<c...>::length };
	}
