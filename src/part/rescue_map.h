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

#ifndef RESCUE_MAP_H
#define RESCUE_MAP_H

#include <QAbstractTableModel>
#include "block_position.h"
#include "block_size.h"
#include "block_status.h"

class RescueTotals;


class RescueMap : public QAbstractTableModel
{
    Q_OBJECT
public:
    RescueMap(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void setMap(const QVector<BlockPosition> &positions, const QVector<BlockSize> &sizes, const QVector<BlockStatus> &statuses);
    RescueMap* extract(BlockPosition start, BlockSize size) const;
    BlockPosition start() const;
    BlockSize size() const;

    friend class RescueTotals;
    friend QDebug operator<<(QDebug dbg, const RescueMap &map);
private:
    int m_columns;
    int m_rows;
    QVector<BlockPosition> m_positions;
    QVector<BlockSize> m_sizes;
    QVector<BlockStatus> m_statuses;
};

QDebug operator<<(QDebug dbg, const RescueMap &map);  // prettify debug output

#endif // RESCUE_MAP_H
