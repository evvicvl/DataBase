#include "mainwindow.h"

void MainWindow::loadWhereParentIsNull()
{
    QSqlQuery sqlQuery(dataBase);
    stringQuery = "SELECT * FROM \"tree\" WHERE parent_id IS NULL";
    if(sqlQuery.exec(stringQuery))
    {
        if(sqlQuery.next())
        {
            qListItem_DataBase->push_back(new QTreeWidgetItem(treeWidget_DataBase));
            qListItem_DataBase->back()->setText(0, sqlQuery.value(2).toString());
            qListItem_DataBase->back()->setText(1, sqlQuery.value(0).toString());
            qListItem_DataBase->back()->setText(2, sqlQuery.value(1).toString());
            qListItem_DataBase->back()->setText(3, sqlQuery.value(3).toString());
            qListItem_DataBase->back()->setBackground(0, QBrush(QColor(230, 230, 230)));
            treeWidget_DataBase->addTopLevelItem(qListItem_DataBase->back());
        }
    }
}

void MainWindow::loadChildren(QTreeWidgetItem* item)
{
    QSqlQuery sqlQuery(dataBase);
    stringQuery = QString("SELECT * FROM \"tree\" WHERE parent_id = %1").arg(item->text(1));
    if(sqlQuery.exec(stringQuery))
    {
        while(item->childCount() > 0)
            item->removeChild(item->child(0));

        while(sqlQuery.next())
        {
            qListItem_DataBase->push_back(new QTreeWidgetItem(item));
            QSqlQuery sqlQuery_(dataBase);
            if( sqlQuery_.exec(QString("SELECT * FROM \"tree\" WHERE parent_id = %1").arg(sqlQuery.value(0).toString()))
                && sqlQuery_.next())
            {
                qListItem_DataBase->back()->setText(0, sqlQuery.value(2).toString());
                qListItem_DataBase->back()->setBackground(0, QBrush(QColor(230, 230, 230)));
            }
            else
            {
                qListItem_DataBase->back()->setText(0, sqlQuery.value(2).toString());
            }
            qListItem_DataBase->back()->setText(1, sqlQuery.value(0).toString());
            qListItem_DataBase->back()->setText(2, sqlQuery.value(1).toString());
            qListItem_DataBase->back()->setText(3, sqlQuery.value(3).toString());
            if(sqlQuery.value(3).toBool())
                qListItem_DataBase->back()->setDisabled(true);
        }
    }
}

void MainWindow::putItem(QList<QTreeWidgetItem *> *listItem, QTreeWidgetItem *item, QTreeWidgetItem *parent)
{
    listItem->push_back(new QTreeWidgetItem(parent));
    listItem->back()->setText(0, item->text(0));
    listItem->back()->setText(1, item->text(1));
    listItem->back()->setText(2, item->text(2));
    listItem->back()->setText(3, item->text(3));
}

void MainWindow::putItem(QList<QTreeWidgetItem*> *listItem, QTreeWidgetItem *item, QTreeWidget *parent)
{
    listItem->push_back(new QTreeWidgetItem(parent));
    listItem->back()->setText(0, item->text(0));
    listItem->back()->setText(1, item->text(1));
    listItem->back()->setText(2, item->text(2));
    listItem->back()->setText(3, item->text(3));
}

void MainWindow::onActionOpenTriggered()
{
    QString fName = QFileDialog::getOpenFileName();
    if(!fName.isEmpty())
    {
        onActionResetTriggered();
        dataBase.setDatabaseName(fName);
        dataBase.open();
        loadWhereParentIsNull();
    }
}

void MainWindow::onActionSaveTriggered()
{
    QSqlQuery sqlQuery(dataBase);
    while(stringListQuery.count() > 0)
    {
        sqlQuery.exec(stringListQuery.first());
        stringListQuery.removeFirst();
    }
    treeWidget_DataBase->clear();
    qListItem_DataBase->clear();
    loadWhereParentIsNull();
}

void MainWindow::onActionResetTriggered()
{
    treeWidget_Cache->clear();
    treeWidget_DataBase->clear();
    qListItem_Cache->clear();
    qListItem_DataBase->clear();
    stringListQuery.clear();
}

void MainWindow::onActionExitTriggered()
{
    this->close();
}

void MainWindow::onDataBaseDoubleClicked(QTreeWidgetItem *item, int column)
{
    loadChildren(item);
}

void MainWindow::onActionCreateTriggered()
{
    qListItem_Cache->push_back(new QTreeWidgetItem(treeWidget_Cache->currentItem()));
    qListItem_Cache->back()->setFlags(qListItem_Cache->back()->flags() | Qt::ItemIsEditable);
    treeWidget_Cache->currentItem()->setExpanded(true);
    treeWidget_Cache->editItem(qListItem_Cache->back());
}

void MainWindow::onActionEditTriggered()
{
    treeWidget_Cache->currentItem()->setFlags(treeWidget_Cache->currentItem()->flags() | Qt::ItemIsEditable);
    treeWidget_Cache->editItem(treeWidget_Cache->currentItem(), 0);
}

void MainWindow::onActionDeleteTriggered()
{
    stringListQuery.push_back(QString("UPDATE \"tree\" SET removed = 1 WHERE id = %1").arg(treeWidget_Cache->currentItem()->text(1)));
    treeWidget_Cache->currentItem()->setDisabled(true);
}

void MainWindow::onActionDownloadTriggered()
{
    bool isRoot = true;
    QList<QTreeWidgetItem*> qListItem_tree;
    QTreeWidgetItem *pp = nullptr;
    pp = treeWidget_DataBase->currentItem();
    while(pp && isRoot)
    {
        for(int i = 0; i < qListItem_Cache->count() && isRoot; i++)
        {
            if(qListItem_Cache->at(i)->text(1) == pp->text(1))
            {
                qListItem_tree.push_back(qListItem_Cache->at(i));
                isRoot = false;
            }
        }
        if(isRoot)
        {
            qListItem_tree.push_back(pp);
            pp = pp->parent();
        }
    }
    for(int i = qListItem_tree.count() - 1; i >= 0; i--)
    {
        if(i == qListItem_tree.count() - 1)
        {
            if(isRoot)
            {
                putItem(qListItem_Cache, qListItem_tree.at(i), treeWidget_Cache);
                treeWidget_Cache->addTopLevelItem(qListItem_Cache->back());
            }
            else
            {
                if(qListItem_tree.count() > 1)
                {
                    i--;
                    putItem(qListItem_Cache, qListItem_tree.at(i), qListItem_tree.last());
                }
            }
        }
        else
        {
            putItem(qListItem_Cache, qListItem_tree.at(i), qListItem_Cache->back());
        }
    }
}

void MainWindow::onItemEdit(QTreeWidgetItem *item, int column)
{
    if( item->flags().testFlag(Qt::ItemIsEditable))
    {
        if(item->text(1).isEmpty() && !item->text(0).isEmpty() && item->flags().testFlag(Qt::ItemIsEnabled))
        {
            stringListQuery.push_back(QString("INSERT INTO \"tree\" (parent_id, data) VALUES (%1, '%2')").arg(item->parent()->text(1), item->text(0)));
        }
        else
        {
            if(!item->text(0).isEmpty())
                stringListQuery.push_back(QString("UPDATE \"tree\" SET data = '%1' WHERE id = %2").arg(item->text(0), item->text(1)));
        }
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(800, 600);
    dataBase = QSqlDatabase::addDatabase("QSQLITE");

    centralwidget = new QWidget(this);
    setCentralWidget(centralwidget);
    gridLayout = new QGridLayout(centralwidget);

    qListItem_Cache = new QList<QTreeWidgetItem*>;
    treeWidget_Cache = new QTreeWidget(centralwidget);
    treeWidget_Cache->setContextMenuPolicy(Qt::ActionsContextMenu);
    treeWidget_Cache->headerItem()->setText(0, QString("Cache"));

    qListItem_DataBase = new QList<QTreeWidgetItem*>;
    treeWidget_DataBase = new QTreeWidget(centralwidget);
    treeWidget_DataBase->setContextMenuPolicy(Qt::ActionsContextMenu);
    treeWidget_DataBase->headerItem()->setText(0, QString("DataBase"));

    gridLayout->addWidget(treeWidget_Cache, 0, 0, 1, 1);
    gridLayout->addWidget(treeWidget_DataBase, 0, 1, 1, 1);

    menubar = new QMenuBar(this);
    this->setMenuBar(menubar);
    menuFile = new QMenu(menubar);
    contextMenu_Cache = new QMenu(treeWidget_Cache);
    contextMenu_DataBase = new QMenu(treeWidget_DataBase);
    actionOpen = new QAction(this);
    actionSave = new QAction(this);
    actionReset = new QAction(this);
    actionExit = new QAction(this);
    saveAction_Cache = new QAction("save", contextMenu_Cache);
    createAction_Cache = new QAction("create", contextMenu_Cache);
    editAction_Cache = new QAction("edit", contextMenu_Cache);
    deleteAction_Cache = new QAction("delete", contextMenu_Cache);
    downloadAction_DataBase = new QAction("download", contextMenu_DataBase);
    treeWidget_Cache->addAction(saveAction_Cache);
    treeWidget_Cache->addAction(createAction_Cache);
    treeWidget_Cache->addAction(editAction_Cache);
    treeWidget_Cache->addAction(deleteAction_Cache);
    treeWidget_DataBase->addAction(downloadAction_DataBase);
    menubar->addAction(menuFile->menuAction());
    menuFile->addAction(actionOpen);
    menuFile->addAction(actionSave);
    menuFile->addAction(actionReset);
    menuFile->addAction(actionExit);
    actionOpen->setText(QCoreApplication::translate("MainWindow", "Open", nullptr));
    actionSave->setText(QCoreApplication::translate("MainWindow", "Save", nullptr));
    actionExit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
    actionReset->setText(QCoreApplication::translate("MainWindow", "Reset", nullptr));
    menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));

    connect(actionOpen, SIGNAL(triggered(bool)), this, SLOT(onActionOpenTriggered()));
    connect(actionSave, SIGNAL(triggered(bool)), this, SLOT(onActionSaveTriggered()));
    connect(actionReset, SIGNAL(triggered(bool)), this, SLOT(onActionResetTriggered()));
    connect(actionExit, SIGNAL(triggered(bool)), this, SLOT(onActionExitTriggered()));
    connect(treeWidget_DataBase, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(onDataBaseDoubleClicked(QTreeWidgetItem*,int)));
    connect(saveAction_Cache, SIGNAL(triggered(bool)), this, SLOT(onActionSaveTriggered()));
    connect(createAction_Cache, SIGNAL(triggered(bool)), this, SLOT(onActionCreateTriggered()));
    connect(editAction_Cache, SIGNAL(triggered(bool)), this, SLOT(onActionEditTriggered()));
    connect(deleteAction_Cache, SIGNAL(triggered(bool)), this, SLOT(onActionDeleteTriggered()));
    connect(downloadAction_DataBase, SIGNAL(triggered(bool)), this, SLOT(onActionDownloadTriggered()));
    connect(treeWidget_Cache, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(onItemEdit(QTreeWidgetItem*,int)));
}

MainWindow::~MainWindow()
{
}

