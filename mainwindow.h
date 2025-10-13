#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>

using std::vector;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct NodePolynomial{
    double a;
    vector<int> power;
    NodePolynomial* next;

    NodePolynomial();
};

struct NodeBase{
    NodePolynomial* data;
    NodeBase* next;

    NodeBase();
};

extern NodeBase* base_begin;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_button_add_clicked();

    void on_button_delete_clicked();

    void on_button_equality_clicked();

    void on_button_addition_clicked();

    void on_button_multiplication_clicked();

    void on_button_value_clicked();

    void on_button_derivative_clicked();

    void on_button_division_clicked();

    void on_button_find_root_clicked();

    void on_button_input_file_clicked();

    void on_button_output_file_clicked();

private:
    Ui::MainWindow *ui;

    void check_str(QString, NodePolynomial*&);
    void base_display();
    void res_of_division_display(NodePolynomial*, NodePolynomial*);
};
#endif // MAINWINDOW_H
