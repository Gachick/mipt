#include <string>
#include <type_traits>
#include <vector>

namespace TL {
template <typename... Types> struct TypeList {};

template <typename... Types> struct Size;

template <typename... Types> struct Size<TypeList<Types...>> {
  static constexpr std::size_t value = sizeof...(Types);
};

template <std::size_t N, typename TypeList> struct TypeAt;

template <std::size_t N, typename Head, typename... Tail>
struct TypeAt<N, TypeList<Head, Tail...>> {
  using type = typename TypeAt<N - 1, TypeList<Tail...>>::type;
};

template <typename Head, typename... Tail>
struct TypeAt<0, TypeList<Head, Tail...>> {
  using type = Head;
};

template <typename T, typename TypeList> struct Contains;

template <typename T, typename... Types>
struct Contains<T, TypeList<Types...>> {
  static constexpr bool value = (std::is_same_v<T, Types> || ...);
};

template <typename T, typename TypeList, std::size_t Index = 0> struct IndexOf;

template <typename T, typename Head, typename... Tail, std::size_t Index>
struct IndexOf<T, TypeList<Head, Tail...>, Index> {
  static constexpr std::size_t value =
      std::is_same_v<T, Head> ? Index
                              : IndexOf<T, TypeList<Tail...>, Index + 1>::value;
};

template <typename T, std::size_t Index> struct IndexOf<T, TypeList<>, Index> {
  static constexpr std::size_t value = static_cast<std::size_t>(-1);
};

template <typename T, typename TypeList> struct PushBack;

template <typename T, typename... Types>
struct PushBack<T, TypeList<Types...>> {
  using type = TypeList<Types..., T>;
};

template <typename T, typename TypeList> struct PushFront;

template <typename T, typename... Types>
struct PushFront<T, TypeList<Types...>> {
  using type = TypeList<T, Types...>;
};
} // namespace TL

int main() {
  using TypeList =
      TL::TypeList<void, int, double, std::string, std::vector<char>>;

  static_assert(std::is_same_v<TL::TypeAt<2, TypeList>::type, double>,
                "Index 2 should be double");
  static_assert(TL::Size<TypeList>::value == 5, "Size should be 5");
  static_assert(TL::Contains<double, TypeList>::value,
                "List should contain double");
  static_assert(TL::IndexOf<void, TypeList>::value == 0,
                "Index of void should be 0");

  using TLPB = TL::PushBack<bool, TypeList>::type;
  static_assert(std::is_same_v<TL::TypeAt<5, TLPB>::type, bool>,
                "Type at index 5 should be bool");
  using TLPF = TL::PushFront<bool, TypeList>::type;
  static_assert(std::is_same_v<TL::TypeAt<0, TLPF>::type, bool>,
                "Type at index 0 should be bool");

  return 0;
}
