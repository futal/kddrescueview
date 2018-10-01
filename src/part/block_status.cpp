/*
 * Kddrescueview - A KPart application to visualise GNU ddrescue mapfiles
 * Copyright 2018  Adrien Cordonnier <adrien.cordonnier@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "block_status.h"

BlockStatus::BlockStatus()
    :m_status("U")
{
}

BlockStatus::BlockStatus(const BlockStatus &other)
    :m_status(other.m_status)
{
}

BlockStatus::~BlockStatus()
{
}

BlockStatus::BlockStatus(QString status)
    :BlockStatus()
{
    if (BlockStatus::isValid(status))
    {
        m_status = status;
    }
}

bool BlockStatus::isValid() const
{
    return statuses.count(m_status);
}

bool BlockStatus::isValid(QString s)  /* static method */
{
    return statuses.count(s);
}

QDebug operator<<(QDebug dbg, const BlockStatus &s)
{
    dbg.nospace().noquote() << "Status(" << s.m_status << ": " << statuses.value(s.m_status, "Unknown block status") << ")";
    return dbg.maybeSpace();
}
