// monitor.h
//
// Monitor widget for glassmonitor(1) Audio Receiver front end
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

#ifndef MONITOR_H
#define MONITOR_H

#include <jack/jack.h>

#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QStringList>
#include <QWidget>

#include "config.h"
#include "playmeter.h"
#include "statsdialog.h"

class Monitor : public QWidget
{
 Q_OBJECT;
 public:
  Monitor(jack_client_t *jack,int streamno,Config *config,QWidget *parent=0);
  QSize sizeHint() const;
  bool start();
  void stop();
  unsigned channels() const;

 signals:
  void stopped(int signalnum);

 private slots:
  void statsClickedData();
  void listenClickedData();
  void processReadyReadData();
  void processFinishedData(int exit_code,QProcess::ExitStatus status);
  void processErrorData(QProcess::ProcessError err);

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void ProcessStats(const QStringList &stats);
  void UpdateStat(const QString &category,const QString &param,
		  const QString &value);
  void ProcessMeterUpdates(const QString &values);
  StatsDialog *mon_stats_dialog;
  PlayMeter *mon_meters[GLASSMONITOR_MAX_AUDIO_CHANNELS];
  QLabel *mon_label;
  QPushButton *mon_stats_button;
  QPushButton *mon_listen_button;
  unsigned mon_channels;
  bool mon_listening;
  QProcess *mon_process;
  QStringList mon_stats_list;
  jack_client_t *mon_jack_client;
  QString mon_client_name;
  int mon_stream_number;
  Config *mon_config;
};


#endif  // MONITOR_H
