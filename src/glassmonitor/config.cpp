// config.cpp
//
// Configuration for GlassMonitor
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

#include "config.h"
#include "profile.h"

Config::Config()
{
}


QString Config::globalJackCommandLine() const
{
  return conf_global_jack_command_line;
}


QString Config::globalJackServerName() const
{
  return conf_global_jack_server_name;
}


QString Config::mailAlertAddress() const
{
  return conf_mail_alert_address;
}


QString Config::mailFromAddress() const
{
  return conf_mail_from_address;
}


QString Config::mailSmtpHostname() const
{
  return conf_mail_smtp_hostname;
}


uint16_t Config::mailSmtpPort() const
{
  return conf_mail_smtp_port;
}


QString Config::mailSmtpUsername() const
{
  return conf_mail_smtp_username;
}


QString Config::mailSmtpPassword() const
{
  return conf_mail_smtp_password;
}


bool Config::mailUseTls() const
{
  return conf_mail_use_tls;
}


unsigned Config::monitorQuantity() const
{
  return conf_monitor_urls.size();
}


QString Config::monitorUrl(unsigned n) const
{
  return conf_monitor_urls[n];
}


QString Config::monitorLabel(unsigned n) const
{
  return conf_monitor_labels[n];
}


void Config::load()
{
  Profile *p=new Profile();
  QString url;
  int count=0;
  bool ok=false;
  p->setSource(GLASSMONITOR_CONF_FILE);

  conf_global_jack_command_line=
    p->stringValue("Global","JackCommandLine","jackd -R -d alsa -d hw:0");
  conf_global_jack_server_name=p->stringValue("Global","JackServerName","");
  conf_mail_alert_address=p->stringValue("Mail","AlertAddress");
  conf_mail_from_address=
    p->stringValue("Mail","FromAddress","noreply@example.com");
  conf_mail_smtp_hostname=p->stringValue("Mail","SmtpHostname");
  conf_mail_smtp_port=p->intValue("Mail","SmtpPort");
  conf_mail_smtp_username=p->stringValue("Mail","SmtpUsername");
  conf_mail_smtp_password=p->stringValue("Mail","SmtpPassword");
  conf_mail_use_tls=p->boolValue("Mail","UseTls");

  QString section=QString().sprintf("Monitor%d",count+1);
  url=p->stringValue(section,"Url","",&ok);
  while(ok) {
    conf_monitor_urls.push_back(url);
    conf_monitor_labels.push_back(p->stringValue(section,"Label",
				   QString().sprintf("Stream %d",count+1)));
    count++;
    section=QString().sprintf("Monitor%d",count+1);
    url=p->stringValue(section,"Url","",&ok);
  }

  delete p;
}


void Config::save()
{
  FILE *f=NULL;
  QString tmpfile=QString(GLASSMONITOR_CONF_FILE)+"-temp";

  if((f=fopen(tmpfile.toUtf8(),"w"))!=NULL) {
    fprintf(f,"[Global]\n");
    fprintf(f,"JackCommandLine=%s\n",
	    (const char *)conf_global_jack_command_line.toUtf8());
    fprintf(f,"\n");
    fprintf(f,"[Mail]\n");
    fprintf(f,"AlertAddress=%s\n",
	    (const char *)conf_mail_alert_address.toUtf8());
    fprintf(f,"FromAddress=%s\n",
	    (const char *)conf_mail_from_address.toUtf8());
    fprintf(f,"SmtpHostname=%s\n",
	    (const char *)conf_mail_smtp_hostname.toUtf8());
    fprintf(f,"SmtpPort=%u\n",0xFFFF&conf_mail_smtp_port);
    fprintf(f,"SmtpUsername=%s\n",
	    (const char *)conf_mail_smtp_username.toUtf8());
    fprintf(f,"SmtpPassword=%s\n",
	    (const char *)conf_mail_smtp_password.toUtf8());
    fprintf(f,"UseTls=%d\n",conf_mail_use_tls);
    fprintf(f,"\n");
    for(unsigned i=0;i<conf_monitor_urls.size();i++) {
      fprintf(f,"[Monitor%d]\n",i+1);
      fprintf(f,"Url=%s\n",(const char *)conf_monitor_urls[i].toUtf8());
      fprintf(f,"\n");
    }
    fclose(f);
    rename(tmpfile.toUtf8(),GLASSMONITOR_CONF_FILE);
  }
}
