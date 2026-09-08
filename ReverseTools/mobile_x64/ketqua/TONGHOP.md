# Khao sat clang-cl (NDK r25 LLVM 14, -fsyntax-only, header MSVC) - 08/09 14:49

| du an | tep | tep co loi | loi |
|---|---|---|---|
| engine | 137 | 7 | 13 |
| core | 103 | 101 | 541 |
| rep3 | 19 | 5 | 20 |
| s3client | 186 | 148 | 1362 |

## engine - 40 loai loi dau

| so lan | tep | loai |
|---|---|---|
| 13 | 7 | `no matching function for call to 'X'` |

## core - 40 loai loi dau

| so lan | tep | loai |
|---|---|---|
| 101 | 101 | `[HEADER HE THONG] static_assert failed "get<T>(tuple<Types...>&) requires T to occur exactly once in Types. (N4971 [tuple.elem]/N)"` |
| 101 | 101 | `[HEADER HE THONG] static_assert failed "get<T>(const tuple<Types...>&) requires T to occur exactly once in Types. (N4971 [tuple.elem]/N)"` |
| 101 | 101 | `[HEADER HE THONG] static_assert failed "get<T>(tuple<Types...>&&) requires T to occur exactly once in Types. (N4971 [tuple.elem]/N)"` |
| 101 | 101 | `[HEADER HE THONG] static_assert failed "get<T>(const tuple<Types...>&&) requires T to occur exactly once in Types. (N4971 [tuple.elem]/N)"` |
| 66 | 6 | `[HEADER HE THONG] redefinition of 'X'` |
| 60 | 6 | `[HEADER HE THONG] expected identifier` |
| 7 | 1 | `cannot initialize a parameter of type 'X' (aka 'X') with an lvalue of type 'X'` |
| 2 | 2 | `'X' file not found` |
| 1 | 1 | `cannot initialize a parameter of type 'X' (aka 'X') with an rvalue of type 'X'` |
| 1 | 1 | `left operand to ? is void, but right operand is of type 'X' (aka 'X')` |

## rep3 - 40 loai loi dau

| so lan | tep | loai |
|---|---|---|
| 5 | 5 | `[HEADER HE THONG] static_assert failed "get<T>(tuple<Types...>&) requires T to occur exactly once in Types. (N4971 [tuple.elem]/N)"` |
| 5 | 5 | `[HEADER HE THONG] static_assert failed "get<T>(const tuple<Types...>&) requires T to occur exactly once in Types. (N4971 [tuple.elem]/N)"` |
| 5 | 5 | `[HEADER HE THONG] static_assert failed "get<T>(tuple<Types...>&&) requires T to occur exactly once in Types. (N4971 [tuple.elem]/N)"` |
| 5 | 5 | `[HEADER HE THONG] static_assert failed "get<T>(const tuple<Types...>&&) requires T to occur exactly once in Types. (N4971 [tuple.elem]/N)"` |

## s3client - 40 loai loi dau

| so lan | tep | loai |
|---|---|---|
| 361 | 101 | `virtual function 'X' has a different return type ('X') than the function it overrides (which has return type 'X')` |
| 361 | 101 | `'X' member function 'X' overrides a virtual function in a base class` |
| 143 | 143 | `[HEADER HE THONG] static_assert failed "get<T>(tuple<Types...>&) requires T to occur exactly once in Types. (N4971 [tuple.elem]/N)"` |
| 143 | 143 | `[HEADER HE THONG] static_assert failed "get<T>(const tuple<Types...>&) requires T to occur exactly once in Types. (N4971 [tuple.elem]/N)"` |
| 143 | 143 | `[HEADER HE THONG] static_assert failed "get<T>(tuple<Types...>&&) requires T to occur exactly once in Types. (N4971 [tuple.elem]/N)"` |
| 143 | 143 | `[HEADER HE THONG] static_assert failed "get<T>(const tuple<Types...>&&) requires T to occur exactly once in Types. (N4971 [tuple.elem]/N)"` |
| 56 | 1 | `expected expression` |
| 3 | 3 | `ordered comparison between pointer and zero ('X' (aka 'X') and 'X')` |
| 2 | 1 | `expected 'X'` |
| 2 | 1 | `\U used with no following hex digits` |
| 1 | 1 | `character <U+00BB> not allowed in an identifier` |
| 1 | 1 | `character <U+00BF> not allowed in an identifier` |
| 1 | 1 | `unknown type name 'X'` |
| 1 | 1 | `[HEADER HE THONG] expected unqualified-id` |
| 1 | 1 | `expected unqualified-id` |

