#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void closeEvent(QCloseEvent *);

private slots:
    void on_actionNew_N_triggered();

    void on_actionCompare_C_triggered();

    void on_action_A_triggered();

private:
    void createMdiView();
    void createMdiChildDialog();

    void readSettings();
    void writeSettings();

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
