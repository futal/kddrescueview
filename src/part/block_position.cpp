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

#include "block_position.h"
#include "block_size.h"
#include <QDebug>

BlockPosition::BlockPosition()
    :m_position(-1)
{
}

BlockPosition::BlockPosition(const BlockPosition &other)
    :m_position(other.m_position)
{
}

BlockPosition::~BlockPosition()
{
}

BlockPosition::BlockPosition(qint64 position)
    :m_position(position)
{
}

BlockPosition::BlockPosition(QString p)
    :BlockPosition()
{
    bool conversion_ok;
    qint64 position = p.toLongLong(&conversion_ok, 0);
    if (conversion_ok)
    {
        m_position = position;
    }
}

void BlockPosition::operator=(const BlockPosition &other)
{
    m_position = other.m_position;
}

BlockPosition BlockPosition::operator+(const BlockSize &size) const
{
    BlockPosition next_position = m_position + size.m_size;
    return next_position;
}

bool BlockPosition::operator==(const BlockPosition &other) const
{
    return m_position == other.m_position;
}

bool BlockPosition::operator!=(const BlockPosition &other) const
{
    return m_position != other.m_position;
}

bool BlockPosition::operator<=(const BlockPosition &other) const
{
    return m_position <= other.m_position;
}

bool BlockPosition::operator<(const BlockPosition &other) const
{
    return m_position < other.m_position;
}

BlockSize BlockPosition::operator-(const BlockPosition &other) const
{
    qint64 total_size = m_position - other.m_position;
    if (total_size < 0)
    {
        return BlockSize();
    }
    return BlockSize(total_size);
}

QDebug operator<<(QDebug dbg, const BlockPosition &p)
{
    dbg.nospace().noquote() << "Position(0x" << QString::number(p.m_position, 16)  << ")";
    return dbg.maybeSpace();
}
