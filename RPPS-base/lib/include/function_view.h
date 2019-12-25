#pragma once
//Borrowed from here: https://github.com/SuperV1234/vittorioromeo.info/blob/master/extra/passing_functions_to_functions/function_view.hpp
#include <type_traits>
#include <functional>

template <typename...>
using void_t = void;

namespace std {

template< class T >
using result_of_t = typename std::result_of<T>::type;
template<bool B, class T = void >
using enable_if_t = typename std::enable_if<B, T>::type;
template< class T >
using decay_t = typename std::decay<T>::type;
template< class T >
using add_pointer_t = typename std::add_pointer<T>::type;

}

template <class T, class R = void, class = void>
struct is_callable : std::false_type
{
};

template <class T>
struct is_callable<T, void, void_t<std::result_of_t<T>>> : std::true_type
{
};

template <class T, class R>
struct is_callable<T, R, void_t<std::result_of_t<T>>>
    : std::is_convertible<std::result_of_t<T>, R>
{
};

template <typename TSignature>
struct signature_helper;

template <typename TReturn, typename... TArgs>
struct signature_helper<TReturn(TArgs...)>
{
    using fn_ptr_type = TReturn (*)(TArgs...);
};

template <typename TSignature>
using fn_ptr = typename signature_helper<TSignature>::fn_ptr_type;

template <typename T>
struct dependent_false : std::false_type
{
};



template <typename TSignature>
class function_view;

template <typename TReturn, typename... TArgs>
class function_view<TReturn(TArgs...)> final
{
private:
    using signature_type = TReturn(void*, TArgs...);

    void* _ptr;
    TReturn (*_erased_fn)(void*, TArgs...);

public:
    template <typename T, typename = std::enable_if_t<
                              is_callable<T&(TArgs...)>{} &&
                              !std::is_same<std::decay_t<T>, function_view>{}>>
    function_view(T&& x) noexcept : _ptr{(void*)std::addressof(x)}
    {
        _erased_fn = [](void* ptr, TArgs... xs) -> TReturn {
            return (*reinterpret_cast<std::add_pointer_t<T>>(ptr))(
                std::forward<TArgs>(xs)...);
        };
    }

    auto operator()(TArgs... xs) const
        noexcept(noexcept(_erased_fn(_ptr, std::forward<TArgs>(xs)...)))
            -> decltype (_erased_fn(_ptr, std::forward<TArgs>(xs)...))
    {
        return _erased_fn(_ptr, std::forward<TArgs>(xs)...);
    }
    void* get_efn_ptr() const { return (void*)_erased_fn; }
    void* get_ptr() const { return _ptr; }

};
