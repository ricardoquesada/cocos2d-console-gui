#ifndef DIALOGNEWGAME_H
#define DIALOGNEWGAME_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class DialogNewGame;
}

class DialogNewGame : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNewGame(QWidget *parent = 0);
    ~DialogNewGame();

private slots:
    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_buttonBox_accepted();

private:
    Ui::DialogNewGame *ui;
};

#endif // DIALOGNEWGAME_H
