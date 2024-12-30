#pragma once

namespace portable_executable::pe_crt
{
    template <bool _Test, class _Ty1, class _Ty2>
    struct conditional
    {
        using type = _Ty1;
    };

    template <class _Ty1, class _Ty2>
    struct conditional<false, _Ty1, _Ty2>
    {
        using type = _Ty2;
    };

    template <class _Ty, _Ty _Val>
    struct integral_constant
    {
        static constexpr _Ty value = _Val;

        using value_type = _Ty;
        using type = integral_constant;

        constexpr operator value_type() const noexcept
        {
            return value;
        }

        constexpr value_type operator()() const noexcept
        {
            return value;
        }
    };

    template <bool _Val>
    using bool_constant = integral_constant<bool, _Val>;

    template <class>
    constexpr bool is_const_v = false;

    template <class _Ty>
    constexpr bool is_const_v<const _Ty> = true;

    template <class _Ty>
    struct is_const : bool_constant<is_const_v<_Ty>>
    {

    };
}