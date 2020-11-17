cmake_minimum_required(VERSION 3.13)
project(SUC VERSION 0.1)

if (UNIX AND NOT APPLE)
    set (LINUX TRUE)
endif ()

# set(CMAKE_CXX_CLANG_TIDY
#     clang-tidy;
#     -header-filter=.;
#     -checks=bugprone-*,cert-*,clang-analyzer-*,misc-*,cppcoreguidelines-avoid-goto,cppcoreguidelines-init-variables,cppcoreguidelines-interfaces-global-init,cppcoreguidelines-narrowing-conversions,cppcoreguidelines-no-malloc,cppcoreguidelines-owning-memory,cppcoreguidelines-array-to-pointer-decay,cppcoreguidelines-pro-type-const-cast,cppcoreguidelines-pro-type-cstyle-cast,cppcoreguidelines-pro-type-member-init,cppcoreguidelines-pro-type-vararg,cppcoreguidelines-slicing,cppcoreguidelines-special-member-functions,modernize-avoid-bind,modernize-avoid-c-arrays,modernize-concat-nested-namespaces,modernize-deprecated-headers,modernize-deprecated-ios-base-aliases,modernize-loop-convert,modernize-make-shared,modernize-make-unique,modernize-pass-by-value,modernize-raw-string-literal,modernize-redundant-void-arg,modernize-replace-auto-ptr,modernize-replace-random-shuffle,modernize-return-braced-init-list,modernize-shrink-to-fit,modernize-unary-static-assert,modernize-use-auto,modernize-use-bool-literals,modernize-use-default-member-init,modernize-use-emplace,modernize-use-equals-default,modernize-use-equals-delete,modernize-use-nodiscard,modernize-use-noexcept,modernize-use-nullptr,modernize-use-override,modernize-use-transparent-functors,modernize-use-uncaught-exceptions,modernize-use-using,performance-*,portablility-*,readability-container-size-empty,readability-convert-member-functions-to-static,readability-delete-null-pointer,readbility-deleted-default,readability-else-after-return,readability-magic-numbers,readability-make-member-function-const,readability-redundant-member-init,readability-redundant-smartptr-get,readability-redundant-string-cstr,readability-redundant-string-init,readability-simplify-boolean-expr
#     ;
# )
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(suc)

add_subdirectory(src)
add_subdirectory(test)

target_include_directories(
    suc
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include/
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/include/suc/
)
target_compile_features(suc PUBLIC cxx_std_20)
target_compile_options(
    suc PRIVATE
    $<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W3>
)
