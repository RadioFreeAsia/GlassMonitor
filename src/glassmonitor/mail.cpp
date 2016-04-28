// mail.cpp
//
// Routines for processing e-mail messages on POSIX architectures.
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

#include <fstream>
#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>

#include <QMessageBox>
#include <QObject>

#include "mail.h"

void BlindCertificateAccepter::verify(vmime::ref<vmime::security::cert::certificateChain> certs)
{
  //
  // FIXME: We probably shouldn't just blindly accept all certs
  //
}


bool SendAlert(const QString &subj,const QString &msg,Config *config,
	       QWidget *parent)
{
  if(config->mailAlertAddress().isEmpty()) {
    return false;
  }
  return SendMail(QStringList(config->mailAlertAddress()),
		  QStringList(),QStringList(),config->mailFromAddress(),
		  "",subj,msg,config,parent);
}


bool SendMail(const QStringList &to_addrs,const QStringList &cc_addrs,
	      const QStringList &bcc_addrs,const QString &from_addr,
	      const QString &reply_addr,const QString &subj,
	      const QString &msg,Config *config,QWidget *parent)
{
  bool ret=true;

  try {
    //
    // Generate Message
    //
    vmime::messageBuilder mb;
    
    mb.setSubject(vmime::text(subj.toUtf8().constData()));
    mb.setExpeditor(vmime::mailbox(from_addr.toUtf8().constData()));
    for(int i=0;i<to_addrs.size();i++) {
      mb.getRecipients().
	appendAddress(vmime::create<vmime::mailbox>(to_addrs[i].toUtf8().constData()));
    }
    for(int i=0;i<cc_addrs.size();i++) {
      mb.getCopyRecipients().
	appendAddress(vmime::create<vmime::mailbox>(cc_addrs[i].toUtf8().constData()));
    }
    for(int i=0;i<bcc_addrs.size();i++) {
      mb.getBlindCopyRecipients().
	appendAddress(vmime::create<vmime::mailbox>(bcc_addrs[i].toUtf8().constData()));
    }
    mb.setSubject(vmime::text(subj.toUtf8().constData()));
    mb.getTextPart()->setText(vmime::create<vmime::stringContentHandler>(msg.toUtf8().constData()));

    vmime::ref<vmime::message> vmsg=mb.construct();
    vmime::utility::outputStreamAdapter out(std::cout);
    vmime::headerFieldFactory *factory=
      vmime::headerFieldFactory::getInstance();

    vmime::ref<vmime::headerField> replyto=
      factory->create(vmime::fields::REPLY_TO);
    replyto->setValue(vmime::mailbox(reply_addr.toUtf8().constData()));
    vmsg->getHeader()->appendField(replyto);

    //
    // Send Message
    //
    vmime::ref<vmime::net::session> sess=
      vmime::create<vmime::net::session>();
    if(config->mailSmtpPort()!=0) {
      sess->getProperties()["transport.smtp.server.port"]=
	config->mailSmtpPort();
    }
    vmime::utility::url url(("smtp://"+config->mailSmtpHostname()).toUtf8().
			    constData());
    vmime::ref<vmime::net::transport> trans=sess->getTransport(url); 
    if(!config->mailSmtpUsername().isEmpty()) {
      trans->setProperty("options.need-authentication",true);
      trans->setProperty("connection.tls",true);
      trans->setProperty("auth.username",
			 config->mailSmtpUsername().toUtf8().constData());
      trans->setProperty("auth.password",
			 config->mailSmtpPassword().toUtf8().constData());
      trans->setCertificateVerifier(vmime::create<BlindCertificateAccepter>());
    }
    trans->connect();
    trans->send(vmsg);
    trans->disconnect();
  }
  catch(vmime::exception &e) {
    QMessageBox::warning(parent,"GlassMonitor - "+QObject::tr("Mail Error"),
			 e.what());
    ret=false;
  }

  return ret;
}
