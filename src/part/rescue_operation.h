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

#ifndef RESCUE_OPERATION_H
#define RESCUE_OPERATION_H

#include <QMap>
#include <QString>

/**
 * Type for current rescue operation. It is used:
 * - in the status line of the mapfile for the current operation being tried in the input file 
 * cf. https://www.gnu.org/software/ddrescue/manual/ddrescue_manual.html#Mapfile-structure
 */

static const QMap<QString, QString> operations {
    { "?", "Copying non-tried blocks" },
    { "*", "Trimming non-trimmed blocks" },
    { "/", "Scraping non-scraped blocks" },
    { "-", "Retrying bad sectors" },
    { "F", "Filling specified blocks" },
    { "G", "Generating approximate mapfile" },
    { "+", "Finished" },
}; 

class Operation {
public:
    Operation(QString s) {
        if (Operation::isValid(s)) { m_operation = s; }
        else { m_operation = " "; }
    }
//    virtual ~Operation() {}
    
    bool setOperation(QString s) {
        if (Operation::isValid(s))
        {
            m_operation = s;
            return true;
        }
        else
        {
            return false;
        }
    }
    
    QString toStr() const { return m_operation; }
    
    QString description() const { return operations.value(m_operation, "Unknown operation"); }
    
    static bool isValid(QString s) { return operations.count(s); }
    
private:
    QString m_operation;
};

#endif // RESCUE_OPERATION_H
