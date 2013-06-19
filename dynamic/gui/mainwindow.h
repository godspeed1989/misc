#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QFileDialog>

class MainWindow : public QWidget
{
	Q_OBJECT
public:
	MainWindow();
	QString caption;
private:
	QTableWidget* table;
	QPushButton* btn_open_dat;
	QPushButton* btn_open_fmt;
	QPushButton* btn_do_parse;
	QFileDialog file_dialog;
	QString dat_file_path;
	QString fmt_file_path;
private slots:
	void open_dat_file();
	void open_fmt_file();
	void do_parse_file();
private:
	QAction *copyAction;
	QTableWidgetSelectionRange selectedRange() const;
public slots:
	void actionCopy();
};

#endif

