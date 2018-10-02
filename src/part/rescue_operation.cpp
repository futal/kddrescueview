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

#include "rescue_operation.h"
#include <QDebug>

RescueOperation::RescueOperation()
    :m_operation("U")
{
}

RescueOperation::RescueOperation(const RescueOperation &other)
    :m_operation(other.m_operation)
{
}

RescueOperation::~RescueOperation()
{
}

RescueOperation::RescueOperation(QString operation)
    :RescueOperation()
{
    if (RescueOperation::isValid(operation))
    {
        m_operation = operation;
    }
}

void RescueOperation::setOperation(QString operation)
{
    if (RescueOperation::isValid(operation))
    {
        m_operation = operation;
    }
    else
    {
        m_operation = "U";
    }
}

QString RescueOperation::data() const
{
    return m_operation;
}

bool RescueOperation::isValid() const
{
    return operations.count(m_operation);
}

bool RescueOperation::isValid(QString s)  /* static method */
{
    return operations.count(s);
}

QDebug operator<<(QDebug dbg, const RescueOperation &o)
{
    dbg.nospace() << "Operation(" << o.m_operation << ": " << operations.value(o.m_operation, "Unknown operation") << ")";
    return dbg.maybeSpace();
}
