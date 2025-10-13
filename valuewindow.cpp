#include "valuewindow.h"
#include "ui_valuewindow.h"
#include <vector>
#include <algorithm>
#include <mainwindow.h>
#include <string>
#include <QMessageBox>

using std::vector;
using std::string;

ValueWindow::ValueWindow(QWidget *parent, int val) :
    QDialog(parent),
    ui(new Ui::ValueWindow)
{
    ui->setupUi(this);
    this->CreateUI(val);
    //ui->label->setText(QString::number(val));
}

ValueWindow::~ValueWindow()
{
    delete ui;
}

vector<int> p;
int number;

void ValueWindow::CreateUI(int val) {
    number = val;
    ui->table->setColumnCount(2);
    ui->table->setShowGrid(true);
    ui->table->verticalHeader()->hide();
    ui->table->horizontalHeader()->hide();
    NodeBase* el = base_begin;
    for (int i = 0; i < val - 1; ++i) {
        el = el->next;
    }
    for (NodePolynomial* t = el->data; t != nullptr; t = t->next) {
        for (int i = 0; i < 26; ++i) {
            if (t->power[i] != 0) {
                p.push_back(i);
            }
        }
    }
    sort(p.begin(), p.end());
    p.resize(unique(p.begin(), p.end()) - p.begin());
    for (int i = 0; i < p.size(); ++i) {
        ui->table->insertRow(i);
        string tmp = "";
        tmp += ('a' + p[i]);
        ui->table->setItem(i,0, new QTableWidgetItem(QString(tmp.c_str())));
        ui->table->setItem(i, 1, new QTableWidgetItem);
    }
}

void ValueWindow::check_table(vector<double>& res) {
    //vector<int> res(26, 0);
    for (int i = 0; i  < p.size(); ++i) {
        QString str = ui->table->item(i, 1)->text();
        /*if (str.size() == 0) {
            throw QString("Not a number in cell " + QString::number(i + 1));
        }
        for (int j = 0; j < str.size(); ++j) {
            if (str[j] > '9' || str[j] < '0') {
                throw QString("Not a number in cell " + QString::number(i + 1));
            }
        }*/
        /*try {

        } catch (std::invalid_argument) {
            throw QString("Not a number in cell " + QString::number(i + 1));
        }*/
        bool ok;
        res[p[i]] = str.toDouble(&ok);
        if (!ok) {
            throw QString("Not a number in cell " + QString::number(i + 1));
        }
    }
    return;
}

void ValueWindow::on_button_count_clicked()
{
    try {
        vector<double> val(26, 0);
        this->check_table(val);
        NodeBase* L = base_begin;
        for (int i = 0; i < number - 1; ++i) {
            L = L->next;
        }
        double ans = 0;
        for (auto el = L->data; el != nullptr; el = el->next) {
            double r = el->a;
            for (int i = 0; i < 26; ++i) {
                if (el->power[i] != 0) {
                    r *= pow(val[i], el->power[i]);
                }
            }
            ans += r;
        }
        QMessageBox::information(this, " ", "Result is " + QString::number(ans));
    } catch (const QString& error) {
        QMessageBox::critical(this, " ", error);
    }
}
