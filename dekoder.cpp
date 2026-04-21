#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstring>

class GRIB0 {
private:
    std::vector<uint8_t> data;

public:
    bool read(std::ifstream &file) {
        data.resize(8);

        file.read(reinterpret_cast<char*>(data.data()), 8);

        if (!file || file.gcount() != 8) {
            std::cout << "Blad odczytu sekcji 0\n";
            return false;
        }

        return true;
    }

    uint32_t getLength() const {
        return (data[4] << 16) | (data[5] << 8) | data[6];
    }

    uint8_t getEdition() const {
        return data[7];
    }

    std::string getSignature() const {
        return std::string(data.begin(), data.begin() + 4);
    }
};

class GRIB1 {
private:
    std::vector<uint8_t> data;

public:
    bool read(std::ifstream &file) {
        uint8_t len[3];

        file.read(reinterpret_cast<char*>(len), 3);
        if (file.gcount() != 3) return false;

        uint32_t length = (len[0] << 16) | (len[1] << 8) | len[2];

        data.resize(length);

        data[0] = len[0];
        data[1] = len[1];
        data[2] = len[2];

        file.read(reinterpret_cast<char*>(data.data() + 3), length - 3);
        if (file.gcount() != static_cast<std::streamsize>(length - 3)) {
            std::cout << "Blad sekcji 1\n";
            return false;
        }

        return true;
    }

    uint32_t getLength() const {
        return (data[0] << 16) | (data[1] << 8) | data[2];
    }

    uint8_t getTableVersion() const { return data[3]; }
    uint8_t getCenter() const { return data[4]; }

    uint8_t getParameter() const { return data[8]; }
    uint8_t getLevelType() const { return data[9]; }

    uint16_t getLevelValue() const {
        return (data[10] << 8) | data[11];
    }

    int getYear() const { return data[12] + 2000; }
    int getMonth() const { return data[13]; }
    int getDay() const { return data[14]; }
    int getHour() const { return data[15]; }

    bool hasGDS() const { return data[7] & 0x80; }
    bool hasBMS() const { return data[7] & 0x40; }
};

class GRIB2 {
private:
    std::vector<uint8_t> data;

public:
    bool read(std::ifstream &file) {
        uint8_t len[3];

        file.read(reinterpret_cast<char*>(len), 3);
        if (file.gcount() != 3) return false;

        uint32_t length = (len[0] << 16) | (len[1] << 8) | len[2];

        data.resize(length);

        data[0] = len[0];
        data[1] = len[1];
        data[2] = len[2];

        file.read(reinterpret_cast<char*>(data.data() + 3), length - 3);
        if (file.gcount() != static_cast<std::streamsize>(length - 3)) {
            std::cout << "Blad sekcji 2\n";
            return false;
        }

        return true;
    }

    uint32_t getLength() const {
        return (data[0] << 16) | (data[1] << 8) | data[2];
    }

    uint8_t getNV() const { return data[3]; }
    uint8_t getPV_PL() const { return data[4]; }
    uint8_t getGridType() const { return data[5]; }

    uint16_t getNx() const {
        return (data[6] << 8) | data[7];
    }

    uint16_t getNy() const {
        return (data[8] << 8) | data[9];
    }
};

class GRIB3 {
private:
    std::vector<uint8_t> data;

public:
  bool read(std::ifstream &file) {
        uint8_t len[3];

        file.read(reinterpret_cast<char*>(len), 3);
        if (file.gcount() != 3) return false;

        uint32_t length = (len[0] << 16) | (len[1] << 8) | len[2];

        data.resize(length);

        data[0] = len[0];
        data[1] = len[1];
        data[2] = len[2];

        file.read(reinterpret_cast<char*>(data.data() + 3), length - 3);
        if (file.gcount() != static_cast<std::streamsize>(length - 3)) {
            std::cout << "Blad sekcji 3\n";
            return false;
        }

        return true;
    }

    uint32_t getLength() const {
        return (data[0] << 16) | (data[1] << 8) | data[2];
    }

    uint8_t getBitmapIndicator() const {
        return data[3];
    }

    bool isBitmapPresent() const {
        return data[3] == 0;
    }
};

class GRIB4 {
private:
    std::vector<uint8_t> data;

public:
    bool read(std::ifstream &file) {
        uint8_t len[3];

        file.read(reinterpret_cast<char*>(len), 3);
        if (file.gcount() != 3) return false;

        uint32_t length = (len[0] << 16) | (len[1] << 8) | len[2];

        data.resize(length);

        data[0] = len[0];
        data[1] = len[1];
        data[2] = len[2];

        file.read(reinterpret_cast<char*>(data.data() + 3), length - 3);
        if (file.gcount() != static_cast<std::streamsize>(length - 3)) {
            std::cout << "Blad sekcji 4\n";
            return false;
        }

        return true;
    }

    uint32_t getLength() const {
        return (data[0] << 16) | (data[1] << 8) | data[2];
    }

    uint8_t getFlags() const { return data[3]; }

    int16_t getBinaryScaleFactor() const {
        return (data[4] << 8) | data[5];
    }

    float getReferenceValue() const {
        uint32_t ref = (data[6] << 24) | (data[7] << 16) |
                       (data[8] << 8) | data[9];
        float result;
        std::memcpy(&result, &ref, sizeof(float));
        return result;
    }

    uint8_t getBitsPerValue() const { return data[10]; }
};

bool findGRIB(std::ifstream &file) {
    char buf[4];

    if (!file.read(buf, 4)) return false;

    while (true) {
        if (buf[0]=='G' && buf[1]=='R' && buf[2]=='I' && buf[3]=='B') {
            file.seekg(-4, std::ios::cur);
            return true;
        }

        buf[0]=buf[1];
        buf[1]=buf[2];
        buf[2]=buf[3];

        if (!file.read(&buf[3], 1)) break;
    }

    return false;
}

bool checkEnd(std::ifstream &file) {
    char end[4];

    file.read(end, 4);
    if (file.gcount() != 4) return false;

    return (end[0] == '7' && end[1] == '7' &&
            end[2] == '7' && end[3] == '7');
}

int main() {
    std::ifstream file("file.grib", std::ios::binary);
    std::ofstream out("daneGRIB.txt");

    if (!file) {
        std::cout << "Nie mozna otworzyc pliku.\n";
        return 1;
    }

    int messageCount = 0;

    while (findGRIB(file)) {
        messageCount++;
        out << "GRIB nr " << messageCount << "\n";

        GRIB0 sec0;
        GRIB1 sec1;
        GRIB2 sec2;
        GRIB3 sec3;
        GRIB4 sec4;

        if (!sec0.read(file)) break;

        out << "Sekcja 0 OK\n";
        out << "Dlugosc: " << sec0.getLength() << "\n";
        out << "Edycja: " << int(sec0.getEdition()) << "\n\n";

        if (!sec1.read(file)) break;

        out << "Sekcja 1 OK\n";
        out << "Center: " << int(sec1.getCenter()) << "\n";
        out << "Parametr: " << int(sec1.getParameter()) << "\n\n";

        if (sec1.hasGDS()) {
            if (sec2.read(file)) {
                out << "Sekcja 2 OK\n";
                out << "Nx: " << sec2.getNx() << "\n";
                out << "Ny: " << sec2.getNy() << "\n\n";
            }
        } else {
            out << "Brak sekcji 2\n";
        }

        if (sec1.hasBMS()) {
            if (sec3.read(file)) {
                out << "Sekcja 3 OK\n";
                out << "Bitmapa: " << sec3.isBitmapPresent() << "\n\n";
            }
        } else {
            out << "Brak sekcji 3\n";
        }


        if (sec4.read(file)) {
            out << "Sekcja 4 OK\n";
            out << "Bits/value: " << int(sec4.getBitsPerValue()) << "\n";
            out << "Skala: " << sec4.getBinaryScaleFactor() << "\n";
            out << "Referencja " << sec4.getReferenceValue() << "\n\n";
        }

        if (checkEnd(file)) {
            out << "Koniec komunikatu\n\n\n";
        } else {
            out << "Brak poprawnego zakonczenia\n";
            break;
        }
    }

    out << "Liczba komunikatow: " << messageCount << "\n";

    return 0;
}