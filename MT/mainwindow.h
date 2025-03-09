#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTimer>
#include <QMap>
#include <QMessageBox>
#include <QColor>
#include <QPropertyAnimation>

#include "dialog.h"
#include "ui_mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnInputString_clicked();
    void on_btnInputAlph_clicked();
    void on_btnPlusString_clicked();
    void on_btnMinusString_clicked();
    void on_btnStart_clicked();
    void on_btnStep_clicked();
    void on_btnPause_clicked();
    void on_btnStop_clicked();
    void on_btnPlusSpeed_clicked();
    void on_btnMinusSpeed_clicked();
    void handleAlphabetsEntered(const QString& newAlph, const QString& newAlphAdd);

private:
    void commandsController(QVector<QString>& commands);
    void updateHighlightedCell(int previousIdx, int newIdx, QStandardItemModel* mod, int row = 0);
    void startActions();
    void reset();
    bool isCellVisible(int column);
    bool checkSymbol(QChar c);
    bool checkAlph();
    bool checkCurrentAdd(QChar c);
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    QStandardItemModel *model2;
    QString str, alph;
    QTimer* timer;
    Dialog *dialog;
    int size, idx, q, speed;
    bool flag, isPaused, isEnd;
};
#endif // MAINWINDOW_H
