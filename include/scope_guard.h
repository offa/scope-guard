
#pragma once

#include <utility>

namespace guards
{

    template<class Deleter>
    class scope_guard_t
    {
    public:

        explicit scope_guard_t(Deleter&& deleter) noexcept : m_deleter(std::move(deleter))
        {
        }

        scope_guard_t(scope_guard_t&&)= default;
        scope_guard_t(const scope_guard_t&) = delete;

        ~scope_guard_t()
        {
            m_deleter();
        }


        scope_guard_t& operator=(const scope_guard_t&) = delete;
        scope_guard_t& operator=(scope_guard_t&&) = delete;


    private:

        Deleter m_deleter;
    };


    template<class Deleter>
    scope_guard_t<Deleter> scope_guard(Deleter&& deleter) noexcept
    {
        return scope_guard_t<Deleter>(std::move(deleter));
    }

}

