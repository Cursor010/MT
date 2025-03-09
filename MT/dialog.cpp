#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog) {
    ui->setupUi(this);
}

Dialog::~Dialog() {
    delete ui;
}

void Dialog::on_btnInputAlph_clicked()
{
    QString alph = ui->lineInputAlph->text();
    QString alphAdd = ui->lineInputAlphAdd->text();

    emit alphabetsEntered(alph, alphAdd);

    this->accept();
}
