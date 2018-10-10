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

#include "rescue_map_view.h"
#include "rescue_map.h"
#include <QHeaderView>


RescueMapView::RescueMapView(QWidget *parent)
    :QTableView(parent)
    ,m_square_size(8)
{
    setShowGrid(true);
    horizontalHeader()->hide();
    verticalHeader()->hide();
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed); 
    verticalHeader()->setDefaultSectionSize(m_square_size); 
    horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed); 
    horizontalHeader()->setDefaultSectionSize(m_square_size);
}

void RescueMapView::resizeEvent(QResizeEvent *event)
{
    int columns = std::max(width() / m_square_size - 1, 2);
    int rows = std::max(height() / m_square_size - 1, 2);
       
    if (columns == m_columns && rows == m_rows) {
        return;
    }
    
    m_columns = columns;
    m_rows = rows;
    RescueMap * rescue_map = dynamic_cast<RescueMap*> (model());
    rescue_map->setDimensions(m_columns, m_rows);
    QTableView::resizeEvent(event);
}

void RescueMapView::setSquareSize(int size)
{
    m_square_size = size;
    verticalHeader()->setDefaultSectionSize(m_square_size);
    horizontalHeader()->setDefaultSectionSize(m_square_size);
    
    int columns = std::max(width() / m_square_size - 1, 2);
    int rows = std::max(height() / m_square_size - 1, 2);
       
    if (columns == m_columns && rows == m_rows) {
        return;
    }
    
    m_columns = columns;
    m_rows = rows;
    RescueMap * rescue_map = dynamic_cast<RescueMap*> (model());
    rescue_map->setDimensions(m_columns, m_rows);
    
}
