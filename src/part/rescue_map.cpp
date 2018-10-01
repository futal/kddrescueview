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

#include "rescue_map.h"
#include "block_position.h"
#include "block_size.h"
#include "block_status.h"
#include <QDebug>

RescueMap::RescueMap(QObject *parent)
    : QAbstractTableModel(parent)
{
    // this->setHorizontalHeaderLabels({"Position", "Size", "Status"});
}

int RescueMap::rowCount(const QModelIndex & /* parent */) const
{
    return m_positions.count();
}

int RescueMap::columnCount(const QModelIndex & /* parent */) const
{
    return 3;
}

QVariant RescueMap::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();
    QVariant result;
    if (index.column() == 0) result.setValue(m_positions[index.row()].data());
    if (index.column() == 1) result.setValue(m_sizes[index.row()].data());
    if (index.column() == 2) result.setValue(m_statuses[index.row()].data());
    return result;
}

QVariant RescueMap::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == 0) return "Position";
        if (section == 1) return "Size";
        if (section == 2) return "Status";
    }
    return QVariant();
}

void RescueMap::setMap(const QVector<BlockPosition> &positions, const QVector<BlockSize> &sizes, const QVector<BlockStatus> &statuses)
{
    beginResetModel();
    m_positions = positions;
    m_sizes = sizes;
    m_statuses = statuses;
    endResetModel();
}

/*
 * Extract a sub map, e.g. the map corresponding to a square on the grid view
 */

RescueMap* RescueMap::extract(BlockPosition p, BlockSize s) const
{
    RescueMap* map = new RescueMap();
    QVector<BlockPosition> positions;
    QVector<BlockSize> sizes;
    QVector<BlockStatus> statuses;
    
    for(int row = 0; row < this->rowCount(); ++row) 
    {
        BlockPosition extract_start = p;
        BlockSize extract_size = s;
        BlockPosition extract_finish = extract_start + extract_size;
        
        BlockPosition block_start = m_positions[row];
        BlockSize block_size = m_sizes[row];
        BlockPosition block_finish = block_start + block_size;
        
        BlockStatus block_status = m_statuses[row];
        
        // case 1: block_start < block_finish < extract_start < extract_finish
        // nothing to extract yet
        if ( block_finish <= extract_start )
        {
            continue;  
        }
        
        // case 2: block_start < extract_start < block_finish < extract_finish
        // extract the beginning of the block
        if ( block_start <= extract_start && extract_start < block_finish && block_finish <= extract_finish )
        {
            positions.append(BlockPosition(extract_start));
            sizes.append(BlockSize(block_finish - extract_start));
            statuses.append(BlockStatus(block_status));
            continue;
        }
        
        // case 3: block_start < extract_start < extract_finish < block_finish
        // the extract is entirely contained in the block
        if ( block_start <= extract_start && extract_start < extract_finish && extract_finish <= block_finish )
        {
            positions.append(BlockPosition(extract_start));
            sizes.append(BlockSize(extract_size));
            statuses.append(BlockStatus(block_status));
            map->setMap(positions, sizes, statuses);
            return map;
        }
        
        // case 4: extract_start < block_start < block_finish < extract_finish )
        // the block is entirely contained in the extract
        if ( extract_start <= block_start && block_finish <= extract_finish )
        {
            positions.append(BlockPosition(block_start));
            sizes.append(BlockSize(block_size));
            statuses.append(BlockStatus(block_status));
            continue;
        }
        
        // case 5: extract_start < block_start < extract_finish < block_finish )
        // extract the end of the block
        if ( extract_start <= block_start && block_start < extract_finish && extract_finish <= block_finish )
        {
            positions.append(BlockPosition(block_start));
            sizes.append(BlockSize(extract_finish - block_start));
            statuses.append(BlockStatus(block_status));
            map->setMap(positions, sizes, statuses);
            return map;
        }

        // case 6: extract_start < extract_finish < block_start < block_finish )
        // nothing to extract (any more)
        if ( extract_finish <= block_start )
        {
            return map;
        }
        
        qDebug() << "Why are we here?" << endl;
        qDebug() << "Extract block:" << extract_start << extract_size << endl;
        qDebug() << "Block:" << block_start << block_size << endl;
    }
    map->setMap(positions, sizes, statuses);
    return map;
}

BlockPosition RescueMap::start() const
{
    if (hasIndex(0,0))
    {
        return m_positions[0];
    }
    return BlockPosition();
}

BlockSize RescueMap::size() const
{
    int last_row = rowCount() - 1;
    if (hasIndex(last_row, 0) && hasIndex(last_row, 1))
    {
        return BlockSize( m_positions[last_row] + m_sizes[last_row] - start() );
    }
    return BlockSize();
}

QDebug operator<<(QDebug dbg, const RescueMap &map)
{
    dbg << "RescueMap" << endl;
    for(int row = 0; row < map.rowCount(); ++row)
    {
        dbg << map.m_positions[row] << " " << map.m_sizes[row] << " " << map.m_statuses[row] << endl;
    }
    return dbg.maybeSpace();
}
