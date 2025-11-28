#include <iostream>
#include <vector>
#include <string>
using namespace std;

struct Event {
    int start, end;
    string name;
};

int parseTimeAt(const string& line, int pos) {
    int hour = (line[pos] - '0') * 10 + (line[pos + 1] - '0');
    int min = (line[pos + 3] - '0') * 10 + (line[pos + 4] - '0');
    if (hour == 24 && min == 0) return 1440;
    return hour * 60 + min;
}

string minToTime(int minutes) {
    if (minutes == 1440) return "24:00";
    int hour = minutes / 60, min = minutes % 60;
    char result[6];
    result[0] = '0' + hour / 10;
    result[1] = '0' + hour % 10;
    result[2] = ':';
    result[3] = '0' + min / 10;
    result[4] = '0' + min % 10;
    result[5] = '\0';
    return string(result);
}

signed main() {
    int n; cin >> n;
    cin.ignore();
    
    vector<vector<Event>> bucket(1441);
    
    for (int i = 0; i < n; i++) {
        string line;

        do {
            getline(cin, line);
        } while(line.empty() && !cin.eof());
        
        if (cin.eof()) break;
        
        if (line.length() < 11) {
            cout << "Format tidak valid: baris terlalu pendek" << endl;
            continue;
        }
        
        int pos1 = -1, pos2 = -1;
        for (int j = 0; j < line.length(); j++) {
            if (line[j] == ' ') {
                if (pos1 == -1) pos1 = j;
                else if (pos2 == -1) {
                    pos2 = j;
                    break;
                }
            }
        }
        
        if (pos1 == -1 || pos2 == -1) {
            cout << "Format tidak valid: spasi tidak mencukupi" << endl;
            continue;
        }
        
        if (pos1 < 5 || pos2 < pos1 + 6) {
            cout << "Format tidak valid: format waktu salah" << endl;
            continue;
        }
        
        int start = parseTimeAt(line, 0);
        int end = parseTimeAt(line, pos1 + 1);
        
        string name = "";
        name.reserve(line.length() - pos2 - 1);
        for (int j = pos2 + 1; j < line.length(); j++) {
            name += line[j];
        }
        
        if (start >= end) {
            cout << "Kegiatan tidak valid: '" << name << "' (waktu mulai >= waktu selesai)" << endl;
            continue;
        }
        
        bucket[end].emplace_back(Event{start, end, move(name)});
    }
    
    vector<Event> result;
    result.reserve(n);
    int last = 0;
    
    for (int time = 0; time <= 1440; ++time) {
        for (const Event& event : bucket[time]) {
            if (event.start >= last) {
                result.push_back(event);
                last = event.end;
            }
        }
    }
    
    cout << result.size() << endl;
    for (const Event& event : result) {
        cout << minToTime(event.start) << " " << minToTime(event.end) << " " << event.name << endl;
    }
    
    return 0;
}