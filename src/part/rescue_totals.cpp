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

#include "rescue_totals.h"
#include "block_size.h"
#include "block_status.h"
#include <QDebug>

RescueTotals::RescueTotals()
    :m_nontried(0),
    m_nontrimmed(0),
    m_nonscraped(0),
    m_badsectors(0),
    m_recovered(0),
    m_unknown(0)
{
}

RescueTotals::RescueTotals(const RescueMap* map)
    :RescueTotals()
{
    for(int line = 0; line < map->m_sizes.count(); ++line)
    {
        BlockSize size = BlockSize(map->m_sizes[line]);
        char status_char = map->m_statuses[line].data().at(0).toLatin1();
        switch(status_char)
        {
            case '?': m_nontried += size; break;
            case '*': m_nontrimmed += size; break;
            case '/': m_nonscraped += size; break;
            case '-': m_badsectors += size; break;
            case '+': m_recovered += size; break;
            default: m_unknown += size;
        }
    }
};

void RescueTotals::reset()
{
    m_nontried = 0;
    m_nontrimmed = 0;
    m_nonscraped = 0;
    m_badsectors = 0;
    m_recovered = 0;
    m_unknown = 0;
}

/*
QList<QPieSlice *> RescueTotals::totals() const
{
    QList result;
    result << new QPieSlice("non-tried block", m_nontried);
    result << new QPieSlice("failed block non-trimmed", m_nontrimmed);
    result << new QPieSlice("failed block non-scraped", m_nonscraped);
    result << new QPieSlice("failed block bad-sector(s)", m_badsectors);
    result << new QPieSlice("block recovered", m_recovered);
    result << new QPieSlice("unknknown block status", m_unknown);
    return result;
}
*/

QDebug operator<<(QDebug dbg, const RescueTotals &t)
{
    dbg.nospace() << "RescueTotals" << endl;
    dbg.nospace() << "    Non-tried:   " << t.nontried() << endl;
    dbg.nospace() << "    Non-trimmed: " << t.nontrimmed() << endl;
    dbg.nospace() << "    Non-scraped: " << t.nonscraped() << endl;
    dbg.nospace() << "    Bad sectors: " << t.badsectors() << endl;
    dbg.nospace() << "    Recovered:   " << t.recovered() << endl;
    dbg.nospace() << "    Unknown:     " << t.unknown() << endl;
    dbg.nospace() << endl;
    return dbg.maybeSpace();
}
