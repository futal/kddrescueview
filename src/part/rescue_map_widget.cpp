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

#include <QPainter>
#include <cmath>
#include <QStandardItemModel>

BlockWidget::BlockWidget(QWidget *parent)
    : QWidget(parent)
{
    m_grid_step = 8;
}


void BlockWidget::setGridSize(const int step){
    m_grid_step = step;
    update();
}

void BlockWidget::paintEvent(QPaintEvent * /* event */)
{
    // the qwidget width and height appears to continue 8 pixels under the window right and bottom borders
    int window_border = 8;
    int columns = (width() - 1 - window_border)/m_grid_step;
    int rows = (height() - 1 - window_border)/m_grid_step;
//    int max_squares = rows * columns;
/*
    long long device_size = m_model->data(m_model->rowCount(), 0, Qt::UserRole) + m_model->data(m_model->rowCount(), 1, Qt::UserRole);
    int device_block_size = 512;
    long long bytes_per_square = (long long)ceil( (long double)device_size / max_squares / device_block_size ) * device_block_size;

    QList<QColor> img_block;
    for(int row = 0; row < m_model.rowCount(); ++row) {
        long long img_start_block = m_model->data(row, 0, Qt::UserRole) / bytes_per_block ;
        long long img_end_block = (m_model->data(row, 0, Qt::UserRole) + m_model->data(row, 1, Qt::UserRole) - 1) / bytes_per_block;
        for(int j = img_start_block; j < img_end_block; ++j) {
            img_block.insert(j, block_status_to_mask(m_model->data(row, 3, Qt::DisplayRole));
        }
    }
*/    
    
    
    
    QRect square(m_grid_step, m_grid_step, m_grid_step, m_grid_step);
    
    QPainter painter(this);

    QPen pen = QPen(Qt::black, 1, Qt::SolidLine);
    QBrush brush = QBrush(Qt::green);
    painter.setPen(pen);
    painter.setBrush(brush);
    
    for (int column = 0, square_nb = 0; column < columns; ++ column) {
        for (int row = 0; row < rows; ++row, ++square_nb) {
            
            
            
            painter.save();
            painter.translate(column*m_grid_step, row*m_grid_step);
            painter.drawRect(square);
            painter.restore();
        }
    }
}

void BlockWidget::setModel(QStandardItemModel* model)
{
    m_model = model;
}
