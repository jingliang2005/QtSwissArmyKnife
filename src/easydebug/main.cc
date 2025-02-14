﻿/*******************************************************************************
 * Copyright 2023 Qsaker(qsaker@foxmail.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part
 * of QtSwissArmyKnife project.
 *
 * QtSwissArmyKnife is licensed according to the terms in
 * the file LICENCE in the root of the source code directory.
 ******************************************************************************/
#include <QFile>
#include <QLoggingCategory>

#include "sakguiapplication.h"
#include "sakinterface.h"
#include "saklog.h"
#include "saksettings.h"

int main(int argc, char* argv[]) {
#ifndef QT_DEBUG
  qInstallMessageHandler(SAKLog::messageOutput);
#endif

  // Initialize some information about application.
  QCoreApplication::setOrganizationName(QString("Qsaker"));
  QCoreApplication::setOrganizationDomain(QString("IT"));
#ifdef SAK_RELEASE_FOR_APP_STORE
  QCoreApplication::setApplicationName(QString("EasyDebug"));
#else
  QCoreApplication::setApplicationName(QString("EasyDebug(Community)"));
#endif
#ifdef SAK_VERSION
  QCoreApplication::setApplicationVersion(SAK_VERSION);
#else
  QCoreApplication::setApplicationVersion("0.0.0");
#endif

  QLoggingCategory lc{"SAK.Main"};
  // Remove settings file and database
  if (SAKSettings::instance()->clearSettings()) {
    SAKSettings::instance()->setClearSettings(false);
    if (QFile::remove(SAKSettings::instance()->fileName())) {
      qCInfo(lc) << "Remove settings file successfully.";
    } else {
      qCWarning(lc) << "Remove settings file failed!";
    }
  }

  // High dpi settings.
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
  int policy = SAKSettings::instance()->hdpiPolicy();
  if (SAKInterface::isQtHighDpiScalePolicy(policy)) {
    auto cookedPolicy = Qt::HighDpiScaleFactorRoundingPolicy(policy);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(cookedPolicy);
  }
#endif

  SAKGuiApplication app(argc, argv);
  SAKLog::instance()->start();
  return app.exec();
}
