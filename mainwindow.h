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
    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionReset;
    QAction *actionExit;
    QAction *createAction_Cache;
    QAction *editAction_Cache;
    QAction *deleteAction_Cache;

bool getWhereParentIsNull();
bool getChildren(QTreeWidgetItem* item);

public slots:
    void onActionOpenTriggered();
    void onActionSaveTriggered();
    void onActionResetTriggered();
    void onActionExitTriggered();
    void onDataBaseDoubleClicked(QTreeWidgetItem* item, int column);
    void onActionCreateTriggered();
    void onActionEditTriggered();
    void onActionDeleteTriggered();
    void onItemEdit(QTreeWidgetItem* item,int column);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H
