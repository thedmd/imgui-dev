# pragma once
# include <type_traits>

# define AX_BITMASK_TYPE(E)                                                     \
                                                                                \
inline E operator&(E lhs, E rhs)                                                \
{                                                                               \
    using base = typename std::underlying_type<E>::type;                        \
    return static_cast<E>(static_cast<base>(lhs) & static_cast<base>(rhs));     \
}                                                                               \
                                                                                \
inline E operator|(E lhs, E rhs)                                                \
{                                                                               \
    using base = typename std::underlying_type<E>::type;                        \
    return static_cast<E>(static_cast<base>(lhs) | static_cast<base>(rhs));     \
}                                                                               \
                                                                                \
inline E operator^(E lhs, E rhs)                                                \
{                                                                               \
    using base = typename std::underlying_type<E>::type;                        \
    return static_cast<E>(static_cast<base>(lhs) ^ static_cast<base>(rhs));     \
}                                                                               \
                                                                                \
inline E operator~(E lhs)                                                       \
{                                                                               \
    using base = typename std::underlying_type<E>::type;                        \
    return static_cast<E>(~static_cast<base>(lhs));                             \
}                                                                               \
                                                                                \
inline E& operator&=(E& lhs, E rhs)                                             \
{                                                                               \
    using base = typename std::underlying_type<E>::type;                        \
    lhs = static_cast<E>(static_cast<base>(lhs) & static_cast<base>(rhs));      \
    return lhs;                                                                 \
}                                                                               \
                                                                                \
inline E& operator|=(E& lhs, E rhs)                                             \
{                                                                               \
    using base = typename std::underlying_type<E>::type;                        \
    lhs = static_cast<E>(static_cast<base>(lhs) | static_cast<base>(rhs));      \
    return lhs;                                                                 \
}                                                                               \
                                                                                \
inline E& operator^=(E& lhs, E rhs)                                             \
{                                                                               \
    using base = typename std::underlying_type<E>::type;                        \
    lhs = static_cast<E>(static_cast<base>(lhs) ^ static_cast<base>(rhs));      \
    return lhs;                                                                 \
}                                                                               \
                                                                                \
inline bool operator!(E lhs)                                                    \
{                                                                               \
    using base = typename std::underlying_type<E>::type;                        \
    return !static_cast<base>(lhs);                                             \
}                                                                               \
                                                                                \
static_assert(std::is_enum<E>::value, #E " is not an enum type")