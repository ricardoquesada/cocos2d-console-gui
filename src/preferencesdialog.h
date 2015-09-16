#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = 0);
    ~PreferencesDialog();

    static QString findCocosPath();

private slots:
    void on_directoryButton_clicked();

    void on_buttonBox_accepted();

    void on_lineEdit_editingFinished();

private:
    Ui::PreferencesDialog *ui;
};

#endif // PREFERENCESDIALOG_H
