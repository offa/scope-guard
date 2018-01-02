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

#include "detail/scope_guard_base.h"
#include <exception>

namespace sr
{
    namespace detail
    {

        struct scope_success_strategy
        {
            bool should_execute() const noexcept
            {
                return std::uncaught_exceptions() <= m_uncaught_on_creation;
            }


            int m_uncaught_on_creation = std::uncaught_exceptions();
        };


        template<class F>
        struct is_noexcept_dtor<F, scope_success_strategy>
        {
            static constexpr bool value = noexcept(std::declval<F>()());
        };

    }


    template<class EF>
    class scope_success : public detail::scope_guard_base<EF, detail::scope_success_strategy>
    {
        using Base = detail::scope_guard_base<EF, detail::scope_success_strategy>;


    public:

        template<class EFP,
            std::enable_if_t<std::is_constructible_v<EF, EFP>, int> = 0,
            std::enable_if_t<!std::is_same_v<detail::remove_cvref_t<EFP>, scope_success<EF>>, int> = 0
            >
        explicit scope_success(EFP&& exitFunction) noexcept(std::is_nothrow_constructible_v<EF, EFP>)
                                                : Base(std::forward<EFP>(exitFunction))
        {
        }

    };


    template<class EF>
    scope_success(EF) -> scope_success<EF>;

}

