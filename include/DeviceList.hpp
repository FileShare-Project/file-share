/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sat Jan 15 21:08:44 2022 Léo Lhuile
** Last update Wed Sep 14 22:26:27 2022 Léo Lhuile
**
** DeviceList.hpp : Class to manage the device list.
*/

#pragma once

#include <TGUI/TGUI.hpp>

namespace FileShare::GUI {
	class DeviceList : public tgui::ScrollablePanel {
		public:
			DeviceList();
			~DeviceList();

			typedef std::shared_ptr<DeviceList> Ptr;
			typedef std::shared_ptr<const DeviceList> ConstPtr;

			static DeviceList::Ptr create();
			static DeviceList::Ptr copy(DeviceList::ConstPtr widget);

			void setCurrentDevice(std::string device);
			void setCurrentDeviceConnected(bool connected);

		protected:
			tgui::Widget::Ptr clone() const override;

		private:
			void createCurrentDeviceSection();
            void createSection(std::string name, std::vector<std::string> options);

			bool currentDeviceConnected = false;
	};
}
