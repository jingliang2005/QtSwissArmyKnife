﻿/*******************************************************************************
 * Copyright 2018-2023 Qsaker(qsaker@foxmail.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part
 * of QtSwissArmyKnife project.
 *
 * QtSwissArmyKnife is licensed according to the terms in
 * the file LICENCE in the root of the source code directory.
 ******************************************************************************/
#include "sakmainwindow.h"

#include <QAction>
#include <QActionGroup>
#include <QButtonGroup>
#include <QClipboard>
#include <QCloseEvent>
#include <QDateTime>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileInfoList>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QLineEdit>
#include <QLocale>
#include <QMessageBox>
#include <QMetaEnum>
#include <QPainter>
#include <QPixmap>
#include <QProcess>
#include <QRect>
#include <QScrollArea>
#include <QScrollBar>
#include <QSettings>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QStatusBar>
#include <QStyleFactory>
#include <QSysInfo>
#include <QTabBar>
#include <QTextBrowser>
#include <QToolBar>
#include <QToolButton>
#include <QVariant>

#include "sakdatastructure.h"
#include "sakinterface.h"
#include "saklogui.h"
#include "saksettings.h"
#include "saktoolboxui.h"
#include "saktranslator.h"
#include "sakuiinterface.h"
#include "sakassistantsfactory.h"

#ifdef SAK_IMPORT_MODULE_CANBUSSTUDIO
#include "sakcanbusstudioui.h"
#endif

#ifdef SAK_IMPORT_MODULE_MODBUSSTUDIO
#include "sakmodbusui.h"
#endif

#ifdef SAK_IMPORT_MODULE_PRIVATE_MODBUS
#include "SAKPrivateModbusClient.h"
#include "SAKPrivateModbusServer.h"
#endif

#include "ui_sakmainwindow.h"

#define SAK_QT_CONF qApp->applicationDirPath() + "/qt.conf"

QString palettePath() {
  QString fileName = SAKSettings::instance()->fileName();
  QUrl url(fileName);
  QString path = fileName;
  QString logPath = path.remove(url.fileName());
  logPath += "palette";
  return logPath;
}

SAKMainWindow::SAKMainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::SAKMainWindow) {
  ui->setupUi(this);

  QDir dir;
  if (dir.exists(palettePath())) {
    qCInfo(mLoggingCategory) << "palette path is:" << palettePath();
  } else {
    if (dir.mkdir(palettePath())) {
      qCInfo(mLoggingCategory) << palettePath() << "has been created";
    } else {
      qCWarning(mLoggingCategory) << palettePath() << "has been created";
    }
  }

#ifdef Q_OS_ANDROID
  setWindowFlags(Qt::FramelessWindowHint);
  QScrollArea* scrollArea = new QScrollArea(this);
  scrollArea->setWidgetResizable(true);
  setCentralWidget(scrollArea);
  scrollArea->setWidget(mTabWidget);
#else

#ifdef SAK_RELEASE_FOR_APP_STORE
  QString title = QString("QSAK");
#else
  QString title = QString("Qt Swiss Army Knife");
#endif
  title.append(QString(" "));
  title.append(QString("v"));
  title.append(qApp->applicationVersion());
#ifndef SAK_RELEASE_FOR_APP_STORE
  title.append(" ");
  title.append(SAK_EDITION);
#endif
  setWindowTitle(title);
  setWindowIcon(QIcon(":/resources/images/SAKLogo.png"));
#endif
  initMenuBar();
  initNav();
  initStatusBar();
}

SAKMainWindow::~SAKMainWindow() { delete ui; }

void SAKMainWindow::initMenuBar() {
  menuBar()->setPalette(qApp->palette());

  initFileMenu();
  initToolMenu();
  initOptionMenu();
  initLanguageMenu();
#ifndef SAK_RELEASE_FOR_APP_STORE
  initLinksMenu();
  initDemoMenu();
#endif
  initHelpMenu();
}

#ifdef Q_OS_WIN
void SAKMainWindow::closeEvent(QCloseEvent* event) {
  auto key = mSettingsKey.exitToSystemTray;
  bool ignore = SAKSettings::instance()->value(key).toBool();
  if (ignore) {
    this->hide();
    event->ignore();
  }
}
#endif

void SAKMainWindow::initFileMenu() {
  QMenu* fileMenu = new QMenu(tr("&File"), this);
  menuBar()->addMenu(fileMenu);
  // Tool box
  QMenu* windowMenu = new QMenu(tr("New Window"), this);
  fileMenu->addMenu(windowMenu);
  QList<int> toolTypeList = SAKToolBoxUi::supportedCommuniticationTools();
  for (auto& toolType : toolTypeList) {
    const QString name = SAKToolBoxUi::communiticationToolName(toolType);
    QAction* action = new QAction(name, this);
    windowMenu->addAction(action);
    connect(action, &QAction::triggered, this, [=]() {
      SAKToolBoxUi* w = new SAKToolBoxUi();
      w->setContentsMargins(9, 9, 9, 9);
      w->setAttribute(Qt::WA_DeleteOnClose, true);
      w->initialize(toolType);
      w->show();
    });
  }

  // Other tools
#ifdef SAK_IMPORT_MODULE_MODBUSSTUDIO
  QAction* modbusAction = new QAction("Modbus Studio", this);
  connect(modbusAction, &QAction::triggered, this, [=]() {
    SAKModbusUi* w = new SAKModbusUi();
    w->setContentsMargins(9, 9, 9, 9);
    w->setAttribute(Qt::WA_DeleteOnClose, true);
    w->resize(1024, 480);
    w->show();
  });
  windowMenu->addAction(modbusAction);
#endif
#ifndef SAK_RELEASE_FOR_APP_STORE
#ifdef SAK_IMPORT_MODULE_CANBUSSTUDIO
  QAction* canbusAction = new QAction("CANBus Studio", this);
  connect(canbusAction, &QAction::triggered, this, [=]() {
    SAKCanBusUi* w = new SAKCanBusUi();
    w->setContentsMargins(9, 9, 9, 9);
    w->setAttribute(Qt::WA_DeleteOnClose, true);
    w->resize(1024, 480);
    w->show();
  });
  windowMenu->addAction(canbusAction);
#endif
#endif

  fileMenu->addSeparator();
  QAction* importAction = new QAction(tr("Import Palette"), fileMenu);
  fileMenu->addAction(importAction);
  connect(importAction, &QAction::triggered, this,
          &SAKMainWindow::onImportActionTriggered);

  QAction* exportAction = new QAction(tr("Export Palette"), fileMenu);
  fileMenu->addAction(exportAction);
  connect(exportAction, &QAction::triggered, this,
          &SAKMainWindow::onExportActionTriggered);

  fileMenu->addSeparator();
  QAction* exitAction = new QAction(tr("Exit"), this);
  fileMenu->addAction(exitAction);
  connect(exitAction, SIGNAL(triggered(bool)), this, SLOT(close()));
}

void SAKMainWindow::initToolMenu() {
  QMenu* toolMenu = new QMenu(tr("&Tools"));
  menuBar()->addMenu(toolMenu);

  for (auto& t : SAKAssistantsFactory::Instance()->SupportedAssistants()) {
    QString name = SAKAssistantsFactory::Instance()->GetAssistantName(t);
    QAction* action = new QAction(name, this);
    QWidget* w = SAKAssistantsFactory::Instance()->NewAssistant(t);

    Q_ASSERT_X(action, __FUNCTION__, "A null action!");
    Q_ASSERT_X(w, __FUNCTION__, "A null assistant widget!");

    w->hide();
    toolMenu->addAction(action);
    connect(action, &QAction::triggered, this, [=]() {
      if (w->isHidden()) {
        w->show();
      } else {
        w->activateWindow();
      }
    });
  }
}

void SAKMainWindow::initOptionMenu() {
  QMenu* optionMenu = new QMenu(tr("&Options"));
  menuBar()->addMenu(optionMenu);

  initOptionMenuAppStyleMenu(optionMenu);
#ifdef Q_OS_WIN
  initOptionMenuMainWindowMenu(optionMenu);
#endif
  initOptionMenuSettingsMenu(optionMenu);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
  initOptionMenuHdpiPolicy(optionMenu);
#endif
  initOptionMenuPalette(optionMenu);
}

void SAKMainWindow::initOptionMenuAppStyleMenu(QMenu* optionMenu) {
  // Initializing application style menu.
  static QActionGroup gActionGroup(this);
  auto actions = gActionGroup.actions();
  for (auto action : actions) {
    gActionGroup.removeAction(action);
  }

  QMenu* appStyleMenu = new QMenu(tr("Application Style"), this);
  optionMenu->addMenu(appStyleMenu);
  auto keys = QStyleFactory::keys();
  for (QString& key : keys) {
    QAction* action = new QAction(key, this);
    action->setObjectName(key);
    action->setCheckable(true);

    gActionGroup.addAction(action);

    connect(action, &QAction::triggered, this, [=]() {
      SAKSettings::instance()->setAppStyle(key);
      rebootRequestion();
    });
  }

  appStyleMenu->addActions(gActionGroup.actions());

  // Reading the specified style.
  QString style = SAKSettings::instance()->appStyle();

  if (style.isEmpty()) {
    return;
  }

  for (QAction*& action : gActionGroup.actions()) {
    if (action->objectName() == style) {
      action->setChecked(true);
      break;
    }
  }
}

void SAKMainWindow::initOptionMenuMainWindowMenu(QMenu* optionMenu) {
  if (!optionMenu) {
    return;
  }

  QMenu* mainWindowMenu = new QMenu(tr("Main Window"), this);
  QAction* action = new QAction(tr("Exit to Sysytem Tray"), this);
  action->setCheckable(true);
  mainWindowMenu->addAction(action);
  optionMenu->addMenu(mainWindowMenu);

  QVariant v = SAKSettings::instance()->value(mSettingsKey.exitToSystemTray);
  if (!v.isNull()) {
    bool isExitToSystemTray = v.toBool();
    action->setChecked(isExitToSystemTray);
  }

  connect(action, &QAction::triggered, this, [=]() {
    bool keep = action->isChecked();
    SAKSettings::instance()->setValue(mSettingsKey.exitToSystemTray, keep);
  });
}

void SAKMainWindow::initOptionMenuSettingsMenu(QMenu* optionMenu) {
  QMenu* menu = new QMenu(tr("Settings"), this);
  optionMenu->addMenu(menu);

  QAction* action = new QAction(tr("Clear Configuration"), this);
  menu->addAction(action);
  connect(action, &QAction::triggered, this,
          &SAKMainWindow::clearConfiguration);
  action = new QAction(tr("Open configuration floder"), this);
  menu->addAction(action);
  connect(action, &QAction::triggered, this, [=]() {
    QString fileName = SAKSettings::instance()->fileName();
    QUrl fileUrl = QUrl(fileName);
    QString floderUrl = fileName.remove(fileUrl.fileName());
    QDesktopServices::openUrl(floderUrl);
  });
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
void SAKMainWindow::initOptionMenuHdpiPolicy(QMenu* optionMenu) {
  QMenu* menu = new QMenu(tr("HDPI Policy"));
  QActionGroup* ag = new QActionGroup(this);

  QAction* roundAction = new QAction(tr("Round up for .5 and above"), this);
  QAction* ceilAction = new QAction(tr("Always round up"), this);
  QAction* floorAction = new QAction(tr("Always round down"), this);
  QAction* rpfAction = new QAction(tr("Round up for .75 and above"), this);
  QAction* passThroughAction = new QAction(tr("Don't round"), this);

  int round = SAKDataStructure::HdpiPolicyRound;
  int ceil = SAKDataStructure::HdpiPolicyCeil;
  int floor = SAKDataStructure::HdpiPolicyFloor;
  int preferFloor = SAKDataStructure::HdpiPolicyRoundPreferFloor;
  int passThrough = SAKDataStructure::HdpiPolicyPassThrough;
  int sysScale = SAKDataStructure::HdpiPolicySystem;

  QString fileName = SAK_QT_CONF;
  auto triggered = [=](int policy) {
    if (QFile::remove(fileName)) {
      qCInfo(mLoggingCategory) << fileName << "was removed!";
    } else {
      qCInfo(mLoggingCategory) << "removed" << fileName << "failed";
    }

    SAKSettings::instance()->setHdpiPolicy(int(policy));
    rebootRequestion();
  };

  connect(roundAction, &QAction::triggered, this,
          [=]() { triggered(int(round)); });
  connect(ceilAction, &QAction::triggered, this,
          [=]() { triggered(int(ceil)); });
  connect(floorAction, &QAction::triggered, this,
          [=]() { triggered(int(floor)); });
  connect(rpfAction, &QAction::triggered, this,
          [=]() { triggered(int(preferFloor)); });
  connect(passThroughAction, &QAction::triggered, this,
          [=]() { triggered(int(passThrough)); });

  ag->addAction(roundAction);
  ag->addAction(ceilAction);
  ag->addAction(floorAction);
  ag->addAction(rpfAction);
  ag->addAction(passThroughAction);

  auto list = ag->actions();
  for (auto& a : list) {
    a->setCheckable(true);
  }

  if (SAKSettings::instance()->hdpiPolicy() == round) {
    roundAction->setChecked(true);
  } else if (SAKSettings::instance()->hdpiPolicy() == ceil) {
    ceilAction->setChecked(true);
  } else if (SAKSettings::instance()->hdpiPolicy() == floor) {
    floorAction->setChecked(true);
  } else if (SAKSettings::instance()->hdpiPolicy() == preferFloor) {
    rpfAction->setChecked(true);
  } else if (SAKSettings::instance()->hdpiPolicy() == passThrough) {
    passThroughAction->setChecked(true);
  }

  menu->addActions(ag->actions());
  optionMenu->addMenu(menu);

#ifdef Q_OS_WIN
  QAction* systemAction = new QAction(tr("System"), this);
  systemAction->setCheckable(true);
  ag->addAction(systemAction);
  menu->addSeparator();
  menu->addAction(systemAction);
  connect(systemAction, &QAction::triggered, this, [=]() {
    createQtConf();

    SAKSettings::instance()->setHdpiPolicy(sysScale);
    rebootRequestion();
  });

  if (SAKSettings::instance()->hdpiPolicy() == sysScale) {
    systemAction->setChecked(true);
    if (!QFile::exists(fileName)) {
      createQtConf();
    }
  }
#else
  Q_UNUSED(sysScale)
#endif
}
#endif

void SAKMainWindow::initOptionMenuPalette(QMenu* optionMenu) {
  static QActionGroup ag(this);
  QAction* systemAction = new QAction(tr("System"), this);
  QAction* lightAction = new QAction(tr("Light"), this);
  QAction* darkAction = new QAction(tr("Dark"), this);
  QMenu* m = new QMenu(tr("Palette"), optionMenu);
  optionMenu->addMenu(m);
  ag.addAction(systemAction);
  ag.addAction(lightAction);
  ag.addAction(darkAction);

  systemAction->setCheckable(true);
  lightAction->setCheckable(true);
  darkAction->setCheckable(true);

  int ret = SAKSettings::instance()->palette();
  if (ret == SAKDataStructure::PaletteLight) {
    lightAction->setChecked(true);
  } else if (ret == SAKDataStructure::PaletteDark) {
    darkAction->setChecked(true);
  } else if (ret == SAKDataStructure::PaletteSystem) {
    systemAction->setChecked(true);
  }

  m->addAction(systemAction);
  m->addAction(lightAction);
  m->addAction(darkAction);

  connect(systemAction, &QAction::triggered, this, [=]() {
    SAKSettings::instance()->setPalette(SAKDataStructure::PaletteSystem);
    rebootRequestion();
  });
  connect(lightAction, &QAction::triggered, this, [=]() {
    SAKSettings::instance()->setPalette(SAKDataStructure::PaletteLight);
    rebootRequestion();
  });
  connect(darkAction, &QAction::triggered, this, [=]() {
    SAKSettings::instance()->setPalette(SAKDataStructure::PaletteDark);
    rebootRequestion();
  });

  QMenu* custom = new QMenu(tr("Custom"), this);
  QDir dir(palettePath());
  QFileInfoList infoList =
      dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
  QString currentCustom = SAKSettings::instance()->customPalette();
  for (auto& info : infoList) {
    if (!info.isFile()) {
      continue;
    }

    QString fileName = info.fileName();
    QUrl url(fileName);
    QString fn = url.fileName();
    QAction* a = new QAction(fn, this);
    a->setCheckable(true);
    ag.addAction(a);
    if (fn == currentCustom && ret == SAKDataStructure::PaletteCustom) {
      a->setChecked(true);
    }

    custom->addAction(a);
    connect(a, &QAction::triggered, this, [=]() {
      SAKSettings* settings = SAKSettings::instance();
      settings->setPalette(SAKDataStructure::PaletteCustom);
      settings->setCustomPalette(fileName);
      rebootRequestion();
    });
  }

  if (!custom->actions().isEmpty()) {
    m->addMenu(custom);
  }
}

void SAKMainWindow::initLanguageMenu() {
  QMenu* languageMenu = new QMenu(tr("&Languages"), this);
#if 0
    QIcon icon = QIcon(":/resources/icon/IconLanguage.svg");
    languageMenu->setIcon(icon);
#endif
  menuBar()->addMenu(languageMenu);

  static QActionGroup ag(this);
  QStringList languages = SAKTranslator::instance()->languanges();
  for (auto& language : languages) {
    QAction* action = new QAction(language, this);
    action->setCheckable(true);
    languageMenu->addAction(action);
    ag.addAction(action);

    connect(action, &QAction::triggered, this, [=]() {
      SAKSettings::instance()->setLanguage(language);
      rebootRequestion();
    });

    QString l = SAKSettings::instance()->language();
    if (l == language) {
      action->setChecked(true);
    }
  }
}

void SAKMainWindow::initHelpMenu() {
  QMenu* helpMenu = new QMenu(tr("&Help"), this);
  menuBar()->addMenu(helpMenu);
#ifndef SAK_RELEASE_FOR_APP_STORE
  QAction* aboutQtAction = new QAction(tr("About Qt"), this);
  helpMenu->addAction(aboutQtAction);
  connect(aboutQtAction, &QAction::triggered, this,
          [=]() { QMessageBox::aboutQt(this, tr("About Qt")); });
#endif

  QAction* aboutAction = new QAction(tr("About QtSwissArmyKnife"), this);
  helpMenu->addAction(aboutAction);
  connect(aboutAction, &QAction::triggered, this,
          &SAKMainWindow::aboutSoftware);
#ifdef Q_OS_WIN
  QString tips = tr("Buy from Microsoft App Store");
  QIcon buy(":/resources/icon/IconBuy.svg");
  QAction* microsoft = new QAction(buy, tips);
  helpMenu->addAction(microsoft);
  connect(microsoft, &QAction::triggered, this, []() {
    QUrl url("https://www.microsoft.com/store/apps/9P29H1NDNKBB");
    QDesktopServices::openUrl(url);
  });
#endif
#ifndef SAK_RELEASE_FOR_APP_STORE
  QMenu* srcMenu = new QMenu(tr("Get Source"), this);
  helpMenu->addMenu(srcMenu);
  QAction* visitGitHubAction =
      new QAction(QIcon(":/resources/images/GitHub.png"), tr("GitHub"), this);
  connect(visitGitHubAction, &QAction::triggered, []() {
    QUrl url = QUrl(QLatin1String(SAK_GITHUB_REPOSITORY_URL));
    QDesktopServices::openUrl(url);
  });
  srcMenu->addAction(visitGitHubAction);
  QAction* visitGiteeAction =
      new QAction(QIcon(":/resources/images/Gitee.png"), tr("Gitee"), this);
  connect(visitGiteeAction, &QAction::triggered, []() {
    QUrl url = QUrl(QLatin1String(SAK_GITEE_REPOSITORY_URL));
    QDesktopServices::openUrl(url);
  });
  srcMenu->addAction(visitGiteeAction);

  QAction* releaseHistoryAction = new QAction(tr("Release History"), this);
  helpMenu->addAction(releaseHistoryAction);
  connect(releaseHistoryAction, &QAction::triggered, this,
          &SAKMainWindow::showHistory);

  helpMenu->addSeparator();
  QAction* qrCodeAction = new QAction(tr("QR Code"), this);
  helpMenu->addAction(qrCodeAction);
  connect(qrCodeAction, &QAction::triggered, this, &SAKMainWindow::showQrCode);

  helpMenu->addAction(tr("Donate"), this, &SAKMainWindow::showDonation);
#endif
}

void SAKMainWindow::initLinksMenu() {
  QMenu* linksMenu = new QMenu(tr("&Links"), this);
  menuBar()->addMenu(linksMenu);

  struct Link {
    QString name;
    QString url;
    QString iconPath;
  };
  QList<Link> linkList;
  linkList << Link{tr("Qt Official Download"),
                   QString("http://download.qt.io/official_releases/qt"),
                   QString(":/resources/images/Qt.png")}
           << Link{tr("Qt Official Blog"), QString("https://www.qt.io/blog"),
                   QString(":/resources/images/Qt.png")}
           << Link{tr("Qt Official Release"),
                   QString("https://wiki.qt.io/Qt_5.15_Release"),
                   QString(":/resources/images/Qt.png")}
           << Link{tr("Download SAK from Github"),
                   QString("%1/releases").arg(SAK_GITHUB_REPOSITORY_URL),
                   QString(":/resources/images/GitHub.png")}
           << Link{tr("Download SAK from Gitee"),
                   QString("%1/releases").arg(SAK_GITEE_REPOSITORY_URL),
                   QString(":/resources/images/Gitee.png")}
           << Link{tr("Office Web Site"),
                   QString("https://qsaker.gitee.io/qsak/"),
                   QString(":/resources/images/Gitee.png")};

  for (auto& var : linkList) {
    QAction* action = new QAction(QIcon(var.iconPath), var.name, this);
    action->setObjectName(var.url);
    linksMenu->addAction(action);

    connect(action, &QAction::triggered, this,
            [=]() { QDesktopServices::openUrl(QUrl(sender()->objectName())); });
  }
}

void SAKMainWindow::initDemoMenu() {
  QMenu* demoMenu = new QMenu(tr("&Demo"), this);
  menuBar()->addMenu(demoMenu);

  struct Link {
    QString name;
    QString url;
    QString iconPath;
  };
  QList<Link> linkList;
  QString baseUrl = "https://gitee.com/qsaker";
  linkList << Link{tr("Qt SerialPort Demo"),
                   QString(baseUrl + "/qt-demo-serial-port-widget.git"),
                   QString(":/resources/images/Qt.png")};

  for (auto& var : linkList) {
    QAction* action = new QAction(QIcon(var.iconPath), var.name, this);
    action->setObjectName(var.url);
    demoMenu->addAction(action);

    connect(action, &QAction::triggered, this,
            [=]() { QDesktopServices::openUrl(QUrl(sender()->objectName())); });
  }
}

void SAKMainWindow::initNav() {
  QToolBar* tb = new QToolBar(this);
  addToolBar(Qt::LeftToolBarArea, tb);
  tb->setFloatable(false);
  tb->setMovable(false);
  tb->setOrientation(Qt::Vertical);
  tb->setAllowedAreas(Qt::LeftToolBarArea);

  static QButtonGroup navButtonGroup;
  QList<int> types = SAKToolBoxUi::supportedCommuniticationTools();
  for (int i = 0; i < types.count(); i++) {
    int type = types.at(i);
    SAKToolBoxUi* toolBoxUi = new SAKToolBoxUi(this);
    toolBoxUi->initialize(type);

    initNav(&navButtonGroup,
            SAKUiInterface::cookedIcon(toolBoxUi->windowIcon()),
            toolBoxUi->windowTitle(), toolBoxUi, tb);
  }

  tb->addSeparator();

  QString path = ":/resources/icon/IconModbus.svg";
#ifdef SAK_IMPORT_MODULE_MODBUSSTUDIO
  SAKModbusUi* modbus = new SAKModbusUi(this);
  initNav(&navButtonGroup, SAKUiInterface::cookedIcon(QIcon(path)),
          "Modbus Studio", modbus, tb);
#ifdef SAK_IMPORT_MODULE_PRIVATE_MODBUS
  SAKPrivateModbusClient* modbusClient = new SAKPrivateModbusClient(this);
  SAKPrivateModbusServer* modbusServer = new SAKPrivateModbusServer(this);
  path = ":/resources/icon/IconModbus.svg";
  initNav(&navButtonGroup, SAKUiInterface::cookedIcon(QIcon(path)),
          "Modbus Master", modbusClient, tb);
  path = ":/resources/icon/IconModbus.svg";
  initNav(&navButtonGroup, SAKUiInterface::cookedIcon(QIcon(path)),
          "Modbus Slave", modbusServer, tb);
  modbusClient->importProject();
  modbusServer->importProject();
#endif
#endif
#ifndef SAK_RELEASE_FOR_APP_STORE
#ifdef SAK_IMPORT_MODULE_CANBUSSTUDIO
  SAKCanBusUi* canbus = new SAKCanBusUi(this);
  path = ":/resources/icon/IconCanBus.svg";
  initNav(&navButtonGroup, SAKUiInterface::cookedIcon(QIcon(path)),
          "CANBus Studio", canbus, tb);
#endif
#endif
  QLabel* lb = new QLabel(" ");
  tb->addWidget(lb);
  lb->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

#if 1
  tb->addSeparator();
  bool isTextBesideIcon = SAKSettings::instance()->isTextBesideIcon();
  auto style =
      isTextBesideIcon ? Qt::ToolButtonTextBesideIcon : Qt::ToolButtonIconOnly;
  QToolButton* tbt = new QToolButton(this);
  path = ":/resources/icon/IconListWithIcon.svg";
  tbt->setIcon(SAKUiInterface::cookedIcon(QIcon(path)));
  tbt->setCheckable(true);
  tbt->setText(" " + tr("Hide Text"));
  tbt->setToolTip(tr("Click to show(hide) nav text"));
  tbt->setAutoRaise(true);
  tbt->setChecked(isTextBesideIcon);
  tbt->setToolButtonStyle(style);
  tbt->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  tb->addWidget(tbt);
  auto bg = &navButtonGroup;
  connect(tbt, &QToolButton::clicked, tbt, [=]() {
    auto bts = bg->buttons();
    auto style = tbt->isChecked() ? Qt::ToolButtonTextBesideIcon
                                  : Qt::ToolButtonIconOnly;
    tbt->setToolButtonStyle(style);
    for (auto& bt : bts) {
      auto cookedBt = qobject_cast<QToolButton*>(bt);

      cookedBt->setToolButtonStyle(style);
    }
    SAKSettings::instance()->setIsTextBesideIcon(tbt->isChecked());
  });
  tb->addSeparator();
#endif

  initNav(&navButtonGroup,
          SAKUiInterface::cookedIcon(QIcon(":/resources/icon/IconLog.svg")),
          tr("Log Viewer"), new SAKLogUi(this), tb);
#if 0
    path = ":/resources/icon/IconSettings.svg";
    initNav(&navButtonGroup,
            SAKUiInterface::cookedIcon(QIcon(path)),
            tr("Preferences"), new SAKPreferencesUi(this), tb);
#endif
}

void SAKMainWindow::initNav(QButtonGroup* bg, const QIcon& icon,
                            const QString& name, QWidget* page, QToolBar* tb) {
  bool isTextBesideIcon = SAKSettings::instance()->isTextBesideIcon();
  auto style =
      isTextBesideIcon ? Qt::ToolButtonTextBesideIcon : Qt::ToolButtonIconOnly;
  QToolButton* bt = new QToolButton();
  bt->setAutoRaise(true);
  bt->setIcon(icon);
  bt->setCheckable(true);
  bt->setToolButtonStyle(style);
  bt->setToolTip(name);
  bt->setText(" " + name);
  bt->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
#if 0
    bt->setIconSize(QSize(32, 32));
#endif
  bg->addButton(bt);
  tb->addWidget(bt);

  if (page->layout()) {
    page->layout()->setContentsMargins(0, 0, 0, 0);
  }
  ui->stackedWidget->addWidget(page);

  int pageCount = bg->buttons().count();
  QObject::connect(bt, &QToolButton::clicked, bt, [=]() {
    ui->stackedWidget->setCurrentIndex(pageCount - 1);
    SAKSettings::instance()->setPageIndex(pageCount - 1);
  });

  if (SAKSettings::instance()->pageIndex() == (pageCount - 1)) {
    bt->setChecked(true);
    ui->stackedWidget->setCurrentIndex(pageCount - 1);
  }
}

void SAKMainWindow::initStatusBar() {
  ui->statusbar->showMessage("Hello world", 10 * 1000);
  ui->statusbar->hide();
}

void SAKMainWindow::aboutSoftware() {
  struct Info {
    QString name;
    QString value;
    bool valueIsUrl;
  };

  QString format = QLocale::system().dateFormat();
  format = format + " " + QLocale::system().timeFormat();
  QString dateTimeString = SAKInterface::buildDateTime(format);
  QList<Info> infoList;
  infoList << Info{tr("Version"), QString(qApp->applicationVersion()), false}
#ifndef SAK_RELEASE_FOR_APP_STORE
           << Info{tr("Edition"), SAK_EDITION, false}
#endif
           << Info{tr("Author"), QString(SAK_AUTHOR), false}
           << Info{tr("Email"), QString(SAK_AUTHOR_EMAIL), false}
           << Info{tr("QQ"), QString("QQ:2869470394"), false}
           << Info{tr("QQ Group"), QString("QQ:952218522"), false}
           << Info{tr("Build Time"), dateTimeString, false}
#ifndef SAK_RELEASE_FOR_APP_STORE
           << Info{tr("Gitee Url"),
                   QString("<a href=%1>%1</a>").arg(SAK_GITEE_REPOSITORY_URL),
                   true}
           << Info{tr("Gitbub Url"),
                   QString("<a href=%1>%1</a>").arg(SAK_GITHUB_REPOSITORY_URL),
                   true}
#endif
           << Info{tr("Copyright"),
                   tr("Copyright 2018-%1 Qsaker(qsaker@foxmail.com)."
                      " All rights reserved.")
                       .arg(SAKInterface::buildDateTime("yyyy")),
                   false};

  QDialog dialog(this);
  dialog.setWindowTitle(tr("About QSAK"));

  QGridLayout* gridLayout = new QGridLayout(&dialog);
  int i = 0;
  for (auto& var : infoList) {
    QLabel* nameLabel = new QLabel(
        QString("<font color=green>%1</font>").arg(var.name), &dialog);
    QLabel* valueLabel = new QLabel(var.value, &dialog);
    gridLayout->addWidget(nameLabel, i, 0, 1, 1);
    gridLayout->addWidget(valueLabel, i, 1, 1, 1);
    i += 1;

    if (var.valueIsUrl) {
      connect(valueLabel, &QLabel::linkActivated,
              [](QString url) { QDesktopServices::openUrl(QUrl(url)); });
    }
  }
  dialog.setLayout(gridLayout);
  dialog.setModal(true);
  dialog.show();
  dialog.exec();
}

void SAKMainWindow::clearConfiguration() {
  SAKSettings::instance()->setClearSettings(true);
  rebootRequestion();
}

void SAKMainWindow::rebootRequestion() {
  int ret =
      QMessageBox::information(this, tr("Reboot application to effective"),
                               tr("Need to reboot, reboot to effective now?"),
                               QMessageBox::Ok | QMessageBox::Cancel);
  if (ret == QMessageBox::Ok) {
    QProcess::startDetached(QCoreApplication::applicationFilePath());

    qApp->closeAllWindows();
    qApp->exit();
  }
}

void SAKMainWindow::showHistory() {
  QDialog dialog;
  dialog.setModal(true);
  dialog.setWindowTitle(tr("Release History"));
  dialog.resize(600, 400);

  QTextBrowser* textBrowser = new QTextBrowser(&dialog);
  QFile file(":/resources/files/History.txt");
  if (file.open(QFile::ReadOnly)) {
    QByteArray data = file.readAll();
    textBrowser->setText(QString::fromUtf8(data));
  }

  QHBoxLayout* layout = new QHBoxLayout(&dialog);
  layout->addWidget(textBrowser);
  dialog.setLayout(layout);
  dialog.show();
  dialog.exec();
}

void SAKMainWindow::showQrCode() {
  QDialog dialog;
  dialog.setWindowTitle(tr("QR Code"));

  struct QrCodeInfo {
    QString title;
    QString qrCode;
  };
  QList<QrCodeInfo> qrCodeInfoList;

  qrCodeInfoList << QrCodeInfo{tr("User QQ Group"),
                               QString(":/resources/images/QSAKQQ.jpg")}
                 << QrCodeInfo{tr("Qt QQ Group"),
                               QString(":/resources/images/QtQQ.jpg")};

  QTabWidget* tabWidget = new QTabWidget(&dialog);
  for (auto& var : qrCodeInfoList) {
    QLabel* label = new QLabel(tabWidget);
    label->setPixmap(QPixmap::fromImage(QImage(var.qrCode)));
    tabWidget->addTab(label, var.title);
  }

  QHBoxLayout* layout = new QHBoxLayout(&dialog);
  layout->addWidget(tabWidget);
  dialog.setLayout(layout);
  dialog.setModal(true);
  dialog.show();
  dialog.exec();
}

void SAKMainWindow::showDonation() {
  QDialog dialog(this);
  dialog.setModal(true);
  QHBoxLayout* hBoxLayout = new QHBoxLayout(&dialog);
  QString image = ":/resources/images/WeChat.jpg";
  QLabel* label = new QLabel(&dialog);
  QPixmap pixMap = QPixmap::fromImage(QImage(image));
  label->setPixmap(pixMap.scaledToHeight(400, Qt::SmoothTransformation));
  hBoxLayout->addWidget(label);
  dialog.layout()->addWidget(label);
  dialog.show();
  dialog.exec();
}

void SAKMainWindow::createQtConf() {
  QString fileName = SAK_QT_CONF;
  QFile file(fileName);
  if (file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate)) {
    QTextStream out(&file);
    out << "[Platforms]\nWindowsArguments = dpiawareness=0\n";
    file.close();
  } else {
    qCWarning(mLoggingCategory) << fileName;
    qCWarning(mLoggingCategory) << "can not open file:" << file.errorString();
  }
}

void SAKMainWindow::onImportActionTriggered() {
  auto str = QFileDialog::getOpenFileName(this, tr("Save Palette"), "Palete",
                                          tr("All (*)"));
  if (str.isEmpty()) {
    qCInfo(mLoggingCategory) << "cancle to import the palette";
  } else {
    QFile inFile(str);
    if (inFile.open(QFile::ReadOnly)) {
      QByteArray bytes = inFile.readAll();
      inFile.close();

      QUrl url(str);
      QString fn = url.fileName();
      QString path = palettePath();
      QString outFileName = path + "/" + fn;
      if (QFile::exists(outFileName)) {
        QMessageBox::warning(this, tr("File Exists"),
                             tr("The file is exists, "
                                "import operaion failed"));
        return;
      }

      QFile outFile(outFileName);
      if (outFile.open(QFile::WriteOnly)) {
        QDataStream out(&outFile);
        out << bytes;
        outFile.close();
      } else {
        qCWarning(mLoggingCategory)
            << "open out file failed:" << inFile.errorString();
      }
    } else {
      qCWarning(mLoggingCategory)
          << "open in file failed:" << inFile.errorString();
    }
  }
}

void SAKMainWindow::onExportActionTriggered() {
  auto str = QFileDialog::getSaveFileName(this, tr("Save Palette"), "Palete",
                                          tr("All (*)"));
  if (str.isEmpty()) {
    qCInfo(mLoggingCategory) << "cancle to export the palette";
  } else {
    QFile file(str);
    if (file.open(QFile::WriteOnly)) {
      QPalette p = qApp->palette();
      QDataStream out(&file);
      out << p;
      file.close();
    } else {
      qCWarning(mLoggingCategory) << "can not open file:" << file.errorString();
    }
  }
}
