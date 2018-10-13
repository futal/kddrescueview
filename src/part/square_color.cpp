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

#include "square_color.h"
#include "rescue_totals.h"


SquareColor::SquareColor()
    :QColor()
{
    this->setRgb(211, 211, 211);
}

SquareColor::SquareColor(const SquareColor &other)
    :QColor(other)
{
}

SquareColor::~SquareColor()
{
}

SquareColor::SquareColor(const RescueTotals &totals)
    :SquareColor()
{
    const int nontried = totals.nontried().data() ? 1 : 0;
    const int nontrimmed = totals.nontrimmed().data() ? 4 : 0;
    const int nonscraped = totals.nonscraped().data() ? 10 : 0;
    const int badsectors = totals.badsectors().data() ? 40 : 0;
    const int recovered = totals.recovered().data() ? 2 : 0;
    const int unknown = totals.unknown().data() ? 127 : 255;
    
    const int color_count = nontried + nontrimmed + nonscraped + badsectors + recovered ;
    
    if (!color_count) {
        this->setRgb(211, 211, 211);  // default to lightgray
    }
    
    int red =  ( nontried * 0x40 +
                 nontrimmed * 0xff +
                 nonscraped * 0x20 +
                 badsectors * 0xff   // + recovered * 0x00
               ) / color_count;
    
    int green =  ( nontried * 0x40 +
                   nontrimmed * 0xe0 +
                   nonscraped * 0x20 + // badsectors * 0x00 + 
                   recovered * 0xff 
                 ) / color_count;
    
    int blue =  ( nontried * 0x40 + // nontrimmed * 0x00 +
                nonscraped * 0xff   // + badsectors * 0x00 + recovered * 0x00
                ) / color_count;

    int alpha = unknown;
    
    this->setRgb(red, green, blue, alpha);
}

QDebug operator<<(QDebug dbg, const SquareColor &c)
{
    dbg.nospace() << "Color(" << c.red() << ", " << c.green() << ", " << c.blue() << ", " << c.alpha() << ")";
    return dbg.maybeSpace();

}
