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
#include <KParts/ReadWritePart>

class QTextEdit;
class QWidget;
class QAction;
class QTextDocument;

/**
 * @short kddrescueview Part
 */
class kddrescueviewPart : public KParts::ReadWritePart
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

    /**
     * Reimplemented to update the internal UI
     */
    void setReadWrite(bool rw) override;

    /**
     * Reimplemented to disable and enable Save action
     */
    void setModified(bool modified) override;

protected: // KParts::ReadWritePart API
    bool openFile() override;
    bool saveFile() override;

private:
    void setupActions();

private Q_SLOTS:
    void fileSave();
    void fileSaveAs();

private:
    QTextEdit* m_textEditWidget;
    QAction* m_saveAction;
    QTextDocument* m_textDocument;
};

#endif // KDDRESCUEVIEWPART_H
