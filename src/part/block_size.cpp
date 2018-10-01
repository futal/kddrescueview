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

BlockSize::BlockSize()
    :QStandardItem()
{
}

BlockSize::BlockSize(const BlockSize &other)
    :QStandardItem(other)
{
}

BlockSize::~BlockSize()
{
}

BlockSize::BlockSize(qint64 size)
    :BlockSize()
{
    setData(QVariant(size), Qt::DisplayRole);
}

BlockSize::BlockSize(QString s)
    :BlockSize()
{
    bool conversion_ok;
    qint64 size = s.toLongLong(&conversion_ok, 0);
    if (conversion_ok)
    {
        setData(QVariant(size), Qt::DisplayRole);
    }
}

int BlockSize::type() const
{
    return UserType+1;
}

void BlockSize::operator=(const BlockSize &other)
{
    setData(QVariant(other.data(Qt::DisplayRole)), Qt::DisplayRole);
}

BlockSize BlockSize::operator+(const BlockSize &size) const
{
    qint64 total = data(Qt::DisplayRole).value<quint64>() + size.data(Qt::DisplayRole).value<quint64>();
    return BlockSize(total);
}

void BlockSize::operator+=(const BlockSize &size)
{
    setData(QVariant(data(Qt::DisplayRole).value<quint64>() + size.data(Qt::DisplayRole).value<quint64>()), Qt::DisplayRole);
}

QDebug operator<<(QDebug dbg, const BlockSize &s)
{
    dbg.nospace().noquote() << "Size(0x" << QString::number(s.data(Qt::DisplayRole).value<qint64>(), 16)  << ")";
    return dbg.maybeSpace();
}
