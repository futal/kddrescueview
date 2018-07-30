/*
 *   Copyright (C) 2018 by Adrien Cordonnier <adrien.cordonnier@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .
 */

#include "kddrescueviewpart.h"
#include "blockwidget.h"

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
    // TODO: replace with your custom UI
    m_view = new BlockWidget(parentWidget);
    setWidget(m_view);

    // set KXMLUI resource file
    setXMLFile(QStringLiteral("kddrescueviewpartui.rc"));

    // setup actions
    setupActions();

    // starting with empty data model, not modified at begin
    // TODO: replace with your custom data model
    m_model = new QStandardItemModel(this);
    /* TODO: draw on QWidget based on the model
     * in kddrescueviewpart.h, it was QtableView* m_view;
     * m_view->setModel(m_model); 
     */
}

kddrescueviewPart::~kddrescueviewPart()
{
}

void kddrescueviewPart::setupActions()
{
}

bool kddrescueviewPart::openFile()
{
    QFile file(localFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
  
    QTextStream stream(&file);
    while (!stream.atEnd()) {
        QString line;
        line = stream.readLine();
        line = line.trimmed();
        
        if( line.isEmpty() ) {
            // qDebug() << "Empty line";
            continue;
        }
                
        if( line.startsWith("# Command line:") ) {
            // qDebug() << "Comment with command line";
            /* try to find if the commandline had a specific block size option */
            /*
            match = re.search( "(-b|--block-size=)\s*(?P<blocksize>[0-9]+)", line);
            if( match ) {
                device_block_size = int(match.group("blocksize"));
            } else {
                device_block_size = 512;
            }
             */
            continue;
        }
        
        if( line.startsWith('#') ) {
            // qDebug() << "Comment line";
            /* comment line */
            continue;
        }
        
        QStringList tokens;
        tokens = line.split(QRegExp("\\s+"));
        
        if( tokens.size() == 2 ) {
            // qDebug() << "Current read position and status";
            /* current read: position and status */
            /*
            if( rescue_status == Null ) {
                rescue_status = { position: int(tokens[0], 0), status: tokens[1] )
                // int(x, 16) for hexadecimal, int(x, 0) to guess the base automatically
            } else {
                MainForm.AppLog.Lines.append('Parser: found more than one line with 2 tokens.')
            }
             */
            continue;
        }
        
        /* block informations: position, size and status */
        if( tokens.size() != 3 ) {
            qDebug() << "Line does not have three tokens";
            // TODO: throw exception for invalid line
            continue;
        }
        
        // qDebug() << "Line has three tokens:" << tokens;
        
        bool conversion_success = true;
        // QString::toInt(&success, base=0) guesses the base automatically
        QStandardItem* block_position = new QStandardItem(tokens[0]);
        block_position->setData(tokens[0].toLongLong(&conversion_success, 0));
        // qDebug() << "block position: " << block_position.data();
        if( !conversion_success ) {
            // TODO: throw exception if conversion fails
            qDebug() << "Position conversion to integer failed";
        }
        
        QStandardItem* block_size  = new QStandardItem(tokens[1]);
        block_size->setData(tokens[1].toLongLong(&conversion_success, 0));
        // qDebug() << "block size: " << block_size.data();
        if( !conversion_success ) {
            // TODO: throw exception if conversion fails
            qDebug() << "Size conversion to integer failed";
        }
        
        QStandardItem* block_status = new QStandardItem(tokens[2]);
        block_status->setData(tokens[2]);
        // qDebug() << "block status: " << block_status.data();
        QList<QStandardItem*> items = { block_position, block_size, block_status };
        
        m_model->appendRow(items);
        /*
        log.append( (int(tokens[0], 0), int(tokens[1], 0), tokens[2]) )
        // update the total sizes for each status
        case = {
                '?': rescue_status.nontried,
                '+': rescue_status.rescued,
                '*': rescue_status.nontrimmed,
                '/': rescue_status.nonsplit,
                '-': rescue_status.bad,
        }
        x = case.get(log[logEntry].status);
        x += log[logEntry].length;
        if( log[logEntry].status in ['-', '*', '/'] ) {
            rescue_status.errors += 1;
            logEntry += 1;
        */
    }

    for(int i=0; i < m_model->rowCount(); ++i) {
        qDebug() << m_model->data(m_model->index(i, 0)) << m_model->data(m_model->index(i, 1)) << m_model->data(m_model->index(i, 2));
    }
    
    file.close();

    return true;
}



// needed for K_PLUGIN_FACTORY
#include <kddrescueviewpart.moc>
