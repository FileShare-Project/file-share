/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sun Feb 13 22:03:32 2022 Léo Lhuile
** Last update Mon Aug 29 20:45:51 2022 Léo Lhuile
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

    std::vector<std::string> Model::getSections() {
        return { "LAN", "Own", "Contacts" };
    }

    std::vector<std::string> Model::getDevicesInSection(const std::string& section) {
        if (section == "LAN") return this->lanDevices;
        else if (section == "Own") return this->ownDevices;
        else if (section == "Contacts") return this->contactDevices;
        return {};
    }
}
