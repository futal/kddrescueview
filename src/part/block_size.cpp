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

#include "block_size.h"

Size::Size()
    :m_size(-1)
{
}

Size::Size(const Size &other)
    :QStandardItem(other)
{
    this->m_size = other.m_size;
}

Size::~Size()
{
}

Size::Size(QString size)
{
    bool conversion_ok;
    m_size = size.toLongLong(&conversion_ok, 0);
    if( !conversion_ok ) {
        m_size = -1;
    }
}

Size::Size(qint64 size)
{
    m_size = size;
}

QVariant Size::data(int role) const 
{
    if( role == Qt::DisplayRole ) {
        return "0x"+QString::number(m_size, 16);
    }
    if( role == Qt::UserRole ) {
        return m_size;
    }
    return QStandardItem::data(role);
}

void Size::operator=(const qint64 &i)
{
    // TODO: refuse negative values
    m_size = i;
}

Size Size::operator+(const Size &s) const
{
    Size result;
    result = this->size() + s.size();  // operator= convert back to Size
    return result;
}

void Size::operator+=(const Size &s)
{
    m_size += s.size();
}

QDebug operator<<(QDebug dbg, const Size &s)
{
    dbg.nospace() << "Size(0x" << QString::number(s.size(), 16)  << ")";
    return dbg.maybeSpace();
}
