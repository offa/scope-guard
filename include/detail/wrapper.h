// MIT License
//
// Copyright (c) 2017-2018 offa
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
