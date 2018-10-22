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
#include "rescue_totals.h"
#include "square_color.h"

#include <cmath>
#include <QDebug>
#include <QSize>

RescueMap::RescueMap(QObject *parent)
    : QAbstractTableModel(parent)
    , m_columns(1)
    , m_rows(1)
    , m_square_colors()
{
}

QVariant RescueMap::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::BackgroundRole) {
        int square = m_columns * index.row() + index.column();
        return m_square_colors.at(square);
    }
    
    if (role == Qt::SizeHintRole) {
        return QSize(1, 1);
    }
    
    return QVariant();
}

QVariant RescueMap::headerData(int /* section */, Qt::Orientation /* orientation */, int role) const
{
    if (role == Qt::SizeHintRole)
        return QSize(1, 1);
    return QVariant();
}

void RescueMap::setMap(const QVector<BlockPosition> &positions, const QVector<BlockSize> &sizes, const QVector<BlockStatus> &statuses)
{
    beginResetModel();
    m_positions = positions;
    m_sizes = sizes;
    m_statuses = statuses;
    computeSquareColors();
    endResetModel();
}

/*
 * Extract a sub map, e.g. the map corresponding to a square on the grid view
 * Usage: showing the details of a square in block inspector
 */
RescueMap* RescueMap::extract(BlockPosition p, BlockSize s) const
{
    RescueMap* map = new RescueMap();
    QVector<BlockPosition> positions;
    QVector<BlockSize> sizes;
    QVector<BlockStatus> statuses;
    
    for(int line = 0; line < m_positions.count(); ++line) {
        BlockPosition extract_start = p;
        BlockSize extract_size = s;
        BlockPosition extract_finish = extract_start + extract_size;
        
        BlockPosition block_start = m_positions[line];
        BlockSize block_size = m_sizes[line];
        BlockPosition block_finish = block_start + block_size;
        
        BlockStatus block_status = m_statuses[line];
        
        // case 1: block_start < block_finish < extract_start < extract_finish
        // nothing to extract yet
        if ( block_finish <= extract_start ) {
            continue;  
        }
        
        // case 2: block_start < extract_start < block_finish < extract_finish
        // extract the beginning of the block
        if ( block_start <= extract_start && extract_start < block_finish && block_finish <= extract_finish ) {
            positions.append(BlockPosition(extract_start));
            sizes.append(BlockSize(block_finish - extract_start));
            statuses.append(BlockStatus(block_status));
            continue;
        }
        
        // case 3: block_start < extract_start < extract_finish < block_finish
        // the extract is entirely contained in the block
        if ( block_start <= extract_start && extract_start < extract_finish && extract_finish <= block_finish ) {
            positions.append(BlockPosition(extract_start));
            sizes.append(BlockSize(extract_size));
            statuses.append(BlockStatus(block_status));
            map->setMap(positions, sizes, statuses);
            return map;
        }
        
        // case 4: extract_start < block_start < block_finish < extract_finish )
        // the block is entirely contained in the extract
        if ( extract_start <= block_start && block_finish <= extract_finish ) {
            positions.append(BlockPosition(block_start));
            sizes.append(BlockSize(block_size));
            statuses.append(BlockStatus(block_status));
            continue;
        }
        
        // case 5: extract_start < block_start < extract_finish < block_finish )
        // extract the end of the block
        if ( extract_start <= block_start && block_start < extract_finish && extract_finish <= block_finish ) {
            positions.append(BlockPosition(block_start));
            sizes.append(BlockSize(extract_finish - block_start));
            statuses.append(BlockStatus(block_status));
            map->setMap(positions, sizes, statuses);
            return map;
        }

        // case 6: extract_start < extract_finish < block_start < block_finish )
        // nothing to extract (any more)
        if ( extract_finish <= block_start ) {
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
    if (m_positions.count()) {
        return m_positions.at(0);
    }
    return BlockPosition();
}

BlockSize RescueMap::size() const
{
    if (m_positions.count() && m_sizes.count()) {
        int last_row = m_positions.count() - 1;
        return BlockSize( m_positions.at(last_row) + m_sizes.at(last_row) - start() );
    }
    return BlockSize();
}

void RescueMap::setDimensions(int columns, int rows)
{
    beginResetModel();
    m_columns = (columns > 0) ? columns : 1;
    m_rows = (rows > 0) ? rows : 1;
    computeSquareColors();
    endResetModel();
}

void RescueMap::computeSquareColors()
{
    m_square_colors.clear();                  // capacity preserved from Qt 5.7
    const int squares = m_columns * m_rows;
    m_square_colors.reserve(squares);
    
    const BlockSize sector_size = 512;
    const BlockSize square_size = sector_size * ceil(size()/sector_size/squares);

    if (!m_positions.count()) {
        m_square_colors.fill(SquareColor(), squares);  // fill the grid with ligthgray
        return;
    }
    
    /* iteration over all the squares and all the mapfile lines */
    RescueTotals square_totals;
    BlockPosition section_start = start();
    BlockPosition square_end = start() + square_size;

    for (int square = 0, line = 0; square < squares && line < m_positions.count(); ) {
        BlockPosition line_end = m_positions.at(line) + m_sizes.at(line);
        
        if (square_end <= line_end) {
            // case 1: square_end <= line_end
            // color can be computed and saved for the square
            square_totals.add(square_end - section_start, m_statuses.at(line));
            m_square_colors.append(SquareColor(square_totals));
            square_totals.reset();
            ++square;
            if (square_end == line_end) { ++line; }
            section_start = square_end;
            square_end = square_end + square_size;
        } 
        else {
            // case 2: square_end > line_end
            // the square totals still have lines to process before computing the color
            square_totals.add(line_end - section_start, m_statuses.at(line));
            ++line;
            if (line == m_positions.count()) { m_square_colors.append(SquareColor(square_totals)); }
            section_start = line_end;
        }
    }
}

QDebug operator<<(QDebug dbg, const RescueMap &map)
{
    dbg << "RescueMap" << endl;
    for(int row = 0; row < map.m_positions.count(); ++row)
    {
        dbg << map.m_positions[row] << " " << map.m_sizes[row] << " " << map.m_statuses[row] << endl;
    }
    return dbg.maybeSpace();
}
