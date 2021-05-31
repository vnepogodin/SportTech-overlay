// Copyright (C) 2021 Vladislav Nepogodin
//
// This file is part of SportTech overlay project.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#ifndef UNIX_MAINWINDOW_H_
#define UNIX_MAINWINDOW_H_

#include <ui_mainwindow.h>

#include <memory>

#include <QMainWindow>
#include <QProcess>
#include <QSystemTrayIcon>

namespace vnepogodin {
class MainWindow : public QMainWindow {
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)
 public:
    explicit MainWindow(QWidget* parent = nullptr);
    virtual ~MainWindow();

 protected:
    virtual bool event(QEvent* ev) override;

 public slots:
    void iconActivated(QSystemTrayIcon::ActivationReason);

 private:
    std::unique_ptr<QSystemTrayIcon> m_trayIcon;
    std::unique_ptr<QMenu> m_trayMenu;
    std::unique_ptr<QAction> m_quitAction;
    std::unique_ptr<QAction> m_settingsAction;
    int m_timer{};

    void createMenu() noexcept;

    std::array<std::uint8_t, 2> m_activated;
    //const int refresh_rate = 500;  // Frequency of keyboard input checking in hertz
    //std::thread poll;

    std::unique_ptr<QProcess> m_process_settings;
    //std::unique_ptr<QProcess> m_process_charts;
    std::unique_ptr<Ui::MainWindow> m_ui = std::make_unique<Ui::MainWindow>();

    //void loadToMemory();
};
}  // namespace vnepogodin

#endif  // UNIX_MAINWINDOW_H_
