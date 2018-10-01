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

#ifndef BLOCK_POSITION_H
#define BLOCK_POSITION_H

#include <QDebug>

/**
 * Type for positions (in bytes). It is used:
 * - in the status line of the mapfile for the current position being tried in the input file 
 *   (The beginning of the block being tried in a forward pass or the end of the block in a 
 *   backward pass).
 * - in data block lines of the mapfile (he starting position of the block in the input file).
 * cf. https://www.gnu.org/software/ddrescue/manual/ddrescue_manual.html#Mapfile-structure
 */

class BlockSize;

class BlockPosition
{
public:
    // to be integrated in the meta-objet system
    BlockPosition();
    BlockPosition(const BlockPosition &other);
    ~BlockPosition();

    // construct from external data
    BlockPosition(qint64 position);
    BlockPosition(QString position);
    
    qint64 data() const { return m_position; }

    // operations with BlockPosition:
    void operator=(const BlockPosition &other);
    BlockPosition operator+(const BlockSize &other) const; // for "next block position = block position + block size"
    bool operator==(const BlockPosition &other) const;     // to check contiguous map with "computed next block position == actual next block position"
    bool operator!=(const BlockPosition &other) const;
    bool operator<=(const BlockPosition &other) const;     // to compare two positions
    bool operator<(const BlockPosition &other) const;
    BlockSize operator-(const BlockPosition &other) const; // to find total map size with "Size = (last position + last size) - first position

    friend QDebug operator<<(QDebug dbg, const BlockPosition &position);
private:
    qint64 m_position;
};

Q_DECLARE_METATYPE(BlockPosition);  // makes it possible for Position values to be stored in QVariant objects and retrieved later

QDebug operator<<(QDebug dbg, const BlockPosition &position);  // prettify debug output


#endif // BLOCK_POSITION_H
