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

SquareColor::SquareColor()
    :QColor()
{
}

SquareColor::SquareColor(const SquareColor &other)
    :QColor(other)
{
}

SquareColor::~SquareColor()
{
}

SquareColor::SquareColor(RescueTotals &totals)
    :SquareColor()
{
    qint64 nontried_size = totals.nontried().data();
    qint64 nontrimmed_size = totals.nontrimmed().data();
    qint64 nonscraped_size = totals.nonscraped().data();
    qint64 badsectors_size = totals.badsectors().data();
    qint64 recovered_size = totals.recovered().data();
    qint64 unknown_size = totals.unknown().data();
    
    int color_count = 0;
    int red = 0;
    int green = 0;
    int blue = 0;
    int alpha = 255;
    
    if (nontried_size)
    {
        red   +=  1 * 0x40;
        green +=  1 * 0x40;
        blue  +=  1 * 0x40;
        color_count += 1;
    }
    
    if (nontrimmed_size)
    {
        red   +=  4 * 0xff;
        green +=  4 * 0xe0;
        blue  +=  4 * 0x00;
        color_count += 4;
    }
    
    if (nonscraped_size)
    {
        red   += 10 * 0x20;
        green += 10 * 0x20;
        blue  += 10 * 0xff;
        color_count +=10;
    }

    if (badsectors_size)
    {
        red   += 40 * 0xff;
        green += 40 * 0x00;
        blue  += 40 * 0x00;
        color_count += 40;
    }
    
    if (recovered_size)
    {
        red   +=  2 * 0x00;
        green +=  2 * 0xff;
        blue  +=  2 * 0x00;
        color_count += 2;
    }
    
    if (unknown_size)
    {
        alpha = 127;
    }
    
    if (color_count == 0)
    {
        alpha = 0;
    }

    if (color_count > 0)
    {
        red /= color_count;
        green /= color_count;
        blue /= color_count;
    }
    
    if (red > 255)
    {
        qDebug() << "Error: red =" << red;
        red = 255;
    }

    if (green > 255)
    {
        qDebug() << "Error: green =" << green;
        green = 255;
    }

    if (blue > 255)
    {
        qDebug() << "Error: blue =" << blue;
        blue = 255;
    }

    this->setRed(red);
    this->setGreen(green);
    this->setBlue(blue);
    this->setAlpha(alpha);
}

QDebug operator<<(QDebug dbg, const SquareColor &c)
{
    dbg.nospace() << "Color(" << c.red() << c.green() << c.blue() << c.alpha() << ")";
    return dbg.maybeSpace();

}
