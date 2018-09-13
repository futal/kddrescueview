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

#ifndef BLOCK_STATUS_H
#define BLOCK_STATUS_H

#include <QStandardItemModel>
#include <QMap>
#include <QMetaType>
#include <QDebug>

/**
 * Type for the recovery status of a block. It is used:
 * - in data block lines of the mapfile.
 * cf. https://www.gnu.org/software/ddrescue/manual/ddrescue_manual.html#Mapfile-structure
 */

static const QMap<QString, QString> statuses {
    { "?", "non-tried block" },
    { "*", "failed block non-trimmed" },
    { "/", "failed block non-scraped" },
    { "-", "failed block bad-sector(s)" },
    { "+", "block recovered" },
}; 

class BlockStatus : public QStandardItem
{
public:
    // to be integrated in the meta-objet system
    BlockStatus();
    BlockStatus(const BlockStatus &other);
    ~BlockStatus();

    // construct from external data
    BlockStatus(QString status);

    // define BlockStatus as a new type of QStandardItem
    int type() const; // +0 for BlockPosition, +1 for BlockSize, +2 for BlockStatus

    bool isValid() const;
    static bool isValid(QString s);

private:
    // QString m_status;  // to be stored with setData(const QVariant &value, int role = Qt::UserRole + 1)
    // QString description() const { return statuses.value(m_status, "unknown block status"); } => to be return for specific roles, e.g. tooltip

};

Q_DECLARE_METATYPE(BlockStatus);  // makes it possible for Position values to be stored in QVariant objects and retrieved later

QDebug operator<<(QDebug dbg, const BlockStatus &status);  // prettify debug output

#endif // BLOCK_STATUS_H
