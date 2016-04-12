// glassmonitor.cpp
//
// glassmonitor(1) Audio Receiver front end
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <stdio.h>

#include <QApplication>
#include <QMessageBox>
#include <QPainter>

#include "cmdswitch.h"
#include "glassmonitor.h"

MainWidget::MainWidget(QWidget *parent)
  : QMainWindow(parent)
{
  glass_jack_process=NULL;
  QString err_msg;
  CmdSwitch *cmd=new CmdSwitch("glassmonitor","[options]\n",VERSION);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(!cmd->processed(i)) {
      QMessageBox::warning(this,"GlassMonitor - "+tr("Error"),
			   tr("Unknown option")+": "+cmd->key(i));
      exit(256);
    }
  }

  //
  // Load Configuration
  //
  glass_config=new Config();
  glass_config->load();

  //
  // Start JACK
  //
  if(!StartJack(&err_msg)) {
    QMessageBox::warning(this,"GlassMonitor - "+tr("JACK Error"),
			 tr("Unable to start JACK")+": \n"+err_msg);
    StopJack();
    exit(256);
  }

  //
  // Start Monitors
  //
  for(unsigned i=0;i<glass_config->monitorQuantity();i++) {
    glass_monitors.
      push_back(new Monitor(glass_jack_client,i,glass_config,this));
    glass_monitors.back()->start();
  }

  setMinimumSize(QSize(500,40*glass_monitors.size()));
  setMaximumHeight(40*glass_monitors.size());
}


QSize MainWidget::sizeHint() const
{
  return QSize(500,100);
}


void MainWidget::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(Qt::black);
  p->setBrush(Qt::black);
  for(unsigned i=1;i<glass_monitors.size();i++) {
    p->drawLine(5,glass_monitors[i-1]->size().height(),size().width()-5,
		glass_monitors[i-1]->size().height());
  }
  delete p;
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  for(unsigned i=0;i<glass_monitors.size();i++) {
    Monitor *m=glass_monitors[i];
    glass_monitors[i]->setGeometry(0,i*m->sizeHint().height(),
				   size().width(),m->sizeHint().height());
  }
}


void MainWidget::closeEvent(QCloseEvent *e)
{
  for(unsigned i=0;i<glass_monitors.size();i++) {
    glass_monitors[i]->stop();
  }
  StopJack();
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  MainWidget *w=new MainWidget();
  w->show();
  return a.exec();
}
