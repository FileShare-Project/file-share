/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Thu May 30 08:59:00 2024 Léo Lhuile
** Last update Thu May 30 08:59:00 2024 Léo Lhuile
**
** DeviceList/Model.cpp : Implementation of DeviceList/Model class
*/

#include "DeviceList/Model.hpp"

namespace FileShare::GUI::DeviceList {
    Model::Model() {
        this->lanDevices = {"Sister", "Father"};
        this->ownDevices = {"My device 1", "My device 2"};
        this->contactDevices = {"Alexandre", "Bart", "Caroline", "Didier"};
    }

    Model::~Model() {}

    void Model::setCurrentDeviceConnected(bool connected) {
        this->currentDeviceConnected = connected;
    }

    std::vector<std::string> Model::getSections() const {
        return { "LAN", "Own", "Contacts" };
    }

    std::vector<std::string> Model::getDevicesInSection(const std::string &section) const {
        if (section == "LAN") return this->lanDevices;
        else if (section == "Own") return this->ownDevices;
        else if (section == "Contacts") return this->contactDevices;
        return {};
    }
}
