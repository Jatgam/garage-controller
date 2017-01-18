#ifndef JATGAMVERSION_H
#define JATGAMVERSION_H

#include <Arduino.h>
//#include <stdio.h>

struct ModuleVersion {
public:
    ModuleVersion(const char* version) {
        char* progressptr;
        char* internalVersion;
        strcpy(internalVersion, version);
        major = atoi(strtok_r(internalVersion, ".", &progressptr));
        minor = atoi(strtok_r(NULL, ".", &progressptr));
        revision = atoi(strtok_r(NULL, ".", &progressptr));
        // Doesnt work with esp8266 core 2.3, and 2.4 is beta and long overdue
		// sscanf(version, "%d.%d.%d", &major, &minor, &revision);
		if (major < 0) major = 0;
		if (minor < 0) minor = 0;
		if (revision < 0) revision = 0;
	}
    bool operator < (const ModuleVersion& other) {
        if (major < other.major)
            return true;
        if (minor < other.minor)
            return true;
        if (revision < other.revision)
            return true;
        return false;
    }
    bool operator == (const ModuleVersion& other) {
        return major == other.major
            && minor == other.minor
            && revision == other.revision;
    }
    bool operator != (const ModuleVersion& other) {
        return !(major == other.major
            && minor == other.minor
            && revision == other.revision);
    }
private:
    int major, minor, revision;
};

#endif
