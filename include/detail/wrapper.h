/*
 * Scope Guard
 * Copyright (C) 2017-2018  offa
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

namespace sr::detail
{

   template<class T>
   class Wrapper
   {
   public:

        template<class TT, class G, std::enable_if_t<std::is_constructible_v<T, TT>, int> = 0>
        Wrapper(TT&& value, G&& g) noexcept(std::is_nothrow_constructible_v<T, TT>) : m_value(std::forward<TT>(value))
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

        void reset(Wrapper<T>&& other) noexcept
        {
            m_value = std::move(other.m_value);
        }

        void reset(T&& newValue) noexcept(std::is_nothrow_assignable_v<T, decltype(std::move_if_noexcept(newValue))>)
        {
            m_value = std::forward<T>(newValue);
        }

        void reset(const T& newValue) noexcept(std::is_nothrow_assignable_v<T, const T&>)
        {
            m_value = newValue;
        }


        using type = T;


    private:

        T m_value;
    };


   template<class T>
   class Wrapper<T&>
   {
   public:

        template<class TT, class G, std::enable_if_t<std::is_convertible_v<TT, T&>, int> = 0>
        Wrapper(TT&& value, G&& g) noexcept(std::is_nothrow_constructible_v<TT, T&>) : m_value(static_cast<T&>(value))
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

        void reset(Wrapper<T>&& other) noexcept
        {
            m_value = std::move(other.m_value);
        }

        void reset(T& newValue) noexcept
        {
            m_value = std::ref(newValue);
        }


        using type = std::reference_wrapper<std::remove_reference_t<T>>;


    private:

        type m_value;
   };

}
