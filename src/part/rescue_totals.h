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
    BlockSize nontried() const { return m_nontried; }
    BlockSize nontrimmed() const { return m_nontrimmed; }
    BlockSize nonscraped() const { return m_nonscraped; }
    BlockSize badsectors() const { return m_badsectors; }
    BlockSize recovered() const { return m_recovered; }
    BlockSize unknown() const { return m_unknown; }

private:
    BlockSize m_nontried;
    BlockSize m_nontrimmed;
    BlockSize m_nonscraped;
    BlockSize m_badsectors;
    BlockSize m_recovered;
    BlockSize m_unknown;
};

QDebug operator<<(QDebug dbg, const RescueTotals &t);

#endif // RESCUE_TOTALS_H
