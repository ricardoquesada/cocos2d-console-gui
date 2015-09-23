#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>

namespace Ui {
class ProgressDialog;
}

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(QWidget *parent = 0);
    ~ProgressDialog();

    void appendData(const QString& str);

public slots:
    void processFinished();


private:
    Ui::ProgressDialog *ui;
};

#endif // PROGRESSDIALOG_H
