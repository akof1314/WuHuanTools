#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "formdialog.h"
#include <QSettings>
#include <QCloseEvent>
#include <QMdiSubWindow>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    readSettings();

    createMdiView();
    createMdiChildDialog();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindow::createMdiView()
{
    setCentralWidget(ui->mdiArea);
    QTabBar *tab = ui->mdiArea->findChild<QTabBar*>();
    tab->setExpanding(false);    //不拉伸标签宽度
}

void MainWindow::createMdiChildDialog()
{
    FormDialog *mdiChildDialog = new FormDialog(this);
    if (mdiChildDialog)
    {
        mdiChildDialog->setAttribute(Qt::WA_DeleteOnClose);
        ui->mdiArea->addSubWindow(mdiChildDialog);
        mdiChildDialog->showMaximized();
    }
}

void MainWindow::readSettings()
{
    QCoreApplication::setOrganizationName("JsTools");
    QCoreApplication::setApplicationName("CompareStr");

    QSettings settings;
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(960, 640)).toSize();
    bool bMaximized = settings.value("maximized", false).toBool();
    if (bMaximized)
    {
        setWindowState(windowState() | Qt::WindowMaximized);
    }
    else
    {
        resize(size);
        move(pos);
    }
    restoreState(settings.value("windowState").toByteArray());
}

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("maximized", isMaximized());
    settings.setValue("windowState", saveState());
}

void MainWindow::on_actionNew_N_triggered()
{
    createMdiChildDialog();
}

void MainWindow::on_actionCompare_C_triggered()
{
    QMdiSubWindow *child = ui->mdiArea->activeSubWindow();
    if (child == nullptr)
    {
        QList<QMdiSubWindow *> listChild = ui->mdiArea->subWindowList();
        if (listChild.count() > 0)
        {
            child = listChild.at(0);
        }
    }
    if (child)
    {
        FormDialog *dialog = qobject_cast<FormDialog *>(child->widget());
        if (dialog)
        {
            dialog->compare();
        }
    }
}
