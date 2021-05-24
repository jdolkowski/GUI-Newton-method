#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "expr.h"
#include <vector>
#include "solve.h"
#include <iostream>
#include <QApplication>
#include <QProcess>
#include <Windows.h>

#include "common.h"
#include "matrix.h"

#include <cmath>
#include <stdexcept>

bool isFirstClick = true;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);    
}

MainWindow::~MainWindow()
{
    delete ui;
};

void MainWindow::addExp(bool isFirst){
    if(isFirst){
        isFirstClick = false;
        QString fx = ui->plainTextFunctionEdit->toPlainText();
        std::string stringFx = fx.toStdString();
        Expr expFx = Expr::parse(stringFx);

        std::unordered_set<std::string> expSet = expFx.variables();

        int i = 0;
        //ui->initialValuesTable->insertRow(ui->tableWidget_2->rowCount());
        std::unordered_set<std::string> :: iterator itr;
        for (itr = expSet.begin(); itr != expSet.end(); itr++){
            ui->initialValuesTable->insertColumn(i);
            ui->initialValuesTable->setItem(0, i, new QTableWidgetItem(QString::fromStdString(*itr)));
            i++;
        }
    }
}

void MainWindow::on_addButton_clicked()
{
    QString fx = ui->plainTextFunctionEdit->toPlainText();

    ui->tableWidget_2->insertRow ( ui->tableWidget_2->rowCount() );

    ui->tableWidget_2->setItem   ( ui->tableWidget_2->rowCount()-1, 0, new QTableWidgetItem(fx));

    addExp(isFirstClick);
    ui->plainTextFunctionEdit->clear();
}

void MainWindow::on_solveButton_clicked()
{
    QTableWidgetItem* item_func;
    QString tmp;
    Expr func;
    std::string stringFunction;
    QTableWidgetItem* item_init;
    std::vector<Expr> funcs;
    std::vector<Binding> inits;
    int min_iterations = ui->minIterationsSpinBox->value();
    int max_iterations = ui->maxIterationsSpinBox->value();
    double abs_epsilon = ui->absSpinBox->value()*pow(10,-16);
    double rel_epsilon = ui->relSpinBox->value()*pow(10,-16);
    int functionTableHeight = ui->tableWidget_2->rowCount();
    int expTableWidth = ui->initialValuesTable->columnCount();

    for(int i = 0; i < functionTableHeight; i++){
        item_func = ui->tableWidget_2->item(i,0);
        tmp = item_func->text();
        stringFunction = tmp.toStdString();
        func = Expr::parse(stringFunction);
        std::cout<<func.show()<<std::endl;
        funcs.push_back(func);
    }

    std::string stringInit;
    std::string::size_type sz;     // alias of size_t
    double value;

    for(int i = 0; i < expTableWidth; i++){
        item_init = ui->initialValuesTable->item(1,i);
        tmp = item_init->text();
        stringInit = tmp.toStdString();
        value = std::stod (stringInit,&sz);
        Binding pair;
        pair.first = ui->initialValuesTable->item(0,i)->text().toStdString();
        pair.second = value;
        inits.push_back(pair);
    }

    struct Constraints constr;
    constr.min_iters = min_iterations;
    constr.max_iters = max_iterations;
    constr.rel_epsilon = rel_epsilon;
    constr.abs_epsilon = abs_epsilon;
    isFirstClick = true;

    try{
        Solution solution = solve(funcs,inits,constr);
        ui->Iterations->setStyleSheet("font-weight: bold; color: red");
        ui->Max_difference->setStyleSheet("font-weight: bold; color: red");
        ui->maxDiffAns->setStyleSheet("font-weight: bold; color: red");
        ui->iterationsAns->setStyleSheet("font-weight: bold; color: red");

        ui->tableAns->insertRow ( ui->tableAns->rowCount() );
        ui->tableAns->insertRow ( ui->tableAns->rowCount() );
        for(unsigned int i = 0; i < solution.vars.size(); i++){
            ui->tableAns->insertColumn(i);
            ui->tableAns->setItem(0,i,new QTableWidgetItem(QString::fromStdString(solution.vars[i].first)));
            ui->tableAns->setItem(1,i, new QTableWidgetItem(QString::number(solution.vars[i].second)));
        }

        ui->maxDiffAns->setText(QString::number(solution.max_diff));
        ui->iterationsAns->setText(QString::number(solution.iters));


    }catch(...){
        ui->errorLabel->setStyleSheet("font-weight: bold; color: red");
        ui->errorLabel->setText("error");
//        int counter = 3; //amount of seconds
//        Sleep(1000);
//        while (counter >= 1)
//        {
//            Sleep(1000);
//            counter--;
//        }
//        on_pushButton_clicked();
    }

}

void MainWindow::on_pushButton_clicked()
{
    qApp->quit();
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}
