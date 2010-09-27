/*
 * win.cpp
 * This file is part of Logram
 *
 * Copyright (C) 2010 - Leo Testard <leo.testard@gmail.com>
 *
 * Logram is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Logram is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Logram; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#ifndef MENUENTRY_H
#define MENUENTRY_H

#include <QWidget>

class MenuEntry : public QWidget
{
    Q_OBJECT
public:
    MenuEntry(const QString &title, const QString &to, const QString &icon, const QString &text, QWidget *parent = 0);
    QString page() { return mTo; }
signals:
    void clicked();
private:
    // Nom du fichier sur lequel pointe l'entée de menu
    QString mTo;
protected:
    virtual void mouseReleaseEvent(QMouseEvent*);
};

#endif // MENUENTRY_H
