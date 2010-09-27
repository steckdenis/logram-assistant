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

#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QtCore/QList>
#include <QtCore/QVariant>

#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>

#include <QtXml/QDomDocument>

class TreeItem : QObject
{
    Q_OBJECT
public:
    TreeItem(const QString &text, const QString &data, TreeItem *parent = 0);
    ~TreeItem();

    void appendChild(TreeItem *child);

    TreeItem *parent();
    TreeItem *child(int at);
    QList<TreeItem *> childs();

    int childCount() const;

    QString title() const;
    QString data() const;

public slots:
    void setTitle(const QString &t);
    void setData(const QString &d);

private:
    QList<TreeItem*> childItems;
    QString itemTitle;
    QString itemData;
    TreeItem *parentItem;
};

class TreeView : public QWidget
{
    Q_OBJECT
public:
    TreeView(QDomElement el, QWidget *parent = 0);
public slots:
    void setPage(const QString&, const QString&);
private slots:
    void buttonClicked();
    void menuItemClicked();
signals:
    void pageChanged(const QString&);
private:
    TreeItem *itemByPath(const QString &);
    TreeItem *itemByPath(const QString &, TreeItem*);
    void setupModelData(const QDomElement &);
    void setupModelData(const QDomElement&, TreeItem*);

    QList<TreeItem*> its;
    QList<QPushButton*> btns;
    QHBoxLayout *layout;

    TreeItem *rootItem;
};

#endif // TREEVIEW_H
