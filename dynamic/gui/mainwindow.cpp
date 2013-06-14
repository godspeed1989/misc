#include "mainwindow.h"
#include "../filereader.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

MainWindow::MainWindow()
{
	QHBoxLayout *up, *down;
	up = new QHBoxLayout();
	down = new QHBoxLayout();

	btn_open_dat = new QPushButton(tr("&MR file"), this);
	connect(btn_open_dat, SIGNAL(clicked()), this, SLOT(open_dat_file()));
	btn_open_fmt = new QPushButton(tr("&XML file"), this);
	connect(btn_open_fmt, SIGNAL(clicked()), this, SLOT(open_fmt_file()));
	btn_do_parse = new QPushButton(tr("&Parse"), this);
	connect(btn_do_parse, SIGNAL(clicked()), this, SLOT(do_parse_file()));
	up->addWidget(btn_open_dat);
	up->addWidget(btn_open_fmt);
	up->addWidget(btn_do_parse);
	up->addStretch(25);

	table = new QTableWidget(this);
	down->addWidget(table);

	QVBoxLayout *layout;
	layout = new QVBoxLayout(this);
	layout->addLayout(up);
	layout->addWidget(table);
	this->setLayout(layout);
	this->resize(800, 600);
}

void MainWindow::do_parse_file()
{
	if(dat_file_path.isNull() || fmt_file_path.isNull())
		return;
	filereader freader(fmt_file_path.toStdString().c_str(), dat_file_path.toStdString().c_str());
	if(freader.parse_fmt_file())
	{
		qDebug() << "error in parse XML file" << endl;
		return;
	}
	if(freader.parse_data_file())
	{
		qDebug() << "error in parse data file" << endl;
		return;
	}
	freader.summary();
	// TODO show table items
	table->setColumnCount(5);
	table->setRowCount(freader.data_file.logs.size());
}

void MainWindow::open_dat_file()
{
	dat_file_path = file_dialog.getOpenFileName(this, tr("open .mr file"), ".");
	caption = dat_file_path + QString(" : ") + fmt_file_path;
	this->setWindowTitle(caption);
}

void MainWindow::open_fmt_file()
{
	fmt_file_path = file_dialog.getOpenFileName(this, tr("open XML file"), ".", tr("*.xml\n*.*"));
	caption = dat_file_path + QString(" : ") + fmt_file_path;
	this->setWindowTitle(caption);
}

