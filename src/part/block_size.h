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

#ifndef BLOCK_SIZE_H
#define BLOCK_SIZE_H

#include <QDebug>

/**
 * Type for block size (in bytes). It is used:
 * - in data block lines of the mapfile (size of the block in the input file).
 * cf. https://www.gnu.org/software/ddrescue/manual/ddrescue_manual.html#Mapfile-structure
 * - to report the size of the recovery area of the input file.
 */

class BlockPosition;

class BlockSize
{
public:
    // to be integrated in the meta-objet system
    BlockSize();
    BlockSize(const BlockSize &other);
    ~BlockSize();

    // construct from external data
    BlockSize(qint64 size);
    BlockSize(QString size);

    qint64 data() const { return m_size; }

    // operations with BlockSize:
    void operator=(const BlockSize &other);
    BlockSize operator+(const BlockSize &other) const;   // for "size = size1 + size2"
    void operator+=(const BlockSize &other);        // for "size += size1"
//    bool operator==(const BlockSize &other) const;     // for "size1 == size2"
//    BlockSize operator-(const BlockSize &other) const; // for "size = size1 - size2"

    friend class BlockPosition;
    friend QDebug operator<<(QDebug dbg, const BlockSize &size);
private:
    qint64 m_size;
};

Q_DECLARE_METATYPE(BlockSize);  // makes it possible for Size values to be stored in QVariant objects and retrieved later

QDebug operator<<(QDebug dbg, const BlockSize &size);  // prettify debug output

#endif // BLOCK_SIZE_H
