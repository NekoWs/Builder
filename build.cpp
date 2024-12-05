#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <windows.h>

using namespace std;
int main(int argc, char *argv[]) {
    if (argc > 2) {
        cout << "This program can only build C/C++ files with dragging!" << endl;
        return 0;
    }
    char buf[512] = { 0 };
    GetModuleFileName(nullptr, buf, sizeof(buf) - 1);
    string cwd = buf;
    auto r = cwd.rfind('\\');
    cwd = cwd.substr(0, r);
    string path_file = cwd + "\\path";
    string path;
    ifstream ifs(path_file);
    stringstream buffer;
    buffer << ifs.rdbuf();
    string buffs = buffer.str();
    if (!ifs.good() || buffs.empty()) {
        ofstream ofs(path_file);
        if (ofs.is_open()) {
            errno = 0;
            cout << "Please enter MinGW path: " << endl;
            getline(cin, path);
            ofs << path;
            int err = errno;
            if (err != 0) {
                cout << strerror(err) << endl;
            } else {
                cout << "MinGW path set successfully! Now you can drag and drop a C/C++ file to build." << endl;
            }
            getchar();
            return 0;
        } else {
            cout << "Couldn't open file \"" << path_file << "\"!" << endl;
            return 0;
        }
    }
    path = buffs + "\\bin";
    cout << "MinGW Path: " << path << endl;
    ifstream gpp(path + "\\g++.exe");
    ifstream gcc(path + "\\gcc.exe");
    if (!gpp.good() || !gcc.good()) {
        ofstream ofs(path_file);
        cout << R"("g++.exe" or "gcc.exe" not found. Please check your MinGW path!)" << endl;
        ofs << "";
        return 0;
    }
    if (argc < 2) {
        cout << "Please drag a C/C++ file into this program." << endl;
        return 0;
    }
    ifstream build("build.txt");
    string args = "-static";
    if (build.good()) {
        cout << "Custom build arguments found in build.txt. Using them..." << endl;
        buffer.str("");
        buffer << build.rdbuf();
        args = buffer.str();
    }
    cout << "Build Args: " << args << endl;
    char *file = argv[1];
    string str = file;
    r = str.rfind('.');
    auto f = str.find('\\');
    string last = str.substr(r, str.length() - r);
    string start = str.substr(0, r);
    string file_name = str.substr(f + 1, str.length() - r);
    string builder = last == ".cpp" ? "g++.exe" : last == ".c" ? "gcc.exe" : "";
    if (builder.empty()) {
        cout << "Invalid file type \"" << last << "\"!" << endl;
        return 0;
    }
    SetEnvironmentVariable("PATH", (path + ";" + std::getenv("PATH")).c_str());
    string cmd = path + "\\" + builder + " " + args + " " +
            file_name + ".cpp -o " + file_name + ".exe";
    cout << "Build Command: " << cmd << endl;
    cout << "Building..." << endl;
    system(cmd.data());
    return 0;
}
