#include <any>
#include <iostream>
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

template <typename... Types> using TypeList = TL::TypeList<Types...>;

template <typename... Args> struct TypeMap;

template <typename Head, typename... Tail> struct TypeMap<Head, Tail...> {
  TypeMap() {
    using keys = TypeList<Head, Tail...>;
    values.resize(sizeof...(Tail) + 1);
  };

  template <typename T> void AddValue(T value) {
    constexpr int i = TL::IndexOf<T, keys>::value;
    values[i] = std::any(value);
  };

  template <typename T> T GetValue() {
    int i = TL::IndexOf<T, keys>::value;
    return std::any_cast<T>(values[i]);
  };

  template <typename T> bool Contains() { return TL::Contains<T, keys>::value; }

  template <typename T> void RemoveValue() {
    constexpr int i = TL::IndexOf<T, keys>::value;
    values[i] = std::any(nullptr);
  };

private:
  using keys = TypeList<Head, Tail...>;
  std::vector<std::any> values;
};

struct DataA {
  std::string value;
};
struct DataB {
  int value;
};
int main() {
  TypeMap<int, DataA, double, DataB> myTypeMap;
  // Добавление элементов в контейнер
  myTypeMap.AddValue<int>(42);
  myTypeMap.AddValue<double>(3.14);
  myTypeMap.AddValue<DataA>({"Hello, TypeMap!"});
  myTypeMap.AddValue<DataB>({10});
  // Получение и вывод значений по типам ключей
  std::cout << "Value for int: " << myTypeMap.GetValue<int>() << std::endl;
  // // Вывод: 42
  std::cout << "Value for double: " << myTypeMap.GetValue<double>()
            << std::endl; // Вывод: 3.14
  std::cout << "Value for DataA: " << myTypeMap.GetValue<DataA>().value
            << std::endl; // Вывод: Hello, TypeMap!
  std::cout << "Value for DataB: " << myTypeMap.GetValue<DataB>().value
            << std::endl; // Вывод: 10
  // Проверка наличия элемента
  std::cout << "Contains int? " << (myTypeMap.Contains<int>() ? "Yes" : "No")
            << std::endl; // Вывод: Yes
  // Удаление элемента
  myTypeMap.RemoveValue<double>();
  // Попытка получения удаленного элемента
  std::cout << "Value for double after removal: "
            << myTypeMap.GetValue<double>()
            << std::endl; // Вывод: (некорректное значение)
  return 0;
}
