#pragma comment(lib, "Dxva2.lib")

#include <iostream>
#include <windows.h>
#include <vector>
#include <sstream>
#include <physicalmonitorenumerationapi.h>
#include <lowlevelmonitorconfigurationapi.h>


using namespace std;

struct MonitorHandles {
    vector<HMONITOR> hvMonitors;

    /**
     * Callback for existing physical monitors enumerating.
     *
     * @param hMon Monitor handle.
     * @param hdc Not used.
     * @param lprcMonitor Not used.
     * @param pData Additional data to push.
     * @return True.
     */
    static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData) {
        auto *pThis = reinterpret_cast<MonitorHandles *>(pData);
        pThis->hvMonitors.push_back(hMon);
        return TRUE;
    }

    /**
     * Iterates through existing physical monitors.
     *
     * @return BOOL EnumDisplayMonitors(...) result.
     */
    BOOL iterate() {
        return EnumDisplayMonitors(nullptr, nullptr, (MONITORENUMPROC) MonitorEnum, (LPARAM) this);
    }
};

/**
 * Prints existing display devices.
 */
static void ListDisplayDevices() {
    DWORD dwCurDeviceNumber = 0;
    DISPLAY_DEVICE displayDevice = {sizeof(DISPLAY_DEVICE)};

    while (EnumDisplayDevices(nullptr, dwCurDeviceNumber, &displayDevice, 0)) {
        DWORD dwCurMonitorNumber = 0;
        DISPLAY_DEVICE newDisplayDevice = {sizeof(DISPLAY_DEVICE)};
        while (EnumDisplayDevices(displayDevice.DeviceName, dwCurMonitorNumber, &newDisplayDevice, 0)) {
            std::cout << dwCurDeviceNumber << " - " << newDisplayDevice.DeviceID << std::endl;
            dwCurMonitorNumber++;
        }
        dwCurDeviceNumber++;
    }
}

/**
 * Parses target @pcTargetPort port to DWORD.
 *
 * @param pcTargetPort
 * @return DWORD representation of @pcTargetPort.
 */
static DWORD ParseTargetPort(char *pcTargetPort) {
    unsigned int temp;
    stringstream ss;

    ss << std::hex << pcTargetPort;
    ss >> temp;

    return static_cast<DWORD>(temp);
}

int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "--list") == 0) {
        ListDisplayDevices();
        return 0;
    }

    if (argc != 5) {
        std::cout << "program usage: switch[.exe] --monitor <ID> --code <CODE>" << std::endl;
        std::cout << "           or: switch[.exe] --list (displays all monitors whose can be set)" << std::endl;
        std::cout << "           or: switch[.exe] --help" << std::endl;
        std::cout << "  <ID> - is a monitor code that can be retrieved from `switch[.exe] --list` command;" << std::endl;
        std::cout << "  <CODE> - is a hex code of your monitor input. table of possible values is given below:" << std::endl;
        std::cout << "      01 - D-SUB/VGA;" << std::endl;
        std::cout << "      03 - DVI;" << std::endl;
        std::cout << "      04 - HDMI;" << std::endl;
        std::cout << "      11 - HDMI;" << std::endl;
        std::cout << "      0F - DISPLAY PORT." << std::endl;
        std::cout << "  the possible value is in range from 0 to 12 (in hex). if you don't know the desired value, you can always enumerate through all." << std::endl;
        return 1;
    }

    int iTargetMonitorIdx = strtol(argv[2], nullptr, 0);
    DWORD dwTargetPort = ParseTargetPort(argv[4]);

    auto monitorHandles = MonitorHandles();
    if (!monitorHandles.iterate()) {
        std::cout << "cannot enumerate active displays. exiting..." << std::endl;
        return 1;
    }

    auto hvMonitor = monitorHandles.hvMonitors[iTargetMonitorIdx];

    DWORD numberOfMonitors;

    GetNumberOfPhysicalMonitorsFromHMONITOR(hvMonitor, &numberOfMonitors);
    if (GetLastError() != 0) {
        std::cout << "error while getting number of physical monitors. exiting..." << std::endl;
        return 1;
    }

    auto pPhysicalMonitors = (LPPHYSICAL_MONITOR) malloc(numberOfMonitors * sizeof(PHYSICAL_MONITOR));
    if (GetPhysicalMonitorsFromHMONITOR(hvMonitor, numberOfMonitors, pPhysicalMonitors)) {
        for (auto i = 0; i < numberOfMonitors; ++i) {
            // 0x0F - DP, 0x11 - HDMI
            SetVCPFeature(pPhysicalMonitors[i].hPhysicalMonitor, (BYTE) 0x60 /* vcp input feature code */,
                          dwTargetPort);
            DestroyPhysicalMonitor(pPhysicalMonitors[i].hPhysicalMonitor);
        }
    }

    return 0;
}
