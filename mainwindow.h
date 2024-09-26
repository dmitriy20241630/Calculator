#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "eventwatcher.h"
#include "functions.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Ui::MainWindow *ui;
    EventWatcher watcher;
    volatile bool commaPressed;
    volatile bool firstOperatorStart;
    volatile bool firstOperatorStarted;
    volatile bool secondOperatorStart;
    volatile bool secondOperatorStarted;
    volatile EAction action;
    QString firstOperator;
    QString secondOperator;
    volatile bool exceed;
    volatile bool divByZero;

    void addChar(char);
    void doAction(char);

public:
    MainWindow(QWidget* = nullptr);
    ~MainWindow();

private slots:
    void keyDown(bool, int);
    void on_bReset_clicked();
    void on_b0_clicked();
    void on_b1_clicked();
    void on_b2_clicked();
    void on_b3_clicked();
    void on_b4_clicked();
    void on_b5_clicked();
    void on_b6_clicked();
    void on_b7_clicked();
    void on_b8_clicked();
    void on_b9_clicked();
    void on_bA_clicked();
    void on_bB_clicked();
    void on_bC_clicked();
    void on_bD_clicked();
    void on_bE_clicked();
    void on_bF_clicked();
    void on_bMinus1_clicked();
    void on_bBackspace_clicked();
    void on_bComma_clicked();
    void on_bDiv_clicked();
    void on_bMul_clicked();
    void on_bMinus_clicked();
    void on_bPlus_clicked();
    void on_bEqual_clicked();
};
#endif // MAINWINDOW_H
