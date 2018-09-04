/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright 2018  <copyright holder> <email>
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

#ifndef RESCUE_TOTALS_H
#define RESCUE_TOTALS_H

#include "rescue_map.h"
#include "block_size.h"


/**
 * Store completion statistics for a recovery area, i.e. totals for each status
 * of the entire input file or for a block (e.g. each block represented by a square)
 */
class RescueTotals
{
public:
    RescueTotals();
//    RescueTotals(const RescueTotals &);
//    ~RescueTotals();

    RescueTotals(const RescueMap* map);
    
//    QList<QPair<String, qreal>> totals() const; // for Pie chart
    void reset();
    Size nontried() const { return m_nontried; }
    Size nontrimmed() const { return m_nontrimmed; }
    Size nonscscraped() const { return m_nonscraped; }
    Size badsdsectors() const { return m_badsectors; }
    Size recovered() const { return m_recovered; }
    Size unknown() const { return m_unknown; }

private:
    Size m_nontried;
    Size m_nontrimmed;
    Size m_nonscraped;
    Size m_badsectors;
    Size m_recovered;
    Size m_unknown;
};

QDebug operator<<(QDebug dbg, const RescueTotals &t);

#endif // RESCUE_TOTALS_H
