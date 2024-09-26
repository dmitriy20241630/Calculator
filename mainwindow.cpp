#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QElapsedTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      watcher(this),
      commaPressed(false),
      firstOperatorStart(true),
      firstOperatorStarted(false),
      secondOperatorStart(false),
      secondOperatorStarted(false),
      action(Action_None),
      exceed(false),
      divByZero(false)
{
    ui->setupUi(this);
    connect(&watcher, SIGNAL(keyDown(bool, int)), this, SLOT(keyDown(bool, int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyDown(bool down, int key)
{
    static QElapsedTimer timer;
    static volatile bool firstTime = true;
    static volatile quint64 msecs = 0;

    if (down)
    {
        if (key == Qt::Key_Minus)
        {
            if (firstTime)
                timer.start();
            msecs = (quint64)timer.elapsed();
        }
    }
    else
    {
        switch (key)
        {
        case Qt::Key_Backspace:
            on_bBackspace_clicked();
            break;

        case Qt::Key_0:
            on_b0_clicked();
            break;

        case Qt::Key_1:
            on_b1_clicked();
            break;

        case Qt::Key_2:
            on_b2_clicked();
            break;

        case Qt::Key_3:
            on_b3_clicked();
            break;

        case Qt::Key_4:
            on_b4_clicked();
            break;

        case Qt::Key_5:
            on_b5_clicked();
            break;

        case Qt::Key_6:
            on_b6_clicked();
            break;

        case Qt::Key_7:
            on_b7_clicked();
            break;

        case Qt::Key_8:
            on_b8_clicked();
            break;

        case Qt::Key_9:
            on_b9_clicked();
            break;

        case Qt::Key_A:
            on_bA_clicked();
            break;

        case Qt::Key_B:
            on_bB_clicked();
            break;

        case Qt::Key_C:
            on_bC_clicked();
            break;

        case Qt::Key_D:
            on_bD_clicked();
            break;

        case Qt::Key_E:
            on_bE_clicked();
            break;

        case Qt::Key_F:
            on_bF_clicked();
            break;

        case Qt::Key_Slash:
            on_bDiv_clicked();
            break;

        case Qt::Key_Asterisk:
            on_bMul_clicked();
            break;

        case Qt::Key_Plus:
            on_bPlus_clicked();
            break;

        case Qt::Key_Minus:
            if (((quint64)timer.elapsed() - msecs) >= 1000)
                on_bMinus1_clicked();
            else
                on_bMinus_clicked();
            break;

        case Qt::Key_Comma:
        case Qt::Key_Period:
            on_bComma_clicked();
            break;

        case Qt::Key_Equal:
            on_bEqual_clicked();
            break;

        case Qt::Key_Escape:
            on_bReset_clicked();
            break;
        }
    }
}

void MainWindow::addChar(char c)
{
    QString t = ui->lineEdit->text();
    char cs[2] = {c, 0};
    bool hasMinus = t[0] == '-';
    if (hasMinus)
        t = t.toStdString().c_str() + 1;
    QStringList l = t.split(',');
    int len = l.length();
    if (len == 1)
    {
        QString s;
        l.append(s);
    }
    if (firstOperatorStart || secondOperatorStart)
    {
        if (commaPressed)
            l[1] = cs;
        else
        {
            l[0] = cs;
            l[1] = "";
        }
        if (secondOperatorStart)
            firstOperatorStart = false;
        if (firstOperatorStart)
        {
            firstOperatorStart = false;
            firstOperatorStarted = true;
        }
        else
        {
            secondOperatorStart = false;
            secondOperatorStarted = true;
        }
    }
    else
    {
        if (commaPressed)
        {
            if (l[1].length() < 8)
                l[1] += cs;
        }
        else
            if (l[0].length() < 24)
                l[0] += cs;
    }
    t = (hasMinus ? "-" : "") + l[0] + "," + l[1];
    ui->lineEdit->setText(t);
}

void MainWindow::doAction(char c)
{
    if (c != '=')
    {
        if (secondOperatorStart || secondOperatorStarted)
        {
            if (!(divByZero || exceed))
            {
                doAction('=');
                if (!(divByZero || exceed))
                    firstOperator = ui->lineEdit->text();
            }
        }
        if (!(divByZero || exceed))
        {
            firstOperator = ui->lineEdit->text();
            secondOperatorStart = true;
            secondOperatorStarted = false;
            commaPressed = false;
            switch (c)
            {
            case '+':
                action = Action_Add;
                ui->label->setText("+");
                break;

            case '-':
                action = Action_Sub;
                ui->label->setText("-");
                break;

            case '*':
                action = Action_Mul;
                ui->label->setText("*");
                break;

            case '/':
                action = Action_Div;
                ui->label->setText("/");
                break;

            default:
                break;
            }
        }
    }
    else
    {
        if (secondOperatorStart || secondOperatorStarted)
        {
            if (!(divByZero || exceed))
            {
                secondOperator = ui->lineEdit->text();
                char *rs;
                int res = performAction(action, firstOperator.toStdString().c_str(), secondOperator.toStdString().c_str(), &rs);
                ui->lineEdit->setText(rs);
                if (res < 0)
                {
                    divByZero = true;
                    ui->label->setText("Error: division by zero");
                }
                if (res > 0)
                {
                    exceed = true;
                    ui->label->setText("Result exceeds digit limit");
                }
                firstOperatorStart = true;
                firstOperatorStarted = secondOperatorStart = secondOperatorStarted = commaPressed = false;
                action = Action_None;
                if (!res)
                    ui->label->setText(" ");
            }
        }
    }
}


void MainWindow::on_bReset_clicked()
{
    divByZero = exceed = firstOperatorStarted = secondOperatorStart = secondOperatorStarted = commaPressed = false;
    firstOperatorStart = true;
    action = Action_None;
    ui->lineEdit->setText("0,");
    ui->label->setText(" ");
}


void MainWindow::on_b0_clicked()
{
    addChar('0');
}


void MainWindow::on_b1_clicked()
{
    addChar('1'); 
}


void MainWindow::on_b2_clicked()
{
    addChar('2');
}


void MainWindow::on_b3_clicked()
{
    addChar('3');
}


void MainWindow::on_b4_clicked()
{
    addChar('4');
}


void MainWindow::on_b5_clicked()
{
    addChar('5');
}


void MainWindow::on_b6_clicked()
{
    addChar('6');
}


void MainWindow::on_b7_clicked()
{
    addChar('7');
}


void MainWindow::on_b8_clicked()
{
    addChar('8');
}


void MainWindow::on_b9_clicked()
{
    addChar('9');
}


void MainWindow::on_bA_clicked()
{
    addChar('A');
}


void MainWindow::on_bB_clicked()
{
    addChar('B');
}


void MainWindow::on_bC_clicked()
{
    addChar('C');
}


void MainWindow::on_bD_clicked()
{
    addChar('D');
}


void MainWindow::on_bE_clicked()
{
    addChar('E');
}


void MainWindow::on_bF_clicked()
{
    addChar('F');
}


void MainWindow::on_bMinus1_clicked()
{
    QString t = ui->lineEdit->text();
    if (t[0] == '-')
        t = t.toStdString().c_str() + 1;
    else
        t = "-" + t;
    ui->lineEdit->setText(t);
}


void MainWindow::on_bBackspace_clicked()
{
    QString t = ui->lineEdit->text();
    if (firstOperatorStart || secondOperatorStart)
    {
        ui->lineEdit->setText("0,");
        return;
    }
    if (t[t.length() - 1] == ',')
    {
        if (commaPressed)
            commaPressed = false;
        else
        {
            if (t[0] == '-')
            {
                if (t.length() == 3)
                {
                    if (t[1] == '0')
                        t = "0,";
                    else
                        t = "-0,";
                }
                else
                {
                    t.resize(t.length() - 2);
                    t += ",";
                }
            }
            else
            {
                if (t.length() == 2)
                    t = "0,";
                else
                {
                    t.resize(t.length() - 2);
                    t += ",";
                }
            }
            ui->lineEdit->setText(t);
        }
    }
    else
    {
        t.resize(t.length() - 1);
        ui->lineEdit->setText(t);
    }
}


void MainWindow::on_bComma_clicked()
{
    commaPressed = true;
}


void MainWindow::on_bDiv_clicked()
{
    doAction('/');
}


void MainWindow::on_bMul_clicked()
{
    doAction('*');
}


void MainWindow::on_bMinus_clicked()
{
    doAction('-');
}


void MainWindow::on_bPlus_clicked()
{
    doAction('+');
}


void MainWindow::on_bEqual_clicked()
{
    doAction('=');
}

