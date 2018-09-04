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

#ifndef KDDRESCUEVIEWPART_H
#define KDDRESCUEVIEWPART_H

#include "rescue_map_widget.h"
#include "rescue_status.h"
#include "rescue_map.h"

// KF headers
#include <KParts/ReadOnlyPart>

class QWidget;
class QAction;
class QTableView;

/**
 * @short kddrescueview Part
 */
class kddrescueviewPart : public KParts::ReadOnlyPart
{
    Q_OBJECT

public:
    /**
     * Default constructor, with arguments as expected by KPluginFactory
     */
    kddrescueviewPart(QWidget* parentWidget, QObject* parent, const QVariantList& arg);

    /**
     * Destructor
     */
    ~kddrescueviewPart() override;


protected: // KParts::ReadOnlyPart API
    bool openFile() override;

private:
    void setupActions();

private:
    QWidget* m_view;  // either BlockWidget* or QTableView*
    
    RescueMap* m_rescue_map;
    RescueStatus m_rescue_status;
};

#endif // KDDRESCUEVIEWPART_H
