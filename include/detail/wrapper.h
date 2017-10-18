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

#include <functional>
#include <type_traits>

namespace sr
{
    namespace detail
    {

        template<class T>
        class Wrapper
        {
        public:

            template<class TT, class G, std::enable_if_t<std::is_constructible_v<T, TT>, int> = 0>
            explicit Wrapper(TT&& value, G&& g) noexcept(noexcept(Wrapper{value})) : Wrapper(value)
            {
                g.release();
            }


            T& get() noexcept
            {
                return m_value;
            }

            const T& get() const noexcept
            {
                return m_value;
            }

            void reset(T&& newValue) noexcept(std::is_nothrow_assignable_v<T, decltype(std::move_if_noexcept(newValue))>)
            {
                m_value = std::move_if_noexcept(newValue);
            }

            void reset(const T& newValue) noexcept(std::is_nothrow_assignable_v<T, const T&>)
            {
                m_value = newValue;
            }


        private:

            Wrapper(const T& value) noexcept(noexcept(T{value})) : m_value(value)
            {
            }

            Wrapper(T&& value) noexcept(noexcept(T{std::move_if_noexcept(value)})) : m_value(std::move_if_noexcept(value))
            {
            }


            T m_value;
        };


        template<class T>
        class Wrapper<T&>
        {
        public:

            template<class TT, class G, std::enable_if_t<std::is_convertible_v<TT, T&>, int> = 0>
            explicit Wrapper(TT&& value, G&& g) noexcept(noexcept(static_cast<T&>(value))) : m_value(static_cast<T&>(value))
            {
                g.release();
            }


            T& get() noexcept
            {
                return m_value.get();
            }

            const T& get() const noexcept
            {
                return m_value.get();
            }

            void reset(T& newValue) noexcept
            {
                m_value = std::ref(newValue);
            }


        private:

            std::reference_wrapper<T> m_value;
        };

    }
}
