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

#include "kddrescueviewshell.h"

// KF headers
#include <KAboutData>
#include <KLocalizedString>

// Qt headers
#include <QApplication>
#include <QCommandLineParser>
#include <QUrl>
#include <QDir>
#include <QIcon>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain("kddrescueview");

    KAboutData aboutData(QStringLiteral("kddrescueview"),
        i18n("kddrescueview"),
        QStringLiteral("0.1"),
        i18n("KPart viewer for GNU ddrescue map files"),
        KAboutLicense::GPL,
        i18n("Copyright 2018 Adrien Cordonnier"));
    aboutData.addAuthor(i18n("Adrien Cordonnier"), i18n("Author"), QStringLiteral("adrien.cordonnier@gmail.com"));
    aboutData.addAuthor(i18n("Martin Bittermann"), i18n("Inspirator for the excellent ddrescueview written in Object Pascal"), QStringLiteral(""));
    aboutData.setOrganizationDomain("kde.org");
    aboutData.setDesktopFileName(QStringLiteral("org.kde.kddrescueview"));

    KAboutData::setApplicationData(aboutData);
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("kddrescueview")));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    aboutData.setupCommandLine(&parser);
    parser.addPositionalArgument(QStringLiteral("urls"), i18n("GNU ddrescue map file(s) to load."), QStringLiteral("[urls...]"));

    parser.process(app);
    aboutData.processCommandLine(&parser);

    const auto urls = parser.positionalArguments();

    if (urls.isEmpty()) {
        auto window = new kddrescueviewShell;
        window->show();
    } else {
        for (const auto &url : urls) {
            auto window = new kddrescueviewShell;
            window->show();
            window->loadDocument(QUrl::fromUserInput(url, QDir::currentPath(), QUrl::AssumeLocalFile));
        }
    }

    return app.exec();
}
