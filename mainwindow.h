#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QCoreApplication>
#include <QStandardItem>
#include <QGridLayout>
#include <QMainWindow>
#include <QMessageBox>
#include <QTreeView>
#include <QMenuBar>
#include <QAction>
#include <QWidget>
#include <QMenu>
#include <QtSql>
#include <QFile>
#include <QDir>

#include <QDebug>

class MainWindow : public QMainWindow
{
    Q_OBJECT

    int maxId;

    QSqlDatabase dataBase;

    QWidget *centralwidget;
    QGridLayout *gridLayout;

    QTreeView *treeView_Cache;
    QStandardItem *rootItem_Cache;
    QStandardItemModel *model_Cache;

    QTreeView *treeView_DataBase;
    QStandardItem *rootItem_DataBase;
    QStandardItemModel *model_DataBase;

    QMenuBar *menubar;
    QMenu *menuFile;
    QAction *actionReset;
    QAction *actionExit;

    QMenu* contextMenu_Cache;
    QMenu* contextMenu_DataBase;
    QAction *actionCreate_Cache;
    QAction *actionEdit_Cache;
    QAction *actionDelete_Cache;
    QAction *actionDownload_DataBase;

    void insertChild(QAbstractItemModel *model, const QModelIndex index, QStringList data);
    QStandardItem* insertItem(QStandardItem *parentItem, QList<QVariant> data);
    QStandardItem* insertItem(QStandardItem *parentItem, QStandardItem *item);
    void readChildrensFromDB(QStandardItem *parent, int id);
    void deleteItem(QStandardItem *item);

public slots:
    void loadFromDB();
    void onActionDownloadTriggered();
    void onActionCreateTriggered();
    void onActionEditTriggered();
    void onActionDeleteTriggered();
    void onItemChanged(QStandardItem *item);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H
