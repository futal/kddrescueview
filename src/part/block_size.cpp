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
#include "block_position.h"
#include <QDebug>

BlockSize::BlockSize()
    :m_size(-1)
{
}

BlockSize::BlockSize(const BlockSize &other)
    :m_size(other.m_size)
{
}

BlockSize::~BlockSize()
{
}

BlockSize::BlockSize(qint64 size)
    :m_size(size)
{
}

BlockSize::BlockSize(QString s)
    :BlockSize()
{
    bool conversion_ok;
    qint64 size = s.toLongLong(&conversion_ok, 0);
    if (conversion_ok)
    {
        m_size = size;
    }
}

void BlockSize::operator=(const BlockSize &other)
{
    m_size = other.m_size;
}

BlockSize BlockSize::operator+(const BlockSize &size) const
{
    qint64 total = m_size + size.m_size;
    return BlockSize(total);
}

void BlockSize::operator+=(const BlockSize &size)
{
    m_size += size.m_size;
}

BlockSize BlockSize::operator/(const BlockSize &other) const
{
    if (!other.m_size)
    {
        qDebug() << "Error: cannot divide by a 0 size." << endl;
        return 0;
    }
    qint64 result = m_size / other.m_size;
    qint64 rest = m_size % other.m_size;
    if ( rest != 0 )
    {
        qDebug() << "Error: " << *this << " is not a multiple of " << other << ": rest is " << rest;
        return 0;
    }
    return BlockSize(result);
}

double BlockSize::operator/(const double &d) const
{
    return m_size / d;
}

BlockSize BlockSize::operator*(const int &i) const
{
    return BlockSize(m_size * i);
}

QDebug operator<<(QDebug dbg, const BlockSize &s)
{
    dbg.nospace().noquote() << "Size(0x" << QString::number(s.m_size, 16)  << ")";
    return dbg.maybeSpace();
}
