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

#include "rescue_map_widget.h"
#include "rescue_map.h"
#include "rescue_totals.h"
#include "square_color.h"

#include <QPainter>
#include <cmath>
#include <QStandardItemModel>

RescueMapWidget::RescueMapWidget(QWidget *parent)
    :QWidget(parent)
    ,m_grid_step(8)
    ,m_model(nullptr)
{
}

void RescueMapWidget::setModel(RescueMap *model)
{
    m_model = model;
    update();
}

void RescueMapWidget::setGridSize(const int step)
{
    m_grid_step = step;
    update();
}

void RescueMapWidget::paintEvent(QPaintEvent * /* event */)
{
    // the qwidget width and height appears to continue 8 pixels under the window right and bottom borders
    int window_border = 8;
    int columns = (width() - 1 - window_border)/m_grid_step;
    int rows = (height() - 1 - window_border)/m_grid_step;
    int max_squares = rows * columns;

    if( !m_model->hasIndex(0,0) && !m_model->hasIndex(m_model->rowCount()-1, 1) )
    {
        return;
    }
    
    qint64 map_start_position = m_model->start().data(Qt::DisplayRole).value<qint64>();
    qint64 map_size = m_model->size().data(Qt::DisplayRole).value<qint64>();
    
    qint64 sector_size = 512;
    qint64 square_size = ceil( (qint64) map_size / max_squares / sector_size ) * sector_size;

    QRect square_geometry(m_grid_step, m_grid_step, m_grid_step, m_grid_step);
    QPainter painter(this);
    QPen pen(Qt::black, 1, Qt::SolidLine);
    QBrush brush(Qt::green);
    painter.setPen(pen);
    painter.setBrush(brush);

    QVector<SquareColor*> square_colors;
    for(int row=0, sq=0; row < rows; ++row) 
    {
        for(int col=0; col < columns && sq < max_squares; ++col, ++sq) 
        {
            BlockPosition square_start(map_start_position + sq * square_size);
            RescueMap *square_map = m_model->extract(square_start, BlockSize(square_size));
            RescueTotals square_totals(square_map);
            SquareColor* square_color = new SquareColor(&square_totals);
            square_colors.append(square_color);

            if(square_totals.badsectors().data(Qt::DisplayRole).value<qint64>() > 0)
            {
                qDebug() << "Square #" << sq << square_color << endl;
                qDebug() << square_totals;
            }
        }
    }
    
    for(int row=0, sq=0; row < rows; ++row)
    {
        for(int col=0; col < columns && sq < max_squares; ++col, ++sq)
        {
            int y = row * m_grid_step;
            int x = col * m_grid_step;
    
            brush.setColor(*square_colors.at(sq));
            painter.setBrush(brush);

            painter.save();
            painter.translate(x, y);
            painter.drawRect(square_geometry);
            painter.restore();
        }
    }
}
