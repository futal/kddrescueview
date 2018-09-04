/*
 * Kddrescueview - A KPart to visualise GNU ddrescue mapfiles
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

#include "kddrescueviewpart.h"
#include "rescue_map_widget.h"
#include "rescue_status.h"
#include "rescue_map.h"
#include "block_status.h"
#include "block_position.h"

// KF headers
#include <KPluginFactory>
#include <KAboutData>
#include <KLocalizedString>
#include <KActionCollection>
#include <KStandardAction>

// Qt headers
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QStandardItemModel>
#include <QtDebug>
#include <QRegularExpression>
#include <QTableView>



K_PLUGIN_FACTORY(kddrescueviewPartFactory, registerPlugin<kddrescueviewPart>();)


kddrescueviewPart::kddrescueviewPart(QWidget* parentWidget, QObject* parent, const QVariantList& /*args*/)
    : KParts::ReadOnlyPart(parent)
{
    // set component data
    // the first arg must be the same as the subdirectory into which the part's rc file is installed
    KAboutData aboutData("kddrescueviewpart", i18n("kddrescueviewPart"), QStringLiteral("0.1"));
    aboutData.addAuthor(i18n("Adrien Cordonnier"), i18n("Author"), QStringLiteral("adrien.cordonnier@gmail.com"));
    aboutData.addAuthor(i18n("Martin Bittermann"), i18n("Inspirator"), QStringLiteral(""));
    setComponentData(aboutData);

    // set internal UI
    m_view = new BlockWidget(parentWidget);
    //m_view = new QTableView(parentWidget);
    setWidget(m_view);

    // set KXMLUI resource file
    setXMLFile(QStringLiteral("kddrescueviewpartui.rc"));

    // setup actions
    setupActions();

    // data model
    m_rescue_map = new RescueMap(this);
    m_rescue_status = RescueStatus();

    /* TODO: draw on QWidget based on the model */
    // m_view->setModel(m_rescue_map); 

}

kddrescueviewPart::~kddrescueviewPart()
{
}

void kddrescueviewPart::setupActions()
{
}

bool kddrescueviewPart::openFile()
{
    /* Parse a GNU ddrescue map file
     * Map file structure is described at https://www.gnu.org/software/ddrescue/manual/ddrescue_manual.html#Mapfile-structure
     */
    
    QFile file(localFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream stream(&file);
    while (!stream.atEnd()) {
        QString line;
        line = stream.readLine();
        line = line.trimmed();
        
        // qDebug() << line;
        
        if( line.isEmpty() ) {
            /* qDebug() << "Empty line"; */
            continue;
        }
                
        if( line.startsWith("# Command line:") ) {
            /* try to find if the command line had a specific block size option */
            QRegularExpression re("(-b|--block-size=)\\s*(?P<blocksize>[0-9]+)");
            QRegularExpressionMatch match = re.match(line);
            QString device_block_size;
            if( match.hasMatch() ) {
                device_block_size = match.captured("blocksize");
            } else {
                device_block_size = "512";
            }
            /* qDebug() << "Command line"; */
            continue;
        }
        
        if( line.startsWith('#') ) {
            /* comment line without a command line */
            /* TODO: parse comment lines with ddrescue_version, start_time, current_time, human_readable_status */
            /* qDebug() << "Comment line"; */
            continue;
        }
        
        /* Non-comment lines can be:
         *  - The former status line with two tokens: current_pos current_status
         *  - The new status line with three tokens: current_pos current_status current_pass
         *  - A block information with three tokens: pos size status
         * Their type is:
         *  - non-negative long long integer for current_pos
         *  - Operation (one ASCII character) for current_status 
         *  - positive decimal integer for current_pass
         *  - non-negative long long integer for position
         *  - non-negative long long integer for size
         *  - Status (one ASCII character) for status
         * Hence three patterns can be found:
         *  - (long long int) (char) : former status line
         *  - (long long int) (char) (int) : new status line
         *  - (long long int) (long long int) (char) : block information
         */
        QStringList tokens;
        tokens = line.split(QRegularExpression("\\s+"));
        bool conversion_ok;
        
        /* former status line pattern: (long long current_position) (Operation char current_operation) (optional comment) */
        if( tokens.count() == 2 || (tokens.count() > 2 && tokens[2].startsWith("#")) )
        if( tokens[0].toLongLong(&conversion_ok, 0) >= 0 ) /* 0: guess the base */
        if( conversion_ok )
        if( Operation::isValid(tokens[1]) )
        if( m_rescue_status.currentOperation() != "unknown" )
        {
            m_rescue_status.setCurrentPosition(tokens[1].toLongLong(&conversion_ok, 0));
            m_rescue_status.setCurrentOperation(tokens[1]);
            continue;
            /* qDebug() << "Two-token status line"; */ 
        }
        /*
        else qDebug() << "current operation already set";
        else qDebug() << "second token not an operation character";
        else qDebug() << "first token cannot be converted to a position";
        else qDebug() << "position < 0";
        else qDebug() << "not a two-token line";
        */
        
        /* status line pattern: (long long current_position) (Operation char current_operation) (int current_pass) (optional comment) */
        if( tokens.count() == 3 || (tokens.count() > 3 && tokens[3].startsWith("#")) )
        if( tokens[0].toLongLong(&conversion_ok, 0) >= 0 ) /* 0: guess the base for position */
        if( conversion_ok )
        if( Operation::isValid(tokens[1]) )
        if( m_rescue_status.currentOperation() == "unknown" )
        if( tokens[2].toInt(&conversion_ok, 10) >= 1 )/* 10: pass number must be in base 10 */
        if( conversion_ok )
        {
            m_rescue_status.setCurrentPosition(tokens[0].toLongLong(&conversion_ok, 0));
            m_rescue_status.setCurrentOperation(tokens[1]);
            m_rescue_status.setCurrentPass(tokens[2].toInt(&conversion_ok, 10));
            /* qDebug() << "Status line"; */
            continue;
        }
        /*
        else qDebug() << "pass number conversion failed";
        else qDebug() << "pass number < 1";
        else qDebug() << "current operation already set";
        else qDebug() << "not an operation character";
        else qDebug() << "position conversion failed";
        else qDebug() << "position < 0";
        else qDebug() << "not a three-token line";
        */
        

        /* block information pattern: (long long current_position) (Operation char current_operation) (int current_pass) (optional comment) */
        if( tokens.count() == 3 || (tokens.count() > 3 && tokens[3].startsWith("#")) )
        if( tokens[0].toLongLong(&conversion_ok, 0) >= 0 ) // == m_rescue_map->lastPosition() + m_rescue_map->lastSize() /* 0: guess the base for position */
        if( conversion_ok )
        if( tokens[1].toLongLong(&conversion_ok, 0) > 0 ) /* 0: guess the base for size */
        if( conversion_ok )
        if( Status::isValid(tokens[2]) )
        {
            QStandardItem* pos = new Position(tokens[0]);
            QStandardItem* size = new Size(tokens[1]);
            QStandardItem* status = new Status(tokens[2]);
            QList<QStandardItem*> block_data = { pos, size, status };
            m_rescue_map->appendRow(block_data);
            continue;
        }
        /*
        else qDebug() << "third token not a status character";
        else qDebug() << "second token cannot be converted to a size";
        else qDebug() << "size < 0";
        else qDebug() << "first token cannot be converted to a position";
        else qDebug() << "position < 0";
        else qDebug() << "not a three-token line";
        */
        qDebug() << "Parsing error: the line does not match a status or block information pattern";
        qDebug() << QString("Error line: %1").arg(line);
        file.close();
        return false;
    }

    file.close();
    
    return true;
}



// needed for K_PLUGIN_FACTORY
#include <kddrescueviewpart.moc>
