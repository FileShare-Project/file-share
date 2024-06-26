/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sun Feb 13 22:03:32 2022 Léo Lhuile
** Last update Mon Aug 29 20:45:51 2022 Léo Lhuile
**
** DeviceList/Controller.cpp : Implementation of DeviceList/Controller class
*/

#include "DeviceList/Controller.hpp"

namespace FileShare::GUI::DeviceList {
    Controller::Controller() {
        this->view = View::create();
        this->view->onSelectDevice.connect([this](const std::string& device) {
            this->handleSelectDevice(device);
        });
        this->view->onToggleDevice.connect([this]() {
            this->handleToggleDevice();
        });

        for (auto section : this->model.getSections()) {
            this->view->addDeviceList(section, this->model.getDevicesInSection(section));
        }
    }

    Controller::~Controller() {}

    void Controller::handleSelectDevice(const std::string& device) {
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
