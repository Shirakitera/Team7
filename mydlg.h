#ifndef MYDLG_H
#define MYDLG_H

#include <QDialog>

namespace Ui {
class Mydlg;
}

class Mydlg : public QDialog
{
    Q_OBJECT

public:
    explicit Mydlg(QWidget *parent = nullptr);
    ~Mydlg();

private:
    Ui::Mydlg *ui;
};

#endif // MYDLG_H
