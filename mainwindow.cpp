#include "mainwindow.h"

bool MainWindow::getWhereParentIsNull()
{
    QSqlQuery sqlQuery(dataBase);
    stringQuery = "SELECT * FROM \"tree\" WHERE parent_id IS NULL";
    if(sqlQuery.exec(stringQuery))
    {
        if(sqlQuery.next())
        {
            qListItem_Cache->push_back(new QTreeWidgetItem(treeWidget_Cache));
            qListItem_Cache->back()->setText(0, sqlQuery.value(2).toString());
            qListItem_Cache->back()->setText(1, sqlQuery.value(0).toString());
            treeWidget_Cache->addTopLevelItem(qListItem_Cache->back());

            qListItem_DataBase->push_back(new QTreeWidgetItem(treeWidget_DataBase));
            qListItem_DataBase->back()->setText(0, "> " + sqlQuery.value(2).toString());
            qListItem_DataBase->back()->setText(1, sqlQuery.value(0).toString());
            treeWidget_DataBase->addTopLevelItem(qListItem_DataBase->back());
        }
        return true;
    }
    return false;
}

bool MainWindow::getChildren(QTreeWidgetItem* item)
{
    QSqlQuery sqlQuery(dataBase);
    stringQuery = QString("SELECT * FROM \"tree\" WHERE parent_id = %1  AND removed = 0").arg(item->text(1));
    if(sqlQuery.exec(stringQuery))
    {
        while(item->childCount() > 0)
            item->removeChild(item->child(0));

        for(int i = 0; i < qListItem_Cache->count(); i++)
        {
            if(qListItem_Cache->at(i)->text(1) == item->text(1))
            {
                while(qListItem_Cache->at(i)->childCount())
                    qListItem_Cache->at(i)->removeChild(qListItem_Cache->at(i)->child(0));
            }
        }

        while(sqlQuery.next())
        {
            qListItem_DataBase->push_back(new QTreeWidgetItem(item));
            QSqlQuery sqlQuery_(dataBase);
            if( sqlQuery_.exec(QString("SELECT * FROM \"tree\" WHERE parent_id = %1  AND removed = 0").arg(sqlQuery.value(0).toString()))
                && sqlQuery_.next())
            {
                qListItem_DataBase->back()->setText(0, "> " + sqlQuery.value(2).toString());
            }
            else
            {
                qListItem_DataBase->back()->setText(0, sqlQuery.value(2).toString());
            }
            qListItem_DataBase->back()->setText(1, sqlQuery.value(0).toString());

            for(int i = 0; i < qListItem_Cache->count(); i++)
            {
                if(qListItem_Cache->at(i)->text(1) == item->text(1))
                {
                    qListItem_Cache->push_back(new QTreeWidgetItem(qListItem_Cache->at(i)));
                    qListItem_Cache->back()->setText(0, sqlQuery.value(2).toString());
                    qListItem_Cache->back()->setText(1, sqlQuery.value(0).toString());
                }
            }
        }
        return true;
    }
    return false;
}

void MainWindow::onActionOpenTriggered()
{
    QString fName = QFileDialog::getOpenFileName();
    if(!fName.isEmpty())
    {
        onActionResetTriggered();
        dataBase.setDatabaseName(fName);
        dataBase.open();
        getWhereParentIsNull();
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
    treeWidget_Cache->clear();
    treeWidget_DataBase->clear();
    qListItem_Cache->clear();
    qListItem_DataBase->clear();
    getWhereParentIsNull();
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
    getChildren(item);
}

void MainWindow::onActionCreateTriggered()
{
    qListItem_Cache->push_back(new QTreeWidgetItem(treeWidget_Cache->currentItem()));
    qListItem_Cache->back()->setFlags(qListItem_Cache->back()->flags() | Qt::ItemIsEditable);
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
    QTreeWidgetItem *pp = nullptr;
    pp = treeWidget_Cache->currentItem()->parent();
    pp->removeChild(treeWidget_Cache->currentItem());
}

void MainWindow::onItemEdit(QTreeWidgetItem *item, int column)
{
    if( item->flags().testFlag(Qt::ItemIsEditable))
    {
        if(item->text(1).isEmpty() && !item->text(0).isEmpty())
        {
            stringListQuery.push_back(QString("INSERT INTO \"tree\" (parent_id, data) VALUES (%1, '%2')").arg(item->parent()->text(1), item->text(0)));
        }
        else
        {
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
    treeWidget_DataBase->headerItem()->setText(0, QString("DataBase"));

    gridLayout->addWidget(treeWidget_Cache, 0, 0, 1, 1);
    gridLayout->addWidget(treeWidget_DataBase, 0, 1, 1, 1);

    menubar = new QMenuBar(this);
    this->setMenuBar(menubar);
    menuFile = new QMenu(menubar);
    contextMenu_Cache = new QMenu(treeWidget_Cache);
    actionOpen = new QAction(this);
    actionSave = new QAction(this);
    actionReset = new QAction(this);
    actionExit = new QAction(this);
    createAction_Cache = new QAction("create", contextMenu_Cache);
    editAction_Cache = new QAction("edit", contextMenu_Cache);
    deleteAction_Cache = new QAction("delete", contextMenu_Cache);
    treeWidget_Cache->addAction(createAction_Cache);
    treeWidget_Cache->addAction(editAction_Cache);
    treeWidget_Cache->addAction(deleteAction_Cache);
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
    connect(createAction_Cache, SIGNAL(triggered(bool)), this, SLOT(onActionCreateTriggered()));
    connect(editAction_Cache, SIGNAL(triggered(bool)), this, SLOT(onActionEditTriggered()));
    connect(deleteAction_Cache, SIGNAL(triggered(bool)), this, SLOT(onActionDeleteTriggered()));
    connect(treeWidget_Cache, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(onItemEdit(QTreeWidgetItem*,int)));
}

MainWindow::~MainWindow()
{
}

