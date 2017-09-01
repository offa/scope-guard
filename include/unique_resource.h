/*
 * Scope Guard
 * Copyright (C) 2017  offa
 *
 * This file is part of Scope Guard.
 *
 * Scope Guard is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Scope Guard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Scope Guard.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <utility>

namespace sr
{

    enum class invoke_it
    {
        once,
        again
    };


    template<class Ressource, class Deleter>
    class unique_resource_t
    {
    public:

        explicit unique_resource_t(Ressource&& res, Deleter&& deleter, bool shouldrun = true) noexcept : m_resource(std::move(res)),
                                                                                                    m_deleter(std::move(deleter)),
                                                                                                    m_execute_on_destruction(shouldrun)
        {
        }

        unique_resource_t(const unique_resource_t&) = delete;

        unique_resource_t(unique_resource_t&& other) noexcept : m_resource(std::move(other.m_resource)),
                                                            m_deleter(std::move(other.m_deleter)),
                                                            m_execute_on_destruction(other.m_execute_on_destruction)
        {
            other.release();
        }

        ~unique_resource_t()
        {
            invoke(invoke_it::once);
        }


        void invoke(const invoke_it strategy = invoke_it::once) noexcept
        {
            if( m_execute_on_destruction == true )
            {
                callDeleterSafe();
            }

            m_execute_on_destruction = ( strategy == invoke_it::again );
        }

        const Ressource& release() noexcept
        {
            m_execute_on_destruction = false;
            return m_resource;
        }


        unique_resource_t& operator=(unique_resource_t&& other) noexcept
        {
            invoke(invoke_it::once);
            m_resource = std::move(other.m_resource);
            m_deleter = std::move(other.m_deleter);
            m_execute_on_destruction = other.m_execute_on_destruction;
            other.release();

            return *this;
        }


        unique_resource_t& operator=(const unique_resource_t&) = delete;


    private:

        void callDeleterSafe() noexcept
        {
            try
            {
                m_deleter(m_resource);
            }
            catch( ... ) { /* Empty */ }
        }

        Ressource m_resource;
        Deleter m_deleter;
        bool m_execute_on_destruction;
    };



    template<class Ressource, class Deleter>
    unique_resource_t<Ressource, Deleter> unique_resource(Ressource&& res, Deleter d) noexcept
    {
        return unique_resource_t<Ressource, Deleter>{std::move(res), std::move(d), true};
    }

}


