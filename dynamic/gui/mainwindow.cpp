#include "mainwindow.h"
#include "../filereader.hpp"
#include "../filereader_static.hpp"
#include <QFile>
#include <QtGui>
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
	table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	down->addWidget(table);

	QVBoxLayout *layout;
	layout = new QVBoxLayout(this);
	layout->addLayout(up);
	layout->addWidget(table);
	this->setLayout(layout);
	this->resize(800, 600);

	copyAction = new QAction(table);
	copyAction->setShortcut(QString("Ctrl+C"));
	connect(copyAction, SIGNAL(triggered()), this, SLOT(actionCopy()));
	table->addAction(copyAction);
}

void MainWindow::do_parse_file()
{
	// read in interested field's name
	QStringList header_list;
	QString file_path = file_dialog.getOpenFileName(this, tr("open file"), ".", tr("*.txt\n*.*"));
	QFile file(file_path);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;
	QTextStream in(&file);
	QString line = in.readLine();
	while(!line.isNull())
	{
		if(line.length() > 0)
			header_list += line;
		line = in.readLine();
	}

	// read in data file
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

	// show table items
	table->setColumnCount(header_list.size());
	table->setHorizontalHeaderLabels(header_list);
	table->setRowCount(freader.data_file.logs.size());
	// show table item
	for(size_t i = 0; i < freader.data_file.logs.size(); ++i)
	{
		for(int j = 0; j < header_list.size(); ++j)
		{
			const struct data &d
			 = get_data_by_name(freader.data_file.logs[i].content, \
						(const xmlChar*)header_list[j].toStdString().c_str());
			QTableWidgetItem *item = new QTableWidgetItem();
			item->setTextAlignment(Qt::AlignRight);
			if(d.p == NULL)
				item->setText(QString("nil"));
			else
			{
				static char str[4];
				QString qstr;
				u32 lenB = (d.lenb >> 3) +  ((d.lenb & 7) != 0);
				for(int k = lenB - 1; k >= 0; --k)
				{
					snprintf(str, 4, " %02x", *((unsigned char*)d.p + k));
					qstr.append(str);
				}
				item->setText(QString(qstr));
			}
			table->setItem(i, j, item);
		}
	}
}

QTableWidgetSelectionRange MainWindow::selectedRange() const
{
	QList<QTableWidgetSelectionRange> ranges = table->selectedRanges();
	if(ranges.isEmpty())
		return QTableWidgetSelectionRange();
	return ranges.first();
}

void MainWindow::actionCopy()
{
	QString str;
	QTableWidgetSelectionRange range = selectedRange();
	for(int i = 0; i< range.rowCount(); ++i)
	{
		if(i > 0)
			str += "\n";
		for(int j = 0; j < range.columnCount(); ++j)
		{
			if(j > 0)
				str += "\t";
			str += table->item(range.topRow() + i, range.leftColumn() + j)->text();
		}
	}
	QApplication::clipboard()->setText(str);
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

