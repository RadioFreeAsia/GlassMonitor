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
    for(unsigned i=0;i<conf_monitor_urls.size();i++) {
      fprintf(f,"[Monitor%d]\n",i+1);
      fprintf(f,"Url=%s\n",(const char *)conf_monitor_urls[i].toUtf8());
      fprintf(f,"\n");
    }
    fclose(f);
    rename(tmpfile.toUtf8(),GLASSMONITOR_CONF_FILE);
  }
}
