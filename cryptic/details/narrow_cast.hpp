#pragma once

namespace cryptic::details {

template <class T, class U>
constexpr T narrow_cast(U&& u) noexcept
{
    return static_cast<T>(std::forward<U>(u));
}

}
