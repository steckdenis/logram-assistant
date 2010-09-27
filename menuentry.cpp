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

#include "menuentry.h"

#include <QtGui/QLabel>
#include <QtGui/QPixmap>
#include <QtGui/QGridLayout>
#include <QtGui/QMouseEvent>

#include <QtDebug>

MenuEntry::MenuEntry(const QString &title, const QString &to, const QString &icon, const QString &text, QWidget *parent) : QWidget(parent)
{
    QGridLayout *mLayout = new QGridLayout(this);
    setLayout(mLayout);

    // afficher l'icône
    QLabel *iconLabel = new QLabel(this);
    QPixmap p(icon);
    if(p.height() > 64 || p.width() > 64)
        p = p.scaled(QSize(64, 64), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    iconLabel->setPixmap(p);
    mLayout->addWidget(iconLabel, 0, 0, 2, 1);

    // le titre
    QLabel *titleLabel = new QLabel(this);
    titleLabel->setText("<strong>" + title + "</strong>");
    titleLabel->setAlignment(Qt::AlignCenter);
    mLayout->addWidget(titleLabel, 0, 1, 1, 2);

    // et le texte
    QLabel *textLabel = new QLabel(this);
    textLabel->setText(text);
    textLabel->setWordWrap(true);
    mLayout->addWidget(textLabel, 1, 1, 1, 2);

    mTo = to;

    setMaximumWidth(300);
    setMinimumWidth(300);
    setCursor(Qt::PointingHandCursor);
    show();
}

void MenuEntry::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
        emit clicked();
    (void)e;
}
