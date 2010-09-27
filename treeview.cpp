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

#include "treeview.h"

#include <QtGui/QMenu>

// TreeItem methods implementation

TreeItem::TreeItem(const QString &t, const QString &data, TreeItem *parent)
{
    itemTitle = t;
    parentItem = parent;
    itemData = data;
}

TreeItem::~TreeItem()
{
    qDeleteAll(childItems);
}

void TreeItem::appendChild(TreeItem *item)
{
    childItems.append(item);
}

TreeItem *TreeItem::parent()
{
    return parentItem;
}

TreeItem *TreeItem::child(int at)
{
    return childItems.value(at);
}

QList<TreeItem*> TreeItem::childs()
{
    return childItems;
}

int TreeItem::childCount() const
{
    return childItems.count();
}

QString TreeItem::title() const
{
    return itemTitle;
}

QString TreeItem::data() const
{
    return itemData;
}

void TreeItem::setTitle(const QString &t)
{
    itemTitle = t;
}

void TreeItem::setData(const QString &d)
{
    itemData = d;
}

// TreeView methods implementation

TreeView::TreeView(QDomElement el, QWidget *parent) : QWidget(parent)
{
    rootItem = new TreeItem("","");

    layout = new QHBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);

    setupModelData(el);

    QPushButton *root = new QPushButton(this);
    connect(root, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    root->setText(rootItem->title());
    root->setFlat(true);
    layout->addWidget(root);

    its.append(rootItem);
    btns.append(root);

    QMenu *menu = new QMenu(root);
    menu->addAction(QIcon(), rootItem->title(), this, SLOT(menuItemClicked()));
    menu->addSeparator();
    foreach(TreeItem *n, rootItem->childs())
        QAction *act = menu->addAction(QIcon(), n->title(), this, SLOT(menuItemClicked()));
    root->setMenu(menu);
}

void TreeView::setPage(const QString &title, const QString &file)
{
    for(int j = 1; j < btns.count(); j++)
    {
        delete btns.takeAt(j);
        its.removeAt(j);
    }

    TreeItem *i = itemByPath(file);
    if(i == rootItem)
        return;

    // on ajoute un bouton approprié
    its.append(i);
    QPushButton *b = new QPushButton(title, this);
    b->setFlat(true);
    connect(b, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    btns.append(b);
    layout->addWidget(b);
    // on regarde si il a lui-même des enfants pour ajouter un menu
    if(i->childCount())
    {
        // on crée un menu qui les contient tous
        QMenu *menu = new QMenu(b);
        menu->addAction(QIcon(), i->title(), this, SLOT(menuItemClicked()));
        menu->addSeparator();
        foreach(TreeItem *n, i->childs())
        {
            QAction *act = new QAction(QIcon(), n->title(), this);
            connect(act, SIGNAL(triggered()), this, SLOT(menuItemClicked()));
            menu->addAction(act);
        }
        b->setMenu(menu);
    }

    // On ajoute les boutons des parents
    TreeItem *parent = i->parent();
    while(parent != rootItem)
    {
        its.append(parent);
        QPushButton *btn = new QPushButton(parent->title(), this);
        btn->setFlat(true);
        connect(btn, SIGNAL(clicked()), this, SLOT(buttonClicked()));
        btns.append(btn);
        layout->insertWidget(1, btn, 0, Qt::AlignCenter);
        // on regarde si il a lui-même des enfants pour ajouter un menu
        if(parent->childCount())
        {
            // on crée un menu qui les contient tous
            QMenu *menu = new QMenu(btn);
            menu->addAction(QIcon(), parent->title(), this, SLOT(menuItemClicked()));
            menu->addSeparator();
            foreach(TreeItem *n, parent->childs())
            {
                QAction *act = new QAction(QIcon(), n->title(), this);
                connect(act, SIGNAL(triggered()), this, SLOT(menuItemClicked()));
                menu->addAction(act);
            }
            btn->setMenu(menu);
        }
        parent = parent->parent();
    }
}

void TreeView::buttonClicked()
{
    QPushButton *b = static_cast<QPushButton*>(sender());
    if(b)
    {
        TreeItem *it = its.at(btns.indexOf(b));
        // On supprime tous les boutons se trouvant après
        for(int i = btns.indexOf(b) + 1; i < btns.count(); i++)
        {
            delete btns.at(i); btns.removeAt(i);
            its.removeAt(i);
        }
        // On informe que la page a été changée
        emit(pageChanged(it->data()));
    }
}

void TreeView::menuItemClicked()
{
    // Retrouver le bouton parent de l'action qui a été clickée
    QAction *act = static_cast<QAction*>(sender());
    if(!act) return;
    QMenu *parentMenu = static_cast<QMenu*>(act->parent());
    if(!parentMenu) return;
    QPushButton *parentBtn = static_cast<QPushButton*>(parentMenu->parent());
    if(!btns.contains(parentBtn)) return;

    // Récupérer l'item correspondant à ce bouton
    TreeItem *it = its.at(btns.indexOf(parentBtn));

    if(!it)
        return;

    // Si l'action correspdond à l'item (première action avant le separator)
    if(it->title() == act->text())
    {
        emit pageChanged(it->data());
        return;
    }

    // Regarder dans ses enfants si une page correspond à l'action cliquée
    foreach(TreeItem *child, it->childs())
    {
        if(child->title() == act->text())
            emit pageChanged(child->data());
    }
}

void TreeView::setupModelData(const QDomElement &main)
{
    rootItem->setTitle(main.attribute("title"));
    rootItem->setData(main.attribute("page"));
    setupModelData(main, rootItem);
}

void TreeView::setupModelData(const QDomElement &parentEl, TreeItem *parent)
{
    // Explorer les enfants de el, et les ajouter
    QDomElement child = parentEl.firstChildElement();

    while(!child.isNull())
    {
        TreeItem *item = new TreeItem(child.attribute("title"),child.attribute("page"),parent);
        parent->appendChild(item);
        setupModelData(child,item);
        child = child.nextSiblingElement();
    }
}

TreeItem *TreeView::itemByPath(const QString &path)
{
    if (rootItem->data() == path)
        return rootItem;

    return itemByPath(path, rootItem);
}

TreeItem *TreeView::itemByPath(const QString &path, TreeItem *parent)
{
    //TreeItem *child = parent->childs().first();

    //for(int i = 1; i < parent->childs().count(); i++)
    foreach(TreeItem *child, parent->childs())
    {
        if (child->data() == path)
            return child;

        // Voir si cet enfant contient ce qu'on cherche
        TreeItem *rs = itemByPath(path, child);

        if(!rs->title().isEmpty())
            return rs;
    }

    return new TreeItem("","");
}

