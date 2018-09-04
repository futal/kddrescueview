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

#ifndef RESCUE_MAP_WIDGET_H
#define RESCUE_MAP_WIDGET_H

#include <QWidget>



#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QTableView>

class QStandardItemModel;


class BlockWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BlockWidget(QWidget *parent = nullptr);
    void setModel(QStandardItemModel* model = nullptr);
public slots:
    void setGridSize(const int step = 8);

protected:
    void paintEvent(QPaintEvent *event);

private:
    int m_grid_step;
    QStandardItemModel* m_model;
};

#endif // RESCUE_MAP_WIDGET_H
