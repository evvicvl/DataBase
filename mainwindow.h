#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCoreApplication>
#include <QStandardItemModel>
#include <QAbstractItemModel>
#include <QMainWindow>
#include <QGridLayout>
#include <QFileDialog>
#include <QTreeView>
#include <QTreeWidget>
#include <QMenuBar>
#include <QAction>
#include <QDialog>
#include <QStringList>
#include <QMenu>
#include <QtSql>

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QSqlDatabase dataBase;
    QString stringQuery;
    QStringList stringListQuery;

    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QList<QTreeWidgetItem*> *qListItem_Cache;
    QTreeWidget *treeWidget_Cache;
    QList<QTreeWidgetItem*> *qListItem_DataBase;
    QTreeWidget *treeWidget_DataBase;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu* contextMenu_Cache;
    QMenu* contextMenu_DataBase;
    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionReset;
    QAction *actionExit;
    QAction *saveAction_Cache;
    QAction *createAction_Cache;
    QAction *editAction_Cache;
    QAction *deleteAction_Cache;
    QAction *downloadAction_DataBase;

void loadWhereParentIsNull();
void loadChildren(QTreeWidgetItem* item);
void putItem(QList<QTreeWidgetItem*> *listItem, QTreeWidgetItem* item, QTreeWidgetItem *parent);
void putItem(QList<QTreeWidgetItem *> *listItem, QTreeWidgetItem* item, QTreeWidget *parent);

public slots:
    void onActionOpenTriggered();
    void onActionSaveTriggered();
    void onActionResetTriggered();
    void onActionExitTriggered();
    void onDataBaseDoubleClicked(QTreeWidgetItem* item, int column);
    void onActionCreateTriggered();
    void onActionEditTriggered();
    void onActionDeleteTriggered();
    void onActionDownloadTriggered();
    void onItemEdit(QTreeWidgetItem* item,int column);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H
