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

#ifndef KDDRESCUEVIEWPART_H
#define KDDRESCUEVIEWPART_H

// KF headers
#include <KParts/ReadOnlyPart>

class QTextEdit;
class QWidget;
class QAction;
class QTextDocument;

/**
 * @short kddrescueview Part
 */
class kddrescueviewPart : public KParts::ReadOnlyPart
{
    Q_OBJECT

public:
    /**
     * Default constructor, with arguments as expected by KPluginFactory
     */
    kddrescueviewPart(QWidget* parentWidget, QObject* parent, const QVariantList& arg);

    /**
     * Destructor
     */
    ~kddrescueviewPart() override;


protected: // KParts::ReadWritePart API
    bool openFile() override;

private:
    void setupActions();

private:
    QTextEdit* m_textEditWidget;
    QTextDocument* m_textDocument;
};

#endif // KDDRESCUEVIEWPART_H
