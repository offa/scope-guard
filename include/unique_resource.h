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
    template<class Ressource, class Deleter>
    class unique_resource_t
    {
    public:

        explicit unique_resource_t(Ressource&& res, Deleter&& deleter, bool shouldrun = true) noexcept : m_deleter(std::move(deleter))
        {
            static_cast<void>(res);
            static_cast<void>(shouldrun);
        }


        ~unique_resource_t()
        {
            m_deleter();
        }


    private:

        Deleter m_deleter;
    };



    template<class Ressource, class Deleter>
    unique_resource_t<Ressource, Deleter> unique_resource(Ressource&& res, Deleter d) noexcept
    {
        return unique_resource_t<Ressource, Deleter>{std::move(res), std::move(d), true};
    }

}


