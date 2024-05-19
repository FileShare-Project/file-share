/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sat Jan 15 21:08:44 2022 Léo Lhuile
** Last update Wed Sep 14 22:26:27 2022 Léo Lhuile
**
** DeviceList/Model.hpp : Class to manage the device list data
*/

#pragma once

#include <vector>
#include <string>

namespace FileShare::GUI::DeviceList {
    class Model {
        public:
            Model();
            ~Model();

            void setCurrentDevice(const std::string& device) { this->currentDevice = device; }
            std::string getCurrentDevice() { return this->currentDevice; }

            void toggleCurrentDeviceConnected() { this->currentDeviceConnected = !this->currentDeviceConnected; }
            bool isCurrentDeviceConnected() { return this->currentDeviceConnected; }

            std::vector<std::string> getSections();
            std::vector<std::string> getDevicesInSection(const std::string& section);

        private:
            std::vector<std::string> lanDevices;
            std::vector<std::string> ownDevices;
            std::vector<std::string> contactDevices;

            std::string currentDevice = "";
            bool currentDeviceConnected = false;
    };
}
