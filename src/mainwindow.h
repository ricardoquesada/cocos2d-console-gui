#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void parseTemplates();

private slots:

    void on_pushButtonNewGame_clicked();

    void on_pushButtonEditGame_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
