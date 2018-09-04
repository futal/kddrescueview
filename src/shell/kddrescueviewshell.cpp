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

#include "kddrescueviewshell.h"

// KF headers
#include <KParts/ReadOnlyPart>
#include <KPluginLoader>
#include <KPluginFactory>
#include <KActionCollection>
#include <KStandardAction>
#include <KLocalizedString>
#include <KMessageBox>

// Qt headers
#include <QApplication>
#include <QFileDialog>

kddrescueviewShell::kddrescueviewShell()
    : KParts::MainWindow()
{
    // set KXMLUI resource file
    setXMLFile(QStringLiteral("kddrescueviewui.rc"));

    // setup our actions
    setupActions();

    // find and load the part
    // Doing it by name, which is a bad idea usually but alright here
    // since this part is made for this shell
    KPluginLoader loader(QStringLiteral("kddrescueviewpart"));
    auto factory = loader.factory();
    if (!factory) {
        // can't do anything useful without part, thus quit the app
        KMessageBox::error(this, i18n("Could not find kddrescueview part!"));

        qApp->quit();
        // return here, because qApp->quit() only means "exit the
        // next time we enter the event loop...
        return;
    }

    m_part = factory->create<KParts::ReadOnlyPart>(this);

    if (m_part) {
        // integrate and setup
        setCentralWidget(m_part->widget());
        setupGUI(ToolBar | Keys | StatusBar | Save);
        createGUI(m_part);
    }
}

kddrescueviewShell::~kddrescueviewShell()
{
}

void kddrescueviewShell::loadDocument(const QUrl& url)
{
    m_part->openUrl(url);
}

void kddrescueviewShell::setupActions()
{
    KStandardAction::openNew(this, &kddrescueviewShell::fileNew, actionCollection());
    KStandardAction::open(this, &kddrescueviewShell::fileOpen, actionCollection());

    KStandardAction::quit(qApp, &QApplication::closeAllWindows, actionCollection());
}

void kddrescueviewShell::fileNew()
{
    // open a new window if the document is _not_ in its initial state
    if (!m_part->url().isValid()) {
        (new kddrescueviewShell)->show();
    };
}

void kddrescueviewShell::fileOpen()
{
    const QUrl url = QFileDialog::getOpenFileUrl(this);

    if (url.isValid()) {
        // open a new window if the document is _not_ in its initial state
        if (!m_part->url().isValid()) {
            // open the file in a new window
            auto window = new kddrescueviewShell;
            window->loadDocument(url);
            window->show();
        } else {
            // open the file in this window
            loadDocument(url);
        }
    }
}
