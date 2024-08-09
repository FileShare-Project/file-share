/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Thu May 30 08:59:00 2024 Léo Lhuile
** Last update Thu May 30 08:59:00 2024 Léo Lhuile
**
** DeviceList/Controller.cpp : Implementation of DeviceList/Controller class
*/

#include "DeviceList/Controller.hpp"

namespace FileShare::GUI::DeviceList {
    Controller::Controller() {
        this->view = View::create();
        this->view->onSelectDevice(&Controller::handleSelectDevice, this);
        this->view->onToggleDevice(&Controller::handleToggleDevice, this);

        for (auto section : this->model.getSections()) {
            this->view->addDeviceList(section, this->model.getDevicesInSection(section));
        }
    }

    Controller::~Controller() {}

    void Controller::handleSelectDevice(const std::string &device) {
        this->model.setCurrentDevice(device);

        auto currentDevice = this->model.getCurrentDevice();
        this->view->setCurrentDevice(currentDevice);
    }

    void Controller::handleToggleDevice() {
        auto connected = !this->model.isCurrentDeviceConnected();

        this->model.setCurrentDeviceConnected(connected);
        this->view->setCurrentDeviceConnected(connected);
    }
}
