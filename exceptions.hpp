#ifndef SJTU_EXCEPTIONS_HPP
#define SJTU_EXCEPTIONS_HPP
#include <exception>

namespace sjtu {
struct index_out_of_bound : std::exception { const char *what() const noexcept override { return index_out_of_bound; } };
struct runtime_error : std::exception { const char *what() const noexcept override { return runtime_error; } };
struct invalid_iterator : std::exception { const char *what() const noexcept override { return invalid_iterator; } };
struct container_is_empty : std::exception { const char *what() const noexcept override { return container_is_empty; } };
}

#endif

