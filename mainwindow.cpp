#include "mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), size(2500), speed(500), isPaused(false), isEnd(false) {
    ui->setupUi(this);

    model = new QStandardItemModel(this);
    ui->tableView->setModel(model);
    ui->tableView->setFixedHeight(30);
    ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableView->horizontalHeader()->hide();
    model->setRowCount(1);
    model->setColumnCount(size);
    model->setHeaderData(0, Qt::Vertical, " ");


    model2 = new QStandardItemModel(this);
    ui->tableView_2->setModel(model2);
    model2->setRowCount(5);
    model2->setColumnCount(1);
    model2->setHeaderData(0, Qt::Horizontal, "^");

    for (int col = 0; col < model->columnCount(); ++col) {
        ui->tableView->setColumnWidth(col, 20);
        QStandardItem* item = model->item(0, col);
        if (!item) {
            item = new QStandardItem();
            model->setItem(0, col, item);
        }
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    }

    for (int row = 0; row < model2->rowCount(); ++row) {
        model2->setHeaderData(row, Qt::Vertical, "q" + QString::number(row));
    }

    ui->tableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    reset();
}

MainWindow::~MainWindow() {
    delete ui;
}

bool MainWindow::isCellVisible(int column) {
    QRect cellRect = ui->tableView->visualRect(model->index(0, column));
    QRect viewportRect = ui->tableView->viewport()->rect();
    return viewportRect.contains(cellRect);
}

void MainWindow::updateHighlightedCell(int previousIdx, int newIdx, QStandardItemModel* mod, int row) {
    QColor color = (mod == model) ? Qt::green : Qt::blue;

    if (previousIdx >= 0 && previousIdx < mod->columnCount()) {
        mod->setData(mod->index(row, previousIdx), QVariant(), Qt::BackgroundRole);
    }

    if (newIdx >= 0 && newIdx < mod->columnCount()) {
        QModelIndex newIndex = mod->index(row, newIdx);
        mod->setData(newIndex, QBrush(color), Qt::BackgroundRole);

        if(mod == model && !isCellVisible(newIdx)) {
            ui->tableView->scrollTo(newIndex, QAbstractItemView::PositionAtCenter);
        }
    }
}

void MainWindow::commandsController(QVector<QString>& commands) {
    int previousIdx = idx;

    QVector<QString> otherCommands;
    QVector<QString> moveCommands;
    QVector<QString> stateCommands;
    QVector<QString> stopCommands;

    for (const QString& cmd : commands) {
        if (cmd != "R" && cmd != "L" && !cmd.startsWith("q") && cmd != "!") {
            otherCommands.push_back(cmd);
        } else if (cmd == "R" || cmd == "L") {
            moveCommands.push_back(cmd);
        } else if (cmd.startsWith(cmd)) {
            stateCommands.push_back(cmd);
        } else if(cmd == "!"){
            stopCommands.push_back(cmd);
        }
    }

    QVector<QString> sortedCommands;
    sortedCommands.append(otherCommands);
    sortedCommands.append(moveCommands);
    sortedCommands.append(stateCommands);
    sortedCommands.append(stopCommands);

    for (const QString& cmd : sortedCommands) {
        if (cmd == "R") {
            if (idx + 1 < size) {
                ++idx;
            }
        }
        else if (cmd == "L") {
            if (idx - 1 >= 0) {
                --idx;
            }
        }
        else if (cmd == "!") {
            qDebug() << "STOP!";
            flag = false;
            isEnd = true;
            break;
        }
        else if (cmd == "^") {
            str[idx] = ' ';
            model->setData(model->index(0, idx), " ");
        }
        else if (*cmd.begin() == 'q') {
            QString newQ = cmd.mid(1);
            bool ok;
            if (q > model2->rowCount()) {
                QMessageBox::warning(this, "Ошибка", "Такого состояния не существует!");
                break;
            }
            int prevQ = q;
            q = newQ.toInt(&ok);
            for (int col = 0; col < model2->columnCount(); ++col) {
                updateHighlightedCell(-1, col, model2, q);
                updateHighlightedCell(col, -1, model2, prevQ);
            }
            break;
        }
        else {
            if (cmd.size() > 1) {
                reset();
                QMessageBox::warning(this, "Ошибка", "Возможно вставить только один символ!");
                break;
            }
            if (!checkCurrentAdd(*cmd.begin())) {
                reset();
                QMessageBox::warning(this, "Ошибка", "Вставляемого символа нет в массиве!");
                break;
            }
            str[idx] = *cmd.begin();
            model->setData(model->index(0, idx), str[idx]);
        }
    }

    updateHighlightedCell(previousIdx, idx, model);
}

void MainWindow::startActions() {
    QStandardItemModel* model2 = qobject_cast<QStandardItemModel*>(ui->tableView_2->model());
    if (!model2) {
        QMessageBox::warning(this, "Ошибка", "Модель таблицы не поддерживает QStandardItemModel!");
        return;
    }

    QTimer* timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, [this, timer]() {
        if (!flag) {
            timer->stop();
            timer->deleteLater();
            return;
        }
        on_btnStep_clicked();
    });

    timer->start(speed);
}

bool MainWindow::checkSymbol(QChar c) {
    for (int row = 0; row < model2->rowCount(); ++row) {
        for (int col = 0; col < model2->columnCount(); ++col) {
            QModelIndex index = model2->index(row, col);
            QString cellText = model2->data(index, Qt::DisplayRole).toString().trimmed();
            if (cellText.contains(c))
                return true;
        }
    }
    return false;
}

bool MainWindow::checkAlph() {
    QMap<QChar, int> mp;
    for (const QChar c : alph) {
        mp[c] = 1;
    }
    for (const QChar c : str) {
        if (mp[c] != 1) {
            return false;
        }
    }
    return true;
}

bool MainWindow::checkCurrentAdd(QChar c){
    for (int col = 0; col < model2->columnCount(); ++col) {
        QString header = model2->headerData(col, Qt::Horizontal).toString();
        if (header == c) {
            return true;
        }
    }
    return false;
}

void MainWindow::reset() {
    updateHighlightedCell(idx, -1, model);
    idx = size / 3;
    q = 0;
    flag = false;
    isPaused = true;
    isEnd = false;
}

void MainWindow::on_btnInputString_clicked() {
    reset();
    str = ui->lineInputString->text();
    ui->btnStep->show();
    if (!checkAlph()) {
        QMessageBox::warning(this, "Ошибка", "Не все символы в строке принадлежат алфавиту!");
        return;
    }

    if (idx >= 0 && idx < model->columnCount()) {
        model->setData(model->index(0, idx), QVariant(), Qt::BackgroundRole);
    }

    for (int var = 0; var < idx; ++var) {
        str = " " + str + " ";
    }

    for (int col = 0; col < model->columnCount(); ++col) {
        model->setData(model->index(0, col + idx), QVariant());
    }

    for (int i = 0; i < str.length(); ++i) {
        if (i >= model->columnCount()) {
            QMessageBox::warning(this, "Ошибка", "Слишком много символов для таблицы!");
            break;
        }
        QChar character = str.at(i);
        model->setData(model->index(0, i), character);
    }
    updateHighlightedCell(-1, idx, model);
}

void MainWindow::handleAlphabetsEntered(const QString& newAlph, const QString& newAlphAdd) {
    alph = newAlph;
    alph.push_back("^");
    alph.push_back(newAlphAdd);

    QList<QChar> uniqueChars;
    for (const QChar& ch : alph) {
        if (!uniqueChars.contains(ch)) {
            uniqueChars.append(ch);
        }
    }

    QString uniqueText;
    for (const QChar& ch : uniqueChars) {
        uniqueText.append(ch);
    }

    QStandardItemModel* model2 = qobject_cast<QStandardItemModel*>(ui->tableView_2->model());
    if (!model2) {
        QMessageBox::warning(this, "Ошибка", "Модель таблицы не поддерживает QStandardItemModel!");
        return;
    }

    model2->clear();
    model2->setColumnCount(uniqueText.length());
    model2->setRowCount(5);

    for (int col = 0; col < model2->columnCount(); ++col) {
        updateHighlightedCell(-1, col, model2, q);
    }

    for (int i = 0; i < uniqueText.length(); ++i) {
        QChar character = uniqueText.at(i);
        model2->setHeaderData(i, Qt::Horizontal, character);
    }

    for (int row = 0; row < model2->rowCount(); ++row) {
        model2->setHeaderData(row, Qt::Vertical, "q" + QString::number(row));
    }
}

void MainWindow::on_btnInputAlph_clicked() {
    dialog = new Dialog(this);
    connect(dialog, &Dialog::alphabetsEntered, this, &MainWindow::handleAlphabetsEntered);
    dialog->exec();
}

void MainWindow::on_btnPlusString_clicked() {
    QStandardItemModel* model2 = qobject_cast<QStandardItemModel*>(ui->tableView_2->model());
    if (!model2) {
        QMessageBox::warning(this, "Ошибка", "Модель таблицы не поддерживает QStandardItemModel!");
        return;
    }

    int newIdx = model2->rowCount();
    model2->insertRow(newIdx);
    model2->setHeaderData(newIdx, Qt::Vertical, "q" + QString::number(newIdx));
}

void MainWindow::on_btnMinusString_clicked() {
    QStandardItemModel* model2 = qobject_cast<QStandardItemModel*>(ui->tableView_2->model());
    if (!model2) {
        QMessageBox::warning(this, "Ошибка", "Модель таблицы не поддерживает QStandardItemModel!");
        return;
    }

    if (model2->rowCount() > 0) {
        model2->removeRow(model2->rowCount() - 1);
    }
    else {
        QMessageBox::warning(this, "Ошибка", "Нет строк для удаления!");
    }
}

void MainWindow::on_btnStart_clicked() {
    if (!flag) {
        flag = true;
        isPaused = false;
        startActions();
        ui->btnStep->hide();
    }
}

void MainWindow::on_btnStep_clicked() {
    if (!checkSymbol('!')) {
        QMessageBox::warning(this, "Ошибка", "В таблице нет команды оставноки!");
        flag = false;
        isEnd = true;
        return;
    }

    if ((!flag && !isPaused) || isEnd)
        return;

    QStandardItemModel* model2 = qobject_cast<QStandardItemModel*>(ui->tableView_2->model());
    if (!model2) {
        QMessageBox::warning(this, "Ошибка", "Модель таблицы не поддерживает QStandardItemModel!");
        return;
    }

    QString c = str[idx];

    for (int col = 0; col < model2->columnCount(); ++col) {
        QString header = model2->headerData(col, Qt::Horizontal).toString();
        if (c == " ") {
            header = " ";
            col = model2->columnCount() - 1;
        }
        if (c == header) {
            QModelIndex index = model2->index(q, col);
            QString cellText = model2->data(index, Qt::DisplayRole).toString().trimmed();
            if (cellText.isEmpty()) continue;
            QString processedText = cellText.replace(" ", "");
            QStringList splitCommands = processedText.split(",", Qt::SkipEmptyParts);
            QVector<QString> commands;
            for (const QString& cmd : splitCommands) {
                if (!cmd.isEmpty()) {
                    commands.push_back(cmd);
                }
            }
            commandsController(commands);
            break;
        }
    }
}

void MainWindow::on_btnStop_clicked() {
    for (int col = 0; col < model2->columnCount(); ++col) {
        updateHighlightedCell(col, -1, model2, q);
        updateHighlightedCell(-1, col, model2, 0);
    }
    on_btnInputString_clicked();
}

void MainWindow::on_btnPause_clicked() {
    flag = false;
    isPaused = true;
    ui->btnStep->show();
}

void MainWindow::on_btnPlusSpeed_clicked() {
    speed = qMax(0, speed - 100);
}

void MainWindow::on_btnMinusSpeed_clicked() {
    speed = qMin(1500, speed + 100);
}
