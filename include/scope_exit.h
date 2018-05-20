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

namespace sr
{
    namespace detail
    {

        struct scope_exit_strategy
        {
            bool should_execute() const noexcept
            {
                return true;
            }

        };

    }


    template<class EF>
    class scope_exit : public detail::scope_guard_base<EF, detail::scope_exit_strategy>
    {
        using ScopeGuardBase = std::enable_if_t<!std::is_same_v<detail::remove_cvref_t<EF>, scope_exit<EF>>,
                                                detail::scope_guard_base<EF, detail::scope_exit_strategy>
                                                >;

    public:

        using ScopeGuardBase::ScopeGuardBase;

    };


    template<class EF>
    scope_exit(EF) -> scope_exit<EF>;

}

