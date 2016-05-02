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

#include <stdint.h>

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
  QString mailAlertAddress() const;
  QString mailFromAddress() const;
  QString mailSmtpHostname() const;
  uint16_t mailSmtpPort() const;
  QString mailSmtpUsername() const;
  QString mailSmtpPassword() const;
  bool mailUseTls() const;
  unsigned monitorQuantity() const;
  QString monitorUrl(unsigned n) const;
  QString monitorLabel(unsigned n) const;
  void load();
  void save();
  static QString hostname();

 private:
  QString conf_global_jack_command_line;
  QString conf_global_jack_server_name;
  QString conf_mail_alert_address;
  QString conf_mail_from_address;
  QString conf_mail_smtp_hostname;
  uint16_t conf_mail_smtp_port;
  QString conf_mail_smtp_username;
  QString conf_mail_smtp_password;
  bool conf_mail_use_tls;
  std::vector<QString> conf_monitor_urls;
  std::vector<QString> conf_monitor_labels;
};


#endif  // CONFIG_H
