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

#ifndef BLOCK_POSITION_H
#define BLOCK_POSITION_H

#include "block_size.h"
#include <QStandardItemModel>
#include <QString>
#include <QDebug>
#include <QMetaType>


/**
 * Type for positions (in bytes). It is used:
 * - in the status line of the mapfile for the current position being tried in the input file 
 *   (The beginning of the block being tried in a forward pass or the end of the block in a 
 *   backward pass).
 * - in data block lines of the mapfile (he starting position of the block in the input file).
 * cf. https://www.gnu.org/software/ddrescue/manual/ddrescue_manual.html#Mapfile-structure
 */

class Position : public QStandardItem
{
public:
    // to be integrated in the meta-objet system
    Position();
    Position(const Position &other);
    ~Position();
    
    Position(QString position);
    
    quint64 position() const { return m_position; }

    // to be usable in a QStandardItemModel
    virtual QStandardItem* clone() const { return new Position("0x"+QString::number(m_position, 16)); }
    QVariant data(int role = Qt::UserRole+1 ) const;
    
    void operator=(const qint64 &i);
    Position operator+(const Size &s) const;

private:
    qint64 m_position;
};

Q_DECLARE_METATYPE(Position);  // makes it possible for Position values to be stored in QVariant objects and retrieved later

QDebug operator<<(QDebug dbg, const Position &position);  // prettify debug output


#endif // BLOCK_POSITION_H
