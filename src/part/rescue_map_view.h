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

#ifndef RESCUE_MAP_VIEW_H
#define RESCUE_MAP_VIEW_H

#include <QTableView>

/**
 * @todo write docs
 */
class RescueMapView : public QTableView
{
    Q_OBJECT

public:
    RescueMapView(QWidget *parent = 0);
    
public slots:
    void setSquareSize(int size);
    
protected:
    void resizeEvent(QResizeEvent *event) override;
    
private:
    int m_columns;
    int m_rows;
    int m_square_size;
    
    
};

#endif // RESCUE_MAP_VIEW_H
