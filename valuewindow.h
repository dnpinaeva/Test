#ifndef VALUEWINDOW_H
#define VALUEWINDOW_H

#include <QDialog>
#include <vector>

using std::vector;

namespace Ui {
class ValueWindow;
}

class ValueWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ValueWindow(QWidget *parent = nullptr, int = 0);
    ~ValueWindow();

private slots:
    void on_button_count_clicked();

private:
    Ui::ValueWindow *ui;

    void CreateUI(int val);

    void check_table(vector<double>&);
};

#endif // VALUEWINDOW_H
