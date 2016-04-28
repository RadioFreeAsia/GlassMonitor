// mail.h
//
// Routines for processing e-mail messages.
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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

#ifndef MAIL_H
#define MAIL_H

#include <vmime/vmime.hpp>

#include <QString>
#include <QStringList>

#include "config.h"

class BlindCertificateAccepter : public vmime::security::cert::certificateVerifier {
public:
  void verify(vmime::ref<vmime::security::cert::certificateChain> certs);
};

bool SendAlert(const QString &subj,const QString &msg,Config *config,
	       QWidget *parent=0);
bool SendMail(const QStringList &to_addrs,const QStringList &cc_addrs,
	      const QStringList &bcc_addrs,const QString &from_addr,
	      const QString &reply_addr,const QString &subj,
	      const QString &msg,Config *config,QWidget *parent);


#endif   // MAIL_H
