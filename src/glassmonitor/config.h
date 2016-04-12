// config.h
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

#ifndef CONFIG_H
#define CONFIG_H

#include <vector>

#include <QString>

#define GLASSMONITOR_CONF_FILE "/etc/glassmonitor.conf"
#define GLASSMONITOR_MAX_AUDIO_CHANNELS 2
class Config
{
 public:
  Config();
  QString globalJackCommandLine() const;
  QString globalJackServerName() const;
  unsigned monitorQuantity() const;
  QString monitorUrl(unsigned n) const;
  QString monitorLabel(unsigned n) const;
  void load();
  void save();

 private:
  QString conf_global_jack_command_line;
  QString conf_global_jack_server_name;
  std::vector<QString> conf_monitor_urls;
  std::vector<QString> conf_monitor_labels;
};


#endif  // CONFIG_H
