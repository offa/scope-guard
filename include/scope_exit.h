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

#include "scope_guard_base.h"

namespace sr
{
    namespace detail
    {
        struct scope_exit_strategy
        {
            constexpr bool should_execute() const
            {
                return true;
            }
        };
    }


    template<class EF>
    class scope_exit : public detail::scope_guard_base<EF, detail::scope_exit_strategy>
    {
    public:

        using detail::scope_guard_base<EF, detail::scope_exit_strategy>::scope_guard_base;


    private:

    };


    template<class EF>
    scope_exit<std::decay_t<EF>> make_scope_exit(EF&& exitFunction)
    {
        return scope_exit<std::decay_t<EF>>{std::forward<EF>(exitFunction)};
    }

}

