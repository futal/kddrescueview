/*
 * Kddrescueview - A KPart to visualise GNU ddrescue mapfiles
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

#include "block_position.h"
#include <QDebug>

Position::Position()
    :m_position(-1)
{
}

Position::Position(const Position &other)
    :QStandardItem(other)
{
    m_position = other.m_position;
}

Position::~Position()
{
}

Position::Position(QString position)
{
    bool conversion_ok;
    m_position = position.toLongLong(&conversion_ok, 0);
    if( !conversion_ok ) {
        m_position = -1;
    }
}
    
QVariant Position::data(int role) const
{
    if( role == Qt::DisplayRole ) {
        return "0x"+QString::number(m_position, 16);
    }
    if( role == Qt::UserRole ) {
        return m_position;
    }
    return QStandardItem::data(role);
}

void Position::operator=(const qint64 &i)
{
    // TODO: refuse negative values
    m_position = i;
}

Position Position::operator+(const Size &s) const
{
    Position result;
    result = this->position() + s.size();  // operator= convert back to Position
    return result;
}


QDebug operator<<(QDebug dbg, const Position &p)
{
    dbg.nospace() << "Position(0x" << QString::number(p.position(), 16)  << ")";
    return dbg.maybeSpace();
}
