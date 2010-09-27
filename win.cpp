/*
 * win.cpp
 * This file is part of Logram
 *
 * Copyright (C) 2010 - Leo Testard         <leo.testard@gmail.com>
 *               2009 - Denis Steckelmacher <steckdenis@logram-project.org>
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

#include "win.h"
#include "treeview.h"
#include "menuentry.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QPropertyAnimation>

#include <QtGui/QFont>
#include <QtGui/QIcon>
#include <QtGui/QLabel>
#include <QtGui/QFrame>
#include <QtGui/QPixmap>
#include <QtGui/QBoxLayout>
#include <QtGui/QSizePolicy>
#include <QtGui/QPushButton>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>

Win::Win(const QString &path) : QWidget(0)
{
    resize(800, 600);
    move(QApplication::desktop()->width() / 2 - 400, QApplication::desktop()->height() / 2 - 300);
    setWindowIcon(QIcon(":/icon.png"));

    // Créer le layout principal
    QVBoxLayout *mLayout = new QVBoxLayout(this);
    this->setLayout(mLayout);

    // Créer le conteneur de la page
    pageWidget = new QWidget(this);
    mLayout->addWidget(pageWidget);
    layout = new QVBoxLayout(pageWidget);
    pageWidget->setLayout(layout);
    layout->addStretch();
    layout->setSpacing(12);

    // Créer les boutons
    QFrame *sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    mLayout->addWidget(sep);
    QHBoxLayout *btnLayout = new QHBoxLayout();
    mLayout->addLayout(btnLayout);
    btnQuit = new QPushButton(tr("&Quitter"));
    btnPrev = new QPushButton(tr("« &Précédant"));
    btnNext = new QPushButton(tr("&Suivant »"));
    connect(btnQuit, SIGNAL(clicked()), qApp, SLOT(quit()));
    connect(btnPrev, SIGNAL(clicked()), this, SLOT(prevClicked()));
    connect(btnNext, SIGNAL(clicked()), this, SLOT(nextClicked()));
    btnLayout->addWidget(btnPrev);
    btnLayout->addStretch();
    btnLayout->addWidget(btnQuit);
    btnLayout->addStretch();
    btnLayout->addWidget(btnNext);

    // Charger le document
    QFile fl(path);
    doc.setContent(&fl);
    QDir::setCurrent(path.section('/', 0, -2));

    // Créer le fil d'Ariane
    v = new TreeView(doc.documentElement(), this);
    displayPage(doc.documentElement());
    connect(v, SIGNAL(pageChanged(QString)), this, SLOT(displayPage(QString)));
    mLayout->insertWidget(0, v, 0, Qt::AlignRight);

    // Redimensionnement
    show();
}

void Win::nextClicked()
{
    if(!currentPage.nextSiblingElement().isNull())
        displayPage(currentPage.nextSiblingElement(), 1);
}

void Win::prevClicked()
{
    if(!currentPage.previousSiblingElement().isNull())
        displayPage(currentPage.previousSiblingElement(), 1);
}

void Win::nextPageClicked()
{
    if (currentPg != maxPg) // Si on est pas encore à la fin
    {   // afficher la page suivante
        currentPg++;
        displayPage(currentPage, currentPg);
    }
}

void Win::prevPageClicked()
{
    if(currentPg != 1) // Si on est pas à la première page
    {   // afficher la page précédente
        currentPg--;
        displayPage(currentPage, currentPg);
    }
}


void Win::buttonClicked()
{
    QPushButton *btn = static_cast<QPushButton *>(sender());
    if (btn == 0) return;

    // Récupérer la page du bouton
    QString page = btn->property("page").toString();

    currentPage = nodeByPath(page);

    displayPage(page);
}

void Win::menuEntryClicked()
{
    MenuEntry *b = qobject_cast<MenuEntry*>(sender());
    currentPage = nodeByPath(b->page());
    displayPage(nodeByPath(b->page()));
}

void Win::displayPage(const QDomElement &page, int pg)
{
    currentPage = page;

    displayPage(page.attribute("page"), pg);
}

void Win::displayPage(const QString &path, int pg)
{
    // Explorer le document de la page
    QDomDocument pdoc;
    QFile fl(path);
    if(!fl.exists())
        return;
    pdoc.setContent(&fl);
    QDomElement el = pdoc.documentElement().firstChildElement();

    // Supprimer tous les widgets de la page précédente
    while(widgets.count())
        delete widgets.takeFirst();

    // mettre à jour les boutons
    btnPrev->setEnabled(!currentPage.previousSiblingElement().isNull());
    btnNext->setEnabled(!currentPage.nextSiblingElement().isNull());

    // Gérer les pages et le titre
    QString tt = pdoc.documentElement().attribute("title");
    currentPg = pg;
    setWindowTitle(tt + tr(" - Logram Assistant"));

    // Créer le titre de la page
    QLabel *title = new QLabel(tt, this);
    QFont fnt = title->font();
    fnt.setItalic(true);
    fnt.setPixelSize(22);
    title->setFont(fnt);
    title->setAlignment(Qt::AlignCenter);
    title->show();
    layout->insertWidget(0, title);
    widgets.append(title);

    v->setPage(tt, path);

    int pageCount = 1;

    while (!el.isNull())
    {
        if(pageCount == pg)
        {
            if(el.tagName() == "text")
            {
                // Élément de texte, créer un QLabel
                QLabel *txt = new QLabel(this);
                txt->setTextFormat(Qt::RichText);
                txt->setText(el.text().trimmed().replace("[", "<").replace("]", ">"));
                txt->setWordWrap(true);
                txt->setAlignment(Qt::AlignTop | Qt::AlignLeft);
                txt->show();
                layout->insertWidget(layout->count()-1, txt);

                widgets.append(txt);
            }
            else if(el.tagName() == "button")
            {
                // Un bouton, lien vers une page
                QPushButton *btn = new QPushButton(this);
                btn->setText(el.attribute("title"));
                btn->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
                btn->show();
                layout->insertWidget(layout->count()-1, btn, 0, Qt::AlignCenter);
                widgets.append(btn);

                // Utiliser une propriété pour retrouver ce bouton
                btn->setProperty("page", el.attribute("to"));

                connect(btn, SIGNAL(clicked()), this, SLOT(buttonClicked()));
            }
            else if(el.tagName() == "img")
            {
                QLabel *img = new QLabel(this);
                img->setAlignment(Qt::AlignCenter);
                img->setPixmap(QPixmap(el.attribute("path")));
                img->setToolTip(el.attribute("tooltip"));

                layout->insertWidget(layout->count()-1, img, 0, Qt::AlignCenter);
                widgets.append(img);
            }
            else if(el.tagName() == "menuentry")
            {
                MenuEntry *b = new MenuEntry(el.attribute("title"), el.attribute("to"), el.attribute("icon"), el.text().trimmed().replace("[", "<").replace("]",">"), this);
                connect(b, SIGNAL(clicked()), this, SLOT(menuEntryClicked()));
                layout->insertWidget(layout->count()-1, b, 0, Qt::AlignCenter);
                widgets.append(b);
            }
        }
        if(el.tagName() == "page")
            pageCount++;
        el = el.nextSiblingElement();
    }

    if(pageCount > 1 && pg != pageCount) // Il y a plus d'une page, et la page actuelle n'est pas la dernière
    {
        QPushButton *bNextPg = new QPushButton(QIcon::fromTheme("go-down"), "", this);
        bNextPg->setToolTip(tr("Page suivante"));
        bNextPg->setFlat(true);
        connect(bNextPg, SIGNAL(clicked()), this, SLOT(nextPageClicked()));
        layout->insertWidget(layout->count()-1, bNextPg, 0, Qt::AlignCenter);
        widgets.append(bNextPg);
    }
    if(pg != 1) // La page actuelle n'est pas la première, on ajoute un bouton pour remonter
    {
        QPushButton *bPrevPg = new QPushButton(QIcon::fromTheme("go-up"), "", this);
        bPrevPg->setToolTip(tr("Page précédente"));
        bPrevPg->setFlat(true);
        connect(bPrevPg, SIGNAL(clicked()), this, SLOT(prevPageClicked()));
        layout->insertWidget(1, bPrevPg, 0, Qt::AlignCenter); // on le place après le titre
        widgets.append(bPrevPg);
    }

    // on stocke le nombre de pages
    maxPg = pageCount;
}

QDomElement Win::nodeByPath(const QString &path)
{
    if (doc.documentElement().attribute("page") == path)
    {
        return doc.documentElement();
    }

    return nodeByPath(path, doc.documentElement());
}

QDomElement Win::nodeByPath(const QString &path, const QDomElement &parent)
{
    // Explorer les enfants
    QDomElement child = parent.firstChildElement();

    while (!child.isNull())
    {
        if (child.attribute("page") == path)
            return child;

        // Voir si cet enfant contient ce qu'on cherche
        QDomElement rs = nodeByPath(path, child);

        if (!rs.isNull())
            return rs;

        child = child.nextSiblingElement();
    }

    return QDomElement();
}
