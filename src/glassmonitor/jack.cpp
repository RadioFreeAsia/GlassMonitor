// jack.cpp
//
// JACK routines for glassmonitor(1) Audio Receiver front end
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

#include <QStringList>
#include <QMessageBox>

#include "glassmonitor.h"

void GlassMonitor_JackPortConnect(jack_port_id_t a,jack_port_id_t b,int connect,
				  void *arg)
{
  //  printf("a: %d  b: %d  connect: %d\n",a,b,connect);
}


void MainWidget::jackProcessFinishedData(int exit_code,QProcess::ExitStatus status)
{
  if(status!=QProcess::NormalExit) {
    QMessageBox::warning(this,"GlassMonitor - "+tr("Process Error"),
			 tr("JACK process crashed!"));
    exit(256);
  }
  if(exit_code!=0) {
    QMessageBox::warning(this,"GlassMonitor - "+tr("Process Error"),
			 tr("JACK process returned non-zero exit code")+
			 QString().sprintf(": %d.",exit_code));
    exit(256);
  }
  exit(0);
}


void MainWidget::jackProcessErrorData(QProcess::ProcessError err)
{
  QMessageBox::warning(this,"GlassMonitor - "+tr("Process Error"),
		       tr("JACK process returned error")+
		       QString().sprintf(": %u.",err));
  exit(256);
}


void MainWidget::jackProcessKillData()
{
  glass_jack_process->kill();
  exit(0);
}


bool MainWidget::StartJack(QString *err_msg)
{
  QStringList args;
  QString cmd;
  jack_options_t jackopts=JackUseExactName;
  jack_status_t jackstat=JackFailure;

  //
  // Kill Timer
  //
  glass_jack_kill_timer=new QTimer(this);
  glass_jack_kill_timer->setSingleShot(true);
  connect(glass_jack_kill_timer,SIGNAL(timeout()),
	  this,SLOT(jackProcessKillData()));

  //
  // Start JACK Instance
  //
  if(!glass_config->globalJackCommandLine().isEmpty()) {
    args=glass_config->globalJackCommandLine().split(" ");
    cmd=args[0];
    args.erase(args.begin());
    glass_jack_process=new QProcess(this);
    connect(glass_jack_process,SIGNAL(finished(int,QProcess::ExitStatus)),
	    this,SLOT(jackProcessFinishedData(int,QProcess::ExitStatus)));
    connect(glass_jack_process,SIGNAL(error(QProcess::ProcessError)),
	    this,SLOT(jackProcessErrorData(QProcess::ProcessError)));
    glass_jack_process->start(cmd,args);
    glass_jack_process->waitForStarted();
  }

  //
  // Connect to JACK Instance
  //
  if(glass_config->globalJackServerName().isEmpty()) {
    glass_jack_client=
      jack_client_open("GlassMonitor",jackopts,&jackstat);
  }
  else {
    glass_jack_client=jack_client_open("GlassMonitor",jackopts,&jackstat,
		(const char *)glass_config->globalJackServerName().toUtf8());
  }
  if(glass_jack_client==NULL) {
    if((jackstat&JackInvalidOption)!=0) {
      *err_msg=tr("invalid or unsupported JACK option");
    }
    if((jackstat&JackServerError)!=0) {
      *err_msg=tr("communication error with the JACK server");
    }
    if((jackstat&JackNoSuchClient)!=0) {
      *err_msg=tr("requested JACK client does not exist");
    }
    if((jackstat&JackLoadFailure)!=0) {
      *err_msg=tr("unable to load internal JACK client");
    }
    if((jackstat&JackInitFailure)!=0) {
      *err_msg=tr("unable to initialize JACK client");
    }
    if((jackstat&JackShmFailure)!=0) {
      *err_msg=tr("unable to access JACK shared memory");
    }
    if((jackstat&JackVersionError)!=0) {
      *err_msg=tr("JACK protocol version mismatch");
    }
    if((jackstat&JackServerStarted)!=0) {
      *err_msg=tr("JACK server started");
    }
    if((jackstat&JackServerFailed)!=0) {
      fprintf (stderr, "unable to communication with JACK server\n");
      *err_msg=tr("unable to communicate with JACK server");
    }
    if((jackstat&JackNameNotUnique)!=0) {
      *err_msg=tr("JACK client name not unique");
    }
    if((jackstat&JackFailure)!=0) {
      *err_msg=tr("JACK general failure");
    }
    *err_msg=tr("no connection to JACK server");
    return false;
  }

  //
  // Set Callbacks
  //
  jack_set_port_connect_callback(glass_jack_client,GlassMonitor_JackPortConnect,
				 this);

  //
  // Join the Graph
  //
  if(jack_activate(glass_jack_client)) {
    *err_msg=tr("unable to join JACK graph");
    return false;
  }
  return true;
}


void MainWidget::StopJack()
{
  if(glass_jack_process==NULL) {
    exit(0);
  }
  glass_jack_process->terminate();
  glass_jack_kill_timer->start(3000);
}
