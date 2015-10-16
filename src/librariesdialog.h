#ifndef LIBRARIESDIALOG_H
#define LIBRARIESDIALOG_H

#include <QDialog>

class GameState;

namespace Ui {
class LibrariesDialog;
}

class LibrariesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LibrariesDialog(GameState *gameState, QWidget *parent = 0);
    ~LibrariesDialog();

    QString getSelectedString() const;

private slots:
    void on_listView_doubleClicked(const QModelIndex &index);

private:
    Ui::LibrariesDialog *ui;
};

#endif // LIBRARIESDIALOG_H
