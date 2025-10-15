#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <vector>
#include <map>
#include <QMessageBox>
#include <QDebug>
#include <string>
#include <cmath>
#include <valuewindow.h>
#include <QFile>
#include <QString>

using std::vector;
using std::string;

// Test for git

NodePolynomial::NodePolynomial(): a(0), power(26, 0), next(nullptr) {}

bool operator== (NodePolynomial& lhs, NodePolynomial& rhs) {
    for (int i = 0; i < 26; ++i) {
        if (lhs.power[i] != rhs.power[i]) {
            return false;
        }
    }
    return true;
}

bool operator!= (NodePolynomial& lhs, NodePolynomial& rhs) {
    return !(lhs == rhs);
}

bool operator< (NodePolynomial& lhs, NodePolynomial& rhs) {
    for (int i = 0; i < 26; ++i) {
        if (lhs.power[i] < rhs.power[i]) {
            return true;
        } else if (lhs.power[i] > rhs.power[i]) {
            return false;
        }
    }
    return false;
}

NodePolynomial operator* (NodePolynomial& lhs, NodePolynomial& rhs) {
    NodePolynomial res;
    res.a = lhs.a * rhs.a;
    for (int i = 0; i < 26; ++i) {
        res.power[i] = lhs.power[i] + rhs.power[i];
    }
    return res;
}

/*bool operator< (const NodePolynomial& lhs, const NodePolynomial& rhs) {
    for (int i = 0; i < 26; ++i) {
        if (lhs.power[i] < rhs.power[i]) {
            return true;
        } else if (lhs.power[i] > rhs.power[i]) {
            return false;
        }
    }
    return false;
}

bool operator<= (NodePolynomial& lhs, NodePolynomial& rhs) {
    return (lhs < rhs) || (lhs == rhs);
}*/

void print_node(NodePolynomial* node) {
    qDebug() << node->a;
    for (int i = 0; i < 26; ++i) {
        if (node->power[i] != 0) {
            string tmp = "";
            tmp += ('a' + i);
            qDebug() << tmp << " " << node->power[i];
        }
    }
}

void print_polynomial(NodePolynomial* L) {
    qDebug() << "print begin";
    for (NodePolynomial* el = L; el != nullptr; el = el->next) {
        // qDebug() << "step";
        print_node(el);
    }
    qDebug() << "print complete";
}

NodeBase::NodeBase(): data(nullptr), next(nullptr) {}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->spinbox_addition_first->setMinimum(1);
    ui->spinbox_addition_second->setMinimum(1);
    ui->spinbox_delete->setMinimum(1);
    ui->spinbox_derivative_number->setMinimum(1);
    ui->spinbox_derivative_polynomial->setMinimum(1);
    ui->spinbox_division_first->setMinimum(1);
    ui->spinbox_division_second->setMinimum(1);
    ui->spinbox_equality_first->setMinimum(1);
    ui->spinbox_equality_second->setMinimum(1);
    ui->spinbox_find_root->setMinimum(1);
    ui->spinbox_multiplication_first->setMinimum(1);
    ui->spinbox_multiplication_second->setMinimum(1);
    ui->spinbox_value->setMinimum(1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

NodeBase* base_begin = nullptr;
int base_size = 0;
double eps = 1e-8;

void delete_polynomial(NodePolynomial*& L) {
    for (NodePolynomial* el = L; el != nullptr; ) {
        NodePolynomial* el_tmp = el;
        el = el->next;
        delete el_tmp;
        el_tmp = nullptr;
    }
    L = nullptr;
    return;
}

void delete_base(NodeBase* L) {
    for (NodeBase* el = L; el != nullptr; ) {
        NodeBase* el_tmp = el;
        el = el->next;
        delete_polynomial(el_tmp->data);
        el_tmp = nullptr;
    }
    L = nullptr;
    return;
}

void insertion(NodePolynomial*& L, NodePolynomial* cur_node) {
    // qDebug() << "insertion";
    NodePolynomial* prev = L;
    NodePolynomial* el = L;
    while (el != nullptr && *cur_node < *el) {
        prev = el;
        el = el->next;
    }
    if (el == nullptr) {
        if (L == nullptr) {
            L = new NodePolynomial;
            L->a = cur_node->a;
            L->power = cur_node->power;
            L->next = nullptr;
        } else {
            prev->next = new NodePolynomial;
            prev = prev->next;
            prev->a = cur_node->a;
            prev->power = cur_node->power;
            prev->next = nullptr;
        }
    } else if (*el == *cur_node) {
        el->a += cur_node->a;
    } else {
        if (prev == el) {
            L = new NodePolynomial;
            L->a = cur_node->a;
            L->power = cur_node->power;
            L->next = el;
        } else {
            prev->next = new NodePolynomial;
            prev = prev->next;
            prev->a = cur_node->a;
            prev->power = cur_node->power;
            prev->next = el;
        }
    }
    // qDebug() << "insertion complete";
}

void MainWindow::check_str(QString str, NodePolynomial*& L) {
    for (int i = 0; i < str.size(); ++i) {
        if (str[i] == '#') {
            throw QString("Error symbol # in position " + QString::number(i + 1));
        }
    }
    str += QString('#');
    // vector<vector<int>> table = {{0, 1, -1}, {-2, 1, -1}};
    vector<vector<int>> table = { {1, 2, 7, -2, -2, 0, -1},
                                  {-2, 2, 7, -2, -2, 1, -2},
                                  {1, 2, 7, -2, 3, 6, -1},
                                  {-2, 4, -2, -2, -2, -2, -2},
                                  {1, 4, 7, -2, -2, 5, -1},

                                  {1, -2, 7, -2, -2, 5, -1},
                                  {1, -2, 7, -2, -2, 6, -1},
                                  {1, -2, 7, 8, -2, 7, -1},
                                  {-2, 9, -2, -2, -2, 8, -2},
                                  {1, 9, 7, -2, -2, 10, -1},

                                  {1, -2, 7, -2, -2, 10, -1}};
    int cur_state = 0;

    /*for (int i = 0; i < str.size(); ++i) {
        if (str[i] == '+') {
            cur_state = table[cur_state][0];
        } else if ('0' <= str[i] && str[i] <= '9') {
            cur_state = table[cur_state][1];
        } else if (str[i] == '#') {
            cur_state = table[cur_state][2];
        } else {
            throw QString("Error symbol " + QString(str[i]));
        }
        if (cur_state == -2) {
            throw QString("Error " + QString(str[i]));
        }
    }
    if (cur_state != -1) {
        throw QString("aaaaaaa");
    }*/
    // NodePolynomial* L = nullptr;
    NodePolynomial* cur_node = new NodePolynomial;
    int c = 0;
    bool sign = false;
    QString p;
    int cur_variable;
    for (int i = 0; i < str.size(); ++i) {
        if (str[i] == '+' || str[i] == '-') {
            //if (cur_state)
            if (cur_state == 2 || cur_state == 6 || cur_state == 4 || cur_state == 5) {
                cur_node->a = p.toDouble() * (sign ? -1 : 1);
                /*sign = (str[i] == '-');
                c = 0;
                p = "";*/
            } else if (cur_state == 0) {

            } else if (cur_state == 9 || cur_state == 10) {
                cur_node->power[cur_variable] += p.toInt();
            } else if (cur_state == 7) {
                cur_node->power[cur_variable] += 1;
            }
            cur_state = table[cur_state][0];
            sign = (str[i] == '-');
            c = 0;
            p = "";
            print_node(cur_node);
            insertion(L, cur_node);
            print_polynomial(L);
            delete_polynomial(cur_node);
            cur_node = new NodePolynomial;
            // вставить одночлен в список
        } else if ('0' <= str[i] && str[i] <= '9') {
            cur_state = table[cur_state][1];
            //c = c * 10 + (str[i] - '0');
            p = p + QString(str[i]);
        } else if ('a' <= str[i] && str[i] <= 'z') {
            if (cur_state == 0) {
                cur_node->a = 1;
            } else if (cur_state == 1) {
                cur_node->a = (sign ? -1 : 1);
            } else if (cur_state == 2 || cur_state == 6 || cur_state == 4 || cur_state == 5) {
                cur_node->a = p.toDouble() * (sign ? -1 : 1);
                qDebug() << cur_node->a;
            } else if (cur_state == 7) {
                cur_node->power[cur_variable] += 1;
            } else if (cur_state == 9 || cur_state == 10) {
                cur_node->power[cur_variable] += p.toInt();
            }
            cur_variable = str[i].unicode() - QChar('a').unicode();
            cur_state = table[cur_state][2];
            p = "";
            /*if (cur_state == 9 || cur_state == 10) {
                cur_node->power[cur_variable] = p.toInt();
                p = "";
                cur_state = table[cur_state][2];
            } else {
                cur_state = table[cur_state][2];
                if (p == "") {
                    cur_node->a = 1 * (sign ? -1 : 1);
                } else {
                    double tmp = p.toDouble();
                    cur_node->a = tmp * (sign ? -1 : 1);
                }
                cur_variable = str[i].unicode() - QChar('a').unicode();
            }*/
        } else if (str[i] == '^') {
            cur_state = table[cur_state][3];
        } else if (str[i] == '.') {
            cur_state = table[cur_state][4];
            p += ".";
        } else if (str[i] == ' ') {
            cur_state = table[cur_state][5];
        } else if (str[i] == '#') {
            if (cur_state == 2 || cur_state == 6 || cur_state == 4 || cur_state == 5) {
                cur_node->a = p.toDouble() * (sign ? -1 : 1);
            } else if (cur_state == 7) {
                cur_node->power[cur_variable] += 1;
            } else if (cur_state == 9 || cur_state == 10) {
                cur_node->power[cur_variable] += p.toInt();
            }
            cur_state = table[cur_state][6];
        } else {
            delete_polynomial(L);
            delete_polynomial(cur_node);
            throw QString("Error symbol " + QString(str[i]) + " in position " + QString::number(i + 1));
        }
        if (cur_state == -2 && i != str.size() - 1) {
            delete_polynomial(L);
            delete_polynomial(cur_node);
            throw QString("Unexpected symbol " + QString(str[i]) + " in position " + QString::number(i + 1));
        }
        qDebug() << str[i] << " " << cur_state;
    }
    if (cur_state != -1) {
        delete_polynomial(L);
        delete_polynomial(cur_node);
        throw QString("Incorrect finish of string");
    }
    print_node(cur_node);
    insertion(L, cur_node);
    print_polynomial(L);
}

void delete_nulls(NodePolynomial*& el) {
    if (el == nullptr) {
        return;
    }
    if (abs(el->a) < eps) {
        NodePolynomial* el_tmp = el;
        el = el->next;
        delete el_tmp;
        el_tmp = nullptr;
        delete_nulls(el);
        return;
    }
    delete_nulls(el->next);
    return;
}

QString polynomial_string(NodePolynomial* L) {
    QString res = "";
    for (NodePolynomial* el = L; el != nullptr; el = el->next) {
        if (el->a < 0) {
            if (res == "") {
                res += "- ";
            } else {
                res += " - ";
            }
        } else {
            if (res != "") {
                res += " + ";
            }
        }
        if (abs(el->a) != 1 || el->power == vector<int>(26, 0)) {
            res += QString::number(abs(el->a), 'g', 15);
        }
        for (int i = 0; i < 26; ++i) {
            if (el->power[i] != 0) {
                string tmp = "";
                tmp += ('a' + i);
                res += tmp;
                if (el->power[i] != 1) {
                    res += "^";
                    res += QString::number(el->power[i]);
                }
            }
        }
    }
    return res;
}

void MainWindow::base_display() {
    int cnt = 1;
    ui->listWidget->clear();
    for (NodeBase* el = base_begin; el != nullptr; el = el->next) {
        QString p = polynomial_string(el->data);
        ui->listWidget->addItem(QString::number(cnt) + ")    " + p);
        ++cnt;
    }
}

void insertion_base(NodePolynomial*& L) {
    NodeBase* el = base_begin;
    NodeBase* prev = base_begin;
    while (el != nullptr) {
        prev = el;
        el = el->next;
    }
    if (base_begin == nullptr) {
        base_begin = new NodeBase;
        base_begin->data = L;
        base_begin->next = nullptr;
    } else {
        prev->next = new NodeBase;
        prev = prev->next;
        prev->data = L;
        prev->next = nullptr;
    }
    qDebug() << "insert to base complete";
}

void MainWindow::on_button_add_clicked()
{
    QString input_str = ui->lineedit_main->text();
    try {
        NodePolynomial* L = nullptr;
        this->check_str(input_str, L);
        print_polynomial(L);
        delete_nulls(L);
        if (L == nullptr) {
            L = new NodePolynomial;
            L->a = 0;
            L->next = nullptr;
        }
        print_polynomial(L);
        insertion_base(L);
        this->base_display();
        ++base_size;
    } catch (const QString& error) {
        QMessageBox::critical(this, " ", error);
    }
}

void MainWindow::on_button_delete_clicked()
{
    if (ui->spinbox_delete->value() > base_size) {
        QMessageBox::critical(this, " ", "Incorrect number");
    } else {
        NodeBase* prev = base_begin;
        NodeBase* el = base_begin;
        for (int i = 0; i < ui->spinbox_delete->value() - 1; ++i) {
            prev = el;
            el = el->next;
        }
        if (el == base_begin) {
            base_begin = base_begin->next;
            delete el;
        } else {
            prev->next = el->next;
            delete_polynomial(el->data);
            delete el;
        }
        this->base_display();
        --base_size;
    }
}

void MainWindow::on_button_equality_clicked()
{
    if (ui->spinbox_equality_first->value() > base_size || ui->spinbox_equality_second->value() > base_size) {
        QMessageBox::critical(this, " ", "Incorrect number");
    } else {
        NodeBase* first = base_begin;
        NodeBase* second = base_begin;
        for (int i = 0; i < ui->spinbox_equality_first->value() - 1; ++i) {
            first = first->next;
        }
        for (int i = 0; i < ui->spinbox_equality_second->value() - 1; ++i) {
            second = second->next;
        }
        NodePolynomial* el1 = first->data;
        NodePolynomial* el2 = second->data;
        bool flag_res = true;
        for(; el1 != nullptr && el2 != nullptr; el1 = el1->next, el2 = el2->next) {
            if (*el1 != *el2 || el1->a != el2->a) {
                flag_res = false;
                break;
            }
        }
        if (flag_res) {
            QMessageBox::information(this, " ", "Polynomials are equal");
        } else {
            QMessageBox::information(this, " ", "Polynomials aren't equal");
        }
    }
}

NodePolynomial* addition(NodePolynomial* el1, NodePolynomial* el2) {
    NodePolynomial* L = nullptr;
    for (; el1 != nullptr; el1 = el1->next) {
        insertion(L, el1);
    }
    for (; el2 != nullptr; el2 = el2->next) {
        insertion(L, el2);
    }
    delete_nulls(L);
    if (L == nullptr) {
        L = new NodePolynomial;
        L->a = 0;
        L->next = nullptr;
    }
    return L;
}

void MainWindow::on_button_addition_clicked()
{
    if (ui->spinbox_addition_first->value() > base_size || ui->spinbox_addition_second->value() > base_size) {
        QMessageBox::critical(this, " ", "Incorrect number");
    } else {
        NodeBase* first = base_begin;
        NodeBase* second = base_begin;
        for (int i = 0; i < ui->spinbox_addition_first->value() - 1; ++i) {
            first = first->next;
        }
        for (int i = 0; i < ui->spinbox_addition_second->value() - 1; ++i) {
            second = second->next;
        }
        NodePolynomial* el1 = first->data;
        NodePolynomial* el2 = second->data;
        //print_polynomial(el1);
        //print_polynomial(el2);
        NodePolynomial* L = addition(el1, el2);
        QMessageBox msgBox;
        msgBox.setText("Result: " + polynomial_string(L));
        // msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch (ret) {
          case QMessageBox::Save:
              insertion_base(L);
              this->base_display();
              ++base_size;
              break;
          case QMessageBox::Cancel:
              break;
          default:
              break;
        }
    }
}

NodePolynomial* multiplication(NodePolynomial* el1, NodePolynomial* el2) {
    NodePolynomial* L = nullptr;
    for (; el1 != nullptr; el1 = el1->next) {
        NodePolynomial* el_tmp = el2;
        for (; el_tmp != nullptr; el_tmp = el_tmp->next) {
            NodePolynomial node = *el1 * *el_tmp;
            NodePolynomial* new_node = new NodePolynomial;
            new_node->a = node.a;
            new_node->power = node.power;
            insertion(L, new_node);
        }
    }
    delete_nulls(L);
    if (L == nullptr) {
        L = new NodePolynomial;
        L->a = 0;
        L->next = nullptr;
    }
    return L;
}

void MainWindow::on_button_multiplication_clicked()
{
    if (ui->spinbox_multiplication_first->value() > base_size || ui->spinbox_multiplication_second->value() > base_size) {
        QMessageBox::critical(this, " ", "Incorrect number");
    } else {
        NodeBase* first = base_begin;
        NodeBase* second = base_begin;
        for (int i = 0; i < ui->spinbox_multiplication_first->value() - 1; ++i) {
            first = first->next;
        }
        for (int i = 0; i < ui->spinbox_multiplication_second->value() - 1; ++i) {
            second = second->next;
        }
        NodePolynomial* el1 = first->data;
        NodePolynomial* el2 = second->data;
        print_polynomial(el1);
        print_polynomial(el2);
        NodePolynomial* L = multiplication(el1, el2);
        QMessageBox msgBox;
        msgBox.setText("Result: " + polynomial_string(L));
        // msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch (ret) {
          case QMessageBox::Save:
              insertion_base(L);
              this->base_display();
              ++base_size;
              break;
          case QMessageBox::Cancel:
              break;
          default:
              break;
        }
    }
}

void MainWindow::on_button_value_clicked()
{
    if (ui->spinbox_value->value() > base_size) {
        QMessageBox::critical(this, " ", "Incorrect number");
    } else {
        /*NodeBase* el = base_begin;
        for (int i = 0; i < ui->spinbox_value->value() - 1; ++i) {
            el = el->next;
        }*/
        int val = ui->spinbox_value->value();
        ValueWindow window(this, val);
        window.setModal(true);
        window.exec();
    }
}

int check_one_variable(NodePolynomial* node) {
    int res = -2;
    for (auto el = node; el != nullptr; el = el->next) {
        for (int i = 0; i < 26; ++i) {
            if (el->power[i] > 0) {
                if (res == -2 || res == i) {
                    res = i;
                } else {
                    return -1;
                }
                // qDebug() << i;
            }
        }
    }
    return res;
}

void MainWindow::on_button_derivative_clicked()
{
    if (ui->spinbox_derivative_polynomial->value() > base_size) {
        QMessageBox::critical(this, " ", "Incorrect number");
        return;
    }
    if ((ui->lineedit_derivative_variable->text()).size() > 1) {
        QMessageBox::critical(this, " ", "Incorrect variable");
        return;
    }
    NodeBase* first = base_begin;
    for (int i = 0; i < ui->spinbox_derivative_polynomial->value() - 1; ++i) {
        first = first->next;
    }
    NodePolynomial* polynom = first->data;
    NodePolynomial* p = nullptr;
    NodePolynomial* res = new NodePolynomial;
    for (auto tmp = res, el = polynom; el != nullptr; el = el->next) {
        tmp->a = el->a;
        tmp->power = el->power;
        if (el->next != nullptr) {
            tmp->next = new NodePolynomial;
            tmp = tmp->next;
        }
    }
    QString var = ui->lineedit_derivative_variable->text();
    int variable = -1;
    if (var == "") {
        variable = check_one_variable(polynom);
        qDebug() << variable;
        if (variable == -1) {
            QMessageBox::critical(this, " ", "Polynomial contains several variables");
            return;
        }
        if (variable == -2) {
            NodePolynomial* L = new NodePolynomial;
            QMessageBox msgBox;
            msgBox.setText("Result: " + polynomial_string(L));
            // msgBox.setInformativeText("Do you want to save your changes?");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Save);
            int ret = msgBox.exec();
            switch (ret) {
              case QMessageBox::Save:
                  insertion_base(L);
                  this->base_display();
                  ++base_size;
                  break;
              case QMessageBox::Cancel:
                  break;
              default:
                  break;
            }
            return;
        }
    } else {
        if ('a' <= var[0] && var[0] <= 'z') {
            variable = var[0].unicode() - QChar('a').unicode();
        } else {
            QMessageBox::critical(this, " ", "Polynomial does not contain this variable");
            return;
        }
    }
    for (int i = 0; i < ui->spinbox_derivative_number->value(); ++i) {
        delete_polynomial(p);
        p = new NodePolynomial;
        for (auto tmp = p, el = res; el != nullptr; el = el->next) {
            tmp->a = el->a;
            tmp->power = el->power;
            if (el->next != nullptr) {
                tmp->next = new NodePolynomial;
                tmp = tmp->next;
            }
        }
        res = nullptr;
        for (auto el = p; el != nullptr; el = el->next) {
            el->a = el->a * el->power[variable];
            el->power[variable] -= 1;
            insertion(res, el);
        }
        delete_nulls(res);
        if (res == nullptr) {
            res = new NodePolynomial;
            res->a = 0;
            res->next = nullptr;
        }
    }
    delete_polynomial(p);
    QMessageBox msgBox;
    msgBox.setText("Result: " + polynomial_string(res));
    // msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    switch (ret) {
      case QMessageBox::Save:
          insertion_base(res);
          this->base_display();
          ++base_size;
          break;
      case QMessageBox::Cancel:
          break;
      default:
          break;
    }
    return;
}

void MainWindow::res_of_division_display(NodePolynomial* res1, NodePolynomial* copy1) {
    QMessageBox msgBox;
    msgBox.setText("Quotient of division: " + polynomial_string(res1) + "\n" + "Remainder from division: " + polynomial_string(copy1));
    // msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::SaveAll | QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    msgBox.setButtonText(QMessageBox::Yes, "Save quotient");
    msgBox.setButtonText(QMessageBox::No, "Save remainder");
    int ret = msgBox.exec();
    switch (ret) {
      case QMessageBox::SaveAll:
          insertion_base(res1);
          insertion_base(copy1);
          this->base_display();
          base_size += 2;
          break;
      case QMessageBox::Yes:
          insertion_base(res1);
          this->base_display();
          ++base_size;
          break;
      case QMessageBox::No:
          insertion_base(copy1);
          this->base_display();
          ++base_size;
          break;
      case QMessageBox::Cancel:
          break;
      default:
          break;
    }
}

void MainWindow::on_button_division_clicked()
{
    if (ui->spinbox_division_first->value() > base_size || ui->spinbox_division_second->value() > base_size) {
        QMessageBox::critical(this, " ", "Incorrect number");
    } else {
        // поиск многочленов в базе
        NodeBase* first = base_begin;
        NodeBase* second = base_begin;
        for (int i = 0; i < ui->spinbox_division_first->value() - 1; ++i) {
            first = first->next;
        }
        for (int i = 0; i < ui->spinbox_division_second->value() - 1; ++i) {
            second = second->next;
        }
        NodePolynomial* el1 = first->data;
        NodePolynomial* el2 = second->data;
        // копирование многочленов
        NodePolynomial* copy1 = new NodePolynomial;
        NodePolynomial* copy2 = new NodePolynomial;
        if (el2->a == copy2->a && el2->power == copy2->power) {
            QMessageBox::critical(this, " ", "Division by zero");
            return;
        }
        for (auto tmp = copy1; el1 != nullptr; el1 = el1->next) {
            tmp->a = el1->a;
            tmp->power = el1->power;
            if (el1->next != nullptr) {
                tmp->next = new NodePolynomial;
                tmp = tmp->next;
            }
        }
        for (auto tmp = copy2; el2 != nullptr; el2 = el2->next) {
            tmp->a = el2->a;
            tmp->power = el2->power;
            if (el2->next != nullptr) {
                tmp->next = new NodePolynomial;
                tmp = tmp->next;
            }
        }
        NodePolynomial* res = nullptr;
        // узнаю, какая переменная в многочлене по номеру в алфавите, -1 если переменных несколько, -2 если многочлен - константа
        int variable1 = check_one_variable(copy1);
        int variable2 = check_one_variable(copy2);
        // разбираю случаи с несколькими переменными
        if (variable1 == -1) {
            QMessageBox::critical(this, " ", "Incorrect polynomials are selected. Polynomial " +
                                  QString::number(ui->spinbox_division_first->value()) + " contains different variables");
            return;
        }
        if (variable2 == -1) {
            QMessageBox::critical(this, " ", "Incorrect polynomials are selected. Polynomial " +
                                  QString::number(ui->spinbox_division_second->value()) + " contains different variables");
            return;
        }
        NodePolynomial* null_polynomial = new NodePolynomial; // нулевой многочлен
        null_polynomial->a = 0;
        // разбираю случаи с константами
        if (variable1 == -2 && variable2 == -2) {
            res = new NodePolynomial;
            res->a = copy1->a / copy2->a;
            copy1->a = 0;
            this->res_of_division_display(res, null_polynomial); // функция вывода результата
            // вызвать функцию вывода результата
            return;
        } else if (variable1 == -2) {
            this->res_of_division_display(null_polynomial, copy1);
            // частное 0, остаток copy1
            return;
        } else if (variable2 == -2) {
            for (auto el = copy1; el != nullptr; el = el->next) {
                el->a = el->a / copy2->a;
            }
            this->res_of_division_display(copy1, null_polynomial);
            // частное copy1, остаток 0
            return;
        }
        if (variable1 != variable2) {
            QMessageBox::critical(this, " ", "Incorrect polynomials are selected. They contain different variables"); // случай с разными переменными
            return;
        }
        // само деление, частное в res, остаток в copy1
        while (copy1->a != 0 && copy1->power[variable1] >= copy2->power[variable1]) {
            int r = copy1->power[variable1] - copy2->power[variable1]; // разность степеней
            NodePolynomial* cur_node = new NodePolynomial; // новый одночлен, на который будем умножать
            cur_node->a = copy1->a / copy2->a;
            cur_node->power[variable1] = r;
            insertion(res, cur_node);
            NodePolynomial* L1 = multiplication(copy2, cur_node);
            // вычитание
            cur_node->a = -1;
            cur_node->power[variable1] = 0;
            NodePolynomial* L2 = multiplication(L1, cur_node);
            copy1 = addition(copy1, L2);
        }
        if (res == nullptr) {
            res = new NodePolynomial;
            *res = *null_polynomial;
        }
        this->res_of_division_display(res, copy1);
        delete_polynomial(copy2);

        // print_polynomial(copy1);

        /*delete_nulls(L);
        if (L == nullptr) {
            L = new NodePolynomial;
            L->a = 0;
            L->next = nullptr;
        }
        */
    }
}

bool is_integer(double num) {
    int d = num;
    if (abs(num - d) < eps) {
        return true;
    }
    return false;
}

vector<int> find_dividers(int num) {
    vector<int> res;
    int d = 1;
    while (d * d < num) {
        if (num % d == 0) {
            res.push_back(d);
            res.push_back(num / d);
        }
        ++d;
    }
    if (d * d == num) {
        res.push_back(d);
    }
    return res;
}

void MainWindow::on_button_find_root_clicked()
{
    if (ui->spinbox_find_root->value() > base_size) {
        QMessageBox::critical(this, " ", "Incorrect number");
        return;
    }
    NodeBase* first = base_begin;
    for (int i = 0; i < ui->spinbox_find_root->value() - 1; ++i) {
        first = first->next;
    }
    NodePolynomial* polynom = first->data;
    delete_nulls(polynom);
    if (polynom == nullptr) {
        polynom = new NodePolynomial;
    }
    int variable = check_one_variable(polynom);
    if (variable == -1) {
        QMessageBox::critical(this, " ", "Polynomial contains several variables");
        return;
    }
    if (variable == -2) {
        QMessageBox::critical(this, " ", "Polynomial does not contain variables");
        return;
    }
    int last_coefficient;
    int last_power;
    delete_nulls(polynom);
    if (polynom == nullptr) {
        polynom = new NodePolynomial;
    }
    print_polynomial(polynom);
    for (auto el = polynom; el != nullptr; el = el->next) {
        qDebug() << el->a;
        //qDebug() << el->power[variable];
        if (!is_integer(el->a)) {
            QMessageBox::critical(this, " ", "Polynomial contains not an integer coefficients");
            return;
        }
        last_coefficient = el->a;
        last_power = el->power[variable];
    }
    vector<int> res;
    if (last_power != 0) {
        res.push_back(0);
    }
    qDebug() << last_coefficient;
    last_coefficient = abs(last_coefficient);
    vector<int> dividers_first = find_dividers(abs(int(polynom->a)));
    vector<int> dividers_last = find_dividers(last_coefficient);
    QString str_dividers_first = "";
    for (auto v : dividers_first) {
        str_dividers_first += " " + QString::number(v);
    }
    QString str_dividers_second = "";
    for (auto v : dividers_last) {
        str_dividers_second += " " + QString::number(v);
    }
    qDebug() << str_dividers_first;
    qDebug() << str_dividers_second;
    for (int i = 0; i < dividers_first.size(); ++i) {
        for (int j = 0; j < dividers_last.size(); ++j) {
            if (dividers_last[j] % dividers_first[i] == 0) {
                int x = dividers_last[j] / dividers_first[i];
                int val = 0;
                for (auto el = polynom; el != nullptr; el = el->next) {
                    val += el->a * pow(x, el->power[variable]);
                }
                if (val == 0) {
                    res.push_back(x);
                }
                x *= -1;
                val = 0;
                for (auto el = polynom; el != nullptr; el = el->next) {
                    val += el->a * pow(x, el->power[variable]);
                }
                if (val == 0) {
                    res.push_back(x);
                }
            }
        }
    }
    QString str = "Integer roots: ";
    if (res.size() == 0) {
        str = "Polynomial has not integer roots";
    } else {
        for (int i = 0; i < res.size(); ++i) {
            if (i != 0) {
                str += ", ";
            }
            str += QString::number(res[i]);
        }
    }
    QMessageBox::information(this, " ", str);
    return;
}

void MainWindow::on_button_input_file_clicked()
{
    QString file_name = ui->lineedit_input_file->text(); // считывание имени файла
    QFile file(file_name); // нужно подключить QFile
    if (!file.exists()) {
        QMessageBox::critical(this, " ", "File does not exsist");
        return;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::critical(this, " ", "Error");
        return;
    }
    base_begin = nullptr; // почистить базу
    base_size = 0;
    QTextStream in(&file); // создание текстового потока из файла
    while (!in.atEnd()) {
        QString input_str = in.readLine();
        // преобразовываю строку в многочлен и, если получилось, вставляю в базу
        try {
            NodePolynomial* L = nullptr;
            this->check_str(input_str, L);
            print_polynomial(L);
            delete_nulls(L);
            if (L == nullptr) {
                L = new NodePolynomial;
                L->a = 0;
                L->next = nullptr;
            }
            print_polynomial(L);
            insertion_base(L);
            ++base_size;
        } catch (const QString& error) {
            continue;
        }
    }
    this->base_display();
    file.close();
}

void MainWindow::on_button_output_file_clicked()
{
    QString file_name = ui->lineedit_output_file->text();
    QFile file(file_name);
    qDebug() << "aaaaaa";
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){     // проверяю существование файла или возможность его создать
        QMessageBox::critical(this, " ", "Incorrect file name");
        qDebug() << "bbbbbbbbbbb";
        return;
    }

    QTextStream out(&file);
    for (auto el = base_begin; el != nullptr; el = el->next) {
        QString p = polynomial_string(el->data);
        out << p << "\n"; // вывод многочлена в текстовый поток
    }
    file.close();
    QMessageBox::information(this, " ", "Saving completed successfully");
}
