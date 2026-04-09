#include <iostream>
#include <modbus/modbus.h>
#include <mutex>
#include <fstream>
#include <ctime>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <cstring>
#include <filesystem>
#include <cmath>

using namespace std;
namespace fs = std::filesystem;

/* ===================== GLOBAL ===================== */
mutex log_mutex;
int fd;
uint8_t msg[8], buf[256];

/* ===================== LOGGING ===================== */

string getLogFileName(const string& prefix) {
    time_t now = time(nullptr);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&now));
    return prefix + "_" + string(buf) + ".log";
}

void cleanupLogs(const string& prefix) {
    time_t now = time(nullptr);

    for (auto &f : fs::directory_iterator(".")) {
        string name = f.path().filename();

        if (name.find(prefix) != string::npos) {
            auto ftime = fs::last_write_time(f);

            auto sctp = chrono::time_point_cast<chrono::system_clock::duration>(
                ftime - fs::file_time_type::clock::now()
                + chrono::system_clock::now());

            time_t file_time = chrono::system_clock::to_time_t(sctp);

            if (difftime(now, file_time) > (7 * 86400))
                fs::remove(f);
        }
    }
}

void logData(string line) {
    lock_guard<mutex> lock(log_mutex);
    ofstream out(getLogFileName("pump_log"), ios::app);
    cleanupLogs("pump_log");
    time_t now = time(nullptr);
    out << "[" << ctime(&now) << "] " << line;
}

void logError(string error) {
    lock_guard<mutex> lock(log_mutex);
    ofstream out(getLogFileName("error_log"), ios::app);
    cleanupLogs("error_log");
    time_t now = time(nullptr);
    out << "[" << ctime(&now) << "] ERROR: " << error;
}

/* ===================== PUMP CALC ===================== */

struct PumpOutput {
    double efficiency;
    double flow;
    double head;
};

PumpOutput calculatePump(double dP, double dT, double power)
{
    const double rho = 998.0;
    const double Cp  = 4186.0;
    const double a   = 0.00045;
    const double g   = 9.81;
    const double ME  = 0.95;

    PumpOutput out;

    double EH = dP / rho;
    double EM = (a * dP) + (Cp * dT);

    out.efficiency = EH / EM;
    out.head = dP / (rho * g);
    out.flow = (out.efficiency * ME * power) / (rho * g * out.head);

    return out;
}

/* ===================== VALIDATION ===================== */

bool validateInputs(float P1, float P2, float T1, float T2, float power)
{
    if (P1 <= 0 || P2 <= 0) { logError("Invalid pressure"); return false; }
    if (P2 <= P1) { logError("P2 <= P1"); return false; }
    if (power <= 0) { logError("Invalid power"); return false; }
    if (T1 == 0 || T2 == 0) { logError("Temp missing"); return false; }
    if (fabs(T2 - T1) < 0.0001) { logError("Delta T too small"); return false; }

    return true;
}

/* ===================== MODBUS TCP ===================== */

class ModbusTCP {
    modbus_t *ctx;

public:
    ModbusTCP(const char *ip, int port) {
        ctx = modbus_new_tcp(ip, port);
    }

    bool connect() {
        return modbus_connect(ctx) != -1;
    }

    bool read(float &P1, float &P2, float &T1, float &T2) {
        uint16_t reg[8];

        if (modbus_read_registers(ctx, 100, 8, reg) == -1) {
            logError("TCP read failed");
            return false;
        }

        P1 = reg[0] / 10.0;
        P2 = reg[2] / 10.0;
        T1 = reg[4] / 10.0;
        T2 = reg[6] / 10.0;

        return true;
    }

    ~ModbusTCP() {
        modbus_close(ctx);
        modbus_free(ctx);
    }
};

/* ===================== RS485 ===================== */

uint16_t crc(uint8_t *buf, int len) {
    uint16_t crc = 0xFFFF;
    for (int i = 0; i < len; i++) {
        crc ^= buf[i];
        for (int j = 0; j < 8; j++)
            crc = (crc & 1) ? (crc >> 1) ^ 0xA001 : crc >> 1;
    }
    return crc;
}

void modbus_req(int slave, int addr) {
    msg[0] = slave;
    msg[1] = 0x04;
    msg[2] = addr >> 8;
    msg[3] = addr & 0xFF;
    msg[4] = 0;
    msg[5] = 2;

    uint16_t c = crc(msg, 6);
    msg[6] = c & 0xFF;
    msg[7] = c >> 8;
}

float readFloat(int slave, int reg)
{
    modbus_req(slave, reg);

    if (write(fd, msg, 8) < 0) {
        logError("RS485 write failed");
        return -1;
    }

    usleep(40000);

    int n = read(fd, buf, sizeof(buf));
    if (n <= 0) {
        logError("RS485 read failed");
        return -1;
    }

    uint32_t raw =
        (buf[3] << 24) |
        (buf[4] << 16) |
        (buf[5] << 8)  |
        buf[6];

    float *f = (float*)&raw;
    return *f;
}

/* ===================== ELECTRICAL DATA ===================== */

struct ElectricalData {
    float V1, V2, V3;
    float I1, I2, I3;
    float PF1, PF2, PF3;
    float freq;
    float power;
};

bool readElectrical(ElectricalData &e)
{
    int s = 1;

    e.V1 = readFloat(s, 0x3927);
    e.V2 = readFloat(s, 0x3941);
    e.V3 = readFloat(s, 0x3955);

    e.I1 = readFloat(s, 0x3929);
    e.I2 = readFloat(s, 0x3943);
    e.I3 = readFloat(s, 0x3957);

    e.PF1 = readFloat(s, 0x3923);
    e.PF2 = readFloat(s, 0x3937);
    e.PF3 = readFloat(s, 0x3951);

    e.freq = readFloat(s, 0x3110);

    e.power = readFloat(s, 0x3903);

    if (e.power <= 0) {
        logError("Invalid power");
        return false;
    }

    return true;
}

/* ===================== MAIN ===================== */

int main()
{
    ModbusTCP tcp("192.168.0.2", 502);

    if (!tcp.connect()) {
        logError("TCP connection failed");
        return -1;
    }

    fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("RS485 open failed");
        return -1;
    }

    struct termios tty;
    tcgetattr(fd, &tty);
    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);
    tcsetattr(fd, TCSANOW, &tty);

    while (1)
    {
        float P1=0, P2=0, T1=0, T2=0;
        ElectricalData e;

        if (!tcp.read(P1, P2, T1, T2)) continue;
        if (!readElectrical(e)) continue;

        if (!validateInputs(P1, P2, T1, T2, e.power)) continue;

        double dP = (P2 - P1) * 100000;
        double dT = T2 - T1;

        PumpOutput out = calculatePump(dP, dT, e.power);

        cout << "Eff: " << out.efficiency * 100 << "% | Flow: "
             << out.flow << " | Head: " << out.head << endl;

        string logLine =
            "Eff=" + to_string(out.efficiency) +
            " Flow=" + to_string(out.flow) +
            " Head=" + to_string(out.head) +
            " V1=" + to_string(e.V1) +
            " V2=" + to_string(e.V2) +
            " V3=" + to_string(e.V3) +
            " I1=" + to_string(e.I1) +
            " I2=" + to_string(e.I2) +
            " I3=" + to_string(e.I3) +
            " PF1=" + to_string(e.PF1) +
            " PF2=" + to_string(e.PF2) +
            " PF3=" + to_string(e.PF3) +
            " Freq=" + to_string(e.freq);

        logData(logLine);

        sleep(1);
    }

    close(fd);
}
