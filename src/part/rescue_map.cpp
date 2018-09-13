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
#include "block_status.h"
#include <QStandardItemModel>
#include <QDebug>

RescueMap::RescueMap(QObject *parent)
    :QStandardItemModel(parent)
{
    this->setHorizontalHeaderLabels({"Position", "Size", "Status"});
}

RescueMap::~RescueMap()
{
}

/*
 * Extract a sub map, e.g. the map corresponding to a square on the grid view
 */

RescueMap* RescueMap::extract(BlockPosition p, BlockSize s) const
{
    RescueMap* map = new RescueMap();
    
    for(int row = 0; row < this->rowCount(); ++row) 
    {
        BlockPosition extract_start = p;
        BlockSize extract_size = s;
        BlockPosition extract_finish = extract_start + extract_size;
        
        BlockPosition block_start = *(dynamic_cast<BlockPosition*>(item(row, 0)));
        BlockSize block_size = *(dynamic_cast<BlockSize*>(item(row, 1)));
        BlockPosition block_finish = block_start + block_size;
        
        BlockStatus block_status = *(dynamic_cast<BlockStatus*>(item(row, 2)));
        
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
            QStandardItem* start = new BlockPosition(extract_start);
            QStandardItem* size = new BlockSize(block_finish - extract_start);
            QStandardItem* status = new BlockStatus(block_status);
            QList<QStandardItem*> block_data = { start, size, status };
            map->appendRow(block_data);
            continue;
        }
        
        // case 3: block_start < extract_start < extract_finish < block_finish
        // the extract is entirely contained in the block
        if ( block_start <= extract_start && extract_start < extract_finish && extract_finish <= block_finish )
        {
            QStandardItem* start = new BlockPosition(extract_start);
            QStandardItem* size = new BlockSize(extract_size);
            QStandardItem* status = new BlockStatus(block_status);
            QList<QStandardItem*> block_data = { start, size, status };
            map->appendRow(block_data);
            return map;
        }
        
        // case 4: extract_start < block_start < block_finish < extract_finish )
        // the block is entirely contained in the extract
        if ( extract_start <= block_start && block_finish <= extract_finish )
        {
            QStandardItem* start = new BlockPosition(block_start);
            QStandardItem* size = new BlockSize(block_size);
            QStandardItem* status = new BlockStatus(block_status);
            QList<QStandardItem*> block_data = { start, size, status };
            map->appendRow(block_data);
            continue;
        }
        
        // case 5: extract_start < block_start < extract_finish < block_finish )
        // extract the end of the block
        if ( extract_start <= block_start && block_start < extract_finish && extract_finish <= block_finish )
        {
            QStandardItem* start = new BlockPosition(block_start);
            QStandardItem* size = new BlockSize(extract_finish - block_start);
            QStandardItem* status = new BlockStatus(block_status);
            QList<QStandardItem*> block_data = { start, size, status };
            map->appendRow(block_data);
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
    return map;
}

BlockPosition RescueMap::start() const
{
    if (hasIndex(0,0))
    {
        return BlockPosition(*(dynamic_cast<BlockPosition*>(item(0,0))));
    }
    return BlockPosition();
}

BlockSize RescueMap::size() const
{
    int last_row = rowCount() - 1;
    if (hasIndex(last_row, 0) && hasIndex(last_row, 1))
    {
        return BlockSize( (*(dynamic_cast<BlockPosition*>(item(last_row,0))) + *(dynamic_cast<BlockSize*>(item(last_row,1)))) - start() );
    }
    return BlockSize();
}

QDebug operator<<(QDebug dbg, const RescueMap &map)
{
    dbg.nospace() << "RescueMap" << endl;
    for(int row = 0; row < map.rowCount(); ++row)
    {
        BlockPosition *block_start = dynamic_cast<BlockPosition*>(map.item(row, 0));
        BlockSize *block_size = dynamic_cast<BlockSize*>(map.item(row, 1));
        BlockStatus *block_status = dynamic_cast<BlockStatus*>(map.item(row, 2));
        dbg.nospace() << "    " << *block_start << endl;
        dbg.nospace() << "    " << *block_size << endl;
        dbg.nospace() << "    " << *block_status << endl;
    }
    return dbg.maybeSpace();
}
