// glassmonitor.h
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

#ifndef GLASSMONITOR_H
#define GLASSMONITOR_H

#include <vector>

#include <jack/jack.h>

#include <QMainWindow>
#include <QProcess>
#include <QTimer>

#include "config.h"
#include "monitor.h"
#include "profile.h"

class MainWidget : public QMainWindow
{
 Q_OBJECT;
 public:
  MainWidget(QWidget *parent=0);
  QSize sizeHint() const;

 private slots:
  void jackProcessFinishedData(int exit_code,QProcess::ExitStatus status);
  void jackProcessErrorData(QProcess::ProcessError err);
  void jackProcessKillData();

 protected:
  void paintEvent(QPaintEvent *e);
  void resizeEvent(QResizeEvent *e);
  void closeEvent(QCloseEvent *e);

 private:
  bool StartJack(QString *err_msg);
  void StopJack();
  std::vector<Monitor *> glass_monitors;
  QProcess *glass_jack_process;
  QTimer *glass_jack_kill_timer;
  jack_client_t *glass_jack_client;
  Config *glass_config;
  friend void GlassMonitor_JackPortConnect(jack_port_id_t,jack_port_id_t,int,
					   void *);
};


#endif  // GLASSMONITOR_H
