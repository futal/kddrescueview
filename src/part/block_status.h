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

#ifndef BLOCK_STATUS_H
#define BLOCK_STATUS_H

#include <QStandardItemModel>
#include <QMap>

/**
 * Type for the recovery status of a block. It is used:
 * - in data block lines of the mapfile.
 * cf. https://www.gnu.org/software/ddrescue/manual/ddrescue_manual.html#Mapfile-structure
 */

static const QMap<QString, QString> statuses {
    { "?", "non-tried block" },
    { "*", "failed block non-trimmed" },
    { "/", "failed block non-scraped" },
    { "-", "failed block bad-sector(s)" },
    { "+", "block recovered" },
}; 

class Status : public QStandardItem
{
public:
    Status(QString status = "unknown") : m_status(status) { }
//    virtual ~Status() {}
    virtual QStandardItem* clone() const { return new Status(m_status); }
    QVariant data(int role = Qt::UserRole+1 ) const;
    QString toStr() const { return statuses.value(m_status, "Unknown block status"); }
    static bool isValid(QString s) { return statuses.count(s); }

private:
    QString m_status;
};


#endif // BLOCK_STATUS_H
