// monitor.cpp
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

#include <QMessageBox>

#include "mail.h"
#include "monitor.h"

Monitor::Monitor(jack_client_t *jack,int streamno,Config *config,
		 QWidget *parent)
  : QWidget(parent)
{
  QStringList to_addrs;
  to_addrs.push_back("gleasonf@rfa.org");
  QStringList dummy;


  /*
  if(SendMail(to_addrs,dummy,dummy,"fredg@paravelsystems.com","fredg@paravelsystems.com","Test Message","This is the message!")) {
    printf("SUCCESSFUL!\n");
  }
  else {
    printf("FAILED\n");
  }
  */

  mon_connected=false;
  mon_channels=0;
  mon_listening=false;
  mon_jack_client=jack;
  mon_stream_number=streamno;
  mon_config=config;

  //
  // Dialogs
  //
  mon_stats_dialog=new StatsDialog(this);
  mon_stats_dialog->
    setWindowTitle(tr("Stream Stats")+" - "+config->monitorLabel(streamno));
  connect(mon_stats_dialog,SIGNAL(closeClicked()),
	  this,SLOT(statsClickedData()));

  //
  // Meters
  //
  for(int i=0;i<GLASSMONITOR_MAX_AUDIO_CHANNELS;i++) {
    mon_meters[i]=new PlayMeter(SegMeter::Right,this);
    mon_meters[i]->setRange(-3000,0);
    mon_meters[i]->setHighThreshold(-800);
    mon_meters[i]->setClipThreshold(-100);
    mon_meters[i]->setMode(SegMeter::Peak);
  }
  mon_meters[0]->setLabel(tr("L"));
  mon_meters[1]->setLabel(tr("R"));

  //
  // Label
  //
  mon_label=new QLabel(this);
  mon_flash_state=false;
  mon_flash_timer=new QTimer(this);
  connect(mon_flash_timer,SIGNAL(timeout()),this,SLOT(flashData()));

  //
  // Buttons
  //
  mon_stats_button=new QPushButton(tr("Stats"),this);
  connect(mon_stats_button,SIGNAL(clicked()),this,SLOT(statsClickedData()));

  mon_listen_button=new QPushButton(tr("Listen"),this);
  connect(mon_listen_button,SIGNAL(clicked()),this,SLOT(listenClickedData()));
}


QSize Monitor::sizeHint() const
{
  return QSize(300,40);
}


bool Monitor::start()
{
  mon_client_name=QString().sprintf("Monitor%d",mon_stream_number+1);
  mon_label->setText(mon_config->monitorLabel(mon_stream_number));
  QStringList args;
  args.push_back("--audio-device=JACK");
  args.push_back("--jack-client-name="+mon_client_name);
  args.push_back("--meter-data");
  args.push_back("--stats-out");
  args.push_back(mon_config->monitorUrl(mon_stream_number));
  mon_process=new QProcess(this);
  connect(mon_process,SIGNAL(readyRead()),this,SLOT(processReadyReadData()));
  connect(mon_process,SIGNAL(finished(int,QProcess::ExitStatus)),
	  this,SLOT(processFinishedData(int,QProcess::ExitStatus)));
  connect(mon_process,SIGNAL(error(QProcess::ProcessError)),
	  this,SLOT(processErrorData(QProcess::ProcessError)));
  mon_process->start("/usr/bin/glassplayer",args);

  return true;
}


void Monitor::stop()
{
  mon_process->terminate();
  mon_process->waitForFinished();
}


unsigned Monitor::channels() const
{
  return mon_channels;
}


void Monitor::statsClickedData()
{
  if(mon_stats_dialog->isVisible()) {
    mon_stats_dialog->hide();
    mon_stats_button->setStyleSheet("");
  }
  else {
    mon_stats_dialog->show();
    mon_stats_button->
      setStyleSheet("color: #FFFFFF; background-color: #FF0000;");
  }
}


void Monitor::listenClickedData()
{
  if(mon_listening) {
    switch(mon_channels) {
    case 1:
      jack_disconnect(mon_jack_client,
		      (mon_client_name+":output1").toUtf8(),
		      "system:playback_1");
      jack_disconnect(mon_jack_client,
		      (mon_client_name+":output1").toUtf8(),
		      "system:playback_2");
      break;

    case 2:
      jack_disconnect(mon_jack_client,
		      (mon_client_name+":output_1").toUtf8(),
		      "system:playback_1");
      jack_disconnect(mon_jack_client,
		      (mon_client_name+":output_2").toUtf8(),
		      "system:playback_2");
      break;
    }
    mon_listen_button->setText(tr("Listen"));
    mon_listen_button->setStyleSheet("");
    mon_listening=false;
  }
  else {
    switch(mon_channels) {
    case 1:
      jack_connect(mon_jack_client,
		   (mon_client_name+":output1").toUtf8(),
		   "system:playback_1");
      jack_connect(mon_jack_client,
		   (mon_client_name+":output1").toUtf8(),
		   "system:playback_2");
      break;

    case 2:
      jack_connect(mon_jack_client,
		   (mon_client_name+":output_1").toUtf8(),
		   "system:playback_1");
      jack_connect(mon_jack_client,
		   (mon_client_name+":output_2").toUtf8(),
		   "system:playback_2");
      break;
    }
    mon_listen_button->setText(tr("Mute"));
    mon_listen_button->
      setStyleSheet("color: #FFFFFF;background-color: #FF0000;");
    mon_listening=true;
  }
}


void Monitor::flashData()
{
  if(mon_flash_state) {
    mon_label->setStyleSheet("");
  }
  else {
    mon_label->setStyleSheet("color: #FFFFFF; background-color: #FF5555;");
  }
  mon_flash_state=!mon_flash_state;
}


void Monitor::processReadyReadData()
{
  QString line;
  QStringList f0;

  while(mon_process->canReadLine()) {
    line=mon_process->readLine().trimmed();
    if(line.isEmpty()) {
      ProcessStats(mon_stats_list);
      mon_stats_list.clear();
    }
    else {
      f0=line.split(" ");
      if(f0[0]=="ME") {
	if(f0.size()==2) {
	  ProcessMeterUpdates(f0[1]);
	}
      }
      else {
	mon_stats_list.push_back(line);
      }
    }
  }
}


void Monitor::processFinishedData(int exit_code,QProcess::ExitStatus status)
{
  if(status!=QProcess::NormalExit) {
    QMessageBox::warning(this,"GlassMonitor - "+tr("Player Error"),
			 tr("Player for stream")+" \""+
			 mon_config->monitorUrl(mon_stream_number)+
			 "\" "+tr("crashed!"));
  }
  if(exit_code!=0) {
    QMessageBox::warning(this,"GlassMonitor - "+tr("Player Error"),
			 tr("Player for stream")+" \""+
			 mon_config->monitorUrl(mon_stream_number)+
			 " "+tr("returned non-zero exit code")+" "+
			 QString().sprintf(": %d.",exit_code));
  }
  emit stopped(mon_stream_number);
}


void Monitor::processErrorData(QProcess::ProcessError err)
{
  QMessageBox::warning(this,"GlassMonitor - "+tr("Player Error"),
		       tr("Player for stream")+" \""+
		       mon_config->monitorUrl(mon_stream_number)+
		       " "+tr("return network error")+": "+
		       QString().sprintf(": %u.",err));
}


void Monitor::resizeEvent(QResizeEvent *e)
{
  int meter_height=(size().height()-10)/GLASSMONITOR_MAX_AUDIO_CHANNELS;
  for(int i=0;i<GLASSMONITOR_MAX_AUDIO_CHANNELS;i++) {
    mon_meters[i]->setGeometry(5,5+i*meter_height,200,meter_height);
  }

  mon_label->setGeometry(210,5,size().width()-360,size().height()-10);
  mon_stats_button->setGeometry(size().width()-140,5,60,size().height()-10);
  mon_listen_button->setGeometry(size().width()-70,5,60,size().height()-10);
}


void Monitor::ProcessStats(const QStringList &stats)
{
  QString category;
  QString param;
  QString value;

  for(int i=0;i<stats.size();i++) {
    if(!stats[i].isEmpty()) {
      QStringList f0=stats[i].split(": ");
      QStringList f1=f0[0].split("|",QString::KeepEmptyParts);
      category=f1[0];
      if(f1.size()==2) {
	param=f1[1];
      }
      f0.erase(f0.begin());
      value=f0.join(": ");
      UpdateStat(category,param,value);
    }
  }
}


void Monitor::UpdateStat(const QString &category,const QString &param,
			 const QString &value)
{
  //  printf("Category: %s  Param: %s\n",(const char *)category.toUtf8(),
  //  	 (const char *)value.toUtf8());

  mon_stats_dialog->update(category,param,value);

  if((category=="Codec")&&(param=="Channels")) {
    mon_channels=value.toUInt();
  }
  if((category=="Connector")&&(param=="Connected")) {
    if(value.toLower()=="yes") {
      if(!mon_connected) {
	mon_listen_button->setEnabled(true);
	mon_flash_timer->stop();
	mon_flash_state=false;
	mon_label->setStyleSheet("");
	mon_connected=true;
      }
    }
    else {
      if(mon_connected) {
	for(int i=0;i<GLASSMONITOR_MAX_AUDIO_CHANNELS;i++) {
	  mon_meters[i]->setPeakBar(-10000);
	}
	mon_listen_button->setText(tr("Listen"));
	mon_listen_button->setStyleSheet("");
	mon_listen_button->setDisabled(true);
	mon_listening=false;
	mon_flash_timer->start(500);
	mon_connected=false;
      }
    }
  }
}

void Monitor::ProcessMeterUpdates(const QString &values)
{
  int level;
  bool ok=false;

  level=values.left(4).toInt(&ok,16);
  if(ok) {
    mon_meters[0]->setPeakBar(-level);
  }
  level=values.right(4).toInt(&ok,16);
  if(ok) {
    mon_meters[1]->setPeakBar(-level);
  }
}
