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

#include "rescue_status.h"
#include "rescue_operation.h"

#include <QDebug>

RescueStatus::RescueStatus():
    m_current_position(),
    m_current_operation(),
    m_current_pass()
{
}

void RescueStatus::setCurrentPosition(qint64 position)
{
    m_current_position = BlockPosition(position);
}

void RescueStatus::setCurrentOperation(QString operation)
{
    m_current_operation.setOperation(operation);
}

bool RescueStatus::setCurrentPass(int pass)
{
    if( pass <= 0 ) {
        qDebug() << "Error: pass number must be greater than 1";
        return false;
    }
    m_current_pass = pass;
    return true;
}
