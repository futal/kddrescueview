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

#ifndef RESCUE_STATUS_H
#define RESCUE_STATUS_H

#include "block_position.h"
#include "rescue_operation.h"
class RescueMap;
class QString;

/**
 * Class for status line operation in the map file. It stores:
 * - the current position being tried in the input file;
 * - the current operation being tried in the input file;
 * - the current pass (since ddrescue version ?.?.?).
 * cf. https://www.gnu.org/software/ddrescue/manual/ddrescue_manual.html#Mapfile-structure
 */
class RescueStatus
{
public:
    RescueStatus();
    BlockPosition currentPosition() const { return m_current_position; }
    void setCurrentPosition(qint64 position);
    RescueOperation currentOperation() const { return m_current_operation; }
    void setCurrentOperation(QString operation);
    int currentPass() const { return m_current_pass; }
    bool setCurrentPass(int pass);

private:
    BlockPosition m_current_position;
    RescueOperation m_current_operation;
    int m_current_pass;
};

#endif // RESCUE_STATUS_H
