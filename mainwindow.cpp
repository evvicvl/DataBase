#include "mainwindow.h"

QStandardItem *MainWindow::insertItem(QStandardItem *parentItem, QList<QVariant> data)
{
    QStandardItem *item = new QStandardItem(data.at(2).toString());
    item->setData(data.at(0), Qt::UserRole);
    item->setData(data.at(1), Qt::UserRole + 1);
    item->setData(data.at(2), Qt::UserRole + 2);
    parentItem->appendRow(item);
    return item;
}

QStandardItem *MainWindow::insertItem(QStandardItem *parentItem, QStandardItem *item)
{
    QStandardItem *item_ = item->clone();
    item_->setEditable(true);
    parentItem->appendRow(item_);
    return item_;
}

void MainWindow::loadFromDB()
{
    maxId = 0;
    while(rootItem_Cache->rowCount() > 0)
        rootItem_Cache->removeRow(0);
    while(rootItem_DataBase->rowCount() > 0)
        rootItem_DataBase->removeRow(0);
    readChildrensFromDB(rootItem_DataBase, 0);
}

void MainWindow::onActionDownloadTriggered()
{
    QStandardItem *itemCache = rootItem_Cache;
    QStandardItem *itemDataBase = model_DataBase->itemFromIndex(treeView_DataBase->currentIndex());
    if(itemDataBase)
    {
        QModelIndexList matchesParent = treeView_Cache->model()->match(model_Cache->index(0, 0), Qt::UserRole, itemDataBase->data(Qt::UserRole + 1), 1, Qt::MatchExactly | Qt::MatchWrap | Qt::MatchRecursive);
        if(matchesParent.count() > 0)
        {
            itemCache = model_Cache->itemFromIndex(matchesParent.at(0));
        }
        itemCache = insertItem(itemCache, itemDataBase);

        QModelIndexList matchesChildrens = treeView_Cache->model()->match(model_Cache->index(0, 0), Qt::UserRole + 1, itemDataBase->data(Qt::UserRole), -1, Qt::MatchExactly | Qt::MatchWrap | Qt::MatchRecursive);
        for(int i = 0; i < matchesChildrens.count(); i++)
        {
            QList<QStandardItem*> items = model_Cache->takeRow(matchesChildrens.at(i).row());
            itemCache->appendRows(items);
        }
    }
}

void MainWindow::onActionCreateTriggered()
{
    QStandardItem *parentItemCache = model_Cache->itemFromIndex(treeView_Cache->currentIndex());
    QStandardItem *newItemCache = new QStandardItem("new item");
    if(!parentItemCache)
        parentItemCache = rootItem_Cache;
    newItemCache->setData(++maxId, Qt::UserRole);
    newItemCache->setData(parentItemCache->data(Qt::UserRole), Qt::UserRole + 1);
    newItemCache->setData(newItemCache->text(), Qt::UserRole + 2);
    parentItemCache->appendRow(newItemCache);
    treeView_Cache->setCurrentIndex(newItemCache->index());
    treeView_Cache->edit(newItemCache->index());
    QStandardItem *newItemDataBase = newItemCache->clone();
    QModelIndexList matchesDataBase = treeView_DataBase->model()->match(model_DataBase->index(0, 0), Qt::UserRole, parentItemCache->data(Qt::UserRole), 1, Qt::MatchExactly | Qt::MatchWrap | Qt::MatchRecursive);
    if(matchesDataBase.count() > 0)
        model_DataBase->itemFromIndex(matchesDataBase.at(0))->appendRow(newItemDataBase);
    else
        rootItem_DataBase->appendRow(newItemDataBase);
}

void MainWindow::onActionEditTriggered()
{
    QStandardItem *itemCache = model_Cache->itemFromIndex(treeView_Cache->currentIndex());
    if(itemCache)
        treeView_Cache->edit(itemCache->index());
}

void MainWindow::onActionDeleteTriggered()
{
    QStandardItem *itemCache = model_Cache->itemFromIndex(treeView_Cache->currentIndex());
    if(itemCache)
    {
        deleteItem(itemCache);
        QModelIndexList matchesDataBase = treeView_DataBase->model()->match(model_DataBase->index(0, 0), Qt::UserRole, itemCache->data(Qt::UserRole), 1, Qt::MatchExactly | Qt::MatchWrap | Qt::MatchRecursive);
        if(matchesDataBase.count() > 0)
        {
            deleteItem(model_DataBase->itemFromIndex(matchesDataBase.at(0)));
        }
        treeView_Cache->setCurrentIndex(QModelIndex());
        treeView_DataBase->setCurrentIndex(QModelIndex());
    }
}

void MainWindow::onItemChanged(QStandardItem *item)
{
    if(item->isEnabled())
    {
        QModelIndexList matchesDataBase = treeView_DataBase->model()->match(model_DataBase->index(0, 0), Qt::UserRole, item->data(Qt::UserRole), 1, Qt::MatchExactly | Qt::MatchWrap | Qt::MatchRecursive);
        if(matchesDataBase.count() > 0)
        {
            model_DataBase->itemFromIndex(matchesDataBase.at(0))->setText(item->text());
        }
    }
}

void MainWindow::readChildrensFromDB(QStandardItem *parent, int id)
{
    QSqlQuery sqlQuery(dataBase);
    if(sqlQuery.exec(QString("SELECT * FROM \"tree\" WHERE parent_id = %1").arg(id)))
    {
        while(sqlQuery.next())
        {
            QStandardItem *item = insertItem(parent, QList({QVariant(sqlQuery.value(0)),
                                                            QVariant(sqlQuery.value(1)),
                                                            QVariant(sqlQuery.value(2))}));
            item->setEditable(false);
            if(item->data(Qt::UserRole).toInt() > maxId)
                maxId = item->data(Qt::UserRole).toInt();
            readChildrensFromDB(item, sqlQuery.value(0).toInt());
        }
    }
}

void MainWindow::deleteItem(QStandardItem *item)
{
    item->setEnabled(false);
    for(int i = 0; i < item->rowCount(); i++)
    {
        deleteItem(item->child(i));
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(800, 600);

    QString dataBaseName = "Tree.sql";
    dataBase = QSqlDatabase::addDatabase("QSQLITE");
    if(QFile::exists(QDir::currentPath() + "/" + dataBaseName))
    {
        dataBase.setDatabaseName(QDir::currentPath() + "/" + dataBaseName);
        dataBase.open();
    }
    else
    {
        QMessageBox::warning(this, "Внимание", "Пожалуйста, положите файл " + dataBaseName + " рядом с исполняемым\n"
                                               "и перезапустите программу");
    }

    centralwidget = new QWidget();
    this->setCentralWidget(centralwidget);
    gridLayout = new QGridLayout(centralwidget);

    treeView_Cache = new QTreeView(centralwidget);
    treeView_Cache->setContextMenuPolicy(Qt::ActionsContextMenu);
    gridLayout->addWidget(treeView_Cache, 0, 0, 1, 1);

    treeView_DataBase = new QTreeView(centralwidget);
    treeView_DataBase->setContextMenuPolicy(Qt::ActionsContextMenu);
    gridLayout->addWidget(treeView_DataBase, 0, 1, 1, 1);

    model_Cache = new QStandardItemModel(treeView_Cache);
    model_Cache->setHorizontalHeaderLabels(QStringList("Cache"));
    treeView_Cache->setModel(model_Cache);
    rootItem_Cache = model_Cache->invisibleRootItem();

    model_DataBase = new QStandardItemModel(treeView_DataBase->cornerWidget());
    model_DataBase->setHorizontalHeaderLabels(QStringList("DataBase"));
    treeView_DataBase->setModel(model_DataBase);
    rootItem_DataBase = model_DataBase->invisibleRootItem();

    menubar = new QMenuBar(this);
    this->setMenuBar(menubar);
    menuFile = new QMenu(menubar);
    actionReset = new QAction(this);
    actionExit = new QAction(this);
    menubar->addAction(menuFile->menuAction());
    menuFile->addAction(actionReset);
    menuFile->addAction(actionExit);
    actionReset->setText(QCoreApplication::translate("MainWindow", "Reset", nullptr));
    actionExit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
    menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));

    contextMenu_Cache = new QMenu(treeView_Cache);
    actionCreate_Cache = new QAction("create", contextMenu_Cache);
    actionEdit_Cache = new QAction("edit", contextMenu_Cache);
    actionDelete_Cache = new QAction("delete", contextMenu_Cache);
    treeView_Cache->addAction(actionCreate_Cache);
    treeView_Cache->addAction(actionEdit_Cache);
    treeView_Cache->addAction(actionDelete_Cache);
    contextMenu_DataBase = new QMenu(treeView_DataBase);
    actionDownload_DataBase = new QAction("download", contextMenu_DataBase);
    treeView_DataBase->addAction(actionDownload_DataBase);

    connect(actionReset, &QAction::triggered, this, &MainWindow::loadFromDB);
    connect(actionExit, &QAction::triggered, this, &QCoreApplication::quit);
    connect(actionDownload_DataBase, &QAction::triggered, this, &MainWindow::onActionDownloadTriggered);
    connect(actionCreate_Cache, &QAction::triggered, this, &MainWindow::onActionCreateTriggered);
    connect(actionEdit_Cache, &QAction::triggered, this, &MainWindow::onActionEditTriggered);
    connect(actionDelete_Cache, &QAction::triggered, this, &MainWindow::onActionDeleteTriggered);
    connect(model_Cache, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(onItemChanged(QStandardItem*)));

    loadFromDB();
}

MainWindow::~MainWindow()
{
}

