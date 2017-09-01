
#pragma once

#include <utility>

namespace sg
{

    template<class Deleter>
    class scope_guard_t
    {
    public:

        explicit scope_guard_t(Deleter&& deleter) noexcept : m_deleter(std::move(deleter)),
                                                        m_execute_on_destruction(true)
        {
        }

        scope_guard_t(scope_guard_t&&)= default;
        scope_guard_t(const scope_guard_t&) = delete;

        ~scope_guard_t()
        {
            if( m_execute_on_destruction == true )
            {
                m_deleter();
            }
        }


        void release()
        {
            m_execute_on_destruction = false;
        }


        scope_guard_t& operator=(const scope_guard_t&) = delete;
        scope_guard_t& operator=(scope_guard_t&&) = delete;


    private:

        Deleter m_deleter;
        bool m_execute_on_destruction;
    };


    template<class Deleter>
    scope_guard_t<Deleter> scope_guard(Deleter&& deleter) noexcept
    {
        return scope_guard_t<Deleter>(std::move(deleter));
    }

}

